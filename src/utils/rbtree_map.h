#pragma once

template <typename T>
class RbtreeMap {
public:
	enum class Color {
		kRed,
		kBlack,
	};

	struct Node {
		Node* left;
		Node* right;
		T data;
		Color color;
	};

	bool Less() {
		return true;
	}

private:
	void Left(Node*) {

	}

	void Right(Node*) {

	}

private:
	Node* root_{ nullptr };
};
