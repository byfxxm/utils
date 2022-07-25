#pragma once
#include <cassert>
#include <vector>
#include <array>

namespace NQueens {
	template <int N>
	class Queens {
	public:
		class Queen {
		public:
			void SetX(int x) {
				assert(x >= 0 && x < N);
				x_ = x;
			}

			void SetY(int y) {
				assert(y >= 0 && y < N);
				y_ = y;
			}

			int GetX() const {
				return x_;
			}

			int GetY() const {
				return y_;
			}

			bool InRange(const Queen& queen) const {
				return x_ == queen.GetX() || y_ == queen.GetY() || abs(x_ - queen.GetX()) == abs(y_ - queen.GetY());
			}

		private:
			int x_ = 0;
			int y_ = 0;
		};

		using QueensArray = std::array<Queen, N>;
		using QueensArrayContainer = std::vector<QueensArray>;

	public:
		Queens() {
			QueensArray queens;
			for (int i = 0; i < N; ++i)
				queens[i].SetX(i);

			Generate(queens);
		}

		int Count() const {
			return resolves_.size();
		}

		const QueensArray& operator[](int index) const {
			if (index < 0 || index >= Count())
				std::terminate();

			return resolves_[index];
		}

	private:
		void Generate(QueensArray& queens, int index = 0) {
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
			for (int i = 0; i < N; ++i)
				for (int j = 0; j < N; ++j)
					if (i != j && queens[i].InRange(queens[j]))
						return false;

			return true;
		}

	private:
		QueensArrayContainer resolves_;
	};
}