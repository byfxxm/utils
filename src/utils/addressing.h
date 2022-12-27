#pragma once
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <mutex>
#include <exception>

namespace byfxxm {
	template <class T>
	concept AddrType = std::is_same_v<std::decay_t<T>, int>
		|| std::is_same_v<std::decay_t<T>, double>
		|| std::is_same_v<std::decay_t<T>, bool>
		|| std::is_same_v<std::decay_t<T>, std::string>;

	class AddressingException : public std::exception {
	public:
		AddressingException(const std::string& s) : _wh(s) {}

		virtual const char* what() const noexcept override {
			return _wh.c_str();
		}

	private:
		std::string _wh;
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
			LeafB(void* p) : _p(p) {}
			virtual ~LeafB() = default;
			virtual Value Get() = 0;
			virtual void Set(Value) = 0;

		protected:
			void* _p;
		};

		template <AddrType T>
		class LeafD : public LeafB {
		public:
			LeafD(void* p) : LeafB(p) {}

			virtual Value Get() override {
				return { *static_cast<T*>(_p) };
			}

			virtual void Set(Value v) override {
				*static_cast<T*>(_p) = std::get<T>(v);
			}
		};

		static Addressing* Instance() {
			static Addressing addr;
			return &addr;
		}

		template <AddrType T>
		void Register(Key k, T* v) {
			std::lock_guard lock(_mtx);
			if (_dictionary.find(k) != _dictionary.end())
				throw AddressingException("Register failure");

			_dictionary.insert({ k, std::make_shared<LeafD<T>>(v) });
		}

		void Unregister(Key k) {
			std::lock_guard lock(_mtx);
			if (_dictionary.find(k) == _dictionary.end())
				throw AddressingException("Unregister failure");

			_dictionary.erase(k);
		}

		Value Get(Key k) {
			std::lock_guard lock(_mtx);
			if (_dictionary.find(k) == _dictionary.end())
				throw AddressingException("Get failure");

			return _dictionary[k]->Get();
		}

		void Set(Key k, Value v) {
			std::lock_guard lock(_mtx);
			if (_dictionary.find(k) == _dictionary.end())
				throw AddressingException("Set failure");

			_dictionary[k]->Set(v);
		}

	private:
		std::unordered_map<Key, std::shared_ptr<LeafB>> _dictionary;
		std::mutex _mtx;
	};
}
