#ifndef MENUGRAPHICS_H
#define MENUGRAPHICS_H

#include <string>

namespace Game
{
    namespace Graphics
    {
        namespace Menu
        {

            std::string MenuTitlePrompt();
            std::string MenuOptionsPrompt();
            std::string InputMenuErrorInvalidInputPrompt();
            std::string InputMenuPrompt();
            std::string MenuGraphicsOverlay(bool ifInputInvalid);

        } // namespace Menu
    } // namespace Graphics
} // namespace Game

#endif // !MENUGRAPHICS_H