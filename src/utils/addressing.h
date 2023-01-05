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

		class LeafBase {
		public:
			LeafBase(void* p) : _p(p) {}
			virtual ~LeafBase() = default;
			virtual Value Get() = 0;
			virtual void Set(Value) = 0;

		protected:
			void* _p;
		};

		template <AddrType T>
		class LeafDerived : public LeafBase {
		public:
			LeafDerived(void* p) : LeafBase(p) {}

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

			_dictionary.insert({ k, std::make_unique<LeafDerived<T>>(v) });
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
		std::unordered_map<Key, std::unique_ptr<LeafBase>> _dictionary;
		std::mutex _mtx;
	};

	template <size_t N>
	void _Print(const Addressing::Value& v) {
		std::cout << std::get<N>(v) << std::endl;
	}

	void Print(const Addressing::Value& v) {
		switch (v.index()) {
		case 0:
			_Print<0>(v);
			break;
		case 1:
			_Print<1>(v);
			break;
		case 2:
			_Print<2>(v);
			break;
		case 3:
			_Print<3>(v);
			break;
		default:
			break;
		}
	}

	Addressing::Value Type(const Addressing::Value& v) {
		const char* rv = nullptr;

		switch (v.index()) {
		case 0:
			rv = "int";
			break;
		case 1:
			rv = "double";
			break;
		case 2:
			rv = "bool";
			break;
		case 3:
			rv = "string";
			break;
		default:
			break;
		}

		return rv;
	}
}
