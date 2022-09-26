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
		requires (... && std::is_base_of_v<Base<Deriveds>, Deriveds>)
	class Container {
	public:
		template <class... Ders>
			requires (... && std::is_same_v<Deriveds, Ders>)
		Container(Ders&... ders) : objs_{ &ders... } {}

	private:
		void* objs_[sizeof...(Deriveds)];
	};
}
