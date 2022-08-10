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
		template <class... Args, class = std::enable_if_t<sizeof...(Args) == N - 1>>
		ArrayNd(size_t first, Args... args) : count_(first) {
			base_addr_ = std::make_shared<_Base[]>(count_);
			for (size_t i = 0; i < count_; ++i)
				new(&base_addr_[i]) _Base(args...);
		}

		ArrayNd() = default;
		ArrayNd(const ArrayNd&) = delete;
		ArrayNd(ArrayNd&&) noexcept = default;

		void Memset(T val) const {
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
		ArrayNd(size_t last) : count_(last) {
			base_addr_ = std::make_shared<T[]>(count_);
			Memset(static_cast<std::decay_t<T>>(0));
		}

		ArrayNd() = default;
		ArrayNd(const ArrayNd&) = delete;
		ArrayNd(ArrayNd&&) noexcept = default;

		void Memset(T val) const {
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