#include "SelectionBoundObject.h"
#include "Camera.h"
#include "Viewer.h"
#include "CommandProxy.h"
#include "ContextualObjectMenu.h"

std::string SelectionBoundsObject::getTypeName() const
{
	return "SelectionBounds";
}

bool SelectionBoundsObject::createRenderer(SDL_Renderer* renderer)
{
	m_renderer = new SelectionBoundsRenderer(renderer);
	return true;
}

bool SelectionBoundsObject::render()
{
    if (!target || !m_renderer)
        return false;

    const auto corners = Camera::getInstance()->getRenderingCorners(target->getShape(), target->isSteady());
    return m_renderer->renderQuad(corners);
}

const rect2D SelectionBoundsObject::getShape() const
{
    if (!target)
        return rect2D(0, 0, 0, 0);

    const auto corners = Camera::getInstance()->getRenderingCorners(target->getShape(), target->isSteady());
    float minX = corners[0].x;
    float maxX = minX;
    float minY = corners[0].y;
    float maxY = minY;
    for (const point2D& corner : corners)
    {
        minX = std::min(minX, corner.x);
        maxX = std::max(maxX, corner.x);
        minY = std::min(minY, corner.y);
        maxY = std::max(maxY, corner.y);
    }
    return rect2D(minX, minY, maxX - minX, maxY - minY);
}

void SelectionBoundsObject::setTarget(ObjectPtr object) 
{ 
    if (const auto menu = m_contextMenu.lock())
        menu->close();
    target = std::move(object); 
    m_steady = target ? target->isSteady() : false;

    setupCallbacks();
}

point2D SelectionBoundsObject::toTargetSpace(point2D windowPos) const
{
    return Camera::getInstance()->windowToWorldPoint(windowPos);
}

bool SelectionBoundsObject::leftClick(point2D pos)
{
    if (!target)
        return false;

    m_grabOffset = target->getPos() - toTargetSpace(pos);
    return true;
}

bool SelectionBoundsObject::releaseLeftClick(point2D pos)
{
    return target != nullptr;
}

void SelectionBoundsObject::dragTarget(point2D windowPos)
{
    if (target && isLeftClicked())
        target->setPos(toTargetSpace(windowPos) + m_grabOffset);
}

void SelectionBoundsObject::hover(point2D pos)
{
    Object::hover(pos);
    dragTarget(pos);
}

void SelectionBoundsObject::leftFocus(point2D pos)
{
    Object::leftFocus(pos);
    dragTarget(pos);
}

bool SelectionBoundsObject::rightClick(point2D pos)
{
    return target != nullptr;
}

bool SelectionBoundsObject::releaseRightClick(point2D pos)
{
    std::cout << "Right click on SelectionBoundsObject" << std::endl;
    if (!target)
        return false;

    auto viewer = Viewer::getInstance();
    auto world = viewer->getWorld();
    if (!world)
        return false;

    if (const auto previous = m_contextMenu.lock())
        previous->close();

    const auto windowSize = Camera::getInstance()->getWindowSize();
    pos.x = std::max(0.0f, std::min(pos.x, windowSize.w - 180.0f));
    pos.y = std::max(0.0f, std::min(pos.y, windowSize.h - 30.0f));
    auto menu = std::make_shared<ContextualObjectMenu>(target, pos, *world);
    if (!menu->createRenderer(viewer->getRenderer()))
        return false;

    m_contextMenu = menu;
    return world->addOverlayObject(menu);
}

const std::vector<timeoutItem> objectControlTimeouts = {
    timeoutItem(MOVE_LEFT, 100),
    timeoutItem(MOVE_DOWN, 100),
    timeoutItem(MOVE_RIGHT, 100),
    timeoutItem(MOVE_UP, 100)
};

void SelectionBoundsObject::setupCallbacks()
{
    auto cmdProxy = Viewer::getInstance()->getCommandProxy();

    // First unbind any previously created callback
    cmdProxy->unbindCallback(SDLK_BACKSPACE);
    cmdProxy->unbindCallback(SDLK_Q);
    cmdProxy->unbindCallback(SDLK_S);
    cmdProxy->unbindCallback(SDLK_D);
    cmdProxy->unbindCallback(SDLK_Z);

    if ( target)
    {
        // We bind new callbacks to manage the object
        cmdProxy->bindCallback(SDLK_BACKSPACE, [this]() {
            bool result{false};
            if (target)
            {
                Viewer::getInstance()->getWorld()->removeObject(target);
                setTarget(nullptr);
                result = true;
            }
            return result;
        });

        const float dm = 1.0f;

        cmdProxy->bindCallback(SDLK_Q, [this, dm]() {
            bool result{false};
            if (target && checkTimeout(objectControlTimeouts.at(MOVE_LEFT)))
            {
                std::cout << "moving object to the left" << std::endl;
                target->setPos(target->getPos() + point2D(-dm, 0));
                result = true;
            }
            return result;
        });
        cmdProxy->bindCallback(SDLK_S, [this, dm]() {
            bool result{false};
            if (target && checkTimeout(objectControlTimeouts.at(MOVE_DOWN)))
            {
                target->setPos(target->getPos() + point2D(0, dm));
                result = true;
            }
            return result;
        });
        cmdProxy->bindCallback(SDLK_D, [this, dm]() {
            bool result{false};
            if (target && checkTimeout(objectControlTimeouts.at(MOVE_RIGHT)))
            {
                target->setPos(target->getPos() + point2D(dm, 0));
                result = true;
            }
            return result;
        });
        cmdProxy->bindCallback(SDLK_Z, [this, dm]() {
            bool result{false};
            if (target && checkTimeout(objectControlTimeouts.at(MOVE_UP)))
            {
                target->setPos(target->getPos() + point2D(0, -dm));
                result = true;
            }
            return result;
        });
    }
}