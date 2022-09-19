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
	*		arr.Memset('z');	 // 内存初始化
	*		ArrayNd<int, 2> arr1{{0, 1}, {2, 3, 4}}; // 支持初始化列表
	*/
	template <class Ty, size_t Num>
		requires std::is_trivially_copyable_v<Ty> && (Num > 0)
	class ArrayNd final {
	private:
		template <class T, size_t N>
		class View final {
		public:
			View(T* ptr, const size_t* shape, const size_t* factor) : ptr_(ptr), shape_(shape), factor_(factor) {
				assert(ptr_);
			}

			const View<T, N - 1> operator[](size_t pos) const&& {
				assert(pos >= 0 && pos < shape_[0]);
				return View<T, N - 1>(ptr_ + pos * factor_[0], shape_ + 1, factor_ + 1);
			}

		private:
			T* ptr_{ nullptr };
			const size_t* shape_{ nullptr };
			const size_t* factor_{ nullptr };
		};

		template <class T>
		class View<T, 1> final {
		public:
			View(T* ptr, const size_t* shape, const size_t*) : ptr_(ptr), shape_(shape) {}

			T& operator[](size_t pos) const&& {
				assert(pos >= 0 && pos < shape_[0]);
				return ptr_[pos];
			}

		private:
			T* ptr_{ nullptr };
			const size_t* shape_{ nullptr };
		};

	public:
		template <class... Args>
			requires (sizeof...(Args) == Num)
		ArrayNd(Args&&... args) : count_((... * args)), shapes_{ static_cast<size_t>(args)... } {
			elems_ = std::make_unique<Ty[]>(count_);
			Memset(0);
			InitializeFactors();
		}

		template <class T, size_t N>
		struct InitializerList {
			using type = std::initializer_list<typename InitializerList<T, N - 1>::type>;
		};

		template <class T>
		struct InitializerList<T, 1> {
			using type = std::initializer_list<T>;
		};

		template <class T, size_t N>
		using InitializerList_t = InitializerList<T, N>::type;

		ArrayNd(InitializerList_t<Ty, Num> list) {
			InitializeShapes(list, 0);
			count_ = 1;
			for (auto it : shapes_)
				count_ *= it;

			elems_ = std::make_unique<Ty[]>(count_);
			Memset(0);
			InitializeFactors();
			Assignment(list, 0, 0);
		}

		ArrayNd(const ArrayNd& arr) : count_(arr.count_), shapes_(arr.shapes_), factors_(arr.factors_) {
			elems_ = std::make_unique<Ty[]>(count_);
			memcpy(elems_.get(), arr.elems_.get(), count_ * sizeof(Ty));
		}

		ArrayNd& operator=(const ArrayNd& arr) {
			new(this) ArrayNd(arr);
			return *this;
		}

		ArrayNd(ArrayNd&&) noexcept = default;
		ArrayNd& operator=(ArrayNd&&) noexcept = default;

		decltype(auto) operator[](size_t pos) {
			return View<Ty, Num>(elems_.get(), &shapes_.front(), &factors_.front())[pos];
		}

		template <class Predicate>
		void ForEach(Predicate pred) {
			for (size_t i = 0; i < count_; ++i)
				elems_[i] = pred(elems_[i]);
		}

		void Memset(Ty val) {
			assert(elems_);
			for (size_t i = 0; i < count_; ++i)
				elems_[i] = val;
		}

		template <size_t N>
			requires (N < Num)
		auto Shape() {
			return shapes_[N];
		}

	private:
		void InitializeFactors() {
			for (size_t i = 0; i < Num; ++i) {
				factors_[i] = 1;
				for (size_t j = i + 1; j < Num; ++j)
					factors_[i] *= shapes_[j];
			}
		}

		void InitializeShapes(std::initializer_list<Ty> list, size_t index) {
			auto list_size = list.size();
			if (list_size > shapes_[index])
				shapes_[index] = list_size;
		}

		template <class T>
		void InitializeShapes(T&& list, size_t index) {
			auto list_size = list.size();
			if (list_size > shapes_[index])
				shapes_[index] = list_size;

			for (auto& it : list)
				InitializeShapes(it, index + 1);
		}

		void Assignment(std::initializer_list<Ty> list, size_t, size_t offset) {
			for (auto it = list.begin(); it != list.end(); ++it)
				elems_[offset + (it - list.begin())] = *it;
		}

		template <class T>
		void Assignment(T&& list, size_t index, size_t offset) {
			for (auto it = list.begin(); it != list.end(); ++it)
				Assignment(*it, index + 1, offset + (it - list.begin()) * factors_[index]);
		}

	private:
		size_t count_{ 0 };
		std::unique_ptr<Ty[]> elems_;
		std::array<size_t, Num> shapes_;
		std::array<size_t, Num> factors_;
	};

	template <class First, class... Args>
	[[nodiscard]] auto MakeArrayNd(First&& first, Args&&... args) {
		return ArrayNd<First, sizeof...(Args) + 1>(std::forward<First>(first), std::forward<Args>(args)...);
	}
}
