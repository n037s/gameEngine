#include "ContextualObjectMenu.h"
#include "ButtonCallbackRegistery.h"
#include "World.h"
#include "Viewer.h"
#include "Sprite.h"
#include "AssetsManager.h"
#include "Camera.h"
#include "SelectionBoundObject.h"
#include "UnitTestEnvironment.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

class MenuTarget : public Object
{
public:
    MenuTarget() : Object(point2D(0, 0), size2D(400, 300)) {}
    std::string getTypeName() const override { return "MenuTarget"; }
    bool createRenderer(SDL_Renderer*) override { return false; }
    bool leftClick(point2D) override { ++clickCount; return true; }
    bool rightClick(point2D pos) override { ++rightClickCount; lastRightClickPos = pos; return true; }
    bool releaseRightClick(point2D pos) override { ++rightReleaseCount; lastRightClickPos = pos; return true; }
    int clickCount{0};
    int rightClickCount{0};
    int rightReleaseCount{0};
    point2D lastRightClickPos{0, 0};
};

struct MenuFixture : UnitTest::Fixture
{
    fs::path directory{"Assets/zzAssetCategoryTests"};
    bool ownsDirectory{false};
    World world;
    std::shared_ptr<MenuTarget> target;
    std::shared_ptr<Sprite> sprite;
    CommandProxy commandProxy;

    void setUp() override;
    void tearDown() override;
};

void MenuFixture::setUp()
{
    ownsDirectory = fs::create_directory(directory);
    if (!ownsDirectory)
        throw std::runtime_error("Asset fixture directory already exists: " + directory.string());
    const unsigned char gif[] = {
        'G', 'I', 'F', '8', '9', 'a', 1, 0, 1, 0, 0x80, 0, 0,
        0, 0, 0, 255, 255, 255, 0x2c, 0, 0, 0, 0, 1, 0, 1, 0, 0,
        2, 2, 0x44, 1, 0, 0x3b
    };
    {
        std::ofstream output(directory / "animation.GIF", std::ios::binary);
        output.write(reinterpret_cast<const char*>(gif), sizeof(gif));
        if (!output)
            throw std::runtime_error("Could not write animation fixture");
        std::ofstream(directory / "broken.png") << "not an image";
        std::ofstream(directory / "unsupported.txt") << "not an asset";
    }
    fs::copy_file("Assets/arial.ttf", directory / "font.TTF");
    SDL_Surface* fixtureSurface = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_RGBA8888);
    if (!fixtureSurface)
        throw std::runtime_error(SDL_GetError());
    const bool savedFixture = SDL_SaveBMP(fixtureSurface, (directory / "still.BMP").string().c_str());
    SDL_DestroySurface(fixtureSurface);
    if (!savedFixture)
        throw std::runtime_error(SDL_GetError());

    world.createCamera(point2D(0, 0), size2D(640, 480));
    auto camera = Camera::getInstance();
    camera->setViewMode(Camera::ViewMode::TopDown);
    camera->moveZ(1.0f - camera->getZ());
    world.setRedirectClickToObject(false);
    target = std::make_shared<MenuTarget>();
    world.addOverlayObject(target);
    sprite = std::make_shared<Sprite>(point2D(50, 60));
    Viewer::getInstance()->setCommandProxy(&commandProxy);
}

void MenuFixture::tearDown()
{
    Viewer::getInstance()->setWorld(nullptr);
    Viewer::getInstance()->setCommandProxy(nullptr);
    auto camera = Camera::getInstance();
    camera->setViewMode(Camera::ViewMode::TopDown);
    camera->moveZ(1.0f - camera->getZ());
    camera->setPos(point2D(0, 0));
    if (ownsDirectory)
        fs::remove_all(directory);
}

struct RenderingFixture : MenuFixture
{
    struct Runtime
    {
        bool sdlInitialized{false};
        bool ttfInitialized{false};

        void initialize()
        {
            if (!sdlInitialized)
                sdlInitialized = SDL_Init(0);
            if (!sdlInitialized)
                throw std::runtime_error(SDL_GetError());
            if (!ttfInitialized)
                ttfInitialized = TTF_Init();
            if (!ttfInitialized)
                throw std::runtime_error(SDL_GetError());
        }

        ~Runtime()
        {
            if (ttfInitialized)
                TTF_Quit();
            if (sdlInitialized)
                SDL_Quit();
        }
    };

    inline static Runtime* runtime{nullptr};
    SDL_Surface* renderTarget{nullptr};
    SDL_Renderer* renderer{nullptr};

    void setUp() override
    {
        MenuFixture::setUp();
        runtime->initialize();
        renderTarget = SDL_CreateSurface(640, 480, SDL_PIXELFORMAT_RGBA8888);
        renderer = renderTarget ? SDL_CreateSoftwareRenderer(renderTarget) : nullptr;
        if (!renderer)
            throw std::runtime_error(SDL_GetError());
    }

    void tearDown() override
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroySurface(renderTarget);
        MenuFixture::tearDown();
    }
};

void spriteAssetValidation(UnitTest::Scenario& scenario, MenuFixture& fixture)
{
    const auto& sprite = fixture.sprite;
    const auto center = sprite->getPos();
    UT_EXPECT(scenario, sprite->setAsset("boule")
        && sprite->serialize().getMember<std::string>("assetID") == "boule"
        && sprite->getPos().x == center.x && sprite->getPos().y == center.y
        && !sprite->setAsset("arial") && !sprite->setAsset("missing-asset")
        && sprite->serialize().getMember<std::string>("assetID") == "boule");
}

void rightClickForwarding(UnitTest::Scenario& scenario, MenuFixture& fixture)
{
    auto& world = fixture.world;
    const auto& target = fixture.target;
    auto viewer = Viewer::getInstance();
    viewer->setWorld(&world);
    SDL_Event event{};
    event.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
    event.button.button = SDL_BUTTON_RIGHT;
    event.button.x = 20;
    event.button.y = 25;
    viewer->manageUserInput(event);
    UT_EXPECT(scenario, target->isRightClicked() && target->rightClickCount == 1
        && target->rightReleaseCount == 0);
    event.type = SDL_EVENT_MOUSE_BUTTON_UP;
    viewer->manageUserInput(event);
    UT_EXPECT(scenario, !target->isRightClicked() && target->rightReleaseCount == 1
        && target->lastRightClickPos.x == 20 && target->lastRightClickPos.y == 25);
}

void selectionDragging(UnitTest::Scenario& scenario, MenuFixture& fixture,
    Camera::ViewMode mode, bool steady, float zoom)
{
    auto camera = Camera::getInstance();
    camera->setViewMode(mode);
    camera->moveZ(zoom - camera->getZ());
    camera->setPos(point2D(35, -20));
    auto dragTarget = std::make_shared<MenuTarget>();
    dragTarget->setSteady(steady);
    dragTarget->setPos(point2D(80, 60));
    auto selection = std::make_shared<SelectionBoundsObject>(dragTarget);
    fixture.world.addOverlayObject(selection);
    const point2D initialPosition = dragTarget->getPos();
    const point2D grabPosition = camera->worldToWindowPoint(initialPosition);
    fixture.world.mouseMove(grabPosition);
    fixture.world.leftClick(grabPosition);
    UT_EXPECT(scenario, selection->isLeftClicked());
    for (const point2D mousePosition : {grabPosition + point2D(30, 20),
        point2D(900, 700), point2D(1800, 1400), point2D(2700, 2100)})
    {
        fixture.world.mouseMove(mousePosition);
        const point2D actualDelta = camera->worldToWindowPoint(dragTarget->getPos())
            - camera->worldToWindowPoint(initialPosition);
        const point2D expectedDelta = mousePosition - grabPosition;
        const bool followsMouse = std::abs(actualDelta.x - expectedDelta.x) < 0.01f
            && std::abs(actualDelta.y - expectedDelta.y) < 0.01f;
        if (!followsMouse)
            std::cerr << "Selection drag drift: steady=" << steady << ", zoom=" << zoom
                << ", isometric=" << (mode == Camera::ViewMode::Isometric)
                << ", mouse=" << mousePosition.toString() << std::endl;
        UT_EXPECT(scenario, followsMouse);
    }
    fixture.world.releaseLeftClick(point2D(3500, 2800));
    UT_EXPECT(scenario, !selection->isLeftClicked());
    const point2D releasedPosition = dragTarget->getPos();
    fixture.world.mouseMove(point2D(4000, 3200));
    fixture.world.mouseMove(grabPosition);
    UT_EXPECT(scenario, dragTarget->getPos().x == releasedPosition.x
        && dragTarget->getPos().y == releasedPosition.y);
    selection->setTarget(nullptr);
}

void toggleSteady(UnitTest::Scenario& scenario, MenuFixture& fixture, bool initialSteady)
{
    auto& world = fixture.world;
    const auto& target = fixture.target;
    target->setSteady(initialSteady);
    auto menu = std::make_shared<ContextualObjectMenu>(target, point2D(10, 10), world);
    menu->display(nullptr);
    const auto uid = menu->getUID();
    const auto callbackID = "ContextualObjectMenu_" + uid;
    std::weak_ptr<ContextualObjectMenu> weakMenu = menu;
    world.addOverlayObject(menu);
    menu.reset();

    world.Hovering(point2D(20, 20));
    UT_EXPECT(scenario, world.getHoveredObject() == weakMenu.lock());
    world.leftClick(point2D(20, 20));
    world.releaseLeftClick(point2D(20, 20));
    UT_EXPECT(scenario, target->isSteady() != initialSteady && target->clickCount == 0
        && !world.getObjectByUID(uid) && !world.getHoveredObject()
        && weakMenu.expired()
        && !ButtonCallbackRegistery::instance()->getCallback(callbackID));
}

void dismissOutside(UnitTest::Scenario& scenario, MenuFixture& fixture,
    bool rightClickOutside, bool enterBeforeLeaving)
{
    auto& world = fixture.world;
    const auto& target = fixture.target;
    const bool steady = target->isSteady();
    auto menu = std::make_shared<ContextualObjectMenu>(target, point2D(10, 10), world);
    menu->display(nullptr);
    const auto uid = menu->getUID();
    std::weak_ptr<ContextualObjectMenu> weakMenu = menu;
    world.addOverlayObject(menu);
    menu.reset();
    if (enterBeforeLeaving)
        world.Hovering(point2D(20, 20));
    world.Hovering(point2D(500, 400));
    UT_EXPECT(scenario, !weakMenu.expired() && world.getObjectByUID(uid)
        && !weakMenu.lock()->isHidden() && !weakMenu.lock()->isHovered()
        && target->isSteady() == steady);
    if (enterBeforeLeaving)
    {
        world.Hovering(point2D(20, 20));
        UT_EXPECT(scenario, world.getHoveredObject() == weakMenu.lock());
    }
    const auto clicks = target->clickCount;
    const auto rightClicks = target->rightClickCount;
    if (rightClickOutside)
    {
        world.rightClick(point2D(300, 200));
        world.releaseRightClick(point2D(300, 200));
    }
    else
    {
        world.leftClick(point2D(300, 200));
        world.releaseLeftClick(point2D(300, 200));
    }
    UT_EXPECT(scenario, weakMenu.expired() && !world.getObjectByUID(uid)
        && !ButtonCallbackRegistery::instance()->getCallback("ContextualObjectMenu_" + uid)
        && target->clickCount == clicks + (rightClickOutside ? 0 : 1)
        && target->rightClickCount == rightClicks + (rightClickOutside ? 1 : 0)
        && target->isSteady() == steady);
}

void assetFolderNavigation(UnitTest::Scenario& scenario, MenuFixture& fixture)
{
    auto& world = fixture.world;
    const auto& sprite = fixture.sprite;
    auto menu = std::make_shared<ContextualObjectMenu>(sprite, point2D(10, 10), world);
    menu->display(nullptr);
    world.addOverlayObject(menu);
    auto assetMenu = menu->getChildItem("set asset", nullptr);
    UT_REQUIRE(scenario, assetMenu && assetMenu->items.size() == 4);
    UT_EXPECT(scenario, assetMenu->items[0]->label == "craftpix"
        && assetMenu->items[1]->label == "FarmRPG"
        && assetMenu->items[2]->label == "zzAssetCategoryTests"
        && assetMenu->items[3]->label == "boule.bmp");

    world.Hovering(point2D(20, 20));
    UT_EXPECT(scenario, assetMenu->isOpen && menu->getShape().size.w == 360);
    world.Hovering(point2D(200, 20));
    UT_EXPECT(scenario, assetMenu->items[0]->isOpen && menu->getShape().size.w == 540);
    world.Hovering(point2D(20, 100));
    UT_EXPECT(scenario, !menu->isHidden() && !menu->isHovered()
        && world.getObjectByUID(menu->getUID()) == menu);
    world.Hovering(point2D(600, 450));
    UT_EXPECT(scenario, !menu->isHidden() && world.getObjectByUID(menu->getUID()) == menu
        && assetMenu->items[0]->isOpen);
    world.Hovering(point2D(200, 50));
    UT_EXPECT(scenario, !assetMenu->items[0]->isOpen && assetMenu->items[1]->isOpen);
    world.Hovering(point2D(200, 110));
    UT_EXPECT(scenario, !assetMenu->items[1]->isOpen && world.getHoveredObject() == menu);

    const auto uid = menu->getUID();
    const auto callbackID = "ContextualObjectMenu_" + uid + "_asset_boule";
    std::weak_ptr<ContextualObjectMenu> weakMenu = menu;
    menu.reset();
    world.leftClick(point2D(200, 110));
    world.releaseLeftClick(point2D(200, 110));
    UT_EXPECT(scenario, sprite->serialize().getMember<std::string>("assetID") == "boule"
        && !world.getObjectByUID(uid) && weakMenu.expired()
        && !ButtonCallbackRegistery::instance()->getCallback(callbackID));
}

void assetCategories(UnitTest::Scenario& scenario, MenuFixture&)
{
    const auto assets = AssetsManager::getInstance();
    const auto images = assets->getAssets();
    const auto animations = assets->getAnimatedAssets();
    const auto fonts = assets->getFont();
    const auto contains = [](const std::list<std::string>& ids, const std::string& id) {
        return std::find(ids.begin(), ids.end(), id) != ids.end();
    };
    UT_EXPECT(scenario, std::is_sorted(images.begin(), images.end()));
    UT_EXPECT(scenario, std::is_sorted(animations.begin(), animations.end()));
    UT_EXPECT(scenario, std::is_sorted(fonts.begin(), fonts.end()));
    UT_EXPECT(scenario, contains(images, "boule"));
    UT_EXPECT(scenario, contains(images, "zzAssetCategoryTests/still"));
    UT_EXPECT(scenario, contains(animations, "zzAssetCategoryTests/animation"));
    UT_EXPECT(scenario, contains(fonts, "arial"));
    UT_EXPECT(scenario, contains(fonts, "zzAssetCategoryTests/font"));
    UT_EXPECT(scenario, !contains(images, "arial"));
    UT_EXPECT(scenario, !contains(images, "zzAssetCategoryTests/animation"));
    UT_EXPECT(scenario, !contains(images, "zzAssetCategoryTests/broken"));
    UT_EXPECT(scenario, !contains(images, "zzAssetCategoryTests/unsupported"));
}

void filterSpriteAssets(UnitTest::Scenario& scenario, MenuFixture& fixture)
{
    auto& world = fixture.world;
    const auto& sprite = fixture.sprite;
    ContextualObjectMenu filteredMenu(sprite, point2D(10, 10), world);
    const auto root = filteredMenu.getChildItem("set asset", nullptr);
    UT_REQUIRE(scenario, root);
    const auto fixtureFolder = filteredMenu.getChildItem("zzAssetCategoryTests", root);
    UT_REQUIRE(scenario, fixtureFolder);
    UT_EXPECT(scenario, !filteredMenu.getChildItem("arial.ttf", root));
    UT_EXPECT(scenario, filteredMenu.getChildItem("still.BMP", fixtureFolder));
    UT_EXPECT(scenario, !filteredMenu.getChildItem("animation.GIF", fixtureFolder));
    UT_EXPECT(scenario, !filteredMenu.getChildItem("font.TTF", fixtureFolder));
    UT_EXPECT(scenario, !filteredMenu.getChildItem("broken.png", fixtureFolder));
    UT_EXPECT(scenario, !filteredMenu.getChildItem("unsupported.txt", fixtureFolder));
}

void selectNestedAsset(UnitTest::Scenario& scenario, MenuFixture& fixture)
{
    auto& world = fixture.world;
    const auto& sprite = fixture.sprite;
    const auto assets = AssetsManager::getInstance();
    const auto images = assets->getAssets();
    bool testedAsset = false;
    for (const auto& assetID : images)
    {
        if (assetID.find('/') == std::string::npos)
            continue;
        auto image = dynamic_cast<Image*>(assets->getAsset(assetID));
        if (!image || image->getAssetID() != assetID || !image->getSurface())
            continue;

        auto nestedMenu = std::make_shared<ContextualObjectMenu>(sprite, point2D(10, 10), world);
        nestedMenu->display(nullptr);
        world.addOverlayObject(nestedMenu);
        world.Hovering(point2D(20, 20));
        auto node = nestedMenu->getChildItem("set asset", nullptr);
        UT_REQUIRE(scenario, node);
        const auto relativePath = fs::path(assets->getPath(assetID)).lexically_relative(assets->getAssetsPath());
        float columnX = 190;
        float columnY = 10;
        point2D leafPosition;
        for (const auto& component : relativePath)
        {
            const auto child = nestedMenu->getChildItem(component.string(), node);
            UT_REQUIRE(scenario, child);
            const auto row = std::distance(node->items.begin(), std::find(node->items.begin(), node->items.end(), child));
            leafPosition = point2D(columnX + 10, columnY + static_cast<float>(row) * 30 + 10);
            world.Hovering(leafPosition);
            UT_EXPECT(scenario, world.getHoveredObject() == nestedMenu);
            columnY += static_cast<float>(row) * 30;
            columnX += 180;
            node = child;
        }
        world.leftClick(leafPosition);
        world.releaseLeftClick(leafPosition);
        UT_EXPECT(scenario, nestedMenu->isHidden()
            && sprite->serialize().getMember<std::string>("assetID") == assetID);
        testedAsset = true;
        break;
    }
    UT_EXPECT(scenario, testedAsset);
}

void scrollAssetMenu(UnitTest::Scenario& scenario, RenderingFixture& fixture)
{
    auto renderer = fixture.renderer;
    bool selected = false;
    const std::string callbackID = "AssetMenuScrollTest";
    struct CallbackCleanup
    {
        std::string id;
        ~CallbackCleanup() { ButtonCallbackRegistery::instance()->unregisterCallback(id); }
    } cleanup{callbackID};
    ButtonCallbackRegistery::instance()->registerCallback(callbackID,
        [&selected](point2D, Object*) { selected = true; return true; });
    DropDownMenu scrollMenu(rect2D(10, 10, 180, 30), size2D(180, 30),
        Color(180, 180, 180, 255), std::make_shared<Font>("arial", 20.0f));
    scrollMenu.setCascading(true);
    for (int index = 0; index < 22; ++index)
        scrollMenu.addItem("folder/entry" + std::to_string(index), callbackID);
    UT_REQUIRE(scenario, scrollMenu.createRenderer(renderer));
    scrollMenu.hover(point2D(20, 20));
    const auto folder = scrollMenu.getChildItem("folder", nullptr);
    UT_REQUIRE(scenario, folder);
    const auto surface = scrollMenu.getRenderer()->getSurface();
    Uint8 red = 0, green = 0, blue = 0, alpha = 0;
    UT_EXPECT(scenario, surface && surface->w == 360 && surface->h == 480
        && SDL_ReadSurfacePixel(surface, 185, 5, &red, &green, &blue, &alpha)
        && alpha == 255);
    scrollMenu.hover(point2D(200, 20));
    for (int index = 0; index < 30; ++index)
        scrollMenu.scroll(-1);
    UT_EXPECT(scenario, folder->firstVisibleItem == 6);
    scrollMenu.leftClick(point2D(200, 465));
    scrollMenu.releaseLeftClick(point2D(200, 465));
    UT_EXPECT(scenario, selected);
    scrollMenu.scroll(1);
    UT_EXPECT(scenario, folder->firstVisibleItem == 5);
}

void replaceRenderedSpriteAsset(UnitTest::Scenario& scenario, RenderingFixture& fixture)
{
    auto renderer = fixture.renderer;
    const auto assets = AssetsManager::getInstance();
    const std::string assetID = "zzAssetCategoryTests/still";
    auto renderedSprite = std::make_shared<Sprite>(point2D(50, 60), "boule");
    UT_REQUIRE(scenario, renderedSprite->createRenderer(renderer));
    const auto oldTexture = renderedSprite->getRenderer()->getTexture();
    const auto oldCenter = renderedSprite->getPos();
    UT_EXPECT(scenario, renderedSprite->setAsset(assetID)
        && renderedSprite->getRenderer()->getTexture() != oldTexture
        && renderedSprite->getRenderer()->getSurface() == static_cast<Image*>(assets->getAsset(assetID))->getSurface()
        && renderedSprite->getPos().x == oldCenter.x && renderedSprite->getPos().y == oldCenter.y);
}

void fitWideMenu(UnitTest::Scenario& scenario, RenderingFixture& fixture)
{
    auto renderer = fixture.renderer;
    DropDownMenu wideMenu(rect2D(500, 450, 180, 30), size2D(180, 30),
        Color(180, 180, 180, 255), std::make_shared<Font>("arial", 20.0f));
    wideMenu.setCascading(true);
    wideMenu.addItem("folder/A very long asset filename that must fit.bmp");
    UT_REQUIRE(scenario, wideMenu.createRenderer(renderer));
    const auto rootShape = wideMenu.getShape();
    wideMenu.hover(rootShape.position + size2D(10, 10));
    const auto shape = wideMenu.getShape();
    UT_EXPECT(scenario, shape.position.x >= 0 && shape.position.y >= 0
        && shape.position.x + shape.size.w <= 640
        && shape.position.y + shape.size.h <= 480
        && shape.size.w > 360);
}

int main()
{
    RenderingFixture::Runtime runtime;
    RenderingFixture::runtime = &runtime;
    UnitTest::Environment tests("ContextualObjectMenu");
    tests.addScenario<MenuFixture>("Sprite asset validation preserves the center", spriteAssetValidation);
    tests.addScenario<MenuFixture>("Viewer forwards right click and release", rightClickForwarding);
    for (const auto mode : {Camera::ViewMode::TopDown, Camera::ViewMode::Isometric})
    for (const bool steady : {false, true})
    for (const float zoom : {0.5f, 1.0f, 2.0f})
    {
        const auto name = std::string("Selection drag / ")
            + (mode == Camera::ViewMode::TopDown ? "top-down" : "isometric")
            + " / steady=" + (steady ? "true" : "false") + " / zoom=" + std::to_string(zoom);
        tests.addScenario<MenuFixture>(name, [=](UnitTest::Scenario& scenario, MenuFixture& fixture) {
            selectionDragging(scenario, fixture, mode, steady, zoom);
        });
    }
    for (const bool initialSteady : {false, true})
        tests.addScenario<MenuFixture>(std::string("Toggle steady / initial=") + (initialSteady ? "true" : "false"),
            [=](UnitTest::Scenario& scenario, MenuFixture& fixture) { toggleSteady(scenario, fixture, initialSteady); });
    for (const bool rightClickOutside : {false, true})
    for (const bool enterBeforeLeaving : {false, true})
    {
        const auto name = std::string("Dismiss outside / ") + (rightClickOutside ? "right" : "left")
            + " / entered=" + (enterBeforeLeaving ? "true" : "false");
        tests.addScenario<MenuFixture>(name, [=](UnitTest::Scenario& scenario, MenuFixture& fixture) {
            dismissOutside(scenario, fixture, rightClickOutside, enterBeforeLeaving);
        });
    }
    tests.addScenario<MenuFixture>("Asset folders sort, open on hover, and select", assetFolderNavigation);
    tests.addScenario<MenuFixture>("Asset categories are sorted and disjoint", assetCategories);
    tests.addScenario<MenuFixture>("Sprite menus exclude unsupported assets", filterSpriteAssets);
    tests.addScenario<MenuFixture>("Nested asset selection updates the sprite", selectNestedAsset);
    tests.addScenario<RenderingFixture>("Asset menu scrolls and selects visible entries", scrollAssetMenu);
    tests.addScenario<RenderingFixture>("Asset replacement refreshes the rendered texture", replaceRenderedSpriteAsset);
    tests.addScenario<RenderingFixture>("Wide menus remain inside the viewport", fitWideMenu);
    return tests.run();
}
