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

	template <class... Ts>
	class Addressing {
	private:
		Addressing() = default;
		~Addressing() = default;
		Addressing(const Addressing&) = delete;
		Addressing(Addressing&&) noexcept = delete;

	public:
		using Key = std::string;
		using Value = std::variant<Ts...>;

		class LeafBase {
		public:
			virtual ~LeafBase() = default;
			virtual Value Get() = 0;
			virtual void Set(Value) = 0;

		//protected:
		//	void* _p;
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

	template <size_t N, class... Ts> requires (N < sizeof...(Ts))
	void _Print(const typename Addressing<Ts...>::Value& v) {
		std::cout << std::get<N>(v) << std::endl;
	}

	//void Print(const Addressing::Value& v) {
	//	switch (v.index()) {
	//	case 0:
	//		return _Print<0>(v);
	//	case 1:
	//		return _Print<1>(v);
	//	case 2:
	//		return _Print<2>(v);
	//	case 3:
	//		return _Print<3>(v);
	//	default:
	//		throw AddressingException("error param");
	//	}
	//}

	//template <class... Ts>
	//Addressing<Ts...>::Value Type(const typename Addressing<Ts...>::Value& v) {
	//	switch (v.index()) {
	//	case 0:
	//		return "int";
	//	case 1:
	//		return "double";
	//	case 2:
	//		return "bool";
	//	case 3:
	//		return "string";
	//	default:
	//		throw AddressingException("error param");
	//	}
	//}

	//bool IsNumber(const Addressing::Value& v) {
	//	return v.index() == 0 || v.index() == 1;
	//}

	//double ToDouble(const Addressing::Value& v) {
	//	switch (v.index()) {
	//	case 0:
	//		return std::get<0>(v);
	//	case 1:
	//		return std::get<1>(v);
	//	default:
	//		throw AddressingException("error param");
	//	}
	//}

	//Addressing::Value operator+(const Addressing::Value& v1, const Addressing::Value& v2) {
	//	return ToDouble(v1) + ToDouble(v2);
	//}

	//Addressing::Value operator-(const Addressing::Value& v1, const Addressing::Value& v2) {
	//	return ToDouble(v1) - ToDouble(v2);
	//}

	//Addressing::Value operator*(const Addressing::Value& v1, const Addressing::Value& v2) {
	//	return ToDouble(v1) * ToDouble(v2);
	//}

	//Addressing::Value operator/(const Addressing::Value& v1, const Addressing::Value& v2) {
	//	return ToDouble(v1) / ToDouble(v2);
	//}
}
