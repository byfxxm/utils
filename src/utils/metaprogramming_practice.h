#pragma once

namespace meta {
	/// <summary>
	/// 判断是否有子类型模板
	/// </summary>
	template <class T>
	concept HasType = requires {
		typename T::type;
	};

	/// <summary>
	/// 判断是否是自定义类型
	/// </summary>
	template <class T>
	concept IsCustomeClass = requires {
		static_cast<int T::*>(nullptr);
	};

	/// <summary>
	/// N->(0, ... , N - 1)
	/// </summary>
	/// <typeparam name="...N"></typeparam>
	template <size_t... N>
	struct Seq {};

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
	consteval auto Countof(T(&)[N]) {
		return N;
	}

	/// <summary>
	/// 判断类型是否是继承关系
	/// </summary>
	template <class Derived, class Base>
	concept IsBaseOf = IsCustomeClass<Derived> && IsCustomeClass<Base> && requires {
		[](Base&&) {}(std::declval<Derived>());
	};

	/// <summary>
	/// 求最大值
	/// </summary>
	/// <typeparam name="...Rest"></typeparam>
	/// <typeparam name="First"></typeparam>
	/// <typeparam name="T"></typeparam>
	/// <param name="first"></param>
	/// <param name="...rest"></param>
	/// <returns></returns>
	template <class T = double, class First, class... Rest>
	consteval T Max(First first, Rest... rest) {
		auto max = static_cast<T>(first);
		(..., (max = (rest > max) ? static_cast<T>(rest) : max));
		return max;
	}

	/// <summary>
	/// 判断是否素数
	/// </summary>
	template <size_t N>
	constexpr bool IsPrime = false;

	template <size_t... Ns>
	constexpr bool _IsPrime(std::index_sequence<Ns...>) {
		constexpr size_t N = sizeof...(Ns) + 2;
		return (true && ... && (N % (Ns + 2) != 0));
	}

	template <size_t N> requires (N >= 2 && _IsPrime(std::make_index_sequence<N - 2>{}))
	constexpr bool IsPrime<N> = true;
}
