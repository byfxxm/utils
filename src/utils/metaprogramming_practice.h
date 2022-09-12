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
	constexpr auto Countof(T(&)[N]) {
		return N;
	}

	/// <summary>
	/// 判断类型是否是继承关系
	/// </summary>
	template <class Derived, class Base>
	struct IsBaseOf {
		static constexpr auto value = IsCustomeClass_v<Derived> && IsCustomeClass_v<Base> && requires {
			[](Base&&) {}(Derived{});
		};
	};

	template <class Derived, class Base>
	inline bool IsBaseOf_v = IsBaseOf<Derived, Base>::value;

	template <class Derived, class Base>
	concept BaseOf = IsBaseOf_v<Derived, Base>;
}
