#pragma once

template<size_t N>
class VariableBuffer {
public:
	struct Head {
		size_t data_length;
	};

public:
	~VariableBuffer() {
		std::cout << std::endl;
	}

	void Reset() {
		read_index_ = write_index_;
	}

	bool IsEmpty() {
		return read_index_ == write_index_;
	}

	bool IsFull() {
		return (write_index_ + 1) % N == read_index_;
	}

	size_t Free() {
		return (read_index_ + N - write_index_ - 1) % N;
	}

	bool Write(const char* data, size_t count) {
		if (Free() < count)
			return false;

		Head head{ count };
		auto index = write_index_;
		Write(index, (char*)&head, sizeof(head));
		Write(index, data, count);
		write_index_ = index;
		return true;
	}

	bool Read(char* data, size_t& count) {
		if (IsEmpty())
			return false;

		Head head{ 0 };
		auto index = read_index_;
		Read(index, (char*)&head, sizeof(head));
		Read(index, data, head.data_length);
		count = head.data_length;
		read_index_ = index;
		return true;
	}

private:
	void Write(size_t& index, const char* data, size_t count) {
		if (N - index < count) {
			auto part1 = N - index;
			auto part2 = count - part1;
			memcpy(&buffer_[index], data, part1);
			memcpy(buffer_, &data[part1], part2);
			index = part2;
			return;
		}

		memcpy(&buffer_[index], data, count);
		index += count;
	}

	void Read(size_t& index, char* data, size_t count) {
		if (N - index < count) {
			auto part1 = N - index;
			auto part2 = count - part1;
			memcpy(data, &buffer_[index], part1);
			memcpy(&data[part1], buffer_, part2);
			index = part2;
			return;
		}

		memcpy(data, &buffer_[index], count);
		index += count;
	}

private:
	volatile size_t read_index_{ 0 };
	volatile size_t write_index_{ 0 };
	char buffer_[N]{};
};