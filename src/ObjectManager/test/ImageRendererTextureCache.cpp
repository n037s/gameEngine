#include "ImageRenderer.h"
#include "UnitTestEnvironment.h"

#include <memory>

namespace
{
    struct TextureFixture : UnitTest::Fixture
    {
        bool initialized{false};
        SDL_Surface* target{nullptr};
        SDL_Renderer* renderer{nullptr};
        std::unique_ptr<Image> first;
        std::unique_ptr<Image> second;
        std::unique_ptr<ImageRenderer> image;

        static std::unique_ptr<Image> createImage(int size)
        {
            auto surface = SDL_CreateSurface(size, size, SDL_PIXELFORMAT_RGBA8888);
            if (!surface)
                throw std::runtime_error(SDL_GetError());
            return std::make_unique<Image>(surface);
        }

        void setUp() override
        {
            initialized = SDL_Init(0);
            if (!initialized)
                throw std::runtime_error(SDL_GetError());
            target = SDL_CreateSurface(64, 64, SDL_PIXELFORMAT_RGBA8888);
            renderer = target ? SDL_CreateSoftwareRenderer(target) : nullptr;
            if (!renderer)
                throw std::runtime_error(SDL_GetError());
            first = createImage(16);
            second = createImage(32);
            image = std::make_unique<ImageRenderer>(renderer, first.get());
        }

        void tearDown() override
        {
            image.reset();
            second.reset();
            first.reset();
            SDL_DestroyRenderer(renderer);
            SDL_DestroySurface(target);
            if (initialized)
                SDL_Quit();
        }
    };
}

int main()
{
    UnitTest::Environment tests("Image renderer texture cache");
    tests.addScenario<TextureFixture>("Switching images reuses distinct cached textures",
        [](UnitTest::Scenario& scenario, TextureFixture& fixture) {
            const auto firstTexture = fixture.image->getTexture();
            UT_REQUIRE(scenario, firstTexture);
            fixture.image->setAsset(fixture.second.get());
            const auto secondTexture = fixture.image->getTexture();
            UT_REQUIRE(scenario, secondTexture);
            UT_EXPECT(scenario, firstTexture != secondTexture);
            fixture.image->setAsset(fixture.first.get());
            UT_EXPECT(scenario, fixture.image->getTexture() == firstTexture);
            fixture.image->setAsset(fixture.second.get());
            UT_EXPECT(scenario, fixture.image->getTexture() == secondTexture);
        });
    tests.addScenario<TextureFixture>("A cached image remains generated and renderable",
        [](UnitTest::Scenario& scenario, TextureFixture& fixture) {
            fixture.image->setAsset(fixture.second.get());
            fixture.image->setAsset(fixture.first.get());
            UT_REQUIRE(scenario, fixture.image->isGenerated());
            UT_EXPECT(scenario, fixture.image->render(SDL_FRect{0, 0, 16, 16}));
        });
    tests.addScenario<TextureFixture>("A missing asset clears state and allows cached recovery",
        [](UnitTest::Scenario& scenario, TextureFixture& fixture) {
            const auto firstTexture = fixture.image->getTexture();
            UT_REQUIRE(scenario, firstTexture);
            fixture.image->setAsset(fixture.second.get());
            fixture.image->setAsset(nullptr);
            UT_EXPECT(scenario, !fixture.image->isGenerated());
            UT_EXPECT(scenario, !fixture.image->getTexture());
            fixture.image->setAsset(fixture.first.get());
            UT_EXPECT(scenario, fixture.image->isGenerated());
            UT_EXPECT(scenario, fixture.image->getTexture() == firstTexture);
        });
    return tests.run();
}