#include "ContextualObjectMenu.h"
#include "ButtonCallbackRegistery.h"
#include "World.h"
#include "Viewer.h"
#include "Sprite.h"
#include "AssetsManager.h"
#include "Camera.h"
#include "SelectionBoundObject.h"

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

int main()
{
    struct AssetFixtures
    {
        fs::path directory{"Assets/zzAssetCategoryTests"};
        ~AssetFixtures() { fs::remove_all(directory); }
    };
    if (!fs::create_directory("Assets/zzAssetCategoryTests"))
        return 1;
    AssetFixtures fixtures;
    const unsigned char gif[] = {
        'G', 'I', 'F', '8', '9', 'a', 1, 0, 1, 0, 0x80, 0, 0,
        0, 0, 0, 255, 255, 255, 0x2c, 0, 0, 0, 0, 1, 0, 1, 0, 0,
        2, 2, 0x44, 1, 0, 0x3b
    };
    {
        std::ofstream output(fixtures.directory / "animation.GIF", std::ios::binary);
        output.write(reinterpret_cast<const char*>(gif), sizeof(gif));
        std::ofstream(fixtures.directory / "broken.png") << "not an image";
        std::ofstream(fixtures.directory / "unsupported.txt") << "not an asset";
    }
    fs::copy_file("Assets/arial.ttf", fixtures.directory / "font.TTF");
    SDL_Surface* fixtureSurface = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_RGBA8888);
    if (!fixtureSurface)
        return 1;
    const bool savedFixture = SDL_SaveBMP(fixtureSurface, (fixtures.directory / "still.BMP").string().c_str());
    SDL_DestroySurface(fixtureSurface);
    if (!savedFixture)
        return 1;

    World world;
    world.createCamera(point2D(0, 0), size2D(640, 480));
    world.setRedirectClickToObject(false);
    auto target = std::make_shared<MenuTarget>();
    world.addOverlayObject(target);
    bool success = true;

    auto sprite = std::make_shared<Sprite>(point2D(50, 60));
    const auto center = sprite->getPos();
    success = sprite->setAsset("boule")
        && sprite->serialize().getMember<std::string>("assetID") == "boule"
        && sprite->getPos().x == center.x && sprite->getPos().y == center.y
        && !sprite->setAsset("arial") && !sprite->setAsset("missing-asset")
        && sprite->serialize().getMember<std::string>("assetID") == "boule" && success;

    auto viewer = Viewer::getInstance();
    viewer->setWorld(&world);
    SDL_Event event{};
    event.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
    event.button.button = SDL_BUTTON_RIGHT;
    event.button.x = 20;
    event.button.y = 25;
    viewer->manageUserInput(event);
    success = target->isRightClicked() && target->rightClickCount == 1
        && target->rightReleaseCount == 0 && success;
    event.type = SDL_EVENT_MOUSE_BUTTON_UP;
    viewer->manageUserInput(event);
    success = !target->isRightClicked() && target->rightReleaseCount == 1
        && target->lastRightClickPos.x == 20 && target->lastRightClickPos.y == 25 && success;
    viewer->setWorld(nullptr);

    CommandProxy commandProxy;
    viewer->setCommandProxy(&commandProxy);
    auto camera = Camera::getInstance();
    for (const auto mode : {Camera::ViewMode::TopDown, Camera::ViewMode::Isometric})
    for (const bool steady : {false, true})
    for (const float zoom : {0.5f, 1.0f, 2.0f})
    {
        camera->setViewMode(mode);
        camera->moveZ(zoom - camera->getZ());
        camera->setPos(point2D(35, -20));
        auto dragTarget = std::make_shared<MenuTarget>();
        dragTarget->setSteady(steady);
        dragTarget->setPos(point2D(80, 60));
        SelectionBoundsObject selection(dragTarget);
        const point2D grabPosition(420, 300);
        const point2D initialPosition = dragTarget->getPos();
        selection.onLeftClick(grabPosition);
        for (const bool outside : {false, true})
        {
            const point2D mousePosition = outside ? point2D(900, 700) : point2D(450, 320);
            if (outside)
                selection.leftFocus(mousePosition);
            else
                selection.hover(mousePosition);
            const point2D actualDelta = camera->worldToWindowPoint(dragTarget->getPos())
                - camera->worldToWindowPoint(initialPosition);
            const point2D expectedDelta = mousePosition - grabPosition;
            const bool followsMouse = std::abs(actualDelta.x - expectedDelta.x) < 0.01f
                && std::abs(actualDelta.y - expectedDelta.y) < 0.01f;
            if (!followsMouse)
                std::cerr << "Selection drag drift: steady=" << steady << ", zoom=" << zoom
                    << ", isometric=" << (mode == Camera::ViewMode::Isometric)
                    << ", outside=" << outside << std::endl;
            success = followsMouse && success;
        }
        selection.offLeftClick(point2D(900, 700));
        const point2D releasedPosition = dragTarget->getPos();
        selection.hover(grabPosition);
        success = dragTarget->getPos().x == releasedPosition.x
            && dragTarget->getPos().y == releasedPosition.y && success;
        selection.setTarget(nullptr);
    }
    camera->setViewMode(Camera::ViewMode::TopDown);
    camera->moveZ(1.0f - camera->getZ());
    camera->setPos(point2D(0, 0));
    viewer->setCommandProxy(nullptr);

    for (const bool initialSteady : {false, true})
    {
        target->setSteady(initialSteady);
        auto menu = std::make_shared<ContextualObjectMenu>(target, point2D(10, 10), world);
        menu->display(nullptr);
        const auto uid = menu->getUID();
        const auto callbackID = "ContextualObjectMenu_" + uid;
        std::weak_ptr<ContextualObjectMenu> weakMenu = menu;
        world.addOverlayObject(menu);
        menu.reset();

        world.Hovering(point2D(20, 20));
        success = world.getHoveredObject() == weakMenu.lock() && success;
        world.leftClick(point2D(20, 20));
        world.releaseLeftClick(point2D(20, 20));
        success = target->isSteady() != initialSteady && target->clickCount == 0
            && !world.getObjectByUID(uid) && !world.getHoveredObject()
            && weakMenu.expired()
            && !ButtonCallbackRegistery::instance()->getCallback(callbackID) && success;
    }

    for (const bool rightClickOutside : {false, true})
    for (const bool enterBeforeLeaving : {false, true})
    {
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
        success = !weakMenu.expired() && world.getObjectByUID(uid)
            && !weakMenu.lock()->isHidden() && !weakMenu.lock()->isHovered()
            && target->isSteady() == steady && success;
        if (enterBeforeLeaving)
        {
            world.Hovering(point2D(20, 20));
            success = world.getHoveredObject() == weakMenu.lock() && success;
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
        success = weakMenu.expired() && !world.getObjectByUID(uid)
            && !ButtonCallbackRegistery::instance()->getCallback("ContextualObjectMenu_" + uid)
            && target->clickCount == clicks + (rightClickOutside ? 0 : 1)
            && target->rightClickCount == rightClicks + (rightClickOutside ? 1 : 0)
            && target->isSteady() == steady && success;
    }

    auto menu = std::make_shared<ContextualObjectMenu>(sprite, point2D(10, 10), world);
    menu->display(nullptr);
    world.addOverlayObject(menu);
    auto assetMenu = menu->getChildItem("set asset", nullptr);
    if (assetMenu)
        for (const auto& item : assetMenu->items)
            std::cerr << "Asset menu row: " << item->label << std::endl;
    success = assetMenu && assetMenu->items.size() == 4 && success;
    if (assetMenu && assetMenu->items.size() == 4)
    {
        success = assetMenu->items[0]->label == "craftpix"
            && assetMenu->items[1]->label == "FarmRPG"
            && assetMenu->items[2]->label == "zzAssetCategoryTests"
            && assetMenu->items[3]->label == "boule.bmp" && success;

        world.Hovering(point2D(20, 20));
        success = assetMenu->isOpen && menu->getShape().size.w == 360 && success;
        world.Hovering(point2D(200, 20));
        success = assetMenu->items[0]->isOpen && menu->getShape().size.w == 540 && success;
        world.Hovering(point2D(20, 100));
        success = !menu->isHidden() && !menu->isHovered()
            && world.getObjectByUID(menu->getUID()) == menu && success;
        world.Hovering(point2D(600, 450));
        success = !menu->isHidden() && world.getObjectByUID(menu->getUID()) == menu
            && assetMenu->items[0]->isOpen && success;
        world.Hovering(point2D(200, 50));
        success = !assetMenu->items[0]->isOpen && assetMenu->items[1]->isOpen && success;
        world.Hovering(point2D(200, 110));
        success = !assetMenu->items[1]->isOpen && world.getHoveredObject() == menu && success;

        const auto uid = menu->getUID();
        const auto callbackID = "ContextualObjectMenu_" + uid + "_asset_boule";
        std::weak_ptr<ContextualObjectMenu> weakMenu = menu;
        menu.reset();
        world.leftClick(point2D(200, 110));
        world.releaseLeftClick(point2D(200, 110));
        success = sprite->serialize().getMember<std::string>("assetID") == "boule"
            && !world.getObjectByUID(uid) && weakMenu.expired()
            && !ButtonCallbackRegistery::instance()->getCallback(callbackID) && success;
    }
    if (menu)
        menu->close();

    const auto assets = AssetsManager::getInstance();
    const auto images = assets->getAssets();
    const auto animations = assets->getAnimatedAssets();
    const auto fonts = assets->getFont();
    const auto contains = [](const std::list<std::string>& ids, const std::string& id) {
        return std::find(ids.begin(), ids.end(), id) != ids.end();
    };
    const bool categoriesValid = std::is_sorted(images.begin(), images.end())
        && std::is_sorted(animations.begin(), animations.end())
        && std::is_sorted(fonts.begin(), fonts.end())
        && contains(images, "boule") && contains(images, "zzAssetCategoryTests/still")
        && contains(animations, "zzAssetCategoryTests/animation")
        && contains(fonts, "arial") && contains(fonts, "zzAssetCategoryTests/font")
        && !contains(images, "arial") && !contains(images, "zzAssetCategoryTests/animation")
        && !contains(images, "zzAssetCategoryTests/broken")
        && !contains(images, "zzAssetCategoryTests/unsupported");
    if (!categoriesValid)
        std::cerr << "Asset categories must contain sorted, disjoint IDs for valid images, GIFs, and fonts." << std::endl;
    success = categoriesValid && success;
    {
        ContextualObjectMenu filteredMenu(sprite, point2D(10, 10), world);
        const auto root = filteredMenu.getChildItem("set asset", nullptr);
        const auto fixtureFolder = filteredMenu.getChildItem("zzAssetCategoryTests", root);
        const bool menuFiltered = root && fixtureFolder && !filteredMenu.getChildItem("arial.ttf", root)
            && filteredMenu.getChildItem("still.BMP", fixtureFolder)
            && !filteredMenu.getChildItem("animation.GIF", fixtureFolder)
            && !filteredMenu.getChildItem("font.TTF", fixtureFolder)
            && !filteredMenu.getChildItem("broken.png", fixtureFolder)
            && !filteredMenu.getChildItem("unsupported.txt", fixtureFolder);
        if (!menuFiltered)
            std::cerr << "Sprite asset menu must exclude GIFs, fonts, and invalid images." << std::endl;
        success = menuFiltered && success;
    }
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
        const auto relativePath = fs::path(assets->getPath(assetID)).lexically_relative(assets->getAssetsPath());
        float columnX = 190;
        float columnY = 10;
        point2D leafPosition;
        for (const auto& component : relativePath)
        {
            const auto child = nestedMenu->getChildItem(component.string(), node);
            if (!child)
            {
                success = false;
                break;
            }
            const auto row = std::distance(node->items.begin(), std::find(node->items.begin(), node->items.end(), child));
            leafPosition = point2D(columnX + 10, columnY + static_cast<float>(row) * 30 + 10);
            world.Hovering(leafPosition);
            success = world.getHoveredObject() == nestedMenu && success;
            columnY += static_cast<float>(row) * 30;
            columnX += 180;
            node = child;
        }
        world.leftClick(leafPosition);
        world.releaseLeftClick(leafPosition);
        success = nestedMenu->isHidden()
            && sprite->serialize().getMember<std::string>("assetID") == assetID && success;
        break;
    }

    if (!SDL_Init(0) || !TTF_Init())
        return 1;
    SDL_Surface* renderTarget = SDL_CreateSurface(640, 480, SDL_PIXELFORMAT_RGBA8888);
    SDL_Renderer* renderer = renderTarget ? SDL_CreateSoftwareRenderer(renderTarget) : nullptr;
    if (!renderer)
        return 1;
    {
        bool selected = false;
        const std::string callbackID = "AssetMenuScrollTest";
        ButtonCallbackRegistery::instance()->registerCallback(callbackID,
            [&selected](point2D, Object*) { selected = true; return true; });
        DropDownMenu scrollMenu(rect2D(10, 10, 180, 30), size2D(180, 30),
            Color(180, 180, 180, 255), std::make_shared<Font>("arial", 20.0f));
        scrollMenu.setCascading(true);
        for (int index = 0; index < 22; ++index)
            scrollMenu.addItem("folder/entry" + std::to_string(index), callbackID);
        success = scrollMenu.createRenderer(renderer) && success;
        scrollMenu.hover(point2D(20, 20));
        const auto folder = scrollMenu.getChildItem("folder", nullptr);
        const auto surface = scrollMenu.getRenderer()->getSurface();
        Uint8 red = 0, green = 0, blue = 0, alpha = 0;
        success = surface && surface->w == 360 && surface->h == 480
            && SDL_ReadSurfacePixel(surface, 185, 5, &red, &green, &blue, &alpha)
            && alpha == 255 && success;
        scrollMenu.hover(point2D(200, 20));
        for (int index = 0; index < 30; ++index)
            scrollMenu.scroll(-1);
        success = folder->firstVisibleItem == 6 && success;
        scrollMenu.leftClick(point2D(200, 465));
        scrollMenu.releaseLeftClick(point2D(200, 465));
        success = selected && success;
        scrollMenu.scroll(1);
        success = folder->firstVisibleItem == 5 && success;
        ButtonCallbackRegistery::instance()->unregisterCallback(callbackID);

        auto renderedSprite = std::make_shared<Sprite>(point2D(50, 60), "boule");
        success = renderedSprite->createRenderer(renderer) && success;
        const auto oldTexture = renderedSprite->getRenderer()->getTexture();
        const auto oldCenter = renderedSprite->getPos();
        const auto assetID = sprite->serialize().getMember<std::string>("assetID");
        success = renderedSprite->setAsset(assetID)
            && renderedSprite->getRenderer()->getTexture() != oldTexture
            && renderedSprite->getRenderer()->getSurface() == static_cast<Image*>(assets->getAsset(assetID))->getSurface()
            && renderedSprite->getPos().x == oldCenter.x && renderedSprite->getPos().y == oldCenter.y && success;

        DropDownMenu wideMenu(rect2D(500, 450, 180, 30), size2D(180, 30),
            Color(180, 180, 180, 255), std::make_shared<Font>("arial", 20.0f));
        wideMenu.setCascading(true);
        wideMenu.addItem("folder/A very long asset filename that must fit.bmp");
        success = wideMenu.createRenderer(renderer) && success;
        const auto rootShape = wideMenu.getShape();
        wideMenu.hover(rootShape.position + size2D(10, 10));
        const auto shape = wideMenu.getShape();
        success = shape.position.x >= 0 && shape.position.y >= 0
            && shape.position.x + shape.size.w <= 640
            && shape.position.y + shape.size.h <= 480
            && shape.size.w > 360 && success;
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroySurface(renderTarget);
    TTF_Quit();
    SDL_Quit();

    if (!success)
        std::cerr << "Context menus must persist across pointer exit, dismiss on outside clicks, sort and open asset folders on hover, and clean up after selection or dismissal." << std::endl;
    return success ? 0 : 1;
}