#pragma once

namespace MP {
	/// <summary>
	/// 判断是否有子类型模板，利用SFINAE特性实现
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <class T>
	struct HasType {
	private:
		template <class T>
		static constexpr bool Check(typename T::type* = nullptr) {
			return true;
		}

		template <class T>
		static constexpr bool Check(...) {
			return false;
		}

	public:
		static constexpr bool value = Check<T>(nullptr);
	};

	template <class T>
	static constexpr bool HasType_v = HasType<T>::value;
}