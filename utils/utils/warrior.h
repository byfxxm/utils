#pragma once
#include <atomic>
#include <thread>
#include <chrono>
#include <string>
#include <cstdio>
#include <cmath>
#include <mutex>

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

	Position operator+(const Position& pos) const {
		return Position(x_ + pos.x_, y_ + pos.y_);
	}

	Position operator-(const Position& pos) const {
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
	enum class Status {
		kIdle,
		kAttacking,
		kMoving,
		kDead,
	};

public:
	Warrior(const WarriorInfo& winfo)
		: name_(winfo.name)
		, life_(winfo.life)
		, att_(winfo.att)
		, def_(winfo.def)
		, freq_(winfo.freq)
		, period_(1'000'000 / freq_)
		, range_(winfo.range)
		, pos_(winfo.pos)
	{}

	virtual ~Warrior() {
		if (thrd_.joinable())
			thrd_.join();
	}

	virtual const std::string Name() const {
		return name_;
	}

	virtual Status GetStatus() const {
		return status_.load();
	}

	virtual bool IsDead() const {
		return status_ == Status::kDead;
	}

	virtual bool IsIdle() const {
		return status_ == Status::kIdle;
	}

	virtual bool IsAttacking() const {
		return status_ == Status::kAttacking;
	}

	virtual bool IsMoving() const {
		return status_ == Status::kMoving;
	}

	virtual void Attack(Warrior* war) {
		status_ = Status::kAttacking;
		thrd_ = std::thread([=, this]() {
			AutoAttack(war);
			});
	}

	virtual int Defense(int att) {
		int damage = att - def_;
		life_ -= damage;
		if (life_ <= 0)
			status_ = Status::kDead;
		return damage;
	}

	virtual int Life() const {
		return life_;
	}

protected:
	virtual bool IsInRange(const Warrior* war) const {
		return (pos_ - war->pos_).Length() <= range_;
	}

	virtual void AutoAttack(Warrior* war) {
		auto t0 = std::chrono::steady_clock::now();
		while (!IsDead() && !war->IsDead() && IsInRange(war)) {
			AttackOnce(war);
			if (war->IsDead())
				break;

			while (std::chrono::steady_clock::now() < t0 + std::chrono::microseconds(period_))
				std::this_thread::yield();
			t0 += std::chrono::microseconds(period_);
		}

		status_ = Status::kIdle;
	}

	void AttackOnce(Warrior* war) {
		auto damage = war->Defense(att_);
		printf("%s attack %s cause %d damage points. %s's life is %d left.\n", name_.c_str(), war->name_.c_str(), damage, war->name_.c_str(), war->Life());
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
	std::thread thrd_;
	std::atomic<Status> status_{ Status::kIdle };
};

struct ArcherInfo : public WarriorInfo {
};

class Archer : public Warrior {
public:
	Archer(const ArcherInfo& arc_info) : Warrior(arc_info) { range_ = 10; }
};

class BattleGround {
public:
	static BattleGround* Instance() {
		static BattleGround inst;
		return &inst;
	}

	~BattleGround() {
		Clear();
	}

	void Clear() {
		std::lock_guard<std::mutex> lck(mtx_);
		for (auto w : warriors_)
			delete w;

		warriors_.clear();
	}

	Warrior* CreateWarrior(const WarriorInfo& winfo) {
		std::lock_guard<std::mutex> lck(mtx_);
		ClearDeadIfNeed();
		auto war = new Warrior(winfo);
		warriors_.push_back(war);
		return war;
	}

	Archer* CreateArcher(const ArcherInfo& ainfo) {
		std::lock_guard<std::mutex> lck(mtx_);
		ClearDeadIfNeed();
		auto archer = new Archer(ainfo);
		warriors_.push_back(archer);
		return archer;
	}

private:
	void ClearDeadIfNeed() {
		for (auto it = warriors_.begin(); it != warriors_.end();) {
			if ((*it)->GetStatus() == Warrior::Status::kDead)
				warriors_.erase(it);

			++it;
		}
	}

private:
	std::vector<Warrior*> warriors_;
	std::mutex mtx_;
};