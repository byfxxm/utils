#pragma once
#include <type_traits>
#include <memory>
#include <cassert>

namespace array_nd {
	template <class T, size_t N, class = std::enable_if_t<(N > 0)>>
	class ArrayNd {
	private:
		using _Base = ArrayNd<T, N - 1>;

	public:
		template <class... Args, class = std::enable_if_t<sizeof...(Args) == N - 1>>
		ArrayNd(size_t first, Args&&... args) : count_(first) {
			base_address_.reset(new _Base[count_]);
			for (size_t i = 0; i < count_; ++i)
				new(&base_address_[i]) _Base(std::forward<Args>(args)...);
		}

		ArrayNd() = default;

		_Base& operator[](size_t idx) {
			assert(idx >= 0 && idx < count_);
			return base_address_[idx];
		}

	private:
		std::shared_ptr<_Base[]> base_address_;
		size_t count_{ 0 };
	};

	template <class T>
	class ArrayNd<T, 1> {
	public:
		ArrayNd(size_t first) : count_(first) {
			base_address_.reset(new T[count_]);
			memset(base_address_.get(), 0, count_ * sizeof(T));
		}

		ArrayNd() = default;

		T& operator[](size_t idx) {
			return base_address_[idx];
		}

	private:
		std::shared_ptr<T[]> base_address_;
		size_t count_{ 0 };
	};
}