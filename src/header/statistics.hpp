#ifndef STATISTICS_H
#define STATISTICS_H

#include "global.hpp"
#include <tuple>
#include <string>
#include <iosfwd>

namespace Scoreboard 
{
	struct Score;
}

namespace Statistics
{

	struct total_game_stats_t 
	{
		ull bestScore{};
		ull totalMoveCount{};
		int gameCount{};
		double totalDuration{};
		int winCount{};
	};

	using load_stats_status_t = std::tuple<bool, total_game_stats_t>;
	load_stats_status_t loadFromFileStatistics(std::string filename);

	ull loadBestScore();
	void saveEndGameStats(Scoreboard::Score finalscore);
	void createFinalScoreAndEndGameDataFile(std::ostream& os, std::istream& is,
		Scoreboard::Score finalscore);

} // Statistics

std::istream& operator>>(std::istream& is, Statistics::total_game_stats_t& s);
std::ostream& operator<<(std::ostream& os, Statistics::total_game_stats_t& s);

#endif // !STATISTICS_H
