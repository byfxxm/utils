#pragma once
#include <optional>
#include <list>
#include <array>

template <typename Key, typename Value>
class HashMap {
public:
	void Insert(Key k, Value v) {
		bucket_[Hash(k)].push_back(std::make_pair(k, v));
	}

	Value& operator[](Key k) {
		auto hash = Hash(k);
		for (auto& it : bucket_[hash])
			if (it.first == k)
				return it.second;

		bucket_[hash].push_back(std::make_pair(k, 0));
		return bucket_[hash].back().second;
	}

private:
	size_t Hash(Key k) {
		if constexpr (std::is_integral_v<Key>)
			return k % bucket_.size();
	}

private:
	std::array<std::list<std::pair<Key, Value>>, 100> bucket_;
};