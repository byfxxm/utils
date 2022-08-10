#pragma once
#include <type_traits>
#include <memory>
#include <cassert>

namespace array_nd {
	template <class T, size_t N, class = std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>> && (N > 0)>>
	class ArrayNd {
	private:
		using _Base = ArrayNd<T, N - 1>;

	public:
		template <class First, class... Rest, class = std::enable_if_t<sizeof...(Rest) == N - 1>>
		ArrayNd(First&& first, Rest&&... rest) : count_(std::forward<First>(first)) {
			base_addr_ = std::make_shared<_Base[]>(count_);
			for (size_t i = 0; i < count_; ++i)
				new(&base_addr_[i]) _Base(std::forward<Rest>(rest)...);
		}

		ArrayNd() = default;
		ArrayNd(const ArrayNd&) = delete;
		ArrayNd(ArrayNd&&) noexcept = default;

		void Memset(T val) {
			for (size_t i = 0; i < count_; ++i)
				base_addr_[i].Memset(val);
		}

		const _Base& operator[](size_t idx) const {
			assert(idx >= 0 && idx < count_);
			assert(base_addr_);
			return base_addr_[idx];
		}

	private:
		std::shared_ptr<_Base[]> base_addr_;
		size_t count_{ 0 };
	};

	template <class T>
	class ArrayNd<T, 1> {
	public:
		template <class Last>
		ArrayNd(Last&& last) : count_(std::forward<Last>(last)) {
			base_addr_ = std::make_shared<T[]>(count_);
			Memset(static_cast<std::decay_t<T>>(0));
		}

		ArrayNd() = default;
		ArrayNd(const ArrayNd&) = delete;
		ArrayNd(ArrayNd&&) noexcept = default;

		void Memset(T val) {
			for (size_t i = 0; i < count_; ++i)
				base_addr_[i] = val;
		}

		T& operator[](size_t idx) const {
			assert(idx >= 0 && idx < count_);
			assert(base_addr_);
			return base_addr_[idx];
		}

	private:
		std::shared_ptr<T[]> base_addr_;
		size_t count_{ 0 };
	};
}