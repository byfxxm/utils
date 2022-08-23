#pragma once

namespace Meta {
	/// <summary>
	/// �ж��Ƿ���������ģ�壬����SFINAE����ʵ��
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
	/// �ж��Ƿ����Զ�������
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
