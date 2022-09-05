#pragma once
#include <array>
#include <cassert>
#include <memory>

namespace byfxxm {
	/*
	* 多维数组
	* usage:
	*		ArrayNd<char, 3> arr(7, 8, 9); // 构建对象（元素类型为 char，维数是 3，第一、第二、第三维度分别是 7、8、9）
	*		char c = arr[1][2][3]; // 随机访问数组，参数个数等于维数时，返回引用
	*		arr.Memset('z'); // 内存初始化
	*/
	template <class Ty, size_t Ct>
		requires (Ct > 0)
	class ArrayNd final {
	private:
		template <class T1, size_t N1>
		class ViewPtr final {
		public:
			ViewPtr(T1* p, const size_t* d, const size_t* f) : ptr_(p), dims_(d), factors_(f) {
				assert(ptr_);
			}

			ViewPtr<T1, N1 - 1> operator[](size_t idx) const&& {
				assert(idx >= 0 && idx < dims_[0]);
				return ViewPtr<T1, N1 - 1>(ptr_ + idx * factors_[0], dims_ + 1, factors_ + 1);
			}

		private:
			T1* ptr_{ nullptr };
			const size_t* dims_{ nullptr };
			const size_t* factors_{ nullptr };
		};

		template <class T1>
		class ViewPtr<T1, 1> final {
		public:
			ViewPtr(T1* p, const size_t* d, const size_t*) : ptr_(p), dims_(d) {}

			T1& operator[](size_t idx) const&& {
				assert(idx >= 0 && idx < dims_[0]);
				return ptr_[idx];
			}

		private:
			T1* ptr_{ nullptr };
			const size_t* dims_{ nullptr };
		};

	public:
		template <class... Args>
			requires (sizeof...(Args) == Ct)
		ArrayNd(Args&&... args) : count_((... * args)), dims_{ static_cast<size_t>(args)... } {
			elems_ = std::make_shared<Ty[]>(count_);
			Memset(0);
			for (size_t i = 0; i < Ct; ++i) {
				factors_[i] = 1;
				for (size_t j = i + 1; j < Ct; ++j)
					factors_[i] *= dims_[j];
			}
		}

		ArrayNd(const ArrayNd&) = delete;
		ArrayNd(ArrayNd&&) noexcept = default;
		ArrayNd& operator=(const ArrayNd&) = delete;
		ArrayNd& operator=(ArrayNd&&) noexcept = default;

		decltype(auto) operator[](size_t idx) {
			return ViewPtr<Ty, Ct>(elems_.get(), &dims_.front(), &factors_.front())[idx];
		}

		void Memset(Ty val) {
			assert(elems_);
			for (size_t i = 0; i < count_; ++i)
				elems_[i] = val;
		}

	private:
		std::shared_ptr<Ty[]> elems_;
		size_t count_{ 0 };
		std::array<size_t, Ct> dims_;
		std::array<size_t, Ct> factors_;
	};

	template <class Ty, class... Args>
	[[nodiscard]] auto MakeArrayNd(Args&&... args) {
		return ArrayNd<Ty, sizeof...(Args)>(std::forward<Args>(args)...);
	}
}
