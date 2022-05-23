#pragma once
#include <atomic>
#include <thread>
#include <chrono>
#include <string>
#include <cstdio>
#include <cmath>

class Position {
public:
	Position(double x, double y) : x_(x), y_(y) {}

	double Length() {
		return sqrt(x_ * x_ + y_ * y_);
	}

	double X() {
		return x_;
	}

	double Y() {
		return y_;
	}

	Position operator+(const Position& pos) {
		return Position(x_ + pos.x_, y_ + pos.y_);
	}

	Position operator-(const Position& pos) {
		return Position(x_ - pos.x_, y_ - pos.y_);
	}

	Position& operator+=(const Position& pos) {
		x_ += pos.x_;
		y_ += pos.y_;
		return *this;
	}

	Position& operator-=(const Position& pos) {
		x_ -= pos.x_;
		y_ -= pos.y_;
		return *this;
	}

private:
	double x_;
	double y_;
};

struct WarriorInfo {
	const char* name{ nullptr };
	int life{ 1 };
	int att{ 0 };
	int def{ 0 };
	int freq{ 1 };
	double range{ 0.5 };
	Position pos{ 0,0 };
};

class Warrior {
public:
	Warrior(const WarriorInfo& war_info)
		: name_(war_info.name)
		, life_(war_info.life)
		, att_(war_info.att)
		, def_(war_info.def)
		, freq_(war_info.freq)
		, period_(1'000'000 / freq_)
		, range_(war_info.range)
		, pos_(war_info.pos)
	{}

	virtual ~Warrior() = default;

	virtual const std::string GetName() const {
		return name_;
	}

	virtual bool IsAlive() const {
		return life_ > 0;
	}

	virtual void Attack(Warrior& war) {
		auto t0 = std::chrono::steady_clock::now();
		while (IsAlive() && war.IsAlive()) {
			AttackOnce(war);
			while (std::chrono::steady_clock::now() < t0 + std::chrono::microseconds(period_))
				std::this_thread::yield();

			t0 += std::chrono::microseconds(period_);
		}
	}

protected:
	void AttackOnce(Warrior& war) {
		int damage = att_ - war.def_;
		if (!IsAlive() || !war.IsAlive() || damage <= 0)
			return;

		int cur_life = war.life_;
		while (!war.life_.compare_exchange_strong(cur_life, cur_life - damage, std::memory_order_relaxed))
			std::this_thread::yield();

		printf("%s attack %s cause %d damage points. %s's life is %d left.\n", name_.c_str(), war.name_.c_str(), damage, war.name_.c_str(), cur_life - damage);
	}

protected:
	std::string name_;
	std::atomic<int> life_{ 1 };
	int def_{ 0 };
	int att_{ 0 };
	size_t freq_{ 1 };		// ¹¥»÷´ÎÊý/Ãë
	size_t period_{ 0 };	// ¹¥»÷¼ä¸ô(Î¢Ãë)
	double range_{ 0.5 };
	Position pos_{ 10,10 };
};

struct ArcherInfo : public WarriorInfo {
};

class Archer : public Warrior {
public:
	Archer(const ArcherInfo& arc_info) : Warrior(arc_info) { range_ = 10; }
};