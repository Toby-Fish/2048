#ifndef TILE_H
#define TILE_H

#include "global.hpp"

namespace Game
{

	struct tile_t
	{
		ull value{ 0 };
		bool blocked{ false };
	};

} // namespace Game

#endif // !TILE_H