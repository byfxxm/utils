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
		template <int N1>
		struct Pi {
			static constexpr double sum = 1. / (N1 * N1) + Pi<N1 - 1>::sum;
			static constexpr double value = sum * 6;
		};

		template <>
		struct Pi<1> {
			static constexpr double sum = 1.;
			static constexpr double value = sum * 6;
		};

		template <int N1>
		struct Factor {
			static constexpr int value = 10 * Factor<N1 - 1>::value;
		};

		template <>
		struct Factor<0> {
			static constexpr int value = 1;
		};

		static constexpr int value = (int)(Pi<N * 500>::value * Factor<N>::value) % 10;
	};

	template <int N>
	static constexpr auto PiN_v = PiN<N>::value;
}