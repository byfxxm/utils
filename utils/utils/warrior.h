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

	bool IsAlive() const {
		return life_ > 0;
	}

	void Attack(Warrior& war) {
		auto t0 = std::chrono::steady_clock::now();
		while (IsAlive() && war.IsAlive()) {
			AttackOnce(war);
			while (std::chrono::steady_clock::now() < t0 + std::chrono::microseconds(period_))
				std::this_thread::yield();

			t0 += std::chrono::microseconds(period_);
		}
	}

private:
	virtual void AttackOnce(Warrior& war) {
		int damage = att_ - war.def_;
		if (!IsAlive() || !war.IsAlive() || damage <= 0)
			return;

		int cur_life = war.life_;
		while (!war.life_.compare_exchange_strong(cur_life, cur_life - damage, std::memory_order_relaxed))
			std::this_thread::yield();

		printf("%s attack %s cause %d damage points. %s's life is %d left.\n", name_.c_str(), war.name_.c_str(), damage, war.name_.c_str(), cur_life - damage);
	}

private:
	std::string name_;
	std::atomic<int> life_{ 1 };
	int def_{ 0 };
	int att_{ 0 };
	size_t freq_{ 1 };		// ¹¥»÷´ÎÊý/Ãë
	size_t period_{ 0 };	// ¹¥»÷¼ä¸ô(Î¢Ãë)
};