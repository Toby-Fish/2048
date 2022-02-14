#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "global.hpp"
#include <string>
#include <vector>
#include <tuple>

namespace Scoreboard
{

	struct Score 
	{
		std::string name;
		ull score;
		bool win;
		ull largestTile;
		long long moveCount;
		double duration;
	};

	using Scoreboard_t = std::vector<Score>;

	void saveScore(Score finalscore);

	bool operator>(const Score& a, const Score& b);
	// List of scores read until "exhausted".
	// Note: returns a tuple containing a std::vector<Score> of all read scores.
	using load_score_status_t = std::tuple<bool, Scoreboard_t>;
	load_score_status_t loadFromFileScore(std::string filename);

} // namespace Scoreboard

std::istream& operator>>(std::istream& is, Scoreboard::Score& s);
std::ostream& operator<<(std::ostream& os, Scoreboard::Score& s);

#endif // !SCOREBOARD_H