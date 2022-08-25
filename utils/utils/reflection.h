#pragma once
#include <type_traits>
#include <filesystem>

namespace byfxxm {
	class ISerialize {
	public:
		virtual ~ISerialize() = default;
		virtual void Serialize() = 0;
		virtual void Deserialize() = 0;
	};

	template <class ObjectType>
	concept Serializer = requires(ObjectType) {
		typename ObjectType::ISerialize;
	};

	class ReflectionHelper {
	public:
		ReflectionHelper() {

		}

		static ReflectionHelper& Instance() {
			static ReflectionHelper inst;
			return inst;
		}

		template <Serializer ObjectType>
		void Serialize(ObjectType&& obj) {
			obj.Serialize();
		}

		template <Serializer ObjectType>
		void Deserialize(ObjectType&& obj) {
			obj.Deserialize();
		}

	private:
		std::filesystem::path meta_data_;
	};
}