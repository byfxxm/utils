#pragma once

template <typename T, size_t N>
class RingBuffer {
public:
	void Reset() {
		read_index_ = write_index_;
	}

	bool IsEmpty() {
		return read_index_ == write_index_;
	}

	bool IsFull() {
		return (write_index_ + 1) % N == read_index_;
	}

	bool Write(T t) {
		if (IsFull())
			return false;

		data_[write_index_] = t;
		write_index_ = (write_index_ + 1) % N;
		return true;
	}

	bool Read(T& t) {
		if (IsEmpty())
			return false;

		t = data_[read_index_];
		read_index_ = (read_index_ + 1) % N;
		return true;
	}

private:
	volatile size_t read_index_{ 0 };
	volatile size_t write_index_{ 0 };
	T data_[N];
};