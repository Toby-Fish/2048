#ifndef GAMEBOARD_GRAPHICS_H
#define GAMEBOARD_GRAPHICS_H

#include <string>

namespace Game
{
	struct GameBoard;

	namespace Gameboard
	{
		
		namespace Graphics
		{

			std::string GameBoardTextOutput(GameBoard gb);

		} // namespace Graphics

	} // namespace Gameboard

} // namespace Game

#endif // !GAMEBOARD_GRAPHICS_H
