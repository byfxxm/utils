#pragma once

template<int Size>
class VariableBuffer {
private:
	using Byte = unsigned char;

private:
	Byte buffer_[Size]{};
};