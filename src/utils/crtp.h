/// ∆Ê“Ïµ›πÈƒ£∞Â Ω
#pragma once
#include <iostream>

namespace crtp {
	template <class Derived>
	class Base {
	public:
		void Func() {
			static_cast<Derived*>(this)->Func();
		}

		int Func1(int n1, double n2) {
			return static_cast<Derived*>(this)->Func1(n1, n2);
		}
	};

	template <class Derived>
	inline void Func(Base<Derived>& obj) {
		obj.Func();
	}

	template <class Derived>
	inline int Func1(Base<Derived>& obj, int n1, double n2) {
		return obj.Func1(n1, n2);
	}

#if 0
	template <template <class> class Base, class... Ders>
		requires requires(Ders... ders) {
		(... , static_cast<Base<Ders>>(ders));
	}
	class Container {
	public:
		template <class First, class... Ders1>
			requires requires{
			(sizeof...(Ders) == sizeof...(Ders1) + 1);
			typename std::enable_if_t<(... && std::is_pointer_v<Ders1>)>;
		}
		Container(First&& first, Ders1&&... ders) : Container(std::forward<Ders1>(ders)...) {
			objs_[sizeof...(Ders) - sizeof...(Ders1) - 1] = static_cast<void*>(first);
		}

		Container() = default;

		void Func() {
			_Func<Ders...>();
		}

	private:
		template <class First, class... Ders1>
		void _Func() {
			static_cast<First*>(objs_[sizeof...(Ders) - sizeof...(Ders1) - 1])->Func();
			if constexpr (sizeof...(Ders1) != 0)
				_Func<Ders1...>();
		}

	private:
		void* objs_[sizeof...(Ders)];
	};
#endif
}
