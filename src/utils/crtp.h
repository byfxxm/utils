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
	};

	template <template <class> class Base, class Derived>
	inline void Func(Base<Derived>& obj) {
		obj.Func();
	}

	template <template <class> class Base, class... Deriveds>
		requires (std::conjunction_v<std::is_base_of<Base<Deriveds>, Deriveds>...>)
	class Container {
	private:
		template <size_t Index, class First, class... Ders>
			requires (Index <= sizeof...(Ders))
		struct GetType {
			using type = typename GetType<Index - 1, Ders...>::type;
		};

		template <class First, class... Ders>
		struct GetType<0, First, Ders...> {
			using type = First;
		};

	public:
		template <class... Ders>
			requires (... && std::is_same_v<Deriveds, Ders>)
		Container(Ders&... ders) : objs_{ &ders... } {}

		template <size_t Index>
		auto Get() {
			return static_cast<GetType<Index, Deriveds...>::type*>(objs_[Index]);
		}

	private:
		void* objs_[sizeof...(Deriveds)];
	};
}
