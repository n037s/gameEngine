#include "UnitTestEnvironment.h"
#include "Player.h"
#include "FrameRate.h"

#include <iostream>

namespace
{
    struct PlayerFixture : UnitTest::Fixture
    {
        std::shared_ptr<Player> player;
        FrameRate* frameRate;
        std::chrono::steady_clock::time_point start;

        void setUp() override
        {
            start = std::chrono::steady_clock::time_point{};

            frameRate = &FrameRate::getInstance();
            frameRate->reset(start);
            
            player = Player::getInstance();
            player->setPos(point2D(0, 0));
        }

        void tearDown() override
        {
            
        }
    };

    void verifySpeedConstancy(UnitTest::Scenario& scenario, PlayerFixture& fixture, int frameRateValue, const vector2D& direction)
    {
        // Simulate movement for a number of frames.
        for (int frame = 1; frame <= frameRateValue; ++frame)
        {
            fixture.frameRate->update(fixture.start + std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                std::chrono::seconds(1)) * frame / frameRateValue);
            fixture.player->move(direction);
            fixture.player->update();
        }

        // Verify the player position
        const vector2D expected = direction.normalized() * 138.0f;
        std::cout << "Expected position: (" << expected.toString() << ")" << std::endl;
        std::cout << "Actual position: (" << fixture.player->getPos().toString() << ")" << std::endl;

        UT_EXPECT(scenario, std::abs(fixture.player->getPos().x - expected.x) < 0.01f);
        UT_EXPECT(scenario, std::abs(fixture.player->getPos().y - expected.y) < 0.01f);

        // Verify the player position remains consistent after an additional update
        fixture.player->update();
        UT_EXPECT(scenario, std::abs(fixture.player->getPos().x - expected.x) < 0.01f);
        UT_EXPECT(scenario, std::abs(fixture.player->getPos().y - expected.y) < 0.01f);
    }
}

int main()
{
    UnitTest::Environment tests("Player");

    const auto TestsFPS = {30, 60, 144};
    const auto TestsDirection = {vector2D(1, 0), vector2D(1, 1)};

    for (const int frameRateValue : TestsFPS)
    {
        for (const vector2D direction : TestsDirection)
        {
            tests.addScenario<PlayerFixture>(
                "Verify player speed does not depend on frame rate with " + std::to_string(frameRateValue) + " FPS and direction " + direction.toString(), 
                [=](UnitTest::Scenario& scenario, PlayerFixture& fixture) { verifySpeedConstancy(scenario, fixture, frameRateValue, direction); }
            );
        }
    }

    return tests.run();
}