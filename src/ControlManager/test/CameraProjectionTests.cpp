#include "Camera.h"
#include "CameraControl.h"
#include "BaseRenderer.h"
#include "Object.h"
#include "World.h"
#include "UnitTestEnvironment.h"

#include <cmath>
#include <sstream>
#include <type_traits>

static_assert(!std::is_convertible_v<float, size2D>);
static_assert(std::is_same_v<decltype(point2D{} - point2D{}), vector2D>);

namespace
{
    bool near(float actual, float expected, float tolerance = 0.02f)
    {
        return std::abs(actual - expected) < tolerance;
    }

    bool nearPoint(point2D actual, point2D expected)
    {
        return near(actual.x, expected.x) && near(actual.y, expected.y);
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

    struct CameraFixture : UnitTest::Fixture
    {
        Camera* camera{Camera::getInstance()};

        void reset()
        {
            camera->setCameraZLimits(0.1f, 5.0f);
            camera->moveZ(1.0f - camera->getZ());
            camera->setViewMode(Camera::ViewMode::TopDown);
            camera->setIsometricAngles(50, 20);
            camera->setWindowSize(size2D(1280, 800));
            camera->setPos(point2D(140, -80));
        }

        void setUp() override { reset(); }
        void tearDown() override { reset(); }

        void configure(Camera::ViewMode mode, float zoom)
        {
            camera->setViewMode(mode);
            camera->moveZ(zoom - camera->getZ());
        }
    };

    struct RenderingFixture : CameraFixture
    {
        bool initialized{false};
        SDL_Surface* target{nullptr};
        SDL_Renderer* renderer{nullptr};

        void setUp() override
        {
            CameraFixture::setUp();
            initialized = SDL_Init(0);
            if (!initialized)
                throw std::runtime_error(SDL_GetError());
            target = SDL_CreateSurface(256, 256, SDL_PIXELFORMAT_RGBA32);
            renderer = target ? SDL_CreateSoftwareRenderer(target) : nullptr;
            if (!renderer)
                throw std::runtime_error(SDL_GetError());
            camera->setPos(point2D(0, 0));
            camera->setWindowSize(size2D(256, 256));
            camera->setIsometricAngles(50, 30);
        }

        void tearDown() override
        {
            SDL_DestroyRenderer(renderer);
            SDL_DestroySurface(target);
            if (initialized)
                SDL_Quit();
            CameraFixture::tearDown();
        }
    };

    void steadySerialization(UnitTest::Scenario& scenario, bool steady)
    {
        TestObject object(point2D(10, 20), size2D(40, 100));
        object.setSteady(steady);
        ObjectMemberHolder saved = object.serialize();
        std::stringstream stream;
        saved.serialize(stream);
        ObjectMemberHolder loaded;
        loaded.deserialize(stream);
        TestObject restored(loaded);
        UT_EXPECT(scenario, restored.isSteady() == steady);
    }

    void legacyOrientation(UnitTest::Scenario& scenario)
    {
        ObjectMemberHolder legacy;
        legacy.addMember("UID", std::string("legacy"));
        legacy.addMember("objectSize", size2D(40, 100));
        legacy.addMember("objectPosition", point2D(10, 20));
        legacy.addMember("objectHeight", 0.0f);
        legacy.addMember("isObjectHidden", false);
        TestObject restored(legacy);
        UT_EXPECT(scenario, !restored.isSteady());
    }

    void steadyProjection(UnitTest::Scenario& scenario, CameraFixture& fixture,
        Camera::ViewMode mode, float zoom)
    {
        fixture.configure(mode, zoom);
        const auto camera = fixture.camera;
        const rect2D actor(point2D(60, -30), size2D(40, 100));
        const auto steadyCorners = camera->getRenderingCorners(actor, true);
        if (mode == Camera::ViewMode::Isometric)
        {
            UT_EXPECT(scenario, nearPoint(steadyCorners[1] - steadyCorners[0], point2D(40 / zoom, 0)));
            UT_EXPECT(scenario, nearPoint(steadyCorners[3] - steadyCorners[0], point2D(0, 100 / zoom)));
            UT_EXPECT(scenario, nearPoint((steadyCorners[2] + steadyCorners[3]).toVector() / 2,
                camera->worldToWindowPoint(actor.position + point2D(20, 100))));
            const rect2D inside(steadyCorners[0] + point2D(1, 1), size2D(1, 1));
            UT_EXPECT(scenario, camera->intersectsWindowRect(actor, inside, true));
            UT_EXPECT(scenario, !camera->intersectsWindowRect(actor,
                rect2D(steadyCorners[1] + point2D(1, 1), size2D(1, 1)), true));
        }
        else
        {
            const auto groundCorners = camera->getRenderingCorners(actor);
            for (size_t index = 0; index < steadyCorners.size(); ++index)
                UT_EXPECT(scenario, nearPoint(steadyCorners[index], groundCorners[index]));
        }
    }

    void projectionRoundTrip(UnitTest::Scenario& scenario, CameraFixture& fixture,
        Camera::ViewMode mode, float zoom)
    {
        fixture.configure(mode, zoom);
        const auto camera = fixture.camera;
        for (const point2D& position : {point2D(0, 0), point2D(-300, 450), point2D(2000, -1800)})
            UT_EXPECT(scenario, nearPoint(camera->windowToWorldPoint(camera->worldToWindowPoint(position)), position));
    }

    void viewportBounds(UnitTest::Scenario& scenario, CameraFixture& fixture,
        Camera::ViewMode mode, float zoom)
    {
        fixture.configure(mode, zoom);
        const auto camera = fixture.camera;
        const rect2D visible = camera->getRenderingRect();
        for (const point2D& screen : {point2D(0, 0), point2D(1280, 0), point2D(1280, 800), point2D(0, 800)})
        {
            const point2D world = camera->windowToWorldPoint(screen);
            UT_EXPECT(scenario, world.x >= visible.position.x - 0.02f && world.y >= visible.position.y - 0.02f
                && world.x <= visible.position.x + visible.size.w + 0.02f
                && world.y <= visible.position.y + visible.size.h + 0.02f);
        }
    }

    void screenRelativeDragging(UnitTest::Scenario& scenario, CameraFixture& fixture,
        Camera::ViewMode mode, float zoom)
    {
        fixture.configure(mode, zoom);
        const auto camera = fixture.camera;
        const point2D start = camera->getPos();
        camera->movePos(camera->windowToWorldDelta(point2D(17, -23)));
        UT_EXPECT(scenario, nearPoint(camera->worldToWindowPoint(start), point2D(623, 423)));
    }

    void configuredAngles(UnitTest::Scenario& scenario, CameraFixture& fixture)
    {
        const auto camera = fixture.camera;
        camera->setViewMode(Camera::ViewMode::Isometric);
        camera->setIsometricAngles(50, 30);
        const float radians = 0.017453292519943295f;
        UT_EXPECT(scenario, nearPoint(camera->worldToWindowPoint(camera->getPos() + point2D(100, 0)),
            point2D(640 + 100 * std::cos(30 * radians), 400 + 50 * std::cos(50 * radians))));
    }

    void precisePicking(UnitTest::Scenario& scenario, CameraFixture& fixture)
    {
        const auto camera = fixture.camera;
        camera->setViewMode(Camera::ViewMode::Isometric);
        camera->setIsometricAngles(50, 30);
        const point2D origin = camera->getPos();
        const rect2D tile(origin, size2D(256, 256));
        const point2D center = camera->worldToWindowPoint(origin + point2D(128, 128));
        UT_EXPECT(scenario, camera->intersectsWindowRect(tile, rect2D(center, size2D(5, 5))));
        const rect2D tileBounds = camera->WorldToWindow(tile);
        const rect2D emptyCorner(tileBounds.position + point2D(1, 1), size2D(5, 5));
        UT_REQUIRE(scenario, tileBounds.isCollide(emptyCorner));
        UT_EXPECT(scenario, !camera->intersectsWindowRect(tile, emptyCorner));
    }

    void keyboardPanning(UnitTest::Scenario& scenario, CameraFixture& fixture)
    {
        const auto camera = fixture.camera;
        camera->setViewMode(Camera::ViewMode::Isometric);
        camera->setIsometricAngles(50, 30);
        const point2D origin = camera->getPos();
        CameraControl::getInstance().moveRight();
        UT_EXPECT(scenario, nearPoint(camera->worldToWindowPoint(origin), point2D(630, 400)));
    }

    void toggleView(UnitTest::Scenario& scenario, CameraFixture& fixture)
    {
        const auto camera = fixture.camera;
        camera->setViewMode(Camera::ViewMode::Isometric);
        camera->setIsometricAngles(50, 30);
        const point2D origin = camera->getPos();
        camera->toggleViewMode();
        UT_EXPECT(scenario, camera->getViewMode() == Camera::ViewMode::TopDown);
        UT_EXPECT(scenario, nearPoint(camera->worldToWindowPoint(origin + point2D(100, 0)), point2D(740, 400)));
        camera->toggleViewMode();
        UT_EXPECT(scenario, camera->getViewMode() == Camera::ViewMode::Isometric);
    }

    void zeroAngles(UnitTest::Scenario& scenario, CameraFixture& fixture)
    {
        const auto camera = fixture.camera;
        camera->setViewMode(Camera::ViewMode::Isometric);
        camera->setIsometricAngles(0, 0);
        UT_EXPECT(scenario, nearPoint(camera->worldToWindowPoint(camera->getPos() + point2D(100, 100)),
            point2D(740, 500)));
    }

    void nonsingularInverse(UnitTest::Scenario& scenario, CameraFixture& fixture)
    {
        const auto camera = fixture.camera;
        camera->setViewMode(Camera::ViewMode::Isometric);
        camera->setIsometricAngles(90, 30);
        const point2D position = camera->getPos() + point2D(10, 10);
        UT_EXPECT(scenario, nearPoint(camera->windowToWorldPoint(camera->worldToWindowPoint(position)), position));
    }

    void zeroZoomLimits(UnitTest::Scenario& scenario, CameraFixture& fixture)
    {
        fixture.camera->setCameraZLimits(0, 0);
        fixture.camera->moveZ(-1000);
        UT_EXPECT(scenario, fixture.camera->getZ() > 0);
        UT_EXPECT(scenario, std::isfinite(fixture.camera->getRenderingScale()));
    }

    void reversedZoomLimits(UnitTest::Scenario& scenario, CameraFixture& fixture)
    {
        fixture.camera->setCameraZLimits(0, 0);
        fixture.camera->moveZ(-1000);
        fixture.camera->setCameraZLimits(4, 1);
        UT_EXPECT(scenario, near(fixture.camera->getZ(), 4));
    }

    void projectedTexture(UnitTest::Scenario& scenario, RenderingFixture& fixture, Camera::ViewMode mode)
    {
        const auto camera = fixture.camera;
        const auto renderer = fixture.renderer;
        camera->setViewMode(mode);
        const rect2D tile(-40, -40, 80, 80);
        TestObject object(tile.position, tile.size);
        UT_REQUIRE(scenario, object.createRenderer(renderer));
        const auto image = object.getRenderer();
        UT_REQUIRE(scenario, SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) && SDL_RenderClear(renderer));
        UT_REQUIRE(scenario, mode == Camera::ViewMode::Isometric ? image->renderQuad(camera->getRenderingCorners(tile))
            : image->render(camera->WorldToWindow(tile).toSDL()));
        UT_REQUIRE(scenario, SDL_RenderPresent(renderer));
        Uint8 red = 0, green = 0, blue = 0, alpha = 0;
        UT_REQUIRE(scenario, SDL_ReadSurfacePixel(fixture.target, 128, 128, &red, &green, &blue, &alpha));
        UT_EXPECT(scenario, red == 255 && green == 0 && blue == 0);
        const rect2D box = camera->WorldToWindow(tile);
        UT_REQUIRE(scenario, SDL_ReadSurfacePixel(fixture.target, static_cast<int>(box.position.x + 2),
            static_cast<int>(box.position.y + 2), &red, &green, &blue, &alpha));
        UT_EXPECT(scenario, red == (mode == Camera::ViewMode::Isometric ? 0 : 255));
    }

    void worldRendering(UnitTest::Scenario& scenario, RenderingFixture& fixture, bool steady)
    {
        const auto renderer = fixture.renderer;
        World world;
        world.createCamera(point2D(0, 0), size2D(256, 256));
        fixture.camera->setViewMode(Camera::ViewMode::Isometric);
        fixture.camera->setIsometricAngles(50, 0);
        auto actor = std::make_shared<TestObject>(point2D(-20, 220), size2D(40, 120));
        UT_REQUIRE(scenario, actor->createRenderer(renderer));
        world.addObject(actor);
        actor->setSteady(steady);
        UT_REQUIRE(scenario, SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) && SDL_RenderClear(renderer));
        world.render(renderer);
        UT_REQUIRE(scenario, SDL_RenderPresent(renderer));
        Uint8 red = 0, green = 0, blue = 0, alpha = 0;
        UT_REQUIRE(scenario, SDL_ReadSurfacePixel(fixture.target, 128, 240, &red, &green, &blue, &alpha));
        UT_EXPECT(scenario, red == (steady ? 255 : 0));
        world.mouseMove(point2D(128, 240));
        UT_EXPECT(scenario, (world.getHoveredObject() == actor) == steady);
    }
}

int main()
{
    UnitTest::Environment tests("Camera projection");
    tests.addScenario("Vector scaling preserves both coordinates", [](UnitTest::Scenario& scenario) {
        const vector2D displacement = point2D(7, 11) - point2D(1, 3);
        UT_EXPECT(scenario, nearPoint(displacement, point2D(6, 8)));
        UT_EXPECT(scenario, near(displacement.length(), 10));
        UT_EXPECT(scenario, nearPoint(displacement * 2, point2D(12, 16)));
        UT_EXPECT(scenario, nearPoint(displacement / 2, point2D(3, 4)));
        UT_EXPECT(scenario, nearPoint(displacement.normalized(), point2D(0.6f, 0.8f)));
        UT_EXPECT(scenario, nearPoint(vector2D().normalized(), point2D(0, 0)));
        UT_EXPECT(scenario, nearPoint(point2D(6, 8).toVector(), displacement));
        UT_EXPECT(scenario, nearPoint(point2D(6, 8) * size2D(2, 3), point2D(12, 24)));
        UT_EXPECT(scenario, nearPoint(point2D(6, 8) / size2D(2, 4), point2D(3, 2)));
    });
    tests.addScenario("Default camera view is top-down", [](UnitTest::Scenario& scenario) {
        UT_EXPECT(scenario, Camera::getInstance()->getViewMode() == Camera::ViewMode::TopDown);
    });
    tests.addScenario("Objects default to ground orientation", [](UnitTest::Scenario& scenario) {
        TestObject object(point2D(10, 20), size2D(40, 100));
        UT_EXPECT(scenario, !object.isSteady());
    });
    for (const bool steady : {true, false})
        tests.addScenario(std::string("Steady orientation survives serialization / steady=") + (steady ? "true" : "false"),
            [=](UnitTest::Scenario& scenario) { steadySerialization(scenario, steady); });
    tests.addScenario("Legacy objects retain ground orientation", legacyOrientation);
    tests.addScenario<CameraFixture>("Top-down mapping keeps the camera centered",
        [](UnitTest::Scenario& scenario, CameraFixture& fixture) {
            UT_EXPECT(scenario, nearPoint(fixture.camera->worldToWindowPoint(fixture.camera->getPos()), point2D(640, 400)));
            UT_EXPECT(scenario, nearPoint(fixture.camera->worldToWindowPoint(point2D(240, 20)), point2D(740, 500)));
        });
    for (const auto mode : {Camera::ViewMode::TopDown, Camera::ViewMode::Isometric})
    for (const float zoom : {0.1f, 1.0f, 5.0f})
    {
        const auto parameters = std::string(" / ") + (mode == Camera::ViewMode::TopDown ? "top-down" : "isometric")
            + " / zoom=" + std::to_string(zoom);
        tests.addScenario<CameraFixture>("Steady projection and picking" + parameters,
            [=](UnitTest::Scenario& scenario, CameraFixture& fixture) { steadyProjection(scenario, fixture, mode, zoom); });
        tests.addScenario<CameraFixture>("Projection round trip" + parameters,
            [=](UnitTest::Scenario& scenario, CameraFixture& fixture) { projectionRoundTrip(scenario, fixture, mode, zoom); });
        tests.addScenario<CameraFixture>("Viewport bounds include all corners" + parameters,
            [=](UnitTest::Scenario& scenario, CameraFixture& fixture) { viewportBounds(scenario, fixture, mode, zoom); });
        tests.addScenario<CameraFixture>("Dragging follows screen axes" + parameters,
            [=](UnitTest::Scenario& scenario, CameraFixture& fixture) { screenRelativeDragging(scenario, fixture, mode, zoom); });
    }
    tests.addScenario<CameraFixture>("Configured tilt and heading determine projection", configuredAngles);
    tests.addScenario<CameraFixture>("Picking rejects empty projected bounding-box corners", precisePicking);
    tests.addScenario<CameraFixture>("Keyboard panning follows screen axes", keyboardPanning);
    tests.addScenario<CameraFixture>("View toggling restores both projections", toggleView);
    tests.addScenario<CameraFixture>("Zero angles match top-down mapping", zeroAngles);
    tests.addScenario<CameraFixture>("Angle limits keep the inverse nonsingular", nonsingularInverse);
    tests.addScenario<CameraFixture>("Zero zoom limits remain positive and finite", zeroZoomLimits);
    tests.addScenario<CameraFixture>("Reversed zoom limits clamp safely", reversedZoomLimits);
    for (const auto mode : {Camera::ViewMode::TopDown, Camera::ViewMode::Isometric})
        tests.addScenario<RenderingFixture>(std::string("Projected texture pixels / ")
            + (mode == Camera::ViewMode::TopDown ? "top-down" : "isometric"),
            [=](UnitTest::Scenario& scenario, RenderingFixture& fixture) { projectedTexture(scenario, fixture, mode); });
    for (const bool steady : {false, true})
        tests.addScenario<RenderingFixture>(std::string("World culling and hovering match rendered orientation / steady=")
            + (steady ? "true" : "false"),
            [=](UnitTest::Scenario& scenario, RenderingFixture& fixture) { worldRendering(scenario, fixture, steady); });
    return tests.run();
}