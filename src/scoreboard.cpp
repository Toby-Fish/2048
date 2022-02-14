#include "scoreboard.hpp"
#include <fstream>
#include <algorithm>

namespace
{
	using namespace Scoreboard;

	/* using Scoreboard_t = std::vector<Score>; */
	bool generateFilefromScoreData(std::ostream& os, Score score) 
	{
		os << score;

		return true;
	}
	bool saveToFileScore(std::string filename, Score s) 
	{
		std::ofstream os(filename, std::ios_base::app);
		return generateFilefromScoreData(os, s);
	}

	Scoreboard_t generateScorefromFileData(std::istream& is) 
	{
		Score tempscore{};
		Scoreboard_t scoreList{};

		while (is >> tempscore) 
		{
			scoreList.push_back(tempscore);
		};

		return scoreList;
	}

} // namespace

namespace Scoreboard
{

	void saveScore(Score finalscore) 
	{
		saveToFileScore("../data/scores.txt", finalscore);
	}

	bool operator>(const Score& lhs, const Score& rhs) 
	{
		return lhs.score > rhs.score;
	}

	load_score_status_t loadFromFileScore(std::string filename) 
	{
		std::ifstream scores(filename);

		if (scores) 
		{
			Scoreboard_t scoreList = generateScorefromFileData(scores);

			std::sort(std::begin(scoreList), std::end(scoreList),
				std::greater<Score>{});

			return load_score_status_t{ true, scoreList };
		}

		return load_score_status_t{ false, Scoreboard_t{} };
	}

} // namespace Scoreboard

using namespace Scoreboard;

std::istream& operator>>(std::istream& is, Score& s) 
{
	is >> s.name >> s.score >> s.win >> s.moveCount >> s.largestTile >>
		s.duration;

	return is;
}

std::ostream& operator<<(std::ostream& os, Score& s)
{
	os << "\n"
		<< s.name << " " << s.score << " " << s.win << " " << s.moveCount << " "
		<< s.largestTile << " " << s.duration;

	return os;
}