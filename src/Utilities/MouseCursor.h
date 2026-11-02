#pragma once

#include "SDL3/SDL.h"

#include "Logger.h"

enum MouseCursorType
{
    Default = 0,
    Text,
    Wait,
    Crosshair,
    Progress,
    NWSE_Resize,
    NESW_Resize,
    EW_Resize,
    NS_Resize,
    Move,
    NotAllowed,
    Pointer,
    NW_Resize,
    N_Resize,
    NE_Resize,
    E_Resize,
    SE_Resize,
    S_Resize,
    SW_Resize,
    W_Resize,
    None
};

class MouseCursor
{
public: 
    static MouseCursor* getInstance();

    void setCursor(MouseCursorType type);
    MouseCursorType getCursorType() const { return m_cursorType; }
    SDL_Cursor* getCursor() const { return m_cursor; }

private: 
    MouseCursor();
    ~MouseCursor();

    static MouseCursor* m_instance;
    SDL_Cursor* m_cursor{nullptr};
    MouseCursorType m_cursorType{Default};

    Logger* m_logger{nullptr};

};