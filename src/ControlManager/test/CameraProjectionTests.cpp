#include "Camera.h"
#include "CameraControl.h"
#include "BaseRenderer.h"
#include "Object.h"
#include "World.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>

namespace
{
    void require(bool condition, const char* message)
    {
        if (!condition)
        {
            std::cerr << message << '\n';
            std::exit(EXIT_FAILURE);
        }
    }

    bool near(float actual, float expected, float tolerance = 0.02f)
    {
        return std::abs(actual - expected) < tolerance;
    }

    void requirePoint(point2D actual, point2D expected, const char* message)
    {
        require(near(actual.x, expected.x) && near(actual.y, expected.y), message);
    }

    class TestRenderer : public BaseRenderer
    {
    public:
        TestRenderer(SDL_Renderer* renderer, SDL_Texture* texture) : BaseRenderer(renderer)
        {
            m_texture = texture;
        }
        ~TestRenderer() override { cleanRenderer(); }
    };

    class TestObject : public Object
    {
    public:
        using Object::Object;
        bool createRenderer(SDL_Renderer* renderer) override
        {
            SDL_Surface* surface = SDL_CreateSurface(8, 8, SDL_PIXELFORMAT_RGBA32);
            if (!surface)
                return false;
            SDL_FillSurfaceRect(surface, nullptr, SDL_MapSurfaceRGBA(surface, 255, 0, 0, 255));
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);
            if (!texture)
                return false;
            m_renderer = new TestRenderer(renderer, texture);
            return true;
        }
        std::string getTypeName() const override { return "TestObject"; }
    };

    void testSteadyState()
    {
        TestObject object(point2D(10, 20), size2D(40, 100));
        require(!object.isSteady(), "Objects must default to ground orientation");
        for (bool steady : { true, false })
        {
            object.setSteady(steady);
            ObjectMemberHolder saved = object.serialize();
            std::stringstream stream;
            saved.serialize(stream);
            ObjectMemberHolder loaded;
            loaded.deserialize(stream);
            TestObject restored(loaded);
            require(restored.isSteady() == steady, "Steady state must survive serialization");
        }
        ObjectMemberHolder legacy;
        legacy.addMember("UID", std::string("legacy"));
        legacy.addMember("objectSize", size2D(40, 100));
        legacy.addMember("objectPosition", point2D(10, 20));
        legacy.addMember("objectHeight", 0.0f);
        legacy.addMember("isObjectHidden", false);
        TestObject restored(legacy);
        require(!restored.isSteady(), "Legacy objects without a steady flag must stay on the ground");
    }

    void testRendering(Camera* camera)
    {
        require(SDL_Init(0), "SDL initialization failed");
        SDL_Surface* target = SDL_CreateSurface(256, 256, SDL_PIXELFORMAT_RGBA32);
        require(target != nullptr, "Software render surface creation failed");
        SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(target);
        require(renderer != nullptr, "Software renderer creation failed");
        SDL_Surface* source = SDL_CreateSurface(8, 8, SDL_PIXELFORMAT_RGBA32);
        require(source != nullptr, "Texture surface creation failed");
        require(SDL_FillSurfaceRect(source, nullptr, SDL_MapSurfaceRGBA(source, 255, 0, 0, 255)),
            "Texture fill failed");
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, source);
        SDL_DestroySurface(source);
        require(texture != nullptr, "Texture creation failed");
        camera->setCameraZLimits(0.1f, 5);
        camera->moveZ(1 - camera->getZ());
        camera->setPos(point2D(0, 0));
        camera->setWindowSize(size2D(256, 256));
        const rect2D tile(-40, -40, 80, 80);
        {
            TestRenderer image(renderer, texture);
            for (Camera::ViewMode mode : { Camera::ViewMode::TopDown, Camera::ViewMode::Isometric })
            {
                camera->setViewMode(mode);
                require(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) && SDL_RenderClear(renderer),
                    "Render clear failed");
                require(mode == Camera::ViewMode::Isometric ? image.renderQuad(camera->getRenderingCorners(tile))
                    : image.render(camera->WorldToWindow(tile).toSDL()), "Texture rendering failed");
                require(SDL_RenderPresent(renderer), "Software render present failed");
                Uint8 red, green, blue, alpha;
                require(SDL_ReadSurfacePixel(target, 128, 128, &red, &green, &blue, &alpha), "Pixel read failed");
                require(red == 255 && green == 0 && blue == 0, "Projected texture center must be red");
                const rect2D box = camera->WorldToWindow(tile);
                require(SDL_ReadSurfacePixel(target, static_cast<int>(box.position.x + 2),
                    static_cast<int>(box.position.y + 2), &red, &green, &blue, &alpha), "Corner pixel read failed");
                require(red == (mode == Camera::ViewMode::Isometric ? 0 : 255),
                    "Isometric rendering must leave the empty bounding-box corner unpainted");
            }
        }
        {
            World world;
            world.createCamera(point2D(0, 0), size2D(256, 256));
            camera->setViewMode(Camera::ViewMode::Isometric);
            camera->setIsometricAngles(50, 0);
            auto actor = std::make_shared<TestObject>(point2D(-20, 220), size2D(40, 120));
            require(actor->createRenderer(renderer), "World test renderer creation failed");
            world.addObject(actor);
            const point2D visiblePoint(128, 240);
            for (bool steady : { false, true })
            {
                actor->setSteady(steady);
                require(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) && SDL_RenderClear(renderer),
                    "World render clear failed");
                world.render(renderer);
                require(SDL_RenderPresent(renderer), "World render present failed");
                Uint8 red, green, blue, alpha;
                require(SDL_ReadSurfacePixel(target, 128, 240, &red, &green, &blue, &alpha),
                    "World pixel read failed");
                require(red == (steady ? 255 : 0),
                    "Steady sprites must render even when their ground rectangle is off-screen");
                world.mouseMove(visiblePoint);
                require((world.getHoveredObject() == actor) == steady,
                    "World hovering must match the displayed steady orientation");
            }
        }
        SDL_DestroyRenderer(renderer);
        SDL_DestroySurface(target);
        SDL_Quit();
    }
}

int main()
{
    testSteadyState();
    Camera* camera = Camera::getInstance();
    camera->setWindowSize(size2D(1280, 800));
    camera->setPos(point2D(140, -80));
    camera->setCameraZLimits(0.1f, 5.0f);
    require(camera->getViewMode() == Camera::ViewMode::TopDown, "Default view must be top-down");
    requirePoint(camera->worldToWindowPoint(camera->getPos()), point2D(640, 400), "Camera must stay centered");
    requirePoint(camera->worldToWindowPoint(point2D(240, 20)), point2D(740, 500), "Top-down mapping changed");

    const point2D positions[] = { point2D(0, 0), point2D(-300, 450), point2D(2000, -1800) };
    for (Camera::ViewMode mode : { Camera::ViewMode::TopDown, Camera::ViewMode::Isometric })
    {
        camera->setViewMode(mode);
        for (float zoom : { 0.1f, 1.0f, 5.0f })
        {
            camera->moveZ(zoom - camera->getZ());
            const rect2D actor(point2D(60, -30), size2D(40, 100));
            const auto steadyCorners = camera->getRenderingCorners(actor, true);
            if (mode == Camera::ViewMode::Isometric)
            {
                requirePoint(steadyCorners[1] - steadyCorners[0], point2D(40 / zoom, 0),
                    "Steady width must stay horizontal and follow zoom");
                requirePoint(steadyCorners[3] - steadyCorners[0], point2D(0, 100 / zoom),
                    "Steady height must stay upright without tilt compression");
                requirePoint((steadyCorners[2] + steadyCorners[3]) / 2,
                    camera->worldToWindowPoint(actor.position + point2D(20, 100)),
                    "Steady bottom center must stay anchored to the ground");
                const rect2D inside(steadyCorners[0] + point2D(1, 1), size2D(1, 1));
                require(camera->intersectsWindowRect(actor, inside, true), "Steady visible pixels must be pickable");
                require(!camera->intersectsWindowRect(actor,
                    rect2D(steadyCorners[1] + point2D(1, 1), size2D(1, 1)), true),
                    "Picking must reject points outside the steady rectangle");
            }
            else
            {
                const auto groundCorners = camera->getRenderingCorners(actor);
                for (size_t index = 0; index < steadyCorners.size(); ++index)
                    requirePoint(steadyCorners[index], groundCorners[index], "Steady must not change top-down projection");
            }
            for (const point2D& position : positions)
                requirePoint(camera->windowToWorldPoint(camera->worldToWindowPoint(position)), position,
                    "Projection round trip failed");

            const rect2D visible = camera->getRenderingRect();
            for (const point2D& screen : { point2D(0, 0), point2D(1280, 0), point2D(1280, 800), point2D(0, 800) })
            {
                const point2D world = camera->windowToWorldPoint(screen);
                require(world.x >= visible.position.x - 0.02f && world.y >= visible.position.y - 0.02f &&
                    world.x <= visible.position.x + visible.size.w + 0.02f &&
                    world.y <= visible.position.y + visible.size.h + 0.02f, "Viewport culling bounds omit a corner");
            }

            const point2D start = camera->getPos();
            camera->movePos(camera->windowToWorldDelta(point2D(17, -23)));
            requirePoint(camera->worldToWindowPoint(start), point2D(623, 423), "Dragging is not screen-relative");
            camera->setPos(start);
        }
    }

    camera->moveZ(1.0f - camera->getZ());
    camera->setViewMode(Camera::ViewMode::Isometric);
    camera->setIsometricAngles(50, 30);
    const point2D origin = camera->getPos();
    const float radians = 0.017453292519943295f;
    requirePoint(camera->worldToWindowPoint(origin + point2D(100, 0)),
        point2D(640 + 100 * std::cos(30 * radians), 400 + 50 * std::cos(50 * radians)),
        "Configured 50-degree tilt and 30-degree heading changed");
    const rect2D tile(origin, size2D(256, 256));
    const point2D center = camera->worldToWindowPoint(origin + point2D(128, 128));
    require(camera->intersectsWindowRect(tile, rect2D(center, size2D(5, 5))), "Projected center must be pickable");
    const rect2D tileBounds = camera->WorldToWindow(tile);
    const rect2D emptyCorner(tileBounds.position + point2D(1, 1), size2D(5, 5));
    require(tileBounds.isCollide(emptyCorner), "Picking regression setup must overlap the bounding rectangle");
    require(!camera->intersectsWindowRect(tile, emptyCorner), "Invisible bounding-box corner must not be pickable");

    CameraControl::getInstance().moveRight();
    requirePoint(camera->worldToWindowPoint(origin), point2D(630, 400), "Keyboard movement must follow screen axes");
    camera->setPos(origin);
    camera->toggleViewMode();
    require(camera->getViewMode() == Camera::ViewMode::TopDown, "Toggle must return to top-down");
    requirePoint(camera->worldToWindowPoint(origin + point2D(100, 0)), point2D(740, 400), "Top-down must be restored");
    camera->toggleViewMode();
    require(camera->getViewMode() == Camera::ViewMode::Isometric, "Toggle must restore isometric view");
    camera->setIsometricAngles(0, 0);
    requirePoint(camera->worldToWindowPoint(origin + point2D(100, 100)), point2D(740, 500), "Zero angles must match top-down");
    camera->setIsometricAngles(90, 30);
    requirePoint(camera->windowToWorldPoint(camera->worldToWindowPoint(origin + point2D(10, 10))),
        origin + point2D(10, 10), "Angle limits must keep the inverse nonsingular");
    camera->setIsometricAngles(50, 30);

    camera->setCameraZLimits(0, 0);
    camera->moveZ(-1000);
    require(camera->getZ() > 0 && std::isfinite(camera->getRenderingScale()), "Zoom must never reach zero");
    camera->setCameraZLimits(4, 1);
    require(near(camera->getZ(), 4), "Reversed zoom limits must be safe");
    testRendering(camera);
    std::cout << "Camera projection, picking, culling, panning, toggle, zoom, and rendering tests passed.\n";
}