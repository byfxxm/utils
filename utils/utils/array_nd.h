#pragma once
#include <type_traits>
#include <memory>
#include <cassert>

namespace array_nd {
	template <class T, size_t N, class = std::enable_if_t<std::is_arithmetic_v<T> && (N > 0)>>
	class ArrayNd final {
	private:
		using _Base = ArrayNd<T, N - 1>;
		friend class ArrayNd<T, N + 1>;
		ArrayNd() = default;

	public:
		template <class... Args, class = std::enable_if_t<sizeof...(Args) == N - 1>>
		ArrayNd(size_t first, Args... args) : count_(first) {
			base_addr_.reset(new _Base[count_]);
			for (size_t i = 0; i < count_; ++i)
				new(&base_addr_[i]) _Base(args...);
		}

		ArrayNd(ArrayNd&&) noexcept = default;
		ArrayNd(const ArrayNd&) = delete;
		ArrayNd& operator=(const ArrayNd&) = delete;

		const _Base& operator[](size_t idx) const {
			assert(idx >= 0 && idx < count_);
			assert(base_addr_);
			return base_addr_[idx];
		}

		explicit operator T* () const {
			return static_cast<T*>(*base_addr_.get());
		}

		void Memset(T val) const {
			for (size_t i = 0; i < count_; ++i)
				base_addr_[i].Memset(val);
		}

	private:
		std::shared_ptr<_Base[]> base_addr_;
		size_t count_{ 0 };
	};

	template <class T>
	class ArrayNd<T, 1> final {
	private:
		friend class ArrayNd<T, 2>;
		ArrayNd() = default;

	public:
		ArrayNd(size_t last) : count_(last) {
			base_addr_ = std::make_shared<T[]>(count_);
			Memset(static_cast<T>(0));
		}

		ArrayNd(ArrayNd&&) noexcept = default;
		ArrayNd(const ArrayNd&) = delete;
		ArrayNd& operator=(const ArrayNd&) = delete;

		T& operator[](size_t idx) const {
			assert(idx >= 0 && idx < count_);
			assert(base_addr_);
			return base_addr_[idx];
		}

		explicit operator T* () const {
			return base_addr_.get();
		}

		void Memset(T val) const {
			for (size_t i = 0; i < count_; ++i)
				base_addr_[i] = val;
		}

	private:
		std::shared_ptr<T[]> base_addr_;
		size_t count_{ 0 };
	};
}