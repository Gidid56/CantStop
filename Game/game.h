#pragma once
#include "board.h"
#include "engine.h"
#include "moves.h"
#include "player.h"
#include "rng.h"

class Game
{
public:
	explicit Game(UI&);
	virtual ~Game();

	void run();
	void castDice() { rng_.cast(dice_); }

	void makeTurn(Player const&);

	Player const* winner() const;
	bool over() const { return winner() != nullptr; }

	auto& board() const { return board_; }
	auto& dice() const { return dice_; }
	auto& players() const { return players_; }
	auto& rng() { return rng_; }
	auto& ui() { return ui_; }

private:
	Board board_;
	Dice dice_;
	Players players_;
	RNG rng_;
	UI& ui_;
};
