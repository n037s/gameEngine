#include "ObjectDropDown.h"

#include "Viewer.h"
#include "World.h"

bool createOneObject(const std::string& objectType)
{
    Viewer* viewer = Viewer::getInstance();
    World* world = viewer->getWorld();
    if (!world)
    {
        std::cerr << "Cannot create object: no world is loaded." << std::endl;
        return false;
    }

    const point2D position(0, 0);
    ObjectPtr object;

    if (objectType == "Sprite")
    {
        object = std::make_shared<Sprite>(position);
    }
    else if (objectType == "AnimatedSprite")
    {
        object = std::make_shared<AnimatedSprite>(position, true, 250);
    }
    else if (objectType == "Button")
    {
        const rect2D buttonShape(position, size2D(120, 30));
        const Color buttonColor(60, 60, 60, 255);
        const Color labelColor(255, 255, 255, 255);
        FontPtr buttonFont = std::make_shared<Font>("arial", 20.0f);
        object = std::make_shared<Button>(buttonShape, "Button", buttonColor, buttonFont, labelColor, "Nothing", "Nothing");
    }

    if (!object || !object->createRenderer(viewer->getRenderer()))
    {
        std::cerr << "Cannot create object of type: " << objectType << std::endl;
        return false;
    }

    return world->addObject(object);
}

ObjectPtr createObjectList()
{
    std::cout << "Creating the object list" << std::endl;

    const rect2D shape(point2D(0, 0), size2D(200, 1000));
    const size2D itemSize(200, 30);
    const Color btnColor = Color(60, 60, 60, 120);
    FontPtr font = std::make_shared<Font>("arial", 20, TextAlignement{ Alignement::LOW, Alignement::CENTER });

    DropDownMenuPtr dropDown = std::make_shared<DropDownMenu>(shape, itemSize, btnColor, font);

    const std::list<std::string> objectTypes = {
        "Sprite",
        "AnimatedSprite",
        "Button",
    };

    for (const auto& objectType : objectTypes)
    {
        auto callbackUDI = UIDGenerator::generateUID();

        ButtonCallbackRegistery* btnRegister = ButtonCallbackRegistery::instance();
        btnRegister->registerCallback(callbackUDI, [objectType](point2D, Object*) {
            return createOneObject(objectType);
        });
        dropDown->addItem(objectType, callbackUDI);
    }

    dropDown->printMenu(nullptr);

    return dropDown;
}