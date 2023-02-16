#pragma once
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <mutex>
#include <exception>

namespace byfxxm {
	class AddressingException : public std::exception {
	public:
		AddressingException(const std::string& s) : _wh(s) {}

		virtual const char* what() const noexcept override {
			return _wh.c_str();
		}

	private:
		std::string _wh;
	};

	using Key = std::string;
	template <class... Ts>
	using Value = std::variant<Ts...>;

	template <class... Ts>
	class Addressing {
	private:
		Addressing() = default;
		~Addressing() = default;
		Addressing(const Addressing&) = delete;
		Addressing(Addressing&&) noexcept = delete;

	public:
		using Key = std::string;
		using Value = byfxxm::Value<Ts...>;

		class LeafBase {
		public:
			virtual ~LeafBase() = default;
			virtual Value Get() = 0;
			virtual void Set(Value) = 0;
		};

		template <class T>
		class LeafDerived : public LeafBase {
		public:
			LeafDerived(void* p) : _p(p) {}

			virtual Value Get() override {
				return { *static_cast<T*>(_p) };
			}

			virtual void Set(Value v) override {
				*static_cast<T*>(_p) = std::get<T>(v);
			}

		private:
			void* _p;
		};

		static Addressing* Instance() {
			static Addressing addr;
			return &addr;
		}

		template <class T>
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

	template <class... Ts, size_t... N, class Fn>
	void _PrintHelp(const Value<Ts...>& v, std::index_sequence<N...>, Fn&& f) {
		(... , f(std::get_if<N>(&v)));
	}

	template <class... Ts>
	void Print(const Value<Ts...>& v) {
		_PrintHelp(v, std::make_index_sequence<sizeof...(Ts)>{}, [](auto&& v) {
			if (v) std::cout << *v << std::endl;
			});
	}

	template <class... Ts, size_t... N, class Fn>
	std::string _TypeHelp(const Value<Ts...>& v, std::index_sequence<N...>, Fn&& f) {
		return (... + f(std::get_if<N>(&v)));
	}

	template <class... Ts>
	std::string Type(const Value<Ts...>& v) {
		return _TypeHelp(v, std::make_index_sequence<sizeof...(Ts)>{}, [](auto&& v) {
			return v ? std::string(typeid(*v).name()) : std::string{};
			});
	}
}
