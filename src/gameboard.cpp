#include "gameboard.hpp"
#include "tile.hpp"
#include "point2d.hpp"
#include <chrono>
#include <random>
#include <sstream>
#include <algorithm>
#include <array>


namespace Game
{

	namespace
	{
		/* using gameboard_data_array_t = std::tuple<size_t, tile_data_array_t>; */
		using gameboard_data_array_t = GameBoard::gameboard_data_array_t;
		enum gameboard_data_array_fields
		{
			IDX_BOARDSIZE,
			IDX_TILE_ARRAY,
			MAX_NO_INDEX
		};

		struct gameboard_data_point_t
		{
			static int point2D_to_1D_index(gameboard_data_array_t& gbda, point2D_t pt)
			{
				int x, y;
				std::tie(x, y) = pt.get();

				return x * getSizeOfGameboard(gbda) + y;
			}

			tile_t operator()(gameboard_data_array_t& gbda, point2D_t& pt) const
			{
				return std::get<IDX_TILE_ARRAY>(gbda)[point2D_to_1D_index(gbda, pt)];
			}
			tile_t& operator()(gameboard_data_array_t& gbda, point2D_t& pt)
			{
				return std::get<IDX_TILE_ARRAY>(gbda)[point2D_to_1D_index(gbda, pt)];
			}
		};

		bool getTileBlockedOnGameboardDataArray(gameboard_data_array_t gbda,
                                        point2D_t pt) 
		{
			return gameboard_data_point_t{}(gbda, pt).blocked;
		}
		void setTileOnGameboardDataArray(gameboard_data_array_t& gbda, point2D_t pt,
			tile_t tile) 
		{
			gameboard_data_point_t{}(gbda, pt) = tile;
		}
		void setTileValueOnGameboardDataArray(gameboard_data_array_t& gbda,
			point2D_t pt, ull value)
		{
			gameboard_data_point_t{}(gbda, pt).value = value;
		}
		ull getTileValueOnGameboardDataArray(gameboard_data_array_t& gbda,
			point2D_t pt) 
		{
			return gameboard_data_point_t{}(gbda, pt).value;
		}

		std::vector<size_t>
		collectFreeTilesOnGameboardDataArray(gameboard_data_array_t gbda) 
		{
			std::vector<size_t> freeTiles;
			auto index_counter{ 0 };
			for (const auto t : std::get<IDX_TILE_ARRAY>(gbda)) 
			{
				if (!t.value) 
				{
					freeTiles.push_back(index_counter);
				}
				index_counter++;
			}

			return freeTiles;
		} // collectFreeTilesOnGameboardDataArray

		gameboard_data_array_t
		unblockTilesOnGameboardDataArray(gameboard_data_array_t gbda) 
		{
			/* using tile_data_array_t = std::vector<tile_t>; */
			using tile_data_array_t = GameBoard::tile_data_array_t;
			auto new_board_data_array =
				tile_data_array_t(std::get<IDX_TILE_ARRAY>(gbda).size());

			std::transform(std::begin(std::get<IDX_TILE_ARRAY>(gbda)),
				std::end(std::get<IDX_TILE_ARRAY>(gbda)),
				std::begin(new_board_data_array), [](const tile_t t) 
				{
					return tile_t{ t.value, false };
				});

			return gameboard_data_array_t{ std::get<IDX_BOARDSIZE>(gbda),
										  new_board_data_array };
		} // unblockTilesOnGameboardDataArray

		bool isPointInBoardArea(point2D_t pt, int boardSize) 
		{
			int x, y;
			std::tie(x, y) = pt.get();
			return !(y < 0 || y > boardSize - 1 || x < 0 || x > boardSize - 1);
		}
		bool canMoveOnGameboardDataArray(gameboard_data_array_t& gbda) 
		{
			auto indexCounter{ 0 };

			const auto canMoveToOffset = [=, &indexCounter, &gbda](const tile_t t) 
			{
				const int boardSize = getSizeOfGameboard(gbda);
				const auto currentPoint =
					point2D_t{ indexCounter / boardSize, indexCounter % boardSize };
				indexCounter++;
				const auto listOfOffsets = { point2D_t{1, 0}, point2D_t{0, 1} };
				const auto currentPointValue = t.value;

				const auto offsetInRangeWithSameValue = [=, &gbda](const point2D_t offset) 
				{
					const auto offsetCheck = 
					{
						currentPoint + offset,		/* Positive adjacent check */
						currentPoint - offset		/* Negative adjacent Check */
					};	
					for (const auto currentOffset : offsetCheck) 
					{
						if (isPointInBoardArea(currentOffset, boardSize)) 
						{
							return getTileValueOnGameboardDataArray(gbda, currentOffset) ==
								currentPointValue;
						}
					}

					return false;
				}; // offset_in_range_with_same_value

				return ((currentPointValue == 0u) ||
					std::any_of(std::begin(listOfOffsets), std::end(listOfOffsets),
						offsetInRangeWithSameValue));
			}; // can_move_to_offset

			return std::any_of(std::begin(std::get<IDX_TILE_ARRAY>(gbda)),
				std::end(std::get<IDX_TILE_ARRAY>(gbda)), canMoveToOffset);
		} // canMoveOnGameboardDataArray

		class RandInt 
		{
		public:
			using clock = std::chrono::system_clock;
			RandInt() : dist{ 0, std::numeric_limits<int>::max() } 
			{
				seed(clock::now().time_since_epoch().count());
			}
			RandInt(const int low, const int high) : dist{ low, high } 
			{
				seed(clock::now().time_since_epoch().count());
			}
			int operator()() { return dist(re); }
			void seed(const unsigned int s) { re.seed(s); }

		private:
			std::minstd_rand re;
			std::uniform_int_distribution<> dist;
		};
		bool addTileOnGameboardDataArray(gameboard_data_array_t& gbda) 
		{
			/* 
				modify { CHANCE_OF_VALUE_FOUR_OVER_TWO } to 
				change the probability of the numbers 2 and 4
			*/
			constexpr auto CHANCE_OF_VALUE_FOUR_OVER_TWO = 75;

			const auto index_list_of_free_tiles =
				collectFreeTilesOnGameboardDataArray(gbda);

			if (!index_list_of_free_tiles.size()) 
			{
				/* game over */
				return true;
			}

			const int boardSize = getSizeOfGameboard(gbda);
			const int rand_selected_index = index_list_of_free_tiles.at(
				RandInt{}() % index_list_of_free_tiles.size());
			const auto rand_index_as_point_t =
				point2D_t{ rand_selected_index / boardSize, rand_selected_index % boardSize };
			const auto value_four_or_two =
				RandInt{}() % 100 > CHANCE_OF_VALUE_FOUR_OVER_TWO ? 4 : 2;
			setTileValueOnGameboardDataArray(gbda, rand_index_as_point_t,
				value_four_or_two);

			return false;
		}

		enum class COLLASPE_OR_SHIFT_T 
		{
			ACTION_NONE,
			ACTION_COLLASPE,
			ACTION_SHIFT,
			MAX_NUM_OF_ACTIONS
		};

		/*
			delta_t.first = focal point,
			delta_t.second = offset distance
		*/
		using delta_t = std::pair<point2D_t, point2D_t>;
		
		using bool_collaspe_shift_t = std::tuple<bool, COLLASPE_OR_SHIFT_T>;
		bool_collaspe_shift_t
		collaspedOrShiftedTilesOnGameboardDataArray(gameboard_data_array_t& gbda,
				delta_t dt_point) 
		{
			const auto currentTile = getTileOnGameboard(gbda, dt_point.first);
			const auto targetTile =
				getTileOnGameboard(gbda, dt_point.first + dt_point.second);
			const auto valueExistInTargetPoint = targetTile.value;
			const auto isValueSameAsTargetValue =
				(currentTile.value == targetTile.value);
			const auto noTilesAreBlocked =
				(!currentTile.blocked && !targetTile.blocked);
			const auto is_there_a_current_value_but_no_target_value =
				(currentTile.value && !targetTile.value);
			const auto doCollapse =
				(valueExistInTargetPoint && isValueSameAsTargetValue &&
					noTilesAreBlocked);
			const auto doShift = is_there_a_current_value_but_no_target_value;
			const auto action_taken = (doCollapse || doShift);

			if (doCollapse) 
			{
				return std::make_tuple(action_taken, COLLASPE_OR_SHIFT_T::ACTION_COLLASPE);
			}
			else if (doShift) 
			{
				return std::make_tuple(action_taken, COLLASPE_OR_SHIFT_T::ACTION_SHIFT);
			}

			return std::make_tuple(action_taken, COLLASPE_OR_SHIFT_T::ACTION_NONE);
		}

		bool collaspeTilesOnGameboardDataArray(gameboard_data_array_t& gbda,
			delta_t dt_point) 
		{
			tile_t currentTile = getTileOnGameboard(gbda, dt_point.first);
			tile_t targetTile =
				getTileOnGameboard(gbda, dt_point.first + dt_point.second);

			currentTile.value = 0;
			targetTile.value *= 2;
			targetTile.blocked = true;

			setTileOnGameboardDataArray(gbda, dt_point.first, currentTile);
			setTileOnGameboardDataArray(gbda, dt_point.first + dt_point.second,
				targetTile);

			return true;
		}
		bool shiftTilesOnGameboardDataArray(gameboard_data_array_t& gbda,
			delta_t dt_point)
		{
			tile_t currentTile = getTileOnGameboard(gbda, dt_point.first);
			tile_t targetTile =
				getTileOnGameboard(gbda, dt_point.first + dt_point.second);

			targetTile.value = currentTile.value;
			currentTile.value = 0;

			setTileOnGameboardDataArray(gbda, dt_point.first, currentTile);
			setTileOnGameboardDataArray(gbda, dt_point.first + dt_point.second,
				targetTile);

			return true;
		}
		bool updateGameBoardStats(GameBoard& gb, ull target_tile_value)
		{
			gb.score += target_tile_value;

			/* Discover the largest tile value on the gameboard */
			gb.largestTile = std::max(gb.largestTile, target_tile_value);

			/* Discover the winning tile value on the gameboard */
			if (!hasWonOnGameboard(gb))
			{
				constexpr auto GAME_TILE_WINNING_SCORE = 2048;
				if (target_tile_value == GAME_TILE_WINNING_SCORE)
				{
					gb.win = true;
				}
			}
			return true;
		}

		bool checkRecursiveOffsetInGameBounds(delta_t dt_point, int boardSize) 
		{
			int focal_x, focal_y, offset_x, offset_y;
			std::tie(focal_x, focal_y) = dt_point.first.get();
			std::tie(offset_x, offset_y) = dt_point.second.get();

			const auto positiveDirection = (offset_y + offset_x == 1);
			const auto negativeDirection = (offset_y + offset_x == -1);
			const auto is_positive_y_direction_flagged = (offset_y == 1);
			const auto is_negative_y_direction_flagged = (offset_y == -1);
			const auto isInsideOuterBounds =
				(positiveDirection &&
					(is_positive_y_direction_flagged ? focal_y : focal_x) < boardSize - 2);
			const auto isInsideInnerBounds =
				(negativeDirection && (is_negative_y_direction_flagged ? focal_y : focal_x) > 1);

			return (isInsideOuterBounds || isInsideInnerBounds);
		}

		void moveOnGameboard(GameBoard& gb, delta_t dt_point) 
		{
			auto didGameboardCollaspeOrShift = bool{false};
			auto actionTaken = COLLASPE_OR_SHIFT_T::ACTION_NONE;

			std::tie(didGameboardCollaspeOrShift, actionTaken) =
				collaspedOrShiftedTilesOnGameboardDataArray(gb.gbda, dt_point);

			if (didGameboardCollaspeOrShift) 
			{
				gb.moved = true;
				if (actionTaken == COLLASPE_OR_SHIFT_T::ACTION_COLLASPE) 
				{
					collaspeTilesOnGameboardDataArray(gb.gbda, dt_point);
					const auto targetTile = getTileOnGameboard(
						gb.gbda, dt_point.first + dt_point.second);
					updateGameBoardStats(gb, targetTile.value);
				}
				if (actionTaken == COLLASPE_OR_SHIFT_T::ACTION_SHIFT) 
				{
					shiftTilesOnGameboardDataArray(gb.gbda, dt_point);
				}
			}
			if (checkRecursiveOffsetInGameBounds(
				dt_point, getSizeOfGameboard(gb.gbda))) 
			{
				moveOnGameboard(
					gb, std::make_pair(dt_point.first + dt_point.second, dt_point.second));
			}
		}

		void doTumbleTilesUpOnGameboard(GameBoard& gb) 
		{
			const int boardSize = getSizeOfGameboard(gb.gbda);
			for (auto x = 1; x < boardSize; x++) 
			{
				auto y = 0;
				while (y < boardSize) 
				{
					const auto current_point = point2D_t{ x, y };
					if (getTileValueOnGameboardDataArray(gb.gbda, current_point)) 
					{
						moveOnGameboard(gb, std::make_pair(current_point, point2D_t{ -1, 0 }));
					}
					y++;
				}
			}
		}
		void doTumbleTilesDownOnGameboard(GameBoard& gb) 
		{
			const int boardSize = getSizeOfGameboard(gb.gbda);
			for (auto x = boardSize - 2; x >= 0; x--) 
			{
				auto y = 0;
				while (y < boardSize) 
				{
					const auto current_point = point2D_t{ x, y };
					if (getTileValueOnGameboardDataArray(gb.gbda, current_point)) {
						moveOnGameboard(gb, std::make_pair(current_point, point2D_t{ 1, 0 }));
					}
					y++;
				}
			}
		}
		void doTumbleTilesLeftOnGameboard(GameBoard& gb)
		{
			const int boardSize = getSizeOfGameboard(gb.gbda);
			for (auto y = 1; y < boardSize; y++)
			{
				auto x = 0;
				while (x < boardSize)
				{
					const auto current_point = point2D_t{ x, y };
					if (getTileValueOnGameboardDataArray(gb.gbda, current_point)) {
						moveOnGameboard(gb, std::make_pair(current_point, point2D_t{ 0, -1 }));
					}
					x++;
				}
			}
		}
		void doTumbleTilesRightOnGameboard(GameBoard& gb)
		{
			const int boardSize = getSizeOfGameboard(gb.gbda);
			for (auto y = boardSize - 2; y >= 0; y--)
			{
				auto x = 0;
				while (x < boardSize)
				{
					const auto current_point = point2D_t{ x, y };
					if (getTileValueOnGameboardDataArray(gb.gbda, current_point)) {
						moveOnGameboard(gb, std::make_pair(current_point, point2D_t{ 0, 1 }));
					}
					x++;
				}
			}
		}

		std::string printStateOfGameBoardDataArray(gameboard_data_array_t gbda) 
		{
			const int boardSize = getSizeOfGameboard(gbda);
			std::ostringstream os;

			for (auto x = 0; x < boardSize; x++) 
			{
				for (auto y = 0; y < boardSize; y++) 
				{
					const auto current_point = point2D_t{ x, y };
					os << getTileValueOnGameboardDataArray(gbda, current_point) << ":"
						<< getTileBlockedOnGameboardDataArray(gbda, current_point) << ",";
				}
				os << "\n";
			}

			return os.str();
		}

	} // namespace

	/* the following two constructors are used to create GameBoard objects */
	GameBoard::GameBoard(ull boardsize)
		: GameBoard{ boardsize, tile_data_array_t(boardsize * boardsize)}
	{ }
	GameBoard::GameBoard(ull boardsize, tile_data_array_t existboard)
		: gbda{ boardsize, existboard } 
	{ }

	/* the following four functions return basic GameBoard object information */
	size_t getSizeOfGameboard(gameboard_data_array_t gbda) 
	{
		return std::get<IDX_BOARDSIZE>(gbda);
	}
	tile_t getTileOnGameboard(gameboard_data_array_t& gbda, point2D_t pt) 
	{
		return gameboard_data_point_t{}(gbda, pt);
	}
	bool hasWonOnGameboard(GameBoard& gb)
	{
		return gb.win;
	}
	long long MoveCountOnGameBoard(const GameBoard& gb)
	{
		return gb.moveCount;
	}

	/* the following four functions are single-step action auxiliary functions */
	void registerMoveByOneOnGameboard(GameBoard& gb)
	{
		gb.moveCount++;
		gb.moved = false;
	}
	bool addTileOnGameboard(GameBoard& gb)
	{
		return addTileOnGameboardDataArray(gb.gbda);
	}
	void unblockTilesOnGameboard(GameBoard& gb) 
	{
		gb.gbda = unblockTilesOnGameboardDataArray(gb.gbda);
	}
	bool canMoveOnGameboard(GameBoard& gb) 
	{
		return canMoveOnGameboardDataArray(gb.gbda);
	}

	/* the following four functions are single-step action logic functions */
	void tumbleTilesUpOnGameboard(GameBoard& gb) 
	{
		doTumbleTilesUpOnGameboard(gb);
	}
	void tumbleTilesDownOnGameboard(GameBoard& gb) 
	{
		doTumbleTilesDownOnGameboard(gb);
	}
	void tumbleTilesLeftOnGameboard(GameBoard& gb) 
	{
		doTumbleTilesLeftOnGameboard(gb);
	}
	void tumbleTilesRightOnGameboard(GameBoard& gb) 
	{
		doTumbleTilesRightOnGameboard(gb);
	}

	std::string printStateOfGameBoard(GameBoard gb)
	{
		return printStateOfGameBoardDataArray(gb.gbda);
	}

} // namespace Game