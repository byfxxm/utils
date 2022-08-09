#pragma once
#include <type_traits>
#include <memory>
#include <cassert>

namespace array_nd {
	template <class T, size_t N, class = std::enable_if_t<(N > 0) && (std::is_integral_v<std::decay_t<T>> || std::is_floating_point_v<std::decay_t<T>>)>>
	class ArrayNd {
	private:
		using _Base = ArrayNd<T, N - 1>;

	public:
		ArrayNd() = default;
		ArrayNd(const ArrayNd&) = delete;
		ArrayNd(ArrayNd&&) noexcept = default;

		template <class... Args, class = std::enable_if_t<sizeof...(Args) == N - 1>>
		ArrayNd(size_t first, Args... args) : count_(first) {
			base_address_.reset(new _Base[count_]);
			for (size_t i = 0; i < count_; ++i)
				new(&base_address_[i]) _Base(args...);
		}

		void Reset(T val) {
			for (size_t i = 0; i < count_; ++i)
				base_address_[i].Reset(val);
		}

		const _Base& operator[](size_t idx) const {
			assert(idx >= 0 && idx < count_);
			assert(base_address_);
			return base_address_[idx];
		}

	private:
		std::shared_ptr<_Base[]> base_address_;
		size_t count_{ 0 };
	};

	template <class T>
	class ArrayNd<T, 1> {
	public:
		ArrayNd() = default;
		ArrayNd(const ArrayNd&) = delete;
		ArrayNd(ArrayNd&&) noexcept = default;

		ArrayNd(size_t first) : count_(first) {
			base_address_.reset(new T[count_]);
			Reset(static_cast<std::decay_t<T>>(0));
		}

		void Reset(T val) {
			for (size_t i = 0; i < count_; ++i)
				base_address_[i] = val;
		}

		T& operator[](size_t idx) const {
			assert(idx >= 0 && idx < count_);
			assert(base_address_);
			return base_address_[idx];
		}

	private:
		std::shared_ptr<T[]> base_address_;
		size_t count_{ 0 };
	};
}