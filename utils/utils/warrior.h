#pragma once
#include <atomic>
#include <thread>
#include <chrono>
#include <string>

class Warrior {
public:
	Warrior(const std::string& name, int life, int att, int def, int freq) : name_(name), life_(life), att_(att), def_(def), freq_(freq), period_(1000 / freq_) {}
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
			left_life = expect - damage;
			std::this_thread::yield();
		}

		std::cout << name_ << " is attaced. Life left " << left_life << "." << std::endl;
	}

	virtual void ContinuousAttack(Warrior& war) {
		while (IsAlive() && war.IsAlive()) {
			Attack(war);
			std::this_thread::sleep_for(std::chrono::milliseconds(period_));
		}
	}

private:
	std::string name_;
	std::atomic<int> life_{ 1 };
	int def_{ 0 };
	int att_{ 0 };
	int freq_{ 1 };		// ¹¥»÷´ÎÊý/Ãë
	int period_{ 0 };
};