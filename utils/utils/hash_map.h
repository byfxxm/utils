#pragma once
#include <list>
#include <vector>

template <typename Key, typename Value>
class HashMap {
private:
	using BucketList = std::list<std::pair<Key, Value>>;

public:
	HashMap() {
		buckets_.resize(kSizes[size_num_], BucketList());
	}

	void Insert(Key k, Value v) {
		auto* bucket = &buckets_[Hash(k)];
		for (auto& it : *bucket)
			if (it.first == k)
				return;

		if (Expand())
			bucket = &buckets_[Hash(k)];
		++ele_cnt;
		bucket->push_back(std::make_pair(k, v));
	}

	void Erase(Key k) {
		auto* bucket = &buckets_[Hash(k)];
		for (auto it = bucket->begin(); it != bucket->end(); ++it) {
			if (it->first == k) {
				bucket->erase(it);
				break;
			}
		}
	}

	Value& operator[](Key k) {
		auto* bucket = &buckets_[Hash(k)];
		for (auto& it : *bucket)
			if (it.first == k)
				return it.second;

		if (Expand())
			bucket = &buckets_[Hash(k)];
		++ele_cnt;
		bucket->push_back(std::make_pair(k, 0));
		return bucket->back().second;
	}

private:
	size_t Hash(Key k) {
		if constexpr (std::is_integral_v<Key>)
			return k % kSizes[size_num_];
	}

	bool Expand() {
		if (ele_cnt < kSizes[size_num_])
			return false;
		
		std::vector<BucketList> tmp;
		tmp.resize(kSizes[++size_num_], BucketList());

		for (auto& it : buckets_)
			for (auto& it1 : it)
				tmp[Hash(it1.first)].swap(it);

		buckets_.swap(tmp);
		return true;
	}

private:
	static constexpr size_t kSizes[] = {8, 64, 256, 512, 1024, 2048, 4096, 8192};
	size_t size_num_{ 0 };
	std::vector<BucketList> buckets_;
	size_t ele_cnt{ 0 };
};