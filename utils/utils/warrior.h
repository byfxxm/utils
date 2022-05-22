#pragma once
#include <atomic>
#include <thread>
#include <chrono>
#include <string>
#include <cstdio>

class Warrior {
public:
	Warrior(const std::string& name, int life, int att, int def, int freq) : name_(name), life_(life), att_(att), def_(def), freq_(freq), period_(1'000'000 / freq_) {}
	~Warrior() = default;

	const std::string GetName() const {
		return name_;
	}

	virtual bool IsAlive() const {
		return life_ > 0;
	}

	virtual void Attack(Warrior& war) {
		if (!IsAlive())
			return;

		war.Defense(att_);
	}

	virtual void Defense(int att) {
		if (!IsAlive() || att <= def_)
			return;

		int damage = att - def_;
		int expect = life_;
		int left_life = expect - damage;
		while (!life_.compare_exchange_strong(expect, left_life, std::memory_order_relaxed)) {
			std::this_thread::yield();
			left_life = expect - damage;
		}

		printf("%s is attaced. Life left %d\n", name_.c_str(), left_life);
	}

	virtual void ContinuousAttack(Warrior& war) {
		auto t0 = std::chrono::steady_clock::now();
		while (IsAlive() && war.IsAlive()) {
			Attack(war);
			while (std::chrono::steady_clock::now() < t0 + std::chrono::microseconds(period_))
				std::this_thread::yield();

			t0 += std::chrono::microseconds(period_);
		}
	}

private:
	std::string name_;
	std::atomic<int> life_{ 1 };
	int def_{ 0 };
	int att_{ 0 };
	size_t freq_{ 1 };		// ¹¥»÷´ÎÊý/Ãë
	size_t period_{ 0 };	// ¹¥»÷¼ä¸ô(Î¢Ãë)
};