#pragma once

namespace byfxxm {
	class VirtualProduct {
	public:
		virtual ~VirtualProduct() = default;
		virtual std::string Name() = 0;
	};

	class VirtualFactory {
	public:
		virtual ~VirtualFactory() = default;
		virtual std::unique_ptr<VirtualProduct> ProduceOne() = 0;
	};

	class ProductA : public VirtualProduct {
	public:
		virtual std::string Name() override {
			return "ProductA";
		}
	};

	class FactoryA : public VirtualFactory {
	public:
		virtual std::unique_ptr<VirtualProduct> ProduceOne() override {
			return std::make_unique<ProductA>();
		}
	};
}