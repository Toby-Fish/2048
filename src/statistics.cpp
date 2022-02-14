#include "statistics.hpp"
#include "statistics-graphics.hpp"
#include "scoreboard.hpp"
#include "scoreboard-graphics.hpp"
#include <fstream>

namespace Statistics
{

    namespace
    {
        total_game_stats_t generateStatsFromInputData(std::istream& is) 
        {
            total_game_stats_t stats;
            is >> stats;

            return stats;
        }

        Scoreboard::Graphics::finalscore_display_data_t
            makeFinalScoreDisplayData(Scoreboard::Score finalscore) 
        {
            const auto fsdd = std::make_tuple(
                std::to_string(finalscore.score), std::to_string(finalscore.largestTile),
                std::to_string(finalscore.moveCount), secondsFormat(finalscore.duration));
            
            return fsdd;
        }

        std::string receiveInputPlayerName(std::istream& is) 
        {
            std::string playerName;
            is >> playerName;
            return playerName;
        }

        bool generateFilefromStatsData(std::ostream& os, total_game_stats_t stats) 
        {
            os << stats;

            return true;
        }
        bool saveToFileEndGameStatistics(std::string filename, total_game_stats_t s) 
        {
            std::ofstream filedata(filename);

            return generateFilefromStatsData(filedata, s);
        }

    } // namespace

    /* using load_stats_status_t = std::tuple<bool, total_game_state_t>; */
    load_stats_status_t loadFromFileStatistics(std::string filename) 
    {
        std::ifstream statistics(filename);

        if (statistics) 
        {
            total_game_stats_t state = generateStatsFromInputData(statistics);
            return load_stats_status_t{ true, state };
        }
        return load_stats_status_t{ false, total_game_stats_t{} };
    }

    ull loadBestScore() 
    {
        total_game_stats_t stats;
        bool stats_file_loaded{ false };
        ull tmpScore{ 0 };

        std::tie(stats_file_loaded, stats) =
            loadFromFileStatistics("../data/statistics.txt");

        if (stats_file_loaded) 
        {
            tmpScore = stats.bestScore;
        }

        return tmpScore;
    }

    void saveEndGameStats(Scoreboard::Score finalscore) 
    {
        total_game_stats_t stats;
        // need some sort of stats data values only
        // no need to care if file loaded successfully or not
        std::tie(std::ignore, stats) =
            loadFromFileStatistics("../data/statistics.txt");
        stats.bestScore =
            stats.bestScore < finalscore.score ? finalscore.score : stats.bestScore;
        stats.gameCount++;
        stats.winCount = finalscore.win ? stats.winCount + 1 : stats.winCount;
        stats.totalMoveCount += finalscore.moveCount;
        stats.totalDuration += finalscore.duration;

        saveToFileEndGameStatistics("../data/statistics.txt", stats);
    }

    void createFinalScoreAndEndGameDataFile(std::ostream& os, std::istream& is,
        Scoreboard::Score finalscore) 
    {
        const auto finalscoreDisplayData = makeFinalScoreDisplayData(finalscore);
        DrawAlways(os, DataSuppliment(finalscoreDisplayData,
            Scoreboard::Graphics::EndGameStatisticsPrompt));

        DrawAlways(os, Graphics::AskForPlayerNamePrompt);
        const auto playerName = receiveInputPlayerName(is);
        finalscore.name = playerName;

        Scoreboard::saveScore(finalscore);
        saveEndGameStats(finalscore);
        DrawAlways(os, Graphics::MessageScoreSavedPrompt);
    }

} // namespace Statistics

using namespace Statistics;

std::istream& operator>>(std::istream& is, total_game_stats_t& s) 
{
    is >> s.bestScore >> s.gameCount >> s.winCount >> s.totalMoveCount >>
        s.totalDuration;

    return is;
}

std::ostream& operator<<(std::ostream& os, total_game_stats_t& s) 
{
    os << s.bestScore << "\n"
        << s.gameCount << "\n"
        << s.winCount << "\n"
        << s.totalMoveCount << "\n"
        << s.totalDuration;

    return os;
}