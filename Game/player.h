#pragma once
#include "engine.h"

class Player
{
public:
	explicit Player(std::string_view name, Color color)
		:
		name_{ name },
		color_{ color }
	{}
	virtual ~Player() = default;

	static std::unique_ptr<Player> create(player::Type,
		std::string_view, Color);

	virtual player::Type type() const = 0;

	virtual Decision decision(Game&) const = 0;
	virtual Move const& move(Moves const&, Game&) const = 0;
	virtual void endTurn(Game&) const = 0;

	bool isAI() const { return type() == player::Type::AI; }
	bool isHuman() const { return type() == player::Type::Human; }

	auto& name() const { return name_; }
	auto color() const { return color_; }

protected:
	std::string name_;
	Color color_;
};

class Human : public Player
{
public:
	using Player::Player;

	player::Type type() const override { return player::Type::Human; }

	Decision decision(Game&) const override;
	Move const& move(Moves const&, Game&) const override;
	void endTurn(Game& game) const override;
};

class AI : public Player
{
public:
	using Player::Player;

	player::Type type() const override { return player::Type::AI; }

	Decision decision(Game&) const override;
	Move const& move(Moves const&, Game&) const override;
	void endTurn(Game&) const override;
};
