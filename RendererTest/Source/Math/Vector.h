#pragma once

struct Vector3f
{
	union
	{
		struct { float x, y, z; };
		float components[3];
	};

	Vector3f() : x(0), y(0), z(0)
	{
	}

	Vector3f(float x, float y, float z)
		: x(x), y(y), z(z)
	{
	}

	inline float operator[](int componentIndex) const
	{
		return components[componentIndex];
	}

	inline float& operator[](int componentIndex)
	{
		return components[componentIndex];
	}

	inline Vector3f operator+(const Vector3f& other) const
	{
		return Vector3f(x + other.x, y + other.y, z + other.z);
	}

	inline Vector3f operator*(float scalar) const
	{
		return Vector3f(x * scalar, y * scalar, z * scalar);
	}

	inline Vector3f Lerp(const Vector3f& other, float t) const
	{
		return other * t + (*this) * (1 - t);
	}
};

inline Vector3f operator*(float scalar, const Vector3f& vector)
{
	return vector * scalar;
}

struct Vector4f
{
	union
	{
		struct { float x, y, z, w; };
		float components[4];
	};

	Vector4f() : x(0), y(0), z(0), w(0)
	{
	}

	Vector4f(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w)
	{
	}

	Vector4f(const Vector4f& other)
		: x(other.x), y(other.y), z(other.z), w(other.w)
	{
	}

	inline float operator[](int componentIndex) const
	{
		return components[componentIndex];
	}

	inline float& operator[](int componentIndex)
	{
		return components[componentIndex];
	}

	inline Vector4f operator+(const Vector4f& other) const
	{
		return Vector4f(x + other.x, y + other.y, z + other.z, w + other.w);
	}

	inline Vector4f operator+=(const Vector4f& other)
	{
		return *this = *this + other;
	}

	inline Vector4f operator-(const Vector4f& other) const
	{
		return Vector4f(x - other.x, y - other.y, z - other.z, w - other.w);
	}

	inline Vector4f operator*(float scalar) const
	{
		return Vector4f(x * scalar, y * scalar, z * scalar, w * scalar);
	}

	inline Vector4f operator/(float scalar) const
	{
		return Vector4f(x / scalar, y / scalar, z / scalar, w / scalar);
	}

	inline Vector4f operator/=(float scalar)
	{
		return *this = *this / scalar;
	}

	inline Vector4f Lerp(const Vector4f& other, float t) const
	{
		return other * t + (*this) * (1 - t);
	}
};

inline Vector4f operator*(float scalar, const Vector4f& vector)
{
	return vector * scalar;
}