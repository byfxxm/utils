#pragma once

namespace meta {
	/// <summary>
	/// �ж��Ƿ���������ģ��
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
	/// �ж��Ƿ����Զ�������
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
	/// �ж������Ƿ��Ǽ̳й�ϵ
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
