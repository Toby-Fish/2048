#include "game-pregame.hpp"
#include "game-graphics.hpp"
#include "game-input.hpp"
#include "gameboard.hpp"
#include "game.hpp"
#include "menu.hpp"
#include "global.hpp"
#include "loadresource.hpp"
#include <array>
#include <sstream>
#include <iostream>
#include <limits>

namespace Game
{
	namespace
	{
		enum PreGameSetupStatusFlag
		{
			FLAG_NULL,
			FLAG_START_GAME,
			FLAG_RETURN_TO_MAIN_MENU,
			MAX_NO_PREGAME_SETUP_STATUS_FLAGS
		};
		using pregameesetup_status_t =
			std::array<bool, MAX_NO_PREGAME_SETUP_STATUS_FLAGS>;
		/* the array { pregamesetup_status } keeps track of player's choices */
		pregameesetup_status_t pregamesetup_status{};

		enum class NewGameFlag
		{
			NewGameFlagNull,
			NoPreviousSaveAvailable
		};

		/* 
			if the player chooses to continue with the previous game,
			but there is no data from the previous game,
			then { noSave } is TRUE and it is considered to start a new game
		*/
		bool noSave{ false };
		/* 
			if the player enters an invalid value, 
			{ flagInputErroneousChoice } is TRUE
		*/
		bool flagInputErroneousChoice{ false };

		/*
			stores the gameboard size selected by the player
		*/
		ull storedGameBoardSize{ 1 };

		int receiveGameBoardSize(std::istream& is)
		{
			int playerInputBoardSize{ 0 };

			if (!(is >> playerInputBoardSize))
			{
				constexpr auto INVALID_INPUT_VALUE_FLAG = -1;
				playerInputBoardSize = INVALID_INPUT_VALUE_FLAG;
				is.clear();
				is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}

			return playerInputBoardSize;
		}
		void receiveInputFromPlayer(std::istream& is)
		{
			using namespace Input::Keypress::Code;

			/* reset flagInputErroneousChoice */
			flagInputErroneousChoice = bool{ false };
			
			/*
				if the value entered by the player is invalid,
				{ gbsize } equals { -1 }
			*/
			const auto gbsize = receiveGameBoardSize(is);

			const auto isValidBoardSize =
				(gbsize >= MIN_GAME_BOARD_PLAY_SIZE) && (gbsize <= MAX_GAME_BOARD_PLAY_SIZE);

			/* regualr case */
			if (isValidBoardSize)
			{
				storedGameBoardSize = gbsize;
				pregamesetup_status[FLAG_START_GAME] = true;
			}

			/* special case */
			bool goBackToMainMenu{ true };

			switch (gbsize)
			{
			case CODE_HOTKEY_PREGAME_BACK_TO_MENU:
				pregamesetup_status[FLAG_RETURN_TO_MAIN_MENU] = true;
				break;
			default:
				goBackToMainMenu = false;
				break;
			}

			if (!isValidBoardSize && !goBackToMainMenu)
			{
				flagInputErroneousChoice = true;
			}
		}

		void processPreGame()
		{
			if (pregamesetup_status[FLAG_START_GAME])
			{
				playGame(PlayGameFlag::BrandNewGame, GameBoard{ storedGameBoardSize },
					storedGameBoardSize);
			}
			if (pregamesetup_status[FLAG_RETURN_TO_MAIN_MENU])
			{
				Menu::startMenu();
			}
		}

		bool soloLoop()
		{
			bool invalidInputValue = flagInputErroneousChoice;

			const auto questionAboutGameBoardSizePrompt = [&invalidInputValue]() {
				std::ostringstream str_os;

				/* prints only if { invalidInputValue } is TRUE */
				DrawOnlyWhen(str_os, invalidInputValue, Graphics::BoardSizeErrorPrompt);
				DrawAlways(str_os, Graphics::BoardInputPrompt);

				return str_os.str();
			};

			/* reset array { pregamesetup_status } */
			pregamesetup_status = pregameesetup_status_t{};

			clearScreen();
			DrawAlways(std::cout, Game::Graphics::AsciiArt2048);
			DrawAsOneTimeFlag(std::cout, noSave, Graphics::GameBoardNoSaveErrorPrompt);
			DrawAlways(std::cout, questionAboutGameBoardSizePrompt);

			receiveInputFromPlayer(std::cin);
			processPreGame();

			return flagInputErroneousChoice;
		}

		void endlessLoop()
		{
			while (soloLoop())
				;
		}

		void SetupNewGame(NewGameFlag ns)
		{
			noSave = (ns == NewGameFlag::NoPreviousSaveAvailable) ? true : false;
			endlessLoop();
		}

		load_gameboard_status_t initialiseContinueBoardArray() 
		{
			using namespace Loader;

			constexpr auto gameboard_data_filename = "../data/previousGame.txt";
			constexpr auto game_stats_data_filename = "../data/previousGameStats.txt";
			auto loaded_gameboard{ false };
			auto loaded_game_stats{ false };
			auto tempGBoard = GameBoard{ 1 };

			// Note: Reserved for gameboard.score and gameboard.moveCount!
			// TODO: Combine data into one resource file.
			auto score_and_movecount =
				std::tuple<decltype(tempGBoard.score), decltype(tempGBoard.moveCount)>{};
			std::tie(loaded_gameboard, tempGBoard) =
				load_GameBoard_data_from_file(gameboard_data_filename);
			std::tie(loaded_game_stats, score_and_movecount) =
				load_game_stats_from_file(game_stats_data_filename);
			std::tie(tempGBoard.score, tempGBoard.moveCount) = score_and_movecount;

			const auto all_files_loaded_ok = (loaded_gameboard && loaded_game_stats);

			return std::make_tuple(all_files_loaded_ok, tempGBoard);

		}
		void DoContinueOldGame() 
		{
			bool load_old_game_ok{ false };
			GameBoard oldGameBoard;

			std::tie(load_old_game_ok, oldGameBoard) = initialiseContinueBoardArray();
			
			if (load_old_game_ok) 
			{
				playGame(PlayGameFlag::ContinuePreviousGame, oldGameBoard);
			}
			else 
			{
				SetupNewGame(NewGameFlag::NoPreviousSaveAvailable);
			}
		}

	} // namespace

	namespace PreGameSetup
	{
		void SetupNewGame()
		{
			SetupNewGame(NewGameFlag::NewGameFlagNull);
		}

		void ContinueOldGame() 
		{
			DoContinueOldGame();
		}

	} // namespace PreGameSetup
} // namespace Game