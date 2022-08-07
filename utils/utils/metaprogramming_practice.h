#pragma once

namespace Meta {
	/// <summary>
	/// �ж��Ƿ���������ģ�壬����SFINAE����ʵ��
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <class T, class = typename T::type>
	constexpr bool _HasType(){
		return true;
	}

	template <class>
	constexpr bool _HasType(...) {
		return false;
	}
	
	template <class T>
	struct HasType {
		static constexpr bool value = _HasType<T>();
	};

	template <class T>
	static constexpr bool HasType_v = HasType<T>::value;
}