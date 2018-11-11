#include "Structs.h"

namespace Structs {
	Float2::Float2()
		: x(0.0f), y(0.0f) {}

	Float2::Float2(float x, float y)
		: x(x), y(y) {}

	Float2 operator+(const Float2 &a, const Float2 &b) {
		Float2 res;

		res.x = a.x + b.x;
		res.y = a.y + b.y;

		return res;
	}

	Float2 operator-(const Float2 &a, const Float2 &b) {
		Float2 res;

		res.x = a.x - b.x;
		res.y = a.y - b.y;

		return res;
	}

	Float2 operator*(const Float2 &a, const Float2 &b) {
		Float2 res;

		res.x = a.x * b.x;
		res.y = a.y * b.y;

		return res;
	}

	Float2 operator/(const Float2 &a, const Float2 &b) {
		Float2 res;

		res.x = a.x / b.x;
		res.y = a.y / b.y;

		return res;
	}




	Rgba::Rgba()
		: val(0) {}

	Rgba::Rgba(int r, int g, int b, int a)
		: r((uint8_t)r), g((uint8_t)g), b((uint8_t)b), a((uint8_t)a) {}

	Rgba::Rgba(float r, float g, float b, float a)
		: r((uint8_t)(r * 255.0f)), g((uint8_t)(g * 255.0f)),
		b((uint8_t)(b * 255.0f)), a((uint8_t)(a * 255.0f)) {}




	Rect::Rect()
		: left(0.0f), top(0.0f), right(0.0f), bottom(0.0f)
	{}

	Rect::Rect(float left, float top, float right, float bottom)
		: left(left), top(top), right(right), bottom(bottom)
	{}
}