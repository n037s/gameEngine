#include "SelectionBoundObject.h"
#include "Camera.h"
#include "Viewer.h"
#include "CommandProxy.h"
#include "ContextualObjectMenu.h"
#include "MouseCursor.h"

constexpr float EDGE_THRESHOLD = 15.0f;

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

    // Verify if we are close to a edge or a corner.
    auto posInTarget = toTargetSpace(pos);
    switch (getEdge(posInTarget))
    {
    case TOP_LEFT:
        m_logger->log("SelectionBoundsObject::leftClick() - clicked the top-left corner");
        m_edgeAction = EdgeAction::Resize_NW;
        break;
    case TOP_RIGHT:
        m_logger->log("SelectionBoundsObject::leftClick() - clicked the top-right corner");
        m_edgeAction = EdgeAction::Resize_NE;
        break;
    case BOTTOM_LEFT:
        m_logger->log("SelectionBoundsObject::leftClick() - clicked the bottom-left corner");
        m_edgeAction = EdgeAction::Resize_SW;
        break;
    case BOTTOM_RIGHT:
        m_logger->log("SelectionBoundsObject::leftClick() - clicked the bottom-right corner");
        m_edgeAction = EdgeAction::Resize_SE;
        break;
    case TOP:
        m_logger->log("SelectionBoundsObject::leftClick() - clicked the top edge");
        m_edgeAction = EdgeAction::Resize_N;
        break;
    case BOTTOM:
        m_logger->log("SelectionBoundsObject::leftClick() - clicked the bottom edge");
        m_edgeAction = EdgeAction::Resize_S;
        break;
    case LEFT:
        m_logger->log("SelectionBoundsObject::leftClick() - clicked the left edge");
        m_edgeAction = EdgeAction::Resize_W;
        break;
    case RIGHT:
        m_logger->log("SelectionBoundsObject::leftClick() - clicked the right edge");
        m_edgeAction = EdgeAction::Resize_E;
        break;
    default:
        m_logger->log("SelectionBoundsObject::leftClick() - clicked outside of edges");
        m_grabOffset = target->getPos() - posInTarget;
        m_edgeAction = EdgeAction::Grab;
        break;
    }

    return true;
}

bool SelectionBoundsObject::releaseLeftClick(point2D pos)
{
    m_edgeAction = EdgeAction::None;
    return target != nullptr;
}

void SelectionBoundsObject::dragTarget(point2D windowPos)
{
    target->setPos(toTargetSpace(windowPos) + m_grabOffset);
}

void SelectionBoundsObject::updateCursor(point2D pos)
{
    auto posInTarget = toTargetSpace(pos);
    auto mouse = MouseCursor::getInstance();

    switch (getEdge(posInTarget))
    {
    case TOP_LEFT:
        mouse->setCursor(MouseCursor::NW_Resize);
        break;
    case TOP_RIGHT:
        mouse->setCursor(MouseCursor::NE_Resize);
        break;
    case BOTTOM_LEFT:
        mouse->setCursor(MouseCursor::SW_Resize);
        break;
    case BOTTOM_RIGHT:
        mouse->setCursor(MouseCursor::SE_Resize);
        break;
    case TOP:
        mouse->setCursor(MouseCursor::N_Resize);
        break;
    case BOTTOM:
        mouse->setCursor(MouseCursor::S_Resize);
        break;
    case LEFT:
        mouse->setCursor(MouseCursor::W_Resize);
        break;
    case RIGHT:
        mouse->setCursor(MouseCursor::E_Resize);
        break;
    default:
        mouse->setCursor(MouseCursor::Move);
        break;
    }
}

void SelectionBoundsObject::hover(point2D pos)
{
    Object::hover(pos);
    if (target && m_edgeAction == EdgeAction::Grab)
        dragTarget(pos);
    else
    {
        updateCursor(pos);
    }

}

void SelectionBoundsObject::leftFocus(point2D pos)
{
    Object::leftFocus(pos);
    if (target && m_edgeAction == EdgeAction::Grab)
        dragTarget(pos);
    MouseCursor::getInstance()->setCursor(MouseCursor::Default);
}

bool SelectionBoundsObject::rightClick(point2D pos)
{
    return target != nullptr;
}

bool SelectionBoundsObject::releaseRightClick(point2D pos)
{
    m_logger->info("Right click on SelectionBoundsObject");
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
                target->setPos(target->getPos().round() + point2D(-dm, 0));
                result = true;
            }
            return result;
        });
        cmdProxy->bindCallback(SDLK_S, [this, dm]() {
            bool result{false};
            if (target && checkTimeout(objectControlTimeouts.at(MOVE_DOWN)))
            {
                target->setPos(target->getPos().round() + point2D(0, dm));
                result = true;
            }
            return result;
        });
        cmdProxy->bindCallback(SDLK_D, [this, dm]() {
            bool result{false};
            if (target && checkTimeout(objectControlTimeouts.at(MOVE_RIGHT)))
            {
                target->setPos(target->getPos().round() + point2D(dm, 0));
                result = true;
            }
            return result;
        });
        cmdProxy->bindCallback(SDLK_Z, [this, dm]() {
            bool result{false};
            if (target && checkTimeout(objectControlTimeouts.at(MOVE_UP)))
            {
                target->setPos(target->getPos().round() + point2D(0, -dm));
                result = true;
            }
            return result;
        });
    }
}

const EdgeType SelectionBoundsObject::getEdge(point2D pos) const
{
    EdgeType output{NONE};

    if (!target)
        return output;

    auto shape = target->getShape();

    if ( std::abs(pos.x - shape.position.x) < EDGE_THRESHOLD || 
        std::abs(pos.x - (shape.position.x + shape.size.w)) < EDGE_THRESHOLD ||
        std::abs(pos.y - shape.position.y) < EDGE_THRESHOLD || 
        std::abs(pos.y - (shape.position.y + shape.size.h)) < EDGE_THRESHOLD )
    {
        // We are on an edge

        if ( std::abs(shape.position.x - pos.x) < EDGE_THRESHOLD )
        {
            // We are on the left edge
            if ( std::abs(shape.position.y - pos.y) < EDGE_THRESHOLD )
            {
                m_logger->log("Edge detected on the top left corner");
                output = TOP_LEFT;
            }
            else if ( std::abs((shape.position.y + shape.size.h) - pos.y) < EDGE_THRESHOLD )
            {
                m_logger->log("Edge detected on the bottom left corner");
                output = BOTTOM_LEFT;
            }
            else
            {
                m_logger->log("Edge detected on the left side");
                output = LEFT;
            }
        }
        else if ( std::abs((shape.position.x + shape.size.w) - pos.x) < EDGE_THRESHOLD )
        {
            // We are on the right edge
            if ( std::abs(shape.position.y - pos.y) < EDGE_THRESHOLD )
            {
                m_logger->log("Edge detected on the top right corner");
                output = TOP_RIGHT;
            }
            else if ( std::abs((shape.position.y + shape.size.h) - pos.y) < EDGE_THRESHOLD )
            {
                m_logger->log("Edge detected on the bottom right corner");
                output = BOTTOM_RIGHT;
            }
            else
            {
                m_logger->log("Edge detected on the right side");
                output = RIGHT;
            }
        }
        else if ( std::abs(shape.position.y - pos.y) < EDGE_THRESHOLD )
        {
            // We are on the top edge
            m_logger->log("Edge detected on the top side");
            output = TOP;
        }
        else if ( std::abs((shape.position.y + shape.size.h) - pos.y) < EDGE_THRESHOLD )
        {
            // We are on the bottom edge
            m_logger->log("Edge detected on the bottom side");
            output = BOTTOM;
        }
    }
    else
    {
        m_logger->info("No edge detected : " + shape.toString() + " - " + pos.toString());
    }

    return output;
}