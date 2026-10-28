#define main WorldEditorMain
#include "../WorldEditor/main.cpp"
#undef main
#include "UnitTestEnvironment.h"

class VisibilityTarget : public Object
{
public:
    VisibilityTarget() { m_uid = "e9337627-78e6-4df5-809c-544d88884ac3"; }
    std::string getTypeName() const override { return "VisibilityTarget"; }
    bool createRenderer(SDL_Renderer*) override { return false; }
};

struct ButtonFixture : UnitTest::Fixture
{
    Viewer* viewer{Viewer::getInstance()};
    World world;
    std::unique_ptr<Button> button;
    const point2D clickPosition{0, 0};

    void setUp() override
    {
        viewer->setWorld(nullptr);
        ButtonCallbackRegistery::instance()->registerCallback("osef2", Osef2);
        button = std::make_unique<Button>(rect2D(0, -100, 100, 25), "blip", Color(100, 100, 100, 255),
            nullptr, Color(255, 255, 255, 255), "osef", "osef2");
    }

    void tearDown() override
    {
        viewer->setWorld(nullptr);
        ButtonCallbackRegistery::instance()->unregisterCallback("osef2");
    }
};

int main()
{
    UnitTest::Environment tests("Button callbacks");
    tests.addScenario<ButtonFixture>("Unregistered click callback returns false",
        [](UnitTest::Scenario& scenario, ButtonFixture& fixture) {
            UT_EXPECT(scenario, !fixture.button->leftClick(fixture.clickPosition));
        });
    tests.addScenario<ButtonFixture>("Visibility callback rejects a missing world",
        [](UnitTest::Scenario& scenario, ButtonFixture& fixture) {
            UT_EXPECT(scenario, !fixture.button->releaseLeftClick(fixture.clickPosition));
        });
    tests.addScenario<ButtonFixture>("Visibility callback rejects a missing target",
        [](UnitTest::Scenario& scenario, ButtonFixture& fixture) {
            fixture.viewer->setWorld(&fixture.world);
            UT_EXPECT(scenario, !fixture.button->releaseLeftClick(fixture.clickPosition));
        });
    tests.addScenario<ButtonFixture>("Visibility callback hides and restores its target",
        [](UnitTest::Scenario& scenario, ButtonFixture& fixture) {
            fixture.viewer->setWorld(&fixture.world);
            auto target = std::make_shared<VisibilityTarget>();
            fixture.world.addObject(target);
            UT_EXPECT(scenario, fixture.button->releaseLeftClick(fixture.clickPosition));
            UT_EXPECT(scenario, target->isHidden());
            UT_EXPECT(scenario, fixture.button->releaseLeftClick(fixture.clickPosition));
            UT_EXPECT(scenario, !target->isHidden());
        });
    tests.addScenario<ButtonFixture>("Visibility callback rejects a removed target",
        [](UnitTest::Scenario& scenario, ButtonFixture& fixture) {
            fixture.viewer->setWorld(&fixture.world);
            auto target = std::make_shared<VisibilityTarget>();
            fixture.world.addObject(target);
            fixture.world.removeObject(target);
            UT_EXPECT(scenario, !fixture.button->releaseLeftClick(fixture.clickPosition));
        });
    return tests.run();
}