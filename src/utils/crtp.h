/// ∆Ê“Ïµ›πÈƒ£∞Â Ω
#pragma once
#include <iostream>

namespace crtp {
	template <class T>
	class Base {
	public:
		void Func(int n) {
			static_cast<T*>(this)->Func(n);
		}
	};

	template <class T>
	inline void Func(Base<T>& obj, int n) {
		obj.Func(n);
	}
}
