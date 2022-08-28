#pragma once

namespace Meta {
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
}
