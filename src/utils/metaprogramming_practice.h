#pragma once

namespace meta {
	/// <summary>
	/// 判断是否有子类型模板，利用SFINAE特性实现
	/// </summary>
	template <class>
	constexpr bool _HasType(...) {
		return false;
	}

	template <class T>
	constexpr bool _HasType(typename T::type*) {
		return true;
	}

	template <class T>
	inline constexpr bool HasType = _HasType<T>(nullptr);

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
	inline constexpr bool IsCustomeClass = _IsCustomeClass<T>(nullptr);

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
	struct Make : Upgrade<typename Make<N - 1>::type>::type {
	};

	template <>
	struct Make<0> : Seq<> {
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
	}
}
