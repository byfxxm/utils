#pragma once
#include <list>
#include <vector>

template <typename Key, typename Value>
class HashMap {
private:
	using BucketList = std::list<std::pair<Key, Value>>;

public:
	HashMap() {
		buckets_.resize(kCapacities[capacity_index_], BucketList());
	}

	void Insert(Key k, Value v) {
		auto* bucket = &buckets_[Hash(k)];
		for (auto& pair : *bucket)
			if (pair.first == k)
				return;

		if (Expand())
			bucket = &buckets_[Hash(k)];
		bucket->push_back(std::make_pair(k, v));
		++kv_cnt_;
	}

	void Erase(Key k) {
		auto* bucket = &buckets_[Hash(k)];
		for (auto it = bucket->begin(); it != bucket->end(); ++it) {
			if (it->first == k) {
				--kv_cnt_;
				bucket->erase(it);
				break;
			}
		}

		Reduce();
	}

	Value& operator[](Key k) {
		auto* bucket = &buckets_[Hash(k)];
		for (auto& pair : *bucket)
			if (pair.first == k)
				return pair.second;

		if (Expand())
			bucket = &buckets_[Hash(k)];
		bucket->push_back(std::make_pair(k, 0));
		++kv_cnt_;
		return bucket->back().second;
	}

	size_t Size() {
		return kv_cnt_;
	}

private:
	size_t Hash(Key k) {
		if constexpr (std::is_integral_v<Key>)
			return k & (kCapacities[capacity_index_] - 1);
	}

	void Rehash(size_t size) {
		std::vector<BucketList> tmp;
		tmp.resize(size, BucketList());

		for (auto& list : buckets_)
			for (auto& pair : list)
				tmp[Hash(pair.first)].push_back(pair);

		buckets_.swap(tmp);
	}

	bool Expand() {
		if (kv_cnt_ < kCapacities[capacity_index_])
			return false;

		Rehash(kCapacities[++capacity_index_]);
		return true;
	}

	bool Reduce() {
		if (capacity_index_ == 0 || kv_cnt_ < kCapacities[capacity_index_ - 1])
			return false;

		Rehash(kCapacities[--capacity_index_]);
		return true;
	}

private:
	static constexpr size_t kCapacities[] = { 8, 64, 256, 512, 1024, 2048, 4096, 8192 };
	size_t capacity_index_{ 0 };
	std::vector<BucketList> buckets_;
	size_t kv_cnt_{ 0 };
};
