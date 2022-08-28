#pragma once

namespace Meta {
	/// <summary>
	/// �ж��Ƿ���������ģ�壬����SFINAE����ʵ��
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
	/// �ж��Ƿ����Զ�������
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
