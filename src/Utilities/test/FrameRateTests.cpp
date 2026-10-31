#include "FrameRate.h"
#include "UnitTestEnvironment.h"

#include <type_traits>

static_assert(!std::is_copy_constructible_v<FrameRate>);
static_assert(!std::is_copy_assignable_v<FrameRate>);

int main()
{
    UnitTest::Environment tests("FrameRate");
    const auto start = std::chrono::steady_clock::time_point{};

    tests.addScenario("Readers share one counter and do not advance it", [=](UnitTest::Scenario& scenario) {
        FrameRate& frameRate = FrameRate::getInstance();
        frameRate.reset(start);
        UT_EXPECT(scenario, &frameRate == &FrameRate::getInstance());
        UT_EXPECT(scenario, frameRate.getFPS() == 0);
        for (int frame = 1; frame <= 30; ++frame)
        {
            frameRate.update(start + std::chrono::milliseconds(frame * 500 / 30));
            if (frame < 30)
                UT_EXPECT(scenario, frameRate.getFPS() == 0);
        }
        UT_EXPECT(scenario, frameRate.getFPS() == 60);
        UT_EXPECT(scenario, FrameRate::getInstance().getFPS() == 60);
    });

    tests.addScenario("Uses actual elapsed time and starts a fresh window", [=](UnitTest::Scenario& scenario) {
        FrameRate& frameRate = FrameRate::getInstance();
        frameRate.reset(start);
        frameRate.update(start + std::chrono::milliseconds(100));
        frameRate.update(start + std::chrono::milliseconds(200));
        frameRate.update(start + std::chrono::milliseconds(750));
        UT_EXPECT(scenario, frameRate.getFPS() == 4);
        frameRate.update(start + std::chrono::milliseconds(1000));
        UT_EXPECT(scenario, frameRate.getFPS() == 4);
        frameRate.update(start + std::chrono::milliseconds(1250));
        UT_EXPECT(scenario, frameRate.getFPS() == 4);
    });

    tests.addScenario("Reset clears the published FPS and pending frames", [=](UnitTest::Scenario& scenario) {
        FrameRate& frameRate = FrameRate::getInstance();
        frameRate.reset(start);
        frameRate.update(start + std::chrono::milliseconds(500));
        UT_EXPECT(scenario, frameRate.getFPS() == 2);
        frameRate.update(start + std::chrono::milliseconds(600));
        frameRate.reset(start + std::chrono::seconds(1));
        UT_EXPECT(scenario, frameRate.getFPS() == 0);
        frameRate.update(start + std::chrono::milliseconds(1500));
        UT_EXPECT(scenario, frameRate.getFPS() == 2);
    });

    return tests.run();
}