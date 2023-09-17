/// ∆Ê“Ïµ›πÈƒ£∞Â Ω
#pragma once
#include <iostream>

namespace crtp {
	template <class T>
	class Base {
	public:
		void Func() {
			static_cast<T*>(this)->Func();
		}
	};

	template <class T>
	inline void Func(Base<T>& obj) {
		obj.Func();
	}
}
