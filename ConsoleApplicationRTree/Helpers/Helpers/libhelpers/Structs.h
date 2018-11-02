#pragma once

#include <cstdint>

namespace Structs {
	struct Float2 {
		union {
			float m[2];

			struct {
				float x;
				float y;
			};
		};

		Float2();
		Float2(float x, float y);

		friend Float2 operator+(const Float2 &a, const Float2 &b);
		friend Float2 operator-(const Float2 &a, const Float2 &b);
		friend Float2 operator*(const Float2 &a, const Float2 &b);
		friend Float2 operator/(const Float2 &a, const Float2 &b);
	};




	struct Rgba {
		union {
			uint8_t m[4];
			uint32_t val;

			struct {
				uint8_t r;
				uint8_t g;
				uint8_t b;
				uint8_t a;
			};
		};

		Rgba();
		Rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		Rgba(float r, float g, float b, float a = 1.0f);
	};




	struct Rect {
		union {
			float m[4];

			struct {
				float left;
				float top;
				float right;
				float bottom;
			};

			struct {
				Float2 pt[2];
			};
		};

		Rect();
		Rect(float left, float top, float right, float bottom);
	};
}