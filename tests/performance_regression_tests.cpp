#include "sc2opt/proof/Regression.hpp"

#include <iostream>
#include <limits>
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

void TestRegressionGate()
{
    using namespace sc2opt::proof;

    const BenchmarkRecord reference{1, 128, 100, 1000.0, 900, 1000, 1200, true};
    const BenchmarkRecord good{1, 128, 100, 1010.0, 900, 1030, 1250, true};
    Check(EvaluatePerformanceRegression(reference, good).passed(),
          "small tail movement stays within default regression budget");

    const BenchmarkRecord bad_p95{1, 128, 100, 1100.0, 950, 1100, 1250, true};
    Check(EvaluatePerformanceRegression(reference, bad_p95).issue == RegressionIssue::P95Regression,
          "p95 regression detected");

    const BenchmarkRecord bad_p99{1, 128, 100, 1100.0, 950, 1040, 1400, true};
    Check(EvaluatePerformanceRegression(reference, bad_p99).issue == RegressionIssue::P99Regression,
          "p99 regression detected");

    const BenchmarkRecord wrong_identity{2, 128, 100, 900.0, 800, 900, 1000, true};
    Check(EvaluatePerformanceRegression(reference, wrong_identity).issue ==
              RegressionIssue::IdentityMismatch,
          "regression comparison requires matching candidate/workload identity");

    PerformanceRegressionPolicy invalid{};
    invalid.maximum_p95_relative_increase = std::numeric_limits<double>::quiet_NaN();
    Check(EvaluatePerformanceRegression(reference, good, invalid).issue ==
              RegressionIssue::InvalidPolicy,
          "invalid regression policy fails closed");
}

}  // namespace

int main()
{
    TestRegressionGate();
    if (failures == 0)
    {
        std::cout << "SC2AIOptimize performance regression tests passed\n";
        return 0;
    }
    return 1;
}
