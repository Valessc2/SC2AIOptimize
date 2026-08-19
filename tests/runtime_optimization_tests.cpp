#include "sc2opt/tuner/BudgetGovernor.hpp"
#include "sc2opt/tuner/Calibration.hpp"
#include "sc2opt/tuner/Defaults.hpp"
#include "sc2opt/tuner/Telemetry.hpp"

#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <span>
#include <string_view>
#include <type_traits>

namespace {

int failures = 0;

void Check(bool condition, std::string_view message)
{
    if (!condition)
    {
        ++failures;
        std::cerr << "FAIL: " << message << '\n';
    }
}

void TestTelemetry()
{
    using namespace sc2opt::tuner;
    static_assert(std::is_empty_v<ScopedLatencyTimer<false>>);

    FixedLatencyHistogram histogram;
    for (const auto value : {10ull, 20ull, 30ull, 40ull, 50ull, 60ull, 70ull, 80ull, 90ull,
                             100ull})
    {
        histogram.Observe(value);
    }

    const LatencySummary summary = histogram.Summary();
    Check(summary.samples == 10, "telemetry sample count");
    Check(summary.minimum_ns == 10 && summary.maximum_ns == 100, "telemetry min/max");
    Check(summary.p50_upper_ns >= 50 && summary.p95_upper_ns >= summary.p50_upper_ns &&
              summary.p99_upper_ns >= summary.p95_upper_ns,
          "telemetry percentile upper bounds");

    histogram.Reset();
    Check(histogram.Summary().samples == 0, "telemetry reset");
}

void TestBudgetGovernor()
{
    using namespace sc2opt::tuner;

    // Default authority is OFF: SC2AIOptimize never limits consumer work unless explicitly
    // delegated a budget.
    BudgetState state{};
    state = UpdateBudgetState(state, 100'000'000);
    Check(state.pressure == BudgetPressure::Normal && state.policy_valid,
          "disabled governor never creates pressure");
    Check(AllowsWork(state, WorkClass::Optional), "disabled governor allows all work");

    const BudgetPolicy policy = BudgetPolicyFromEnvelope(40'000'000);
    Check(policy.enabled && policy.resume_ns == 22'000'000 && policy.soft_ns == 28'000'000 &&
              policy.hard_ns == 38'000'000,
          "40ms envelope maps to 55/70/95 hysteresis profile");

    state = UpdateBudgetState({}, 29'000'000, policy);
    Check(state.pressure == BudgetPressure::Soft, "soft budget pressure");
    Check(AllowsWork(state, WorkClass::Important) && !AllowsWork(state, WorkClass::Optional),
          "soft pressure sheds consumer-declared optional work only");

    state = UpdateBudgetState(state, 23'000'000, policy);
    Check(state.pressure == BudgetPressure::Soft, "hysteresis retains soft pressure");
    state = UpdateBudgetState(state, 21'000'000, policy);
    Check(state.pressure == BudgetPressure::Normal, "resume threshold clears pressure");

    state = UpdateBudgetState(state, 40'000'000, policy);
    Check(state.pressure == BudgetPressure::Hard && !AllowsWork(state, WorkClass::Important),
          "hard pressure allows critical work only after consumer opt-in");

    const BudgetState invalid = UpdateBudgetState({}, 1, BudgetPolicy{true, 30, 20, 40});
    Check(!invalid.policy_valid && AllowsWork(invalid, WorkClass::Critical) &&
              !AllowsWork(invalid, WorkClass::Optional),
          "invalid enabled budget policy fails closed");

    const BudgetPolicy zero = BudgetPolicyFromEnvelope(0);
    Check(!zero.enabled, "zero envelope leaves governor disabled");
}

void TestCalibration()
{
    using namespace sc2opt::tuner;

    const std::array small{CandidateEvidence{0, 20, 100'000.0, true},
                           CandidateEvidence{1, 20, 120'000.0, true}};
    const std::array medium{CandidateEvidence{0, 20, 100'000.0, true},
                            CandidateEvidence{1, 20, 70'000.0, true}};
    const std::array large{CandidateEvidence{0, 20, 100'000.0, true},
                           CandidateEvidence{1, 20, 60'000.0, true}};
    const std::array points{CalibrationPoint{8, small}, CalibrationPoint{32, medium},
                            CalibrationPoint{128, large}};

    std::array<CrossoverBand, 4> bands{};
    const CalibrationResult result = BuildCrossoverCalibration(points, bands);
    Check(result.ok() && result.workload_points == 3 && result.bands_written == 2,
          "calibration compresses adjacent champions");
    Check(bands[0].maximum_workload_size == 8 && bands[0].champion == kBaselineCandidate,
          "small workload keeps baseline");
    Check(bands[1].maximum_workload_size == 128 && bands[1].champion == 1,
          "larger workloads use proven candidate");

    const auto calibrated =
        std::span<const CrossoverBand>{bands.data(), result.bands_written};
    Check(SelectCrossoverChampion(calibrated, 50) == 1, "calibrated crossover selector");

    Check(CheckControlOverhead(5, 1000).accepted, "cheap control overhead accepted");
    Check(!CheckControlOverhead(20, 1000).accepted, "expensive relative control overhead rejected");
    Check(!CheckControlOverhead(60'000, 100'000'000).accepted,
          "absolute control overhead ceiling enforced");

    const std::array no_baseline{CandidateEvidence{1, 20, 50'000.0, true}};
    const std::array missing{CalibrationPoint{8, no_baseline}};
    Check(BuildCrossoverCalibration(missing, bands).issue == CalibrationIssue::MissingBaseline,
          "calibration requires baseline evidence");

    const auto invalid_policy = ChooseNetBenefitChampion(
        small, NetBenefitPolicy{1, 0.0, std::numeric_limits<double>::quiet_NaN()});
    Check(invalid_policy.reason == DecisionReason::PolicyInvalid &&
              invalid_policy.champion == kBaselineCandidate,
          "invalid net-benefit policy fails closed");

    Check(defaults::kWorkloadSizeCandidates.front() == 8 &&
              defaults::kWorkloadSizeCandidates.back() == 1024,
          "geometric workload seed range");
    Check(defaults::kSpatialCellSizeCandidates.front() == 2.0f &&
              defaults::kSpatialCellSizeCandidates.back() == 12.0f,
          "spatial cell seed range");
}

}  // namespace

int main()
{
    TestTelemetry();
    TestBudgetGovernor();
    TestCalibration();

    if (failures == 0)
    {
        std::cout << "SC2AIOptimize runtime optimisation tests passed\n";
        return 0;
    }

    std::cerr << failures << " runtime optimisation test(s) failed\n";
    return 1;
}
