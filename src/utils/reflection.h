#pragma once
#include <type_traits>
#include <filesystem>
#include <functional>

namespace byfxxm {
	template <class ObjectType>
	concept Serializer = requires(ObjectType) {
		typename ObjectType::ISerialize;
	};

	class ISerialize {
	public:
		virtual ~ISerialize() = default;
		virtual void Serialize() = 0;
		virtual void Deserialize() = 0;

	protected:
		template <class FieldType>
		using ValueConverter = std::function<void(FieldType*, const std::string&)>;

		template <Serializer StructType, class FieldType>
		class FieldConverter {
		public:
			FieldConverter(FieldType StructType::* field_ptr, const std::string& field_name, ValueConverter<FieldType> converter)
				: pointer_(field_ptr), name_(field_name), converter_(converter) {}

			void operator()(const StructType& obj) {
				converter_(&(obj.*pointer_), name_);
			}

		private:
			FieldType StructType::* pointer_;
			std::string name_;
			ValueConverter<FieldType> converter_;
		};

		template <Serializer StructType>
		class StructConverter {
		private:
			//std::vector<std::unique_ptr<FieldConverter<>>>
		};

		template <Serializer StructType, class FieldType>
		void RegistField(FieldType StructType::* field_ptr, const std::string& field_name, ValueConverter<FieldType> converter) {

		}

	};

	class ReflectionHelper {
	public:
		using Container = std::vector<ISerialize>;

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

		template <Serializer ObjectType>
		void Register(ObjectType&& obj) {

		}

	private:
		std::filesystem::path meta_data_ = std::filesystem::current_path().string() + "\\" + (std::filesystem::current_path().filename().string() + ".class");
		Container objs_;
	};
}