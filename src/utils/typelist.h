#pragma once

namespace byfxxm {
	namespace typelist {
		template <class... Types>
		struct Typelist {};

		template <class TpList>
		struct Front;

		template <class First, class... Rest>
		struct Front<Typelist<First, Rest...>> {
			using type = First;
		};

		template <class TpList>
		struct Back;

		template <class First, class... Rest>
		struct Back<Typelist<First, Rest...>> {
			using type = Back<Typelist<Rest...>>::type;
		};

		template <class Arg>
		struct Back<Typelist<Arg>> {
			using type = Arg;
		};

		template <class TpList, class Tp>
		struct PushFront;

		template <class... Args, class Tp>
		struct PushFront<Typelist<Args...>, Tp> {
			using type = Typelist<Tp, Args...>;
		};

		template <class TpList, class Tp>
		struct PushBack;

		template <class... Args, class Tp>
		struct PushBack<Typelist<Args...>, Tp> {
			using type = Typelist<Args..., Tp>;
		};

		template <class TpList>
		struct PopFront;

		template <class First, class... Rest>
		struct PopFront<Typelist<First, Rest...>> {
			using type = Typelist<Rest...>;
		};

		template <class TpList, class... Args>
		struct _AddArgsExceptLast;

		template <class TpList, class First, class... Rest>
		struct _AddArgsExceptLast<TpList, First, Rest...> {
			using type = _AddArgsExceptLast<typename PushBack<TpList, First>::type, Rest...>::type;
		};

		template <class TpList, class Last>
		struct _AddArgsExceptLast<TpList, Last> {
			using type = TpList;
		};

		template <class TpList>
		struct PopBack;

		template <class... Args> requires (sizeof...(Args) > 0)
		struct PopBack<Typelist<Args...>> {
			using type = _AddArgsExceptLast<Typelist<>, Args...>::type;
		};

		template <class TpList>
		struct Empty : std::false_type {};

		template <>
		struct Empty<Typelist<>> : std::true_type {};

		template <class TpList>
		struct Size;

		template <class... Args>
		struct Size<Typelist<Args...>> {
			static constexpr size_t value = sizeof...(Args);
		};
	}
}
