#include "AnimatedSprite.h"
#include "AssetsManager.h"
#include "UnitTestEnvironment.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>

namespace
{
    struct AnimationFixture : UnitTest::Fixture
    {
        inline static const std::filesystem::path directory = std::filesystem::path("Assets") /
            ("AnimatedSpriteTest_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        bool ownsDirectory{false};
        bool initialized{false};
        SDL_Surface* target{nullptr};
        SDL_Renderer* renderer{nullptr};
        std::unique_ptr<AnimatedSprite> sprite;

        void setUp() override
        {
            initialized = SDL_Init(0);
            if (!initialized)
                throw std::runtime_error(SDL_GetError());
            ownsDirectory = std::filesystem::create_directory(directory);
            if (!ownsDirectory)
                throw std::runtime_error("Animation fixture directory already exists: " + directory.string());
            const unsigned char gif[] = {
                'G', 'I', 'F', '8', '9', 'a', 1, 0, 1, 0, 0x80, 0, 0,
                0, 0, 0, 255, 255, 255,
                0x21, 0xf9, 4, 0, 1, 0, 0, 0,
                0x2c, 0, 0, 0, 0, 1, 0, 1, 0, 0, 2, 2, 0x44, 1, 0,
                0x21, 0xf9, 4, 0, 1, 0, 0, 0,
                0x2c, 0, 0, 0, 0, 1, 0, 1, 0, 0, 2, 2, 0x4c, 1, 0,
                0x3b
            };
            std::ofstream output(directory / "animation.gif", std::ios::binary);
            output.write(reinterpret_cast<const char*>(gif), sizeof(gif));
            if (!output.good())
                throw std::runtime_error("Could not write GIF fixture");
            target = SDL_CreateSurface(16, 16, SDL_PIXELFORMAT_RGBA8888);
            renderer = target ? SDL_CreateSoftwareRenderer(target) : nullptr;
            if (!renderer)
                throw std::runtime_error(SDL_GetError());
        }

        void tearDown() override
        {
            sprite.reset();
            SDL_DestroyRenderer(renderer);
            SDL_DestroySurface(target);
            if (initialized)
                SDL_Quit();
            if (ownsDirectory)
                std::filesystem::remove_all(directory);
        }

        std::string assetID() const { return directory.filename().generic_string() + "/animation"; }

        void loadAnimation(UnitTest::Scenario& scenario)
        {
            sprite = std::make_unique<AnimatedSprite>(point2D(0, 0), false, -1);
            UT_REQUIRE(scenario, sprite->setAnimation(assetID()));
            sprite->update();
            UT_REQUIRE(scenario, sprite->createRenderer(renderer));
        }
    };

    bool hasPixel(const AnimatedSprite& sprite, Uint8 expected)
    {
        Uint8 red = 0, green = 0, blue = 0, alpha = 0;
        auto renderer = sprite.getRenderer();
        return renderer && renderer->isGenerated()
            && SDL_ReadSurfacePixel(renderer->getSurface(), 0, 0, &red, &green, &blue, &alpha)
            && red == expected && green == expected && blue == expected && alpha == 255;
    }

    void playback(UnitTest::Scenario& scenario, AnimationFixture& fixture)
    {
        fixture.loadAnimation(scenario);
        UT_EXPECT(scenario, hasPixel(*fixture.sprite, 0));
        fixture.sprite->update();
        UT_EXPECT(scenario, hasPixel(*fixture.sprite, 255));
        fixture.sprite->update();
        UT_EXPECT(scenario, hasPixel(*fixture.sprite, 0));
    }

    void rejectUnknownIDs(UnitTest::Scenario& scenario, AnimationFixture& fixture)
    {
        AnimatedSprite empty(point2D(0, 0), false, -1);
        UT_EXPECT(scenario, !empty.setAnimation("missing-animation"));
        fixture.loadAnimation(scenario);
        UT_EXPECT(scenario, !fixture.sprite->setAnimation("missing-animation"));
        UT_EXPECT(scenario, hasPixel(*fixture.sprite, 0));
    }

    void rejectStaticImages(UnitTest::Scenario& scenario, AnimationFixture& fixture)
    {
        fixture.loadAnimation(scenario);
        const auto imageIDs = AssetsManager::getInstance()->getAssets();
        UT_REQUIRE(scenario, !imageIDs.empty());
        UT_EXPECT(scenario, !fixture.sprite->setAnimation(imageIDs.front()));
    }

    void replaceAnimation(UnitTest::Scenario& scenario, AnimationFixture& fixture)
    {
        fixture.loadAnimation(scenario);
        for (int iteration = 0; iteration < 4; ++iteration)
        {
            UT_REQUIRE(scenario, fixture.sprite->setAnimation(fixture.assetID()));
            UT_EXPECT(scenario, hasPixel(*fixture.sprite, 0));
            fixture.sprite->update();
            UT_EXPECT(scenario, hasPixel(*fixture.sprite, 255));
        }
    }

    void serializeAnimation(UnitTest::Scenario& scenario, AnimationFixture& fixture)
    {
        fixture.loadAnimation(scenario);
        fixture.sprite->update();
        auto members = fixture.sprite->serialize();
        UT_EXPECT(scenario, members.getMember<std::string>("AnimationID") == fixture.assetID());
        AnimatedSprite restored(members);
        UT_REQUIRE(scenario, restored.createRenderer(fixture.renderer));
        UT_EXPECT(scenario, hasPixel(restored, 0));
        restored.update();
        UT_EXPECT(scenario, hasPixel(restored, 255));
    }

    void legacyFrameLists(UnitTest::Scenario& scenario, AnimationFixture& fixture)
    {
        const auto imageIDs = AssetsManager::getInstance()->getAssets();
        UT_REQUIRE(scenario, !imageIDs.empty());
        AnimatedSprite legacy(point2D(0, 0), false, std::vector<std::string>{imageIDs.front()}, -1);
        auto legacyMembers = legacy.serialize();
        UT_EXPECT(scenario, legacyMembers.getMember<size_t>("AssetsCount") == 1);
        AnimatedSprite legacyRestored(legacyMembers);
        UT_EXPECT(scenario, legacyRestored.createRenderer(fixture.renderer));
    }

    void baseClassOwnership(UnitTest::Scenario& scenario, AnimationFixture& fixture)
    {
        fixture.loadAnimation(scenario);
        std::unique_ptr<Object> polymorphic = std::make_unique<AnimatedSprite>(fixture.sprite->serialize());
        UT_EXPECT(scenario, polymorphic->createRenderer(fixture.renderer));
        polymorphic.reset();
    }

    void unreadableGIF(UnitTest::Scenario& scenario, AnimationFixture& fixture)
    {
        fixture.loadAnimation(scenario);
        fixture.sprite->update();
        UT_REQUIRE(scenario, hasPixel(*fixture.sprite, 255));
        UT_REQUIRE(scenario, std::filesystem::remove(fixture.directory / "animation.gif"));
        UT_EXPECT(scenario, !fixture.sprite->setAnimation(fixture.assetID()));
        UT_EXPECT(scenario, hasPixel(*fixture.sprite, 255));
    }
}

int main()
{
    UnitTest::Environment tests("Animated sprite");
    tests.addScenario<AnimationFixture>("GIF playback starts on frame zero, advances, and wraps", playback);
    tests.addScenario<AnimationFixture>("Unknown IDs are rejected without changing the current frame", rejectUnknownIDs);
    tests.addScenario<AnimationFixture>("Static images are rejected as animation sources", rejectStaticImages);
    tests.addScenario<AnimationFixture>("Replacement resets playback without stale textures", replaceAnimation);
    tests.addScenario<AnimationFixture>("Serialization preserves the source and reloads all frames", serializeAnimation);
    tests.addScenario<AnimationFixture>("Legacy frame lists still serialize and reload", legacyFrameLists);
    tests.addScenario<AnimationFixture>("Animations support base-class ownership", baseClassOwnership);
    tests.addScenario<AnimationFixture>("Unreadable GIFs preserve the current animation", unreadableGIF);
    return tests.run();
}