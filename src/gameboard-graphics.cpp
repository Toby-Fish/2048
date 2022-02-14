#include "gameboard-graphics.hpp"
#include "gameboard.hpp"
#include "point2d.hpp"
#include "tile.hpp"
#include "tile-graphics.hpp"
#include <array>
#include <sstream>
#include <algorithm>

namespace Game
{

	namespace Gameboard
	{

		namespace Graphics
		{

			namespace
			{

				template<size_t num_of_bars>
				std::array<std::string, num_of_bars> makePatternedBars(int boardSize) 
				{
					auto temp_bars = std::array<std::string, num_of_bars>{};
					using bar_pattern_t = std::tuple<std::string, std::string, std::string>;

					const auto bar_pattern_list = { std::make_tuple("©°", "©Ð", "©´"),
												   std::make_tuple("©À", "©à", "©È"),
												   std::make_tuple("©¸", "©Ø", "©¼") };

					/* generate types of horizontal bars */
					const auto generate_x_bar_pattern = [boardSize](const bar_pattern_t t) 
					{
						enum { PATTERN_HEAD, PATTERN_MID, PATTERN_TAIL };
						constexpr auto sp = "  ";
						constexpr auto separator = "©¤©¤©¤©¤©¤©¤";

						std::ostringstream temp_richtext;
						temp_richtext << sp << std::get<PATTERN_HEAD>(t);

						for (auto i = 0; i < boardSize; i++) 
						{
							const auto is_not_last_column = (i < boardSize - 1);
							temp_richtext << separator
								<< (is_not_last_column ? std::get<PATTERN_MID>(t) :
									std::get<PATTERN_TAIL>(t));
						}
						temp_richtext << "\n";

						return temp_richtext.str();
					};

					std::transform(std::begin(bar_pattern_list), std::end(bar_pattern_list),
						std::begin(temp_bars), generate_x_bar_pattern);

					return temp_bars;
				}

				std::string drawGameBoard(GameBoard::gameboard_data_array_t gbda) 
				{
					enum 
					{ 
						TOP_BAR, 
						XN_BAR, 
						BASE_BAR, 
						MAX_TYPES_OF_BARS 
					};
					const int boardSize = getSizeOfGameboard(gbda);
					const auto vertibar = makePatternedBars<MAX_TYPES_OF_BARS>(boardSize);
					std::ostringstream str_os;

					for (auto x = 0; x < boardSize; x++) 
					{
						const auto is_first_row = (x == 0);
						str_os << (is_first_row ? 
							std::get<TOP_BAR>(vertibar) : std::get<XN_BAR>(vertibar));

						for (auto y = 0; y < boardSize; y++) 
						{
							const auto is_first_col = (y == 0);
							const auto sp = (is_first_col ? "  " : " ");
							const auto tile = getTileOnGameboard(gbda, point2D_t{ x, y });
							str_os << sp;
							str_os << "©¦ ";
							str_os << drawTileString(tile);
						}

						str_os << " ©¦";
						str_os << "\n";
					}

					str_os << std::get<BASE_BAR>(vertibar);
					str_os << "\n";

					return str_os.str();
				}

			} // namespace

			std::string GameBoardTextOutput(GameBoard gb)
			{
				return drawGameBoard(gb.gbda);
			}

		} // namespace Graphics

	} // namespace Gameboard

} // namespace Game