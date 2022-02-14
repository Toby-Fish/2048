#ifndef POINT2D_H
#define POINT2D_H

#include <tuple>

class point2D_t
{
	/* simple { x, y } data-structure = std::tuple<int, int> */
	using point_datatype_t = typename std::tuple<int, int>;
	point_datatype_t pointVector{ 0, 0 };

	explicit point2D_t(const point_datatype_t pt)
		: pointVector{ pt }
	{ }

public:
	enum class PointCoord
	{
		COORD_X,
		COORD_Y,
	};

	point2D_t() = default;
	point2D_t(const int x, const int y) : point2D_t(std::make_tuple(x, y)) { }

	template<PointCoord dimension>
	int get() const
	{
		return std::get<static_cast<int>(dimension)>(pointVector);
	}
	template<PointCoord dimension>
	void set(int val)
	{
		std::get<static_cast<int>(dimension)>(pointVector) = val;
	}

	point_datatype_t get() const
	{
		return pointVector;
	}
	void set(point_datatype_t val)
	{
		pointVector = val;
	}

	void set(const int x, const int y)
	{
		set(std::make_tuple(x, y));
	}

	point2D_t& operator+=(const point2D_t& rhs) 
	{
		this->pointVector = std::make_tuple(
			get<PointCoord::COORD_X>() + rhs.get<PointCoord::COORD_X>(),
			get<PointCoord::COORD_Y>() + rhs.get<PointCoord::COORD_Y>());
		return *this;
	}

	point2D_t& operator-=(const point2D_t& rhs) 
	{
		this->pointVector = std::make_tuple(
			get<PointCoord::COORD_X>() - rhs.get<PointCoord::COORD_X>(),
			get<PointCoord::COORD_Y>() - rhs.get<PointCoord::COORD_Y>());
		return *this;
	}
};

inline point2D_t operator+(point2D_t lhs, const point2D_t& rhs) {
	lhs += rhs;
	return lhs;
}

inline point2D_t operator-(point2D_t lhs, const point2D_t& rhs) {
	lhs -= rhs;
	return lhs;
}

#endif // !POINT2D_H