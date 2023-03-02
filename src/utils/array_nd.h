#pragma once
#include <array>
#include <cassert>
#include <concepts>
#include <cstring>
#include <memory>
#include <type_traits>

namespace byfxxm {
	template <class T>
	concept ElementT = std::is_trivial_v<T> && std::convertible_to<T, double>;

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

	/*
	* ��ά����
	* usage:
	*		ArrayNd<char, 3> arr(7, 8, 9); // ��������Ԫ������Ϊ char��ά���� 3����һ���ڶ�������ά�ȷֱ��� 7��8��9��
	*		char c = arr[1][2][3]; // ����������飬������������ά��ʱ����������
	*		arr.Memset('z');	 // �ڴ��ʼ��
	*		ArrayNd<int, 2> arr1{{0, 1}, {2, 3, 4}}; // ֧�ֳ�ʼ���б�
	*/
	template <ElementT Ty, size_t Num> requires (Num > 0)
	class ArrayNd final {
	private:
		template <class T, size_t N, bool RO>
		class _Proxy {
		public:
			_Proxy(T* ptr, const size_t* shape, const size_t* factor) : _ptr(ptr), _shape(shape), _factor(factor) {
				assert(_ptr);
			}

			const _Proxy<T, N - 1, RO> operator[](size_t pos) const {
				assert(pos >= 0 && pos < _shape[0]);
				return _Proxy<T, N - 1, RO>(_ptr + pos * _factor[0], _shape + 1, _factor + 1);
			}

		private:
			T* _ptr{ nullptr };
			const size_t* _shape{ nullptr };
			const size_t* _factor{ nullptr };
		};

		template <class T, bool RO>
		class _Proxy<T, 1, RO> {
		public:
			_Proxy(T* ptr, const size_t* shape, const size_t*) : _ptr(ptr), _shape(shape) {}

			auto& operator[](size_t pos) const {
				if constexpr (RO)
					return static_cast<const T&>(_ptr[pos]);
				else
					return static_cast<T&>(_ptr[pos]);
			}

		private:
			T* _ptr{ nullptr };
			const size_t* _shape{ nullptr };
		};

	public:
		template <std::integral... Args> requires (sizeof...(Args) == Num)
		ArrayNd(Args... args) : _count((... * args)), _shapes{ static_cast<size_t>(args)... } {
			_elems = std::make_unique<Ty[]>(_count);
			Memset(0);
			_InitializeFactors();
		}

		ArrayNd(InitializerList_t<Ty, Num> list) {
			_shapes.fill(0);
			_InitializeShapes(list, 0);
			_count = 1;
			for (auto it : _shapes) {
				_count *= it;
			}

			_elems = std::make_unique<Ty[]>(_count);
			Memset(0);
			_InitializeFactors();
			_Assignment(list, 0, 0);
		}

		ArrayNd(const ArrayNd& arr) {
			*this = arr;
		}

		ArrayNd& operator=(const ArrayNd& arr) {
			if (this == &arr)
				return *this;

			_count = arr._count;
			_shapes = arr._shapes;
			_factors = arr._factors;
			_elems = std::make_unique<Ty[]>(_count);
			memcpy(_elems.get(), arr._elems.get(), _count * sizeof(Ty));
			return *this;
		}

		ArrayNd(ArrayNd&& arr) noexcept = default;
		ArrayNd& operator=(ArrayNd&& arr) noexcept = default;

		decltype(auto) operator[](size_t pos) const {
			return _Proxy<Ty, Num, true>(_elems.get(), &_shapes.front(), &_factors.front())[pos];
		}

		decltype(auto) operator[](size_t pos) {
			return _Proxy<Ty, Num, false>(_elems.get(), &_shapes.front(), &_factors.front())[pos];
		}

		template <class Predicate>
		void ForEach(Predicate&& pred) {
			for (size_t i = 0; i < _count; ++i) {
				_elems[i] = pred(_elems[i]);
			}
		}

		void Memset(Ty val) {
			assert(_elems);
			for (size_t i = 0; i < _count; ++i) {
				_elems[i] = val;
			}
		}

		template <size_t N> requires (N < Num)
		size_t Shape() const {
			return _shapes[N];
		}

	private:
		void _InitializeShapes(std::initializer_list<Ty> list, size_t index) {
			if (auto list_size = list.size(); list_size > _shapes[index]) {
				_shapes[index] = list_size;
			}
		}

		template <class List>
		void _InitializeShapes(List&& list, size_t index) {
			if (auto list_size = list.size(); list_size > _shapes[index]) {
				_shapes[index] = list_size;
			}

			for (auto& it : list) {
				_InitializeShapes(it, index + 1);
			}
		}

		void _InitializeFactors() {
			for (size_t i = 0; i < Num; ++i) {
				_factors[i] = 1;
				for (size_t j = i + 1; j < Num; ++j) {
					_factors[i] *= _shapes[j];
				}
			}
		}

		void _Assignment(std::initializer_list<Ty> list, size_t, size_t offset) {
			for (auto it = list.begin(); it != list.end(); ++it) {
				_elems[offset + (it - list.begin())] = *it;
			}
		}

		template <class List>
		void _Assignment(List&& list, size_t index, size_t offset) {
			for (auto it = list.begin(); it != list.end(); ++it) {
				_Assignment(*it, index + 1, offset + (it - list.begin()) * _factors[index]);
			}
		}

	private:
		size_t _count{ 0 };
		std::unique_ptr<Ty[]> _elems;
		std::array<size_t, Num> _shapes;
		std::array<size_t, Num> _factors;
	};

	template <class First, class... Rest>
	ArrayNd(First, Rest...)->ArrayNd<First, sizeof...(Rest) + 1>;
}
