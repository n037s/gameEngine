#pragma once

#include "SDL3/SDL.h"

#include "Logger.h"


class MouseCursor
{
public: 
    enum Type
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
    
    static MouseCursor* getInstance();

    void setCursor(Type type);
    Type getCursorType() const { return m_cursorType; }
    SDL_Cursor* getCursor() const { return m_cursor; }

private: 
    MouseCursor();
    ~MouseCursor();

    static MouseCursor* m_instance;
    SDL_Cursor* m_cursor{nullptr};
    Type m_cursorType{Default};

    Logger* m_logger{nullptr};

};