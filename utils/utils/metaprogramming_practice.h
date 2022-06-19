#pragma once

namespace MP {
	/// <summary>
	/// �ж��Ƿ���������ģ�壬����SFINAE����ʵ��
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