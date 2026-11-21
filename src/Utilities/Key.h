#pragma once

#include <SDL3/SDL_keycode.h>
#include <vector>

enum class KeyModifier : SDL_Keymod
{
    None       = SDL_KMOD_NONE,
    Shift      = SDL_KMOD_SHIFT,
    Ctrl       = SDL_KMOD_CTRL,
    Alt        = SDL_KMOD_ALT,
    Gui        = SDL_KMOD_GUI,
    Num        = SDL_KMOD_NUM,
    Caps       = SDL_KMOD_CAPS,
    Mode       = SDL_KMOD_MODE,
};

struct Key
{
public:
    Key(SDL_Keycode keycode) : m_keycode(keycode) {}
    Key(SDL_Keycode keycode, std::vector<KeyModifier> modifiers);
    static Key fromSDL(SDL_Keycode keycode, SDL_Keymod modifiers);

    SDL_Keycode getKeycode() const { return m_keycode; }
    const std::vector<SDL_Keymod>& getModifiers() const { return m_modifiers; }
    SDL_Keymod getModifierMask() const;

    bool operator==(const Key& other) const;
    bool operator<(const Key& other) const;

private:
    SDL_Keycode m_keycode;
    std::vector<SDL_Keymod> m_modifiers;
};