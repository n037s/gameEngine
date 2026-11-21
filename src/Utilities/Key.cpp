#include "Key.h"


Key::Key(SDL_Keycode keycode, std::vector<KeyModifier> modifiers)
    : m_keycode(keycode)
{
    for (const auto& modifier : modifiers)
    {
        m_modifiers.push_back(static_cast<SDL_Keymod>(modifier));
    }
}

Key Key::fromSDL(SDL_Keycode keycode, SDL_Keymod modifiers)
{
    Key key(keycode);
    if (modifiers != SDL_KMOD_NONE)
        key.m_modifiers.push_back(modifiers);
    return key;
}

SDL_Keymod Key::getModifierMask() const
{
    SDL_Keymod mask = SDL_KMOD_NONE;
    for (SDL_Keymod modifier : m_modifiers)
        mask |= modifier;
    return mask;
}

bool Key::operator==(const Key& other) const
{
    return m_keycode == other.m_keycode && getModifierMask() == other.getModifierMask();
}

bool Key::operator<(const Key& other) const
{
    if (m_keycode != other.m_keycode)
        return m_keycode < other.m_keycode;
    return getModifierMask() < other.getModifierMask();
}