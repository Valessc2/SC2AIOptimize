#include "sc2opt/tuner/Champion.hpp"
#include "sc2opt/tuner/Context.hpp"
#include "sc2opt/tuner/NetBenefit.hpp"
#include "sc2opt/tuner/Persistence.hpp"
#include "sc2opt/tuner/TunableRegistry.hpp"

#include <array>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string_view>

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

void TestTunableRegistry()
{
    using namespace sc2opt::tuner;
    const std::array specs{
        TunableSpec{1, "grid.cell", TunableKind::Continuous, 4.0, 2.0, 8.0, 0.5,
                    TunableOnlineSafe},
        TunableSpec{2, "budget.enabled", TunableKind::Boolean, 0.0, 0.0, 1.0, 0.0,
                    TunableOnlineSafe},
    };
    Check(ValidateTunableRegistry(specs).ok(), "valid registry");
    Check(FindTunableById(specs, 2) != nullptr, "find tunable by id");
    Check(FindTunableByName(specs, "grid.cell") != nullptr, "find tunable by name");

    const std::array duplicate{
        TunableSpec{1, "a", TunableKind::Integer, 1.0, 0.0, 2.0, 1.0, 0},
        TunableSpec{1, "b", TunableKind::Integer, 1.0, 0.0, 2.0, 1.0, 0},
    };
    Check(ValidateTunableRegistry(duplicate).issue == TunableRegistryIssue::DuplicateId,
          "duplicate tunable id rejected");
}

void AddSamples(sc2opt::tuner::CandidateStats& stats, double value, int count)
{
    for (int i = 0; i < count; ++i)
        Check(stats.total_cost_ns.Add(value), "sample accepted");
}

void TestChampion()
{
    using namespace sc2opt::tuner;
    std::array<CandidateStats, 2> stats{{
        CandidateStats{0, {}, true},
        CandidateStats{1, {}, true},
    }};
    AddSamples(stats[0], 100.0, 10);
    AddSamples(stats[1], 70.0, 10);

    ChampionState state{};
    const auto decision = EvaluateChampion(stats, state);
    Check(decision.champion == 1, "confident faster challenger wins");
    Check(decision.reason == ChampionReason::ChallengerWon, "challenger win reason");

    ApplyChampionDecision(state, decision);
    Check(state.champion == 1, "champion decision applies deterministically");

    state.mode = TuningMode::BaselineOnly;
    const auto baseline = EvaluateChampion(stats, state);
    Check(baseline.champion == kBaselineCandidate, "baseline-only mode");

    ResetToBaseline(state);
    Check(state.champion == kBaselineCandidate && state.mode == TuningMode::Adaptive,
          "deterministic reset to baseline");

    ResetEvidence(stats);
    Check(stats[0].total_cost_ns.samples == 0 && !stats[0].correctness_passed,
          "evidence reset");

    std::array<CandidateStats, 2> noisy{{
        CandidateStats{0, {}, true},
        CandidateStats{1, {}, true},
    }};
    for (double value : {90.0, 110.0, 95.0, 105.0, 100.0})
        Check(noisy[0].total_cost_ns.Add(value), "baseline noisy sample");
    for (double value : {80.0, 120.0, 90.0, 110.0, 95.0})
        Check(noisy[1].total_cost_ns.Add(value), "challenger noisy sample");
    const auto uncertain = EvaluateChampion(noisy, {});
    Check(uncertain.champion == kBaselineCandidate,
          "overlapping confidence keeps baseline");

    const std::array<CandidateStats, 2> duplicate{{
        CandidateStats{0, RunningStats{5, 100.0, 0.0}, true},
        CandidateStats{0, RunningStats{5, 50.0, 0.0}, true},
    }};
    Check(EvaluateChampion(duplicate, {}).reason == ChampionReason::EvidenceInvalid,
          "duplicate candidate ids fail closed");
}

void TestContext()
{
    using namespace sc2opt::tuner;
    const ContextKey macro{10, 20, 1};
    const ContextKey fight{10, 20, 2};
    const std::array contexts{
        ContextChampion{macro, 1},
        ContextChampion{fight, 2},
    };
    Check(SelectContextChampion(contexts, fight) == 2, "context champion");
    Check(SelectContextChampion(contexts, ContextKey{99, 99, 0}) == kBaselineCandidate,
          "unknown context fails to baseline");

    const std::array bands{
        CrossoverBand{16, 0},
        CrossoverBand{96, 1},
        CrossoverBand{256, 2},
    };
    Check(ValidateCrossoverBands(bands) == CrossoverValidationIssue::None,
          "valid crossover bands");
    Check(SelectCrossoverChampion(bands, 8) == 0, "small workload baseline");
    Check(SelectCrossoverChampion(bands, 50) == 1, "medium workload candidate one");
    Check(SelectCrossoverChampion(bands, 200) == 2, "large workload candidate two");
    Check(SelectCrossoverChampion(bands, 1000) == 0,
          "uncovered size fails to baseline");
}

void TestPersistence()
{
    using namespace sc2opt::tuner;
    ChampionRecord record{};
    record.decision_id = 42;
    record.context = {11, 22, 3};
    record.champion = 2;
    record.samples = 100;
    record.mean_total_cost_ns = 123.5;
    record.sample_variance_ns2 = 4.25;
    record.provenance = {"abcdef", "75689", "B89", "gcc14-release", "ryzen-test",
                         "dense-fight"};

    std::stringstream stream;
    Check(WriteChampionRecord(stream, record), "write champion record");

    ChampionRecord round_trip{};
    Check(ReadChampionRecord(stream, round_trip), "read champion record");
    Check(round_trip.decision_id == record.decision_id, "persist decision id");
    Check(round_trip.context == record.context, "persist context");
    Check(round_trip.champion == record.champion, "persist champion");
    Check(std::fabs(round_trip.mean_total_cost_ns - record.mean_total_cost_ns) < 1e-12,
          "persist mean");
    Check(round_trip.provenance.hardware_profile == "ryzen-test", "persist provenance");

    std::stringstream bad(
        "SC2OPT_CHAMPION 999 1 0 0 0 0 1 1 0 \"x\" \"x\" \"x\" \"x\" \"x\" \"x\"\n");
    ChampionRecord rejected{};
    Check(!ReadChampionRecord(bad, rejected), "wrong schema rejected");
}

}  // namespace

int main()
{
    TestTunableRegistry();
    TestChampion();
    TestContext();
    TestPersistence();

    if (failures == 0)
    {
        std::cout << "SC2AIOptimize tuner tests passed\n";
        return 0;
    }

    std::cerr << failures << " tuner test(s) failed\n";
    return 1;
}
