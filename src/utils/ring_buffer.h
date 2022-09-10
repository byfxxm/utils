#pragma once
#include <type_traits>

template <typename Ty, size_t Num>
	requires (Num > 0)
class RingBuffer {
public:
	void Reset() {
		read_index_ = write_index_;
	}

	bool IsEmpty() {
		return read_index_ == write_index_;
	}

	bool IsFull() {
		return Mod(write_index_ + 1) == read_index_;
	}

	template <class T>
		requires std::same_as<Ty, std::decay_t<T>>
	bool Write(T&& t) {
		if (IsFull())
			return false;

		data_[write_index_] = t;
		write_index_ = Mod(write_index_ + 1);
		return true;
	}

	template <class T>
		requires std::same_as<Ty, std::decay_t<T>>
	bool Read(T&& t) {
		if (IsEmpty())
			return false;

		t = data_[read_index_];
		read_index_ = Mod(read_index_ + 1);
		return true;
	}

private:
	size_t Mod(size_t num) {
		if constexpr ((Num & (Num - 1)) == 0)
			return (num & (Num - 1));
		else
			return (num % Num);
	}

private:
	volatile size_t read_index_{ 0 };
	volatile size_t write_index_{ 0 };
	Ty data_[Num];
};
