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
		for (auto& it : buckets_[Hash(k)])
			if (it.first == k)
				return;

		Expand();
		++ele_cnt;
		buckets_[Hash(k)].push_back(std::make_pair(k, v));
	}

	Value& operator[](Key k) {
		for (auto& it : buckets_[Hash(k)])
			if (it.first == k)
				return it.second;

		Expand();
		++ele_cnt;
		auto& bucket = buckets_[Hash(k)];
		bucket.push_back(std::make_pair(k, 0));
		return bucket.back().second;
	}

private:
	size_t Hash(Key k) {
		if constexpr (std::is_integral_v<Key>)
			return k % kSizes[size_num_];
	}

	void Expand() {
		if (ele_cnt < kSizes[size_num_])
			return;
		
		std::vector<BucketList> tmp;
		tmp.resize(kSizes[++size_num_], BucketList());

		for (auto& it : buckets_)
			for (auto& it1 : it)
				tmp[Hash(it1.first)].swap(it);

		buckets_.swap(tmp);
	}

private:
	static constexpr size_t kSizes[] = {8, 64, 256, 512, 1024, 2048, 4096, 8192};
	size_t size_num_{ 0 };
	std::vector<BucketList> buckets_;
	size_t ele_cnt{ 0 };
};