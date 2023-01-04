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
		: _name(winfo.name)
		, _life(winfo.life)
		, _att(winfo.att)
		, _def(winfo.def)
		, _freq(winfo.freq)
		, _period(1'000'000 / _freq)
		, _range(winfo.range)
		, _pos(winfo.pos)
	{}

	virtual ~Warrior() {
		if (_thread.joinable())
			_thread.join();
	}

	virtual const std::string Name() const {
		return _name;
	}

	virtual Status GetStatus() const {
		return _status.load();
	}

	virtual bool IsDead() const {
		return _status == Status::kDead;
	}

	virtual bool IsIdle() const {
		return _status == Status::kIdle;
	}

	virtual bool IsAttacking() const {
		return _status == Status::kAttacking;
	}

	virtual bool IsMoving() const {
		return _status == Status::kMoving;
	}

	virtual void Attack(Warrior* war) {
		_status = Status::kAttacking;
		_thread = std::thread([=, this]() {
			_AutoAttack(war);
			});
	}

	virtual int Defense(int att) {
		int damage = att - _def;
		_life -= damage;
		if (_life <= 0)
			_status = Status::kDead;
		return damage;
	}

	virtual int Life() const {
		return _life;
	}

protected:
	virtual bool _IsInRange(const Warrior* war) const {
		return (_pos - war->_pos).Length() <= _range;
	}

	virtual void _AutoAttack(Warrior* war) {
		auto t0 = std::chrono::steady_clock::now();
		while (!IsDead() && !war->IsDead() && _IsInRange(war)) {
			_AttackOnce(war);
			if (war->IsDead())
				break;

			while (std::chrono::steady_clock::now() < t0 + std::chrono::microseconds(_period))
				std::this_thread::yield();
			t0 += std::chrono::microseconds(_period);
		}

		_status = Status::kIdle;
	}

	void _AttackOnce(Warrior* war) {
		auto damage = war->Defense(_att);
		printf("%s attack %s cause %d damage points. %s's life is %d left.\n", _name.c_str(), war->_name.c_str(), damage, war->_name.c_str(), war->Life());
	}

protected:
	std::string _name;
	std::atomic<int> _life{ 1 };
	int _def{ 0 };
	int _att{ 0 };
	size_t _freq{ 1 };		// ¹¥»÷´ÎÊý/Ãë
	size_t _period{ 0 };	// ¹¥»÷¼ä¸ô(Î¢Ãë)
	double _range{ 0.5 };
	Position _pos{ 10,10 };
	std::thread _thread;
	std::atomic<Status> _status{ Status::kIdle };
};

struct ArcherInfo : public WarriorInfo {
};

class Archer : public Warrior {
public:
	Archer(const ArcherInfo& arc_info) : Warrior(arc_info) { _range = 10; }
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
		std::lock_guard<std::mutex> lck(_mtx);
		for (auto w : _warriors)
			delete w;

		_warriors.clear();
	}

	Warrior* CreateWarrior(const WarriorInfo& winfo) {
		std::lock_guard<std::mutex> lck(_mtx);
		_ClearDeadIfNeed();
		auto war = new Warrior(winfo);
		_warriors.push_back(war);
		return war;
	}

	Archer* CreateArcher(const ArcherInfo& ainfo) {
		std::lock_guard<std::mutex> lck(_mtx);
		_ClearDeadIfNeed();
		auto archer = new Archer(ainfo);
		_warriors.push_back(archer);
		return archer;
	}

private:
	void _ClearDeadIfNeed() {
		for (auto it = _warriors.begin(); it != _warriors.end();) {
			if ((*it)->GetStatus() == Warrior::Status::kDead)
				_warriors.erase(it);

			++it;
		}
	}

private:
	std::vector<Warrior*> _warriors;
	std::mutex _mtx;
};
