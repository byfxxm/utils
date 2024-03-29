#pragma once
#include <cstring>
#include <cassert>

namespace byfxxm {
	template <size_t Cap>
	class VariableBuffer {
	public:
		struct Head {
			size_t data_size;
		};

	public:
		void Reset() {
			using std::swap;
			VariableBuffer copy;
			swap(this, copy);
		}

		bool IsEmpty() const {
			return read_index_ == write_index_;
		}

		bool IsFull() const {
			return (write_index_ + 1) % Cap == read_index_;
		}

		size_t Free() const {
			return (read_index_ + Cap - write_index_ - 1) % Cap;
		}

		bool Write(const char* buffer, size_t count) {
			if (Free() < count + sizeof(Head))
				return false;

			Head head{ count };
			auto index = write_index_;
			Write(index, (char*)&head, sizeof(head));
			Write(index, buffer, count);
			write_index_ = index;
			assert(write_index_ < Cap);
			return true;
		}

		template <typename T, size_t N>
		bool Write(const T(&arr)[N]) {
			return Write((char*)arr, sizeof(arr));
		}

		bool Read(char* buffer, size_t count, size_t& size) {
			if (IsEmpty() || count < PeekSize())
				return false;

			Head head{ 0 };
			auto index = read_index_;
			Read(index, (char*)&head, sizeof(head));
			Read(index, buffer, head.data_size);
			size = head.data_size;
			read_index_ = index;
			assert(read_index_ < Cap);
			return true;
		}

		template <typename T, size_t N>
		bool Read(T(&arr)[N], size_t& size) {
			return Read((char*)arr, sizeof(arr), size);
		}

	private:
		void Write(size_t& index, const char* buffer, size_t count) {
			if (Cap - index < count) {
				auto part1 = Cap - index;
				auto part2 = count - part1;
				memcpy(&data_[index], buffer, part1);
				memcpy(data_, &buffer[part1], part2);
				index = part2;
				return;
			}

			memcpy(&data_[index], buffer, count);
			index = (index + count) % Cap;
		}

		void Read(size_t& index, char* buffer, size_t count) {
			if (Cap - index < count) {
				auto part1 = Cap - index;
				auto part2 = count - part1;
				memcpy(buffer, &data_[index], part1);
				memcpy(&buffer[part1], data_, part2);
				index = part2;
				return;
			}

			memcpy(buffer, &data_[index], count);
			index = (index + count) % Cap;
		}

		size_t PeekSize() const {
			assert(!IsEmpty());
			if (Cap - read_index_ < sizeof(Head)) {
				auto part1 = Cap - read_index_;
				auto part2 = sizeof(Head) - part1;
				Head head;
				memcpy(&head, &data_[read_index_], part1);
				memcpy((char*)&head + part1, data_, part2);
				return head.data_size;
			}

			return ((Head*)&data_[read_index_])->data_size;
		}

	private:
		volatile size_t read_index_{ 0 };
		volatile size_t write_index_{ 0 };
		char data_[Cap]{};
	};
}
