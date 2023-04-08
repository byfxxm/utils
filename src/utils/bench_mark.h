#pragma once
#include <chrono>

namespace byfxxm {
	struct BenchMark {
		template <class T>
		auto operator()(auto&& callable, T&& strategy) const {
			auto start = std::chrono::high_resolution_clock::now();
			callable();
			auto end = std::chrono::high_resolution_clock::now();
			return std::chrono::duration_cast<std::remove_cvref_t<T>>(end - start).count();
		}
	};

	inline constexpr BenchMark benchmark;
}