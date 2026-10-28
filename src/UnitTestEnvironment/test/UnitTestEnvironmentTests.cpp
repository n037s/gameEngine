#include "UnitTestEnvironment.h"

#include <sstream>

namespace
{
    int setups = 0;
    int teardowns = 0;
    int destructions = 0;

    struct CountingFixture : UnitTest::Fixture
    {
        int value{0};
        void setUp() override { ++setups; }
        void tearDown() override { ++teardowns; }
        ~CountingFixture() override { ++destructions; }
    };

    struct BrokenSetup : CountingFixture
    {
        void setUp() override { throw std::runtime_error("setup failed"); }
    };

    struct BrokenTeardown : CountingFixture
    {
        void tearDown() override { throw std::runtime_error("teardown failed"); }
    };
}

int main()
{
    UnitTest::Environment tests("UnitTestEnvironment");
    tests.addScenario("Successful scenarios report success", [](UnitTest::Scenario& scenario) {
        UnitTest::Environment inner("passing");
        inner.addScenario("pass", [](UnitTest::Scenario& check) { UT_EXPECT(check, true); });
        std::ostringstream output;
        UT_EXPECT(scenario, inner.run(output) == 0);
        UT_EXPECT(scenario, output.str().find("1 passed, 0 failed") != std::string::npos);
    });
    tests.addScenario("Failures and exceptions do not stop later scenarios", [](UnitTest::Scenario& scenario) {
        UnitTest::Environment inner("failing");
        bool continued = false;
        bool aborted = true;
        inner.addScenario("expect", [&continued](UnitTest::Scenario& check) {
            UT_EXPECT(check, false);
            continued = true;
        });
        inner.addScenario("require", [&aborted](UnitTest::Scenario& check) {
            UT_REQUIRE(check, false);
            aborted = false;
        });
        inner.addScenario("exception", [](UnitTest::Scenario&) { throw std::runtime_error("broken"); });
        inner.addScenario("unknown", [](UnitTest::Scenario&) { throw 42; });
        inner.addScenario("last", [](UnitTest::Scenario& check) { UT_EXPECT(check, true); });
        std::ostringstream output;
        UT_EXPECT(scenario, inner.run(output) == 1);
        UT_EXPECT(scenario, continued && aborted);
        UT_EXPECT(scenario, output.str().find("1 passed, 4 failed") != std::string::npos);
        UT_EXPECT(scenario, output.str().find("UnitTestEnvironmentTests.cpp:") != std::string::npos);
        UT_EXPECT(scenario, output.str().find("broken") != std::string::npos);
    });
    tests.addScenario("Fixtures are fresh and cleaned up on every exit path", [](UnitTest::Scenario& scenario) {
        setups = teardowns = destructions = 0;
        UnitTest::Environment inner("fixtures");
        inner.addScenario<CountingFixture>("first", [](UnitTest::Scenario& check, CountingFixture& fixture) {
            UT_EXPECT(check, fixture.value == 0);
            fixture.value = 42;
        });
        inner.addScenario<CountingFixture>("second", [](UnitTest::Scenario& check, CountingFixture& fixture) {
            UT_REQUIRE(check, fixture.value == 0);
            UT_REQUIRE(check, false);
        });
        inner.addScenario<BrokenSetup>("setup", [](UnitTest::Scenario& check, BrokenSetup&) {
            check.fail("Body must not run after failed setup");
        });
        inner.addScenario<BrokenTeardown>("teardown", [](UnitTest::Scenario&, BrokenTeardown&) {});
        std::ostringstream output;
        UT_EXPECT(scenario, inner.run(output) == 1);
        UT_EXPECT(scenario, setups == 3 && teardowns == 3 && destructions == 4);
        UT_EXPECT(scenario, output.str().find("1 passed, 3 failed") != std::string::npos);
        UT_EXPECT(scenario, output.str().find("setup failed") != std::string::npos);
        UT_EXPECT(scenario, output.str().find("teardown failed") != std::string::npos);
    });
    tests.addScenario("Empty suites and duplicate names are rejected", [](UnitTest::Scenario& scenario) {
        UnitTest::Environment inner("empty");
        std::ostringstream output;
        UT_EXPECT(scenario, inner.run(output) == 1);
        inner.addScenario("same", [](UnitTest::Scenario&) {});
        bool rejected = false;
        try
        {
            inner.addScenario("same", [](UnitTest::Scenario&) {});
        }
        catch (const std::invalid_argument&)
        {
            rejected = true;
        }
        UT_EXPECT(scenario, rejected);
    });
    return tests.run();
}