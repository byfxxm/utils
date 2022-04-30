#pragma once
#include <optional>
#include <list>
#include <vector>

template <typename Key, typename Value>
class HashMap {
public:
	HashMap() {
		buckets_.resize(kSizes[size_num_], std::nullopt);
	}

	void Insert(Key k, Value v) {
		auto& bucket = buckets_[Hash(k)];
		if (!bucket.has_value())
			bucket = std::list<std::pair<Key, Value>>();

		for (auto& it : bucket.value())
			if (it.first == k)
				return;

		++ele_cnt;
		bucket.value().push_back(std::make_pair(k, v));
	}

	Value& operator[](Key k) {
		auto& bucket = buckets_[Hash(k)];
		if (!bucket.has_value())
			bucket = std::list<std::pair<Key, Value>>();

		for (auto& it : bucket.value())
			if (it.first == k)
				return it.second;

		++ele_cnt;
		bucket.value().push_back(std::make_pair(k, 0));
		return bucket.value().back().second;
	}

private:
	size_t Hash(Key k) {
		if constexpr (std::is_integral_v<Key>)
			return k % buckets_.size();
	}

private:
	static constexpr size_t kSizes[] = {8, 64, 256, 512, 1024, 2048, 4096, 8192};
	size_t size_num_{ 0 };
	std::vector<std::optional<std::list<std::pair<Key, Value>>>> buckets_;
	size_t ele_cnt{ 0 };
};