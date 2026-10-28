#include "AnimatedSprite.h"
#include "AssetsManager.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace
{
    void require(bool condition, const char* message)
    {
        if (!condition)
            throw std::runtime_error(message);
    }

    struct AnimationFixture
    {
        std::filesystem::path directory = std::filesystem::path("Assets") /
            ("AnimatedSpriteTest_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));

        AnimationFixture()
        {
            std::filesystem::create_directories(directory);
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
            require(output.good(), "Could not write GIF fixture.");
        }

        ~AnimationFixture()
        {
            std::error_code error;
            std::filesystem::remove_all(directory, error);
        }

        std::string assetID() const { return directory.filename().generic_string() + "/animation"; }
    };

    bool hasPixel(const AnimatedSprite& sprite, Uint8 expected)
    {
        Uint8 red = 0, green = 0, blue = 0, alpha = 0;
        auto renderer = sprite.getRenderer();
        return renderer && renderer->isGenerated()
            && SDL_ReadSurfacePixel(renderer->getSurface(), 0, 0, &red, &green, &blue, &alpha)
            && red == expected && green == expected && blue == expected && alpha == 255;
    }

    void testAnimation()
    {
        AnimationFixture fixture;
        std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> target(
            SDL_CreateSurface(16, 16, SDL_PIXELFORMAT_RGBA8888), SDL_DestroySurface);
        std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer(
            target ? SDL_CreateSoftwareRenderer(target.get()) : nullptr, SDL_DestroyRenderer);
        require(renderer != nullptr, "Could not create software renderer.");

        AnimatedSprite sprite(point2D(0, 0), false, -1);
        require(!sprite.setAnimation("missing-animation"), "Unknown IDs must be rejected.");
        require(sprite.setAnimation(fixture.assetID()), "Could not decode the GIF.");
        sprite.update();
        require(sprite.createRenderer(renderer.get()), "Could not render the first frame.");
        require(hasPixel(sprite, 0), "The animation must start on its black frame.");
        sprite.update();
        require(hasPixel(sprite, 255), "The second GIF frame must be white.");
        sprite.update();
        require(hasPixel(sprite, 0), "Playback must wrap after the second frame.");

        require(!sprite.setAnimation("missing-animation"), "Unknown IDs must not load a fallback asset.");
        require(hasPixel(sprite, 0), "A failed switch must preserve the current frame.");
        const auto imageIDs = AssetsManager::getInstance()->getAssets();
        require(!imageIDs.empty(), "Expected a static image in the asset folder.");
        require(!sprite.setAnimation(imageIDs.front()), "Static images must not be accepted as animations.");

        for (int iteration = 0; iteration < 4; ++iteration)
        {
            require(sprite.setAnimation(fixture.assetID()), "Replacing an animation must succeed.");
            require(hasPixel(sprite, 0), "Replacing an animation must reset to its first frame.");
            sprite.update();
            require(hasPixel(sprite, 255), "Replacement frames must not reuse stale cached textures.");
        }

        auto members = sprite.serialize();
        require(members.getMember<std::string>("AnimationID") == fixture.assetID(), "The source animation ID must be saved.");
        AnimatedSprite restored(members);
        require(restored.createRenderer(renderer.get()) && hasPixel(restored, 0), "Reloading must restore the first frame.");
        restored.update();
        require(hasPixel(restored, 255), "Reloading must decode all GIF frames.");

        AnimatedSprite legacy(point2D(0, 0), false, std::vector<std::string>{imageIDs.front()}, -1);
        auto legacyMembers = legacy.serialize();
        require(legacyMembers.getMember<size_t>("AssetsCount") == 1, "Legacy frame lists must still serialize.");
        AnimatedSprite legacyRestored(legacyMembers);
        require(legacyRestored.createRenderer(renderer.get()), "Legacy frame lists must still reload.");

        std::unique_ptr<Object> polymorphic = std::make_unique<AnimatedSprite>(members);
        require(polymorphic->createRenderer(renderer.get()), "Animations must support base-class ownership.");
        polymorphic.reset();

        std::filesystem::remove(fixture.directory / "animation.gif");
        require(!sprite.setAnimation(fixture.assetID()), "An unreadable GIF must be rejected.");
        require(hasPixel(sprite, 255), "Decode failure must preserve the existing animation.");
    }
}

int main()
{
    if (!SDL_Init(0))
        return 1;
    int result = 0;
    try
    {
        testAnimation();
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        result = 1;
    }
    SDL_Quit();
    return result;
}