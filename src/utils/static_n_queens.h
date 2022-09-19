#pragma once

namespace byfxxm {
	struct Queen {
		constexpr bool InRange(const Queen& queen) const {
			return x == queen.y || y == queen.y || abs(x, queen.x) == abs(y, queen.y);
		}

		constexpr size_t abs(size_t a, size_t b) const {
			return a > b ? a - b : b - a;
		}

		size_t x = 0;
		size_t y = 0;
	};

	template <Queen... Qs>
	struct QueensArray {

	};

	template <class... QArrays>
	struct QueensArrayContainer {

	};

	template <size_t N>
	struct NQueens {

	};
}