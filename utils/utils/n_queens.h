#pragma once
#include <cassert>
#include <vector>
#include <array>

namespace NQueens {
	template <size_t N>
	class Queens {
	public:
		class Queen {
		public:
			void SetX(size_t x) {
				assert(x >= 0 && x < N);
				x_ = x;
			}

			void SetY(size_t y) {
				assert(y >= 0 && y < N);
				y_ = y;
			}

			size_t GetX() const {
				return x_;
			}

			size_t GetY() const {
				return y_;
			}

			bool InRange(const Queen& queen) const {
				return x_ == queen.GetX() || y_ == queen.GetY() || abs(intptr_t(x_ - queen.GetX())) == abs(intptr_t(y_ - queen.GetY()));
			}

		private:
			size_t x_ = 0;
			size_t y_ = 0;
		};

		using QueensArray = std::array<Queen, N>;
		using QueensArrayContainer = std::vector<QueensArray>;

	public:
		Queens() {
			QueensArray queens;
			for (size_t i = 0; i < N; ++i)
				queens[i].SetX(i);

			Generate(queens);
		}

		size_t Count() const {
			return resolves_.size();
		}

		const QueensArray& operator[](size_t index) const {
			if (index >= Count())
				std::terminate();

			return resolves_[index];
		}

	private:
		void Generate(QueensArray& queens, size_t index = 0) {
			if (index == N) {
				if (Checkmate(queens))
					resolves_.push_back(queens);

				return;
			}

			for (int i = 0; i < N; ++i) {
				queens[index].SetY(i);
				Generate(queens, index + 1);
			}
		}

		bool Checkmate(const QueensArray& queens) const {
			for (size_t i = 0; i < N; ++i)
				for (size_t j = 0; j < N; ++j)
					if (i != j && queens[i].InRange(queens[j]))
						return false;

			return true;
		}

	private:
		QueensArrayContainer resolves_;
	};
}
