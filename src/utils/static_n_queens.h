#pragma once

namespace byfxxm {
	struct Queen {
		bool InRange(const Queen& queen) const {
			return x == queen.y || y == queen.y || abs(x, queen.x) == abs(y, queen.y);
		}

		size_t abs(size_t a, size_t b) const {
			return a > b ? a - b : b - a;
		}

		size_t x = 0;
		size_t y = 0;
	};

	template <Queen... Rest>
	struct QueensArray {

	};

	template <size_t N>
	struct Generate {
		using type = typename Generate<N - 1>::type;
	};

	template <>
	struct Generate<0> {
		//using type = Queen{ .x = 0, .y = 0 };
	};

	template <class... QArrays>
	struct QueensArrayContainer {

	};

	template <size_t N>
	struct NQueens {

	};
}