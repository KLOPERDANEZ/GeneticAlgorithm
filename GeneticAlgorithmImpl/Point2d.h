#pragma once

class Point2d
{

public:

	Point2d() = default;
	Point2d(const double x, const double y) : x_(x), y_(y)
	{}

	Point2d& operator=(const Point2d& other) = default;

	double x() const
	{
		return x_;
	}

	double y() const
	{
		return y_;
	}

private:
	double x_;
	double y_;
};
