#pragma once

namespace meta {
	/// <summary>
	/// 判断是否有子类型模板
	/// </summary>
	template <class T>
	struct HasType {
		static constexpr auto value = requires {
			typename T::type;
		};
	};

	template <class T>
	inline constexpr bool HasType_v = HasType<T>::value;

	/// <summary>
	/// 判断是否是自定义类型
	/// </summary>
	template <class T>
	struct IsCustomeClass {
		static constexpr auto value = requires {
			static_cast<int T::*>(nullptr);
		};
	};

	template <class T>
	inline constexpr bool IsCustomeClass_v = IsCustomeClass<T>::value;

	/// <summary>
	/// N->(0, ... , N - 1)
	/// </summary>
	/// <typeparam name="...N"></typeparam>
	template <size_t... N>
	struct Seq {
		using type = Seq;
	};

	template <class>
	struct Upgrade;

	template <size_t... Ns>
	struct Upgrade<Seq<Ns...>> {
		using type = Seq<0, (Ns + 1)...>;
	};

	template <size_t N>
	struct Make {
		using type = Upgrade<typename Make<N - 1>::type>::type;
	};

	template <>
	struct Make<0> {
		using type = Seq<>;
	};

	/// <summary>
	/// countof
	/// </summary>
	template <class T, size_t N>
	constexpr auto Countof(T(&)[N]) {
		static_assert(std::is_same_v<decltype(N), size_t>);
		return N;
	}

	/// <summary>
	/// 判断是否素数
	/// </summary>
	namespace prime {
		template <size_t... Ns>
		struct Seq {
			using type = Seq;
		};

		template <class>
		struct Upgrade;

		template <size_t... Ns>
		struct Upgrade<Seq<Ns...>> {
			using type = Seq<Ns..., sizeof...(Ns) + 2>;
		};

		template <size_t N>
		struct Make {
			using type = Upgrade<typename Make<N - 1>::type>::type;
		};

		template <>
		struct Make<2> {
			using type = Seq<>::type;
		};

		template <size_t... Ns>
		constexpr bool _IsPrime(Seq<Ns...>) {
			return (... && ((sizeof...(Ns) + 2) % Ns != 0));
		}

		template <size_t N>
		struct IsPrime {
			static constexpr bool value = _IsPrime(typename Make<N>::type());
		};

		template <>
		struct IsPrime<1> {
			static constexpr bool value = false;
		};

		template <>
		struct IsPrime<0> {
			static constexpr bool value = false;
		};

		template <size_t N>
		static constexpr bool IsPrimeV = IsPrime<N>::value;

		constexpr bool IsPrimeF(size_t n) {
			if (n < 2)
				return false;

			bool ret = true;
			for (size_t i = 2; i < n; ++i) {
				ret = ret && (n % i);
				if (!ret)
					break;
			}

			return ret;
		}
	}

	/// <summary>
	/// 判断类型是否是继承关系
	/// </summary>
	template <class Derived, class Base>
	constexpr auto _IsBaseOf(Base, int Derived::* = nullptr, int Base::* = nullptr) {
		return true;
	}

	template <class, class>
	constexpr auto _IsBaseOf(...) {
		return false;
	}

	template <class Derived, class Base>
	struct IsBaseOf {
		static constexpr bool value = _IsBaseOf<Derived, Base>(Derived());
	};

	template <class Derived, class Base>
	concept BaseOf = IsBaseOf<Derived, Base>::value;
}
