#pragma once
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>

namespace byfxxm {
	template <class T>
	concept Leaf = std::_Is_any_of_v<T, int, double, bool, std::string>;

	class Addressing {
	private:
		Addressing() = default;
		~Addressing() = default;
		Addressing(const Addressing&) = delete;
		Addressing(Addressing&&) noexcept = delete;

	public:
		using Key = std::string;
		using Value = std::variant<int, double, bool, std::string>;

		class LeafB {
		public:
			LeafB(void* p) : p_(p) {}
			virtual ~LeafB() = default;
			virtual Value Get() = 0;
			virtual void Set(Value) = 0;

		protected:
			void* p_;
		};

		template <Leaf L>
		class LeafD : public LeafB {
		public:
			LeafD(void* p) : LeafB(p) {}

			virtual Value Get() override {
				return { *static_cast<L*>(p_) };
			}

			virtual void Set(Value v) override {
				*static_cast<L*>(p_) = std::get<L>(v);
			}
		};

		static Addressing* Instance() {
			static Addressing addr;
			return &addr;
		}

		template <Leaf L>
		void Register(Key k, L* v) {
			dictionary_.insert({ k, std::make_shared<LeafD<L>>(v) });
		}

		Value Get(Key k) {
			if (dictionary_.find(k) == dictionary_.end())
				throw std::exception();

			return dictionary_[k]->Get();
		}

		void Set(Key k, auto&& v) {
			if (dictionary_.find(k) == dictionary_.end())
				throw std::exception();

			dictionary_[k]->Set(v);
		}

	private:
		std::unordered_map<Key, std::shared_ptr<LeafB>> dictionary_;
	};
}
