#pragma once

namespace MP {
	/// <summary>
	/// 判断是否有子类型模板，利用SFINAE特性实现
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <class T>
	struct HasType {
	private:
		template <class T1, class = typename T1::type>
		static constexpr bool Check() {
			return true;
		}

		template <class>
		static constexpr bool Check(...) {
			return false;
		}

	public:
		static constexpr bool value = Check<T>();
	};

	template <class T>
	static constexpr bool HasType_v = HasType<T>::value;

	/// <summary>
	/// 求pi的第N位的数值
	/// </summary>
	template <int N>
	struct PiN {
		static constexpr double Pi() {
			return 3.1415926;
		}

		template <int N1>
		struct Factor {
			static constexpr int value = 10 * Factor<N1 - 1>::value;
		};

		template <>
		struct Factor<0> {
			static constexpr int value = 1;
		};

		static constexpr int value = (int)(Pi() * Factor<N>::value) % 10;
	};

	template <int N>
	static constexpr auto PiN_v = PiN<N>::value;
}