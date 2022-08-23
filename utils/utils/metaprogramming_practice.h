#pragma once

namespace Meta {
	/// <summary>
	/// 判断是否有子类型模板，利用SFINAE特性实现
	/// </summary>
	template <class T>
	constexpr bool _HasType(typename T::type*){
		return true;
	}

	template <class>
	constexpr bool _HasType(...) {
		return false;
	}

	template <class T>
	static constexpr bool HasType = _HasType<T>(nullptr);

	/// <summary>
	/// 判断是否是自定义类型
	/// </summary>
	template <class>
	constexpr bool _IsCustomeClass(...) {
		return false;
	}

	template <class T>
	constexpr bool _IsCustomeClass(int T::*) {
		return true;
	}

	template <class T>
	static constexpr bool IsCustomeClass = _IsCustomeClass<T>(nullptr);
}
