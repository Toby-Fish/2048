#ifndef GAME_GRAPHICS_H
#define GAME_GRAPHICS_H

#include <string>
#include <tuple>

enum GameBoardDimensions
{
	MIN_GAME_BOARD_PLAY_SIZE = 3,
	MAX_GAME_BOARD_PLAY_SIZE = 10
};

namespace Game
{
	namespace Graphics
	{
		std::string AsciiArt2048();
		std::string BoardSizeErrorPrompt();
		std::string BoardInputPrompt();
		std::string GameBoardNoSaveErrorPrompt();
		std::string GameStateNowSavedPrompt();
		std::string QuestionEndOfWinningGamePrompt();
		std::string InvalidInputGameBoardErrorPrompt();

		/*
			scoreboard_display_data_t =
			(competition_mode_t, gameboardScore, BestScore, movecount)
		*/
		using scoreboard_display_data_t =
			std::tuple<bool, std::string, std::string, std::string>;
		std::string GameScoreBoardBox(scoreboard_display_data_t scdd);
		std::string GameScoreBoardOverlay(scoreboard_display_data_t scdd);

		std::string InputCommandListPrompt();
		std::string EndlessModeCommandListPrompt();
		std::string InputCommandListFooterPrompt();
		using input_controls_display_data_t = std::tuple<bool, bool>;
		std::string GameInputControlsOverlay(input_controls_display_data_t gamestatus);

		std::string GameOverWinPrompt();
		std::string GameOverLosePrompt();
		std::string EndOfEndlessPrompt();
		using end_screen_display_data_t = std::tuple<bool, bool>;
		std::string GameEndScreenOverlay(end_screen_display_data_t esdd);

	} // namespace Graphics
} // namespace Game

#endif // !GAME_GRAPHICS_H