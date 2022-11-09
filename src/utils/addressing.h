#pragma once
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>

namespace byfxxm {
	template <class T>
	concept AddrType = std::_Is_any_of_v<T, int, double, bool, std::string>;

	class AddressingException : public std::exception {
	public:
		AddressingException(const std::string& s) : std::exception(s.c_str()) {}
	};

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

		template <AddrType T>
		class LeafD : public LeafB {
		public:
			LeafD(void* p) : LeafB(p) {}

			virtual Value Get() override {
				return { *static_cast<T*>(p_) };
			}

			virtual void Set(Value v) override {
				*static_cast<T*>(p_) = std::get<T>(v);
			}
		};

		static Addressing* Instance() {
			static Addressing addr;
			return &addr;
		}

		template <AddrType T>
		void Register(Key k, T* v) {
			if (dictionary_.find(k) != dictionary_.end())
				throw AddressingException("Register failure");

			dictionary_.insert({ k, std::make_shared<LeafD<T>>(v) });
		}

		void Unregister(Key k) {
			if (dictionary_.find(k) == dictionary_.end())
				throw AddressingException("Unregister failure");

			dictionary_.erase(k);
		}

		Value Get(Key k) {
			if (dictionary_.find(k) == dictionary_.end())
				throw AddressingException("Get failure");

			return dictionary_[k]->Get();
		}

		void Set(Key k, Value v) {
			if (dictionary_.find(k) == dictionary_.end())
				throw AddressingException("Set failure");

			dictionary_[k]->Set(v);
		}

	private:
		std::unordered_map<Key, std::shared_ptr<LeafB>> dictionary_;
	};
}
