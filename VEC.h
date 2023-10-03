#pragma once
#include <cmath>

template <typename T> struct vec2
{
	union
	{
		struct { T x, y; };
		struct { T u, v; };
		T raw[2];
	};
	vec2 (T _u, T _v) : u(_u), v(_v){}
	vec2 () : u(0),v(0){}

	inline vec2<T>& operator +(const vec2<T>& u) const { return vec2<T>(u.x + x, u.y + y); }
	inline vec2<T>& operator -()				 const { return vec2<T>(-x, -y); }
	inline vec2<T>& operator -(const vec2<T>& u) const { return vec2<T>(*this+(-u)); }
	inline vec2<T>& operator *(const vec2<T>& u) const { return vec2<T>(x * u.x, y * u.y); }
	inline vec2<T>& operator *(const float& t)	 const { return vec2<T>(x * t, y * t); }
};
template <typename T> struct vec3
{
	union
	{
		struct { T ivert, iuv, inorm; };
		struct { T x, y, z; };
		T raw[3];
	};
	vec3() : x(0), y(0), z(0) {}
	vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

	inline vec3<T> operator^ (const vec3<T>& v) const { return vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	inline T operator* (const vec3<T>& v)        const { return v.x * x + v.y * y + v.z * z; }
	inline vec3<T> operator* (const float& t)   const { return vec3<T>(t*x, t*y, t*z); }
	inline vec3<T> operator+ (const vec3<T> v)  const { return vec3<T>(v.x + x, v.y + y, v.z + z); }
	inline vec3<T> operator-()					 const { return vec3<T>(-x, -y, -z); }
	inline vec3<T> operator-(const vec3<T>& u)  const { return *this + (-u); }

	float norm() const { return std::sqrt(x * x + y * y + z * z); }
	vec3<T> normalize(T l = 1) { *this = (*this) * (l / norm()); return *this; }
};

typedef vec2<int> pixel;
typedef vec2<int> vec2i;
typedef vec2<float> vec2f;
typedef vec3<int> vec3i;
typedef vec3<float> vec3f;