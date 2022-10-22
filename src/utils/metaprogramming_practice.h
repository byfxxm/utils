#pragma once

namespace meta {
	/// <summary>
	/// �ж��Ƿ���������ģ��
	/// </summary>
	template <class T>
	concept HasType = requires {
		typename T::type;
	};

	/// <summary>
	/// �ж��Ƿ����Զ�������
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
	constexpr auto Countof(T(&)[N]) {
		return N;
	}

	/// <summary>
	/// �ж������Ƿ��Ǽ̳й�ϵ
	/// </summary>
	template <class Derived, class Base>
	concept IsBaseOf = IsCustomeClass<Derived> && IsCustomeClass<Base> && requires {
		[](Base&&) {}(std::declval<Derived>());
	};

	/// <summary>
	/// �����ֵ
	/// </summary>
	/// <typeparam name="...Rest"></typeparam>
	/// <typeparam name="First"></typeparam>
	/// <typeparam name="T"></typeparam>
	/// <param name="first"></param>
	/// <param name="...rest"></param>
	/// <returns></returns>
	template <class T, class First, class... Rest>
	consteval auto Max(First first, Rest... rest) {
		T max = first;
		(..., (max = (rest > max) ? static_cast<decltype(max)>(rest) : max));
		return max;
	}
}
