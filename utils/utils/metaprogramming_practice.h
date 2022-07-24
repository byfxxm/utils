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

		static constexpr int value = (int)(Pi<N * 10>::value * Factor<N>::value) % 10;
	};

	template <int N>
	static constexpr auto PiN_v = PiN<N>::value;

	/// <summary>
	/// 求pi的第N位的数值（第二种实现）
	/// </summary>
	template <int N>
	struct PiN1 {
		template <double N>
		struct Sqrt {
			static constexpr double _Sqrt(double n, double low, double high) {
				double half = (low + high) / 2;
				if ((n > half && n - half < 1e-1) || (n <= half && half - n < 1e-1))
					return half;

				double half_pow = half * half;
				if (half_pow > n)
					_Sqrt(n, low, half);
				else
					_Sqrt(n, half, high);
			}

			static constexpr double value = _Sqrt(N, 0., N);
		};

		template <int N>
		struct Pi {
			static constexpr double Sum(int n) {
				if (n == 1)
					return 1;

				return 1. / (n * n) + Sum(n - 1);
			}

			static constexpr double value = Sqrt<Sum(N) * 6>::value;
		};

		static constexpr int Factor(int n) {
			if (n == 1)
				return 1;

			return 10 * Factor(n - 1);
		}

		static constexpr int value = int(Pi<N * 100>::value * Factor(N)) % 10;
	};

	template <int N>
	static constexpr auto PiN1_v = PiN1<N>::value;
}