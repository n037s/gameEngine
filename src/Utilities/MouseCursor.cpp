#include "MouseCursor.h"

MouseCursor* MouseCursor::m_instance{nullptr};



MouseCursor* MouseCursor::getInstance()
{
    if (!m_instance)
        m_instance = new MouseCursor();
    return m_instance;
}

MouseCursor::MouseCursor()
{
    m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
    m_cursorType = Default;
    m_logger = Logger::getInstance(Components::Utilities);
}

MouseCursor::~MouseCursor()
{
    if (m_cursor)
        SDL_DestroyCursor(m_cursor);
    m_cursorType = Default;
}

void MouseCursor::setCursor(Type type)
{
    if (m_cursorType == type)
        return;

    if (m_cursor)
        SDL_DestroyCursor(m_cursor);

    m_cursorType = type;
    switch (type)
    {
        case Default:
            m_logger->info("Setting cursor to Default");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
            break;
        case Text: 
            m_logger->info("Setting cursor to Text");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
            break;
        case Wait:
            m_logger->info("Setting cursor to Wait");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
            break;
        case Crosshair: 
            m_logger->info("Setting cursor to Crosshair");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR); 
            break;
        case Progress:
            m_logger->info("Setting cursor to Progress");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_PROGRESS);
            break;
        case NWSE_Resize:
            m_logger->info("Setting cursor to NWSE_Resize");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE);
            break;
        case NESW_Resize:
            m_logger->info("Setting cursor to NESW_Resize");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE);
            break;
        case EW_Resize:
            m_logger->info("Setting cursor to EW_Resize");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
            break;
        case NS_Resize:
            m_logger->info("Setting cursor to NS_Resize");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
            break;
        case Move:
            m_logger->info("Setting cursor to Move");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
            break;
        case NotAllowed:
            m_logger->info("Setting cursor to NotAllowed");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);
            break;
        case Pointer:
            m_logger->info("Setting cursor to Pointer");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
            break;
        case NW_Resize:
            m_logger->info("Setting cursor to NW_Resize");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NW_RESIZE);
            break;
        case N_Resize:
            m_logger->info("Setting cursor to N_Resize");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_N_RESIZE);
            break;
        case NE_Resize:
            m_logger->info("Setting cursor to NE_Resize");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NE_RESIZE);
            break;
        case E_Resize:
            m_logger->info("Setting cursor to E_Resize");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_E_RESIZE);
            break;
        case SE_Resize:
            m_logger->info("Setting cursor to SE_Resize");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SE_RESIZE);
            break;
        case S_Resize:
            m_logger->info("Setting cursor to S_Resize");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_S_RESIZE);
            break;
        case SW_Resize:
            m_logger->info("Setting cursor to SW_Resize");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SW_RESIZE);
            break;
        case W_Resize:
            m_logger->info("Setting cursor to W_Resize");
            m_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_W_RESIZE);
            break;
        case None: 
            m_logger->info("Setting cursor to None");
            m_cursor = nullptr; 
            break;
    }

    SDL_SetCursor(m_cursor);
}