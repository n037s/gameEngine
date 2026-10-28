#define main WorldEditorMain
#include "../WorldEditor/main.cpp"
#undef main

class VisibilityTarget : public Object
{
public:
    VisibilityTarget() { m_uid = "fef27a59-0719-4a77-bd1d-d6473ba048ef"; }
    std::string getTypeName() const override { return "VisibilityTarget"; }
    bool createRenderer(SDL_Renderer*) override { return false; }
};

int main()
{
    Viewer* viewer = Viewer::getInstance();
    ButtonCallbackRegistery::instance()->registerCallback("osef2", Osef2);
    Button button(rect2D(0, -100, 100, 25), "blip", Color(100, 100, 100, 255),
        nullptr, Color(255, 255, 255, 255), "osef", "osef2");
    const point2D clickPosition(0, 0);

    viewer->setWorld(nullptr);
    bool success = !button.leftClick(clickPosition) && !button.releaseLeftClick(clickPosition);

    World world;
    viewer->setWorld(&world);
    success = !button.releaseLeftClick(clickPosition) && success;

    auto target = std::make_shared<VisibilityTarget>();
    world.addObject(target);
    success = button.releaseLeftClick(clickPosition) && target->isHidden() && success;
    success = button.releaseLeftClick(clickPosition) && !target->isHidden() && success;

    world.removeObject(target);
    success = !button.releaseLeftClick(clickPosition) && success;
    viewer->setWorld(nullptr);

    if (!success)
        std::cerr << "Button visibility callback must tolerate missing targets and toggle existing targets." << std::endl;
    return success ? 0 : 1;
}