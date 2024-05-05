#pragma once
#include <memory>
#include <type_traits>
#include <vector>

namespace byfxxm {
	class AnyContainer {
	public:
		template <class T>
		void PushBack(T&& t) {
			_elements.emplace_back(std::make_unique<Element<std::remove_cvref_t<T>>>(std::forward<T>(t)));
		}

	private:
		class ElementBase {
		public:
			virtual ~ElementBase() = default;
		};

		template <class T>
		class Element : public ElementBase {
		public:
			template <class T1>
				requires std::is_same_v<T, std::remove_cvref_t<T1>>
			Element(T1&& t1) : _point{ std::make_unique<T>(std::forward<T1>(t1)) } {}

		private:
			std::unique_ptr<T> _point;
		};

	private:
		std::vector<std::unique_ptr<ElementBase>> _elements;
	};
}
