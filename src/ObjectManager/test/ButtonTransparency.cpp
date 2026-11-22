#include "ButtonRenderer.h"
#include "DropDownMenuRenderer.h"
#include "IntSelector.h"
#include "ObjectManager.h"
#include "World.h"
#include "UnitTestEnvironment.h"

#include <limits>
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

    void selectorInteraction(UnitTest::Scenario& scenario, ButtonFixture& fixture)
    {
        IntSelector selector(rect2D(10, 20, 150, 30), 0, Color(60, 60, 60, 150), fixture.font,
            Color(255, 255, 255, 255));
        UT_EXPECT(scenario, selector.getValue() == 0);
        UT_REQUIRE(scenario, selector.createRenderer(fixture.renderer));
        auto initialSurface = SDL_DuplicateSurface(selector.getRenderer()->getSurface());
        UT_REQUIRE(scenario, initialSurface);
        bool regionHasText[3] = {false, false, false};
        for (int row = 0; row < 30; ++row)
        for (int column = 0; column < 150; ++column)
        {
            Uint8 red, green, blue, alpha;
            SDL_ReadSurfacePixel(initialSurface, column, row, &red, &green, &blue, &alpha);
            const int region = column < 30 ? 0 : (column < 120 ? 1 : 2);
            regionHasText[region] = regionHasText[region] || alpha > 150;
        }
        UT_EXPECT(scenario, regionHasText[0] && regionHasText[1] && regionHasText[2]);
        UT_EXPECT(scenario, selector.onLeftClick(point2D(150, 35))
            && selector.offLeftClick(point2D(150, 35)) && selector.getValue() == 1);
        bool valueChangedOnSurface = false;
        auto updatedSurface = selector.getRenderer()->getSurface();
        UT_REQUIRE(scenario, updatedSurface);
        for (int row = 0; row < 30; ++row)
        for (int column = 30; column < 120; ++column)
        {
            Uint8 initialRed, initialGreen, initialBlue, initialAlpha;
            Uint8 updatedRed, updatedGreen, updatedBlue, updatedAlpha;
            SDL_ReadSurfacePixel(initialSurface, column, row, &initialRed, &initialGreen, &initialBlue, &initialAlpha);
            SDL_ReadSurfacePixel(updatedSurface, column, row, &updatedRed, &updatedGreen, &updatedBlue, &updatedAlpha);
            valueChangedOnSurface = valueChangedOnSurface || initialRed != updatedRed || initialGreen != updatedGreen
                || initialBlue != updatedBlue || initialAlpha != updatedAlpha;
        }
        SDL_DestroySurface(initialSurface);
        UT_EXPECT(scenario, valueChangedOnSurface);
        UT_EXPECT(scenario, selector.onLeftClick(point2D(15, 35))
            && selector.offLeftClick(point2D(15, 35)) && selector.getValue() == 0);
        UT_EXPECT(scenario, selector.onLeftClick(point2D(15, 35))
            && selector.offLeftClick(point2D(15, 35)) && selector.getValue() == -1);
        UT_EXPECT(scenario, selector.onLeftClick(point2D(80, 35))
            && selector.offLeftClick(point2D(80, 35)) && selector.wantsTextInput() && selector.getValue() == -1);
        selector.handleTextInput("-123");
        selector.handleTextKey(SDLK_RETURN, SDL_KMOD_NONE);
        UT_EXPECT(scenario, !selector.wantsTextInput() && selector.getValue() == -123);
        selector.setValue(-1);
        UT_EXPECT(scenario, selector.onLeftClick(point2D(150, 35))
            && !selector.offLeftClick(point2D(15, 35)) && selector.getValue() == -1);
        UT_EXPECT(scenario, selector.onLeftClick(point2D(150, 35))
            && !selector.offLeftClick(point2D(200, 35)) && selector.getValue() == -1);
        selector.setValue(std::numeric_limits<int>::max());
        UT_EXPECT(scenario, selector.onLeftClick(point2D(150, 35))
            && !selector.offLeftClick(point2D(150, 35)) && selector.getValue() == std::numeric_limits<int>::max());
        selector.setValue(std::numeric_limits<int>::min());
        UT_EXPECT(scenario, selector.onLeftClick(point2D(15, 35))
            && !selector.offLeftClick(point2D(15, 35)) && selector.getValue() == std::numeric_limits<int>::min());
        auto minimumSurface = SDL_DuplicateSurface(selector.getRenderer()->getSurface());
        UT_REQUIRE(scenario, minimumSurface);
        selector.setValue(std::numeric_limits<int>::min() + 1);
        bool lastDigitChanged = false;
        updatedSurface = selector.getRenderer()->getSurface();
        for (int row = 0; row < 30; ++row)
        for (int column = 30; column < 120; ++column)
        {
            Uint8 initialRed, initialGreen, initialBlue, initialAlpha;
            Uint8 updatedRed, updatedGreen, updatedBlue, updatedAlpha;
            SDL_ReadSurfacePixel(minimumSurface, column, row, &initialRed, &initialGreen, &initialBlue, &initialAlpha);
            SDL_ReadSurfacePixel(updatedSurface, column, row, &updatedRed, &updatedGreen, &updatedBlue, &updatedAlpha);
            lastDigitChanged = lastDigitChanged || initialRed != updatedRed || initialGreen != updatedGreen
                || initialBlue != updatedBlue || initialAlpha != updatedAlpha;
        }
        SDL_DestroySurface(minimumSurface);
        UT_EXPECT(scenario, lastDigitChanged);
    }

    void selectorWorldInput(UnitTest::Scenario& scenario, ButtonFixture& fixture)
    {
        World world;
        world.createCamera(point2D(0, 0), size2D(200, 100));
        auto selector = std::make_shared<IntSelector>(rect2D(10, 20, 150, 30), 0,
            Color(60, 60, 60, 150), fixture.font, Color(255, 255, 255, 255));
        UT_REQUIRE(scenario, selector->createRenderer(fixture.renderer));
        world.addOverlayObject(selector);
        world.leftClick(point2D(150, 35));
        world.releaseLeftClick(point2D(150, 35));
        UT_EXPECT(scenario, selector->getValue() == 1);
        world.leftClick(point2D(80, 35));
        UT_EXPECT(scenario, !world.isLeftClicked());
        world.releaseLeftClick(point2D(80, 35));
        UT_EXPECT(scenario, selector->getValue() == 1);
    }

    void selectorEditing(UnitTest::Scenario& scenario, ButtonFixture& fixture)
    {
        World world;
        world.createCamera(point2D(0, 0), size2D(200, 100));
        auto selector = std::make_shared<IntSelector>(rect2D(10, 20, 150, 30), 7,
            Color(60, 60, 60, 150), fixture.font, Color(255, 255, 255, 255));
        UT_REQUIRE(scenario, selector->createRenderer(fixture.renderer));
        world.addOverlayObject(selector);
        const auto startEditing = [&world] {
            world.leftClick(point2D(80, 35));
            world.releaseLeftClick(point2D(80, 35));
        };
        startEditing();
        UT_REQUIRE(scenario, world.hasTextInputFocus());
        world.mouseMove(point2D(190, 90));
        UT_EXPECT(scenario, world.hasTextInputFocus());
        world.textInput("-123");
        UT_EXPECT(scenario, selector->getValue() == 7);
        UT_EXPECT(scenario, world.textKey(SDLK_RETURN, SDL_KMOD_NONE)
            && !world.hasTextInputFocus() && selector->getValue() == -123);

        startEditing();
        world.textInput("2147483648");
        world.textKey(SDLK_RETURN, SDL_KMOD_NONE);
        UT_EXPECT(scenario, selector->getValue() == -123);
        startEditing();
        world.textKey(SDLK_BACKSPACE, SDL_KMOD_NONE);
        world.textKey(SDLK_RETURN, SDL_KMOD_NONE);
        UT_EXPECT(scenario, selector->getValue() == -123);
        startEditing();
        world.textInput("123");
        world.textKey(SDLK_LEFT, SDL_KMOD_NONE);
        world.textInput("4");
        world.textInput("x");
        world.textKey(SDLK_RETURN, SDL_KMOD_NONE);
        UT_EXPECT(scenario, selector->getValue() == 1243);
        startEditing();
        world.textInput("99");
        world.textKey(SDLK_ESCAPE, SDL_KMOD_NONE);
        UT_EXPECT(scenario, !world.hasTextInputFocus() && selector->getValue() == 1243);
        startEditing();
        world.textKey(SDLK_A, SDL_KMOD_CTRL);
        world.textInput("42");
        world.leftClick(point2D(150, 35));
        world.releaseLeftClick(point2D(150, 35));
        UT_EXPECT(scenario, !world.hasTextInputFocus() && selector->getValue() == 43);
        startEditing();
        world.textInput("-2147483648");
        world.textKey(SDLK_RETURN, SDL_KMOD_NONE);
        UT_EXPECT(scenario, selector->getValue() == std::numeric_limits<int>::min());
        startEditing();
        world.textInput("51");
        world.removeOverlayObject(selector);
        UT_EXPECT(scenario, !world.hasTextInputFocus() && selector->getValue() == 51);
        world.addOverlayObject(selector);
        startEditing();
        world.textInput("73");
        selector->setIsHidden(true);
        world.update();
        UT_EXPECT(scenario, !world.hasTextInputFocus() && selector->getValue() == 73);
    }

    void selectorSerialization(UnitTest::Scenario& scenario, ButtonFixture& fixture)
    {
        IntSelector original(rect2D(10, 20, 150, 30), -42, Color(60, 60, 60, 150), fixture.font,
            Color(255, 255, 255, 255));
        auto members = original.serialize();
        auto object = ObjectManager::createObject("IntSelector", members);
        auto selector = std::dynamic_pointer_cast<IntSelector>(object);
        UT_REQUIRE(scenario, selector);
        UT_EXPECT(scenario, selector->getValue() == -42 && selector->getPos().x == 10
            && selector->getPos().y == 20 && selector->getShape().size.w == 150);
        UT_EXPECT(scenario, selector->createRenderer(fixture.renderer));
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
    tests.addScenario<ButtonFixture>("Integer selector updates and limits values", selectorInteraction);
    tests.addScenario<ButtonFixture>("Integer selector handles world input", selectorWorldInput);
    tests.addScenario<ButtonFixture>("Integer selector edits, validates, and manages focus", selectorEditing);
    tests.addScenario<ButtonFixture>("Integer selector restores through the object factory", selectorSerialization);
    return tests.run();
}