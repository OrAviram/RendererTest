#pragma once
#include <Common.h>

#undef RGB
#define BIT(x) (1 << x)
#define RGBA(red, green, blue, alpha) (BIT(0) * (uint32)(blue) + BIT(8) * (uint32)(green) + BIT(16) * (uint32)(red) + BIT(24) * (uint32)(alpha))
#define RGB(red, green, blue) RGBA(red, green, blue, 255)

#define ALPHA(color) (((color) & 0xFF000000) >> 24)
#define RED(color) (((color) & 0x00FF0000) >> 16)
#define GREEN(color) (((color) & 0x0000FF00) >> 8)
#define BLUE(color) (((color) & 0x000000FF))

typedef uint32 Color32;

struct Color4f
{
	static const Color4f White;
	static const Color4f Black;
	static const Color4f Red;
	static const Color4f Green;
	static const Color4f Blue;
	static const Color4f Zero;
	
	union
	{
		struct { float red, green, blue, alpha; };
		float components[4];
	};
	
	Color4f() : red(0), green(0), blue(0), alpha(0)
	{
	}

	Color4f(float red, float green, float blue, float alpha)
		: red(red), green(green), blue(blue), alpha(alpha)
	{
	}

	Color4f(Color32 color)
	{
		red = RED(color) / 255.f;
		green = GREEN(color) / 255.f;
		blue = BLUE(color) / 255.f;
		alpha = ALPHA(color) / 255.f;
	}

	explicit operator Color32() const
	{
		return RGBA(red * 255, green * 255, blue * 255, alpha * 255);
	}

	inline float operator[](int componentIndex) const
	{
		return components[componentIndex];
	}

	inline float& operator[](int componentIndex)
	{
		return components[componentIndex];
	}

	inline Color4f operator+(const Color4f& other) const
	{
		return Color4f(red + other.red, green + other.green, blue + other.blue, alpha + other.alpha);
	}

	inline Color4f operator*(const Color4f& other) const
	{
		return Color4f(red * other.red, green * other.green, blue * other.blue, alpha * other.alpha);
	}
	
	inline Color4f operator*(float scalar) const
	{
		return Color4f(red * scalar, green * scalar, blue * scalar, alpha * scalar);
	}

	Color4f Lerp(const Color4f& other, float t) const
	{
		return other * t + (*this) * (1 - t);
	}

	static Color4f Lerp(const Color4f a, const Color4f b, float t)
	{
		return a.Lerp(b, t);
	}
};

inline Color4f operator*(float scalar, const Color4f& color)
{
	return Color4f(color.red * scalar, color.green * scalar, color.blue * scalar, color.alpha * scalar);
}