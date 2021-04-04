#pragma once


#include <cmath>
#include <iostream>


template <class T>
struct VectorTwo
{
	union
	{
		struct
		{
			T u, v;
		};

		struct
		{
			T x, y;
		};

		T raw[2];
	};
	VectorTwo ()
	: u(0), v(0) { }

	VectorTwo(T _u, T _v)
	: u(_u), v(_v) { }

	VectorTwo<T> operator +(const VectorTwo<T> &Vector) const
	{
		return VectorTwo<T>(u + Vector.u, v + Vector.v);
	}

	VectorTwo<T> operator -(const VectorTwo<T> &Vector) const
	{
		return VectorTwo<T>(u - Vector.u, v - Vector.v);
	}

	VectorTwo<T> operator *(float Float) const
	{
		return VectorTwo<T>(u * Float, v * Float);
	}

	template <class > friend std::ostream& operator<<(std::ostream &S, VectorTwo<T> &V);
};

template <class T>
struct VectorThree
{
	union
	{
		struct
		{
			T x, y, z;
		};

		struct
		{
			T i_vert, i_uv, i_norm;
		};

		T raw[3];
	};

	VectorThree ()
	: x(0), y(0), z(0) { }

	VectorThree(T _x, T _y, T _z)
	: x(_x), y(_y), z(_z) { }

	VectorThree<T> operator ^(const VectorThree<T> &Vector) const
	{
		return VectorThree<T>(y * Vector.z - z * Vector.y, z * Vector.x - x * Vector.z, x * Vector.y - y * Vector.x);
	}

	VectorThree<T> operator +(const VectorThree<T> &Vector) const
	{
		return VectorThree<T>(x + Vector.x, y + Vector.y, z + Vector.z);
	}

	VectorThree<T> operator -(const VectorThree<T> &Vector) const
	{
		return VectorThree<T>(x - Vector.x, y - Vector.y, z - Vector.z);
	}

	VectorThree<T> operator *(float Float) const
	{
		return VectorThree<T>(x * Float, y * Float, z * Float);
	}

	T operator *(const VectorThree<T> &Vector) const
	{
		return x * Vector.x + y * Vector.y + z * Vector.z;
	}

	[[nodiscard]] float norm () const
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	VectorThree<T> & normalize(T Line = 1)
	{
		*this = (*this) * (Line / norm());
		return *this;
	}

	template <class > friend std::ostream& operator<<(std::ostream &S, VectorThree<T> &V);
};

using VectorTwoFloat = VectorTwo<float>;
using VectorTwoInt = VectorTwo<int>;
using VectorThreeFloat = VectorThree<float>;
using VectorThreeInt = VectorThree<int>;

template <class T> std::ostream& operator<<(std::ostream &S, VectorTwo<T> &V)
{
	S << "(" << V.x << ", " << V.y << ")\n";
	return S;
}

template <class T> std::ostream& operator<<(std::ostream &S, VectorThree<T> &V)
{
	S << "(" << V.x << ", " << V.y << ", " << V.z << ")\n";
	return S;
}
