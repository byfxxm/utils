#pragma once
#include <unordered_map>
#include <string>
#include <tuple>
#include <type_traits>
#include <optional>

namespace byfxxm {
	template <class T>
	concept AddrType = std::is_same_v<T, int> || std::is_same_v<T, double> || std::is_same_v<T, bool> || std::is_same_v<T, std::string>;

	class Addressing {
	private:
		Addressing() = default;
		~Addressing() = default;
		Addressing(const Addressing&) = delete;
		Addressing(Addressing&&) noexcept = delete;

	public:
		enum class Type {
			STRING,
			BOOL,
			DOUBLE,
			INT,
		};

		struct Addr {
			void* p;
			Type t;
		};

		using Key = std::string;

		static Addressing* Instance() {
			static Addressing addr;
			return &addr;
		}

		template <AddrType T>
		void Register(Key k, T* v) {
			if constexpr (std::is_same_v<T, int>)
				dictionary_.insert({ k, {v, Type::INT} });
			else if constexpr (std::is_same_v<T, double>)
				dictionary_.insert({ k, {v, Type::DOUBLE} });
			else if constexpr (std::is_same_v<T, bool>)
				dictionary_.insert({ k, {v, Type::BOOL} });
			else
				dictionary_.insert({ k, {v, Type::STRING} });
		}

		std::optional<Addr> Get(Key k) {
			if (dictionary_.find(k) == dictionary_.end())
				return std::nullopt;

			return dictionary_[k];
		}

	private:
		std::unordered_map<std::string, Addr> dictionary_;
	};
}
