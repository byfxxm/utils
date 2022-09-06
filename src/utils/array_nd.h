#pragma once
#include <array>
#include <cassert>
#include <memory>

namespace byfxxm {
	/*
	* ��ά����
	* usage:
	*		ArrayNd<char, 3> arr(7, 8, 9); // ��������Ԫ������Ϊ char��ά���� 3����һ���ڶ�������ά�ȷֱ��� 7��8��9��
	*		char c = arr[1][2][3]; // ����������飬������������ά��ʱ����������
	*		arr.Memset('z');	 // �ڴ��ʼ��
	*		ArrayNd<int, 2> arr1{{0, 1}, {2, 3, 4}}; // ֧�ֳ�ʼ���б�
	*/
	template <class Ty, size_t Num>
		requires (Num > 0)
	class ArrayNd final {
	private:
		template <class T, size_t N>
		class ViewPtr final {
		public:
			ViewPtr(T* p, const size_t* shape, const size_t* factor) : ptr_(p), shape_(shape), factor_(factor) {
				assert(ptr_);
			}

			ViewPtr<T, N - 1> operator[](size_t idx) const&& {
				assert(idx >= 0 && idx < shape_[0]);
				return ViewPtr<T, N - 1>(ptr_ + idx * factor_[0], shape_ + 1, factor_ + 1);
			}

		private:
			T* ptr_{ nullptr };
			const size_t* shape_{ nullptr };
			const size_t* factor_{ nullptr };
		};

		template <class T>
		class ViewPtr<T, 1> final {
		public:
			ViewPtr(T* p, const size_t* shape, const size_t*) : ptr_(p), shape_(shape) {}

			T& operator[](size_t idx) const&& {
				assert(idx >= 0 && idx < shape_[0]);
				return ptr_[idx];
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

		ArrayNd(const ArrayNd&) = delete;
		ArrayNd(ArrayNd&&) noexcept = default;
		ArrayNd& operator=(const ArrayNd&) = delete;
		ArrayNd& operator=(ArrayNd&&) noexcept = default;

		decltype(auto) operator[](size_t idx) {
			return ViewPtr<Ty, Num>(elems_.get(), &shapes_.front(), &factors_.front())[idx];
		}

		void Memset(Ty val) {
			assert(elems_);
			for (size_t i = 0; i < count_; ++i)
				elems_[i] = val;
		}

	private:
		void InitializeFactors() {
			for (size_t i = 0; i < Num; ++i) {
				factors_[i] = 1;
				for (size_t j = i + 1; j < Num; ++j)
					factors_[i] *= shapes_[j];
			}
		}

		void InitializeShapes(std::initializer_list<Ty> list, size_t idx) {
			auto list_size = list.size();
			if (list_size > shapes_[idx])
				shapes_[idx] = list_size;
		}

		template <class T>
		void InitializeShapes(T list, size_t idx) {
			auto list_size = list.size();
			if (list_size > shapes_[idx])
				shapes_[idx] = list_size;

			for (auto& it : list)
				InitializeShapes(it, idx + 1);
		}

		void Assignment(std::initializer_list<Ty> list, size_t idx, size_t off) {
			for (auto it = list.begin(); it != list.end(); ++it)
				elems_[off + (it - list.begin())] = *it;
		}

		template <class T>
		void Assignment(T list, size_t idx, size_t off) {
			for (auto it = list.begin(); it != list.end(); ++it)
				Assignment(*it, idx + 1, off + (it - list.begin()) * factors_[idx]);
		}

	private:
		std::unique_ptr<Ty[]> elems_;
		size_t count_{ 0 };
		std::array<size_t, Num> shapes_;
		std::array<size_t, Num> factors_;
	};

	template <class First, class... Args>
	[[nodiscard]] auto MakeArrayNd(First&& first, Args&&... args) {
		return ArrayNd<First, sizeof...(Args) + 1>(std::forward<First>(first), std::forward<Args>(args)...);
	}
}
