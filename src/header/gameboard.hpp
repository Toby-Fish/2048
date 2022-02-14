#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "tile.hpp"
#include <tuple>
#include <vector>

struct point2D_t;

namespace Game
{
	struct GameBoard
	{
		using tile_data_array_t = std::vector<tile_t>;

		/* { size_t } stands for gameboard size */
		using gameboard_data_array_t = std::tuple<size_t, tile_data_array_t>;
		gameboard_data_array_t gbda;

		bool win{ false };
		bool moved{ false };
		ull score{ 0 };
		/* { largestTile } stands for maximum score for a single brick in all tiles */
		ull largestTile{ 2 };
		long long moveCount{ -1 };

		GameBoard() = default;
		explicit GameBoard(ull boardsize);
		explicit GameBoard(ull boardsize, tile_data_array_t existboard);
	};

	/* the following four functions return basic GameBoard object information */
	size_t getSizeOfGameboard(GameBoard::gameboard_data_array_t gbda);
	tile_t getTileOnGameboard(GameBoard::gameboard_data_array_t& gbda, point2D_t pt);
	bool hasWonOnGameboard(GameBoard& gb);
	long long MoveCountOnGameBoard(const GameBoard& gb);

	/* the following four functions are single-step action auxiliary functions */
	void registerMoveByOneOnGameboard(GameBoard& gb);
	bool addTileOnGameboard(GameBoard& gb);
	void unblockTilesOnGameboard(GameBoard& gb);
	bool canMoveOnGameboard(GameBoard& gb);

	/* the following four functions are single-step action logic functions */
	void tumbleTilesUpOnGameboard(GameBoard& gb);
	void tumbleTilesDownOnGameboard(GameBoard& gb);
	void tumbleTilesLeftOnGameboard(GameBoard& gb);
	void tumbleTilesRightOnGameboard(GameBoard& gb);

	std::string printStateOfGameBoard(GameBoard gb);

} // namespace Game

#endif // !GAMEBOARD_H