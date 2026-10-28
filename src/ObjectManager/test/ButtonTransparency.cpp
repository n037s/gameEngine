#include "ButtonRenderer.h"
#include "DropDownMenuRenderer.h"
#include "UnitTestEnvironment.h"

#include <memory>

namespace
{
    struct ButtonFixture : UnitTest::Fixture
    {
        SDL_Surface* target{nullptr};
        SDL_Renderer* renderer{nullptr};
        FontPtr font;

        void setUp() override
        {
            if (!SDL_Init(0) || !TTF_Init())
                throw std::runtime_error(SDL_GetError());
            target = SDL_CreateSurface(200, 30, SDL_PIXELFORMAT_RGBA8888);
            renderer = target ? SDL_CreateSoftwareRenderer(target) : nullptr;
            if (!renderer)
                throw std::runtime_error(SDL_GetError());
            font = std::make_shared<Font>("arial", 20.0f);
            if (!font->isGenerated())
                throw std::runtime_error("Could not load test font");
        }

        void tearDown() override
        {
            font.reset();
            SDL_DestroyRenderer(renderer);
            SDL_DestroySurface(target);
            TTF_Quit();
            SDL_Quit();
        }
    };

    void expectIndependentAlpha(UnitTest::Scenario& scenario, SDL_Surface* surface, Uint8 backgroundAlpha)
    {
        UT_REQUIRE(scenario, surface);
        bool opaqueText = false;
        bool translucentEdge = false;
        for (int row = 0; row < surface->h; ++row)
        {
            for (int column = 0; column < surface->w; ++column)
            {
                Uint8 red, green, blue, alpha;
                UT_REQUIRE(scenario, SDL_ReadSurfacePixel(surface, column, row, &red, &green, &blue, &alpha));
                opaqueText = opaqueText || (red == 255 && green == 255 && blue == 255 && alpha == 255);
                translucentEdge = translucentEdge || (alpha > backgroundAlpha && alpha < 255);
                if (backgroundAlpha == 0 && alpha > 0)
                    UT_EXPECT(scenario, red == 255 && green == 255 && blue == 255);
            }
        }
        UT_EXPECT(scenario, opaqueText);
        UT_EXPECT(scenario, translucentEdge);
        Uint8 red, green, blue, alpha;
        UT_REQUIRE(scenario, SDL_ReadSurfacePixel(surface, 0, 0, &red, &green, &blue, &alpha));
        UT_EXPECT(scenario, red == 60 && green == 60 && blue == 60 && alpha == backgroundAlpha);
    }
}

int main()
{
    UnitTest::Environment tests("Button transparency");
    tests.addScenario<ButtonFixture>("Button text stays opaque over a translucent background",
        [](UnitTest::Scenario& scenario, ButtonFixture& fixture) {
            ButtonRenderer button(fixture.renderer, size2D(200, 30), SDL_Color{60, 60, 60, 150},
                "Button", fixture.font, SDL_Color{255, 255, 255, 255});
            expectIndependentAlpha(scenario, button.getSurface(), 150);
            button.setColor(SDL_Color{60, 60, 60, 0});
            expectIndependentAlpha(scenario, button.getSurface(), 0);
        });
    tests.addScenario<ButtonFixture>("Dropdown composition preserves button text opacity",
        [](UnitTest::Scenario& scenario, ButtonFixture& fixture) {
            ButtonRenderer button(fixture.renderer, size2D(200, 30), SDL_Color{60, 60, 60, 150},
                "Button", fixture.font, SDL_Color{255, 255, 255, 255});
            DropDownMenuRenderer menu(fixture.renderer, size2D(200, 30), size2D(200, 30), SDL_Color{60, 60, 60, 150});
            menu.addItem(&button, point2D(0, 0));
            menu.update();
            expectIndependentAlpha(scenario, menu.getSurface(), 150);
        });
    return tests.run();
}