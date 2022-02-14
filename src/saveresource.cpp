#include "saveresource.hpp"
#include "gameboard.hpp"
#include <fstream>

namespace Game
{

	namespace Saver
	{

		namespace
		{
			bool generateFilefromPreviousGameStateData(std::ostream& os,
				const GameBoard& gb) 
			{
				os << printStateOfGameBoard(gb);

				return true;
			}
			void saveToFilePreviousGameStateData(std::string filename,
				const GameBoard& gb) 
			{
				std::ofstream stateFile(filename, std::ios_base::app);
				generateFilefromPreviousGameStateData(stateFile, gb);
			}


			bool generateFilefromPreviousGameStatisticsData(std::ostream& os,
				const GameBoard& gb) 
			{
				os << gb.score << ":" << MoveCountOnGameBoard(gb);

				return true;
			}
			void saveToFilePreviousGameStatisticsData(std::string filename,
				const GameBoard& gb) 
			{
				std::ofstream stats(filename, std::ios_base::app);
				generateFilefromPreviousGameStatisticsData(stats, gb);
			}

		} // namespace

		void saveGamePlayState(GameBoard gb)
		{
			constexpr auto filename_game_data_state = "../data/previousGame.txt";
			constexpr auto filename_game_data_statistics = "../data/previousGameStats.txt";
			std::remove(filename_game_data_state);
			std::remove(filename_game_data_statistics);

			saveToFilePreviousGameStateData(filename_game_data_state, gb);
			saveToFilePreviousGameStatisticsData(filename_game_data_statistics, gb);
		}

	} // namespace Saver

} // namespace Game