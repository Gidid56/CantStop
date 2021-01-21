#include "std.pc.h"
#include "game.h"

#include "ui.h"

Game::Game(UI& ui)
	:
	players_{ ui.players() },
	board_{ ui.rules() },
	ui_{ ui }
{}

Game::~Game() {}

void Game::run()
{
	for (auto& player : players_ | cycle
						| drop(rng_.startPlayer(players_)))
	{
		ui_.select(*player);
		this->makeTurn(*player);

		if (board_.wins(*player))
			break;
	}
	ui_.updateUI();
}

void Game::makeTurn(Player const& player)
{
	while (player.decision(*this) == Decision::CastDice
		or not board_.canStop())
	{
		this->castDice();

		auto moves{ Move::possibleMoves(dice_, {board_,player}) };
		sort(moves, &Move::operator <);

		if (moves.empty())
		{
			board_.loseProgress();
			player.endTurn(*this);
			break;
		}
		else
		{
			board_.move(player.move(moves, *this), player);
			dice_ = {};
		}
	}
	board_.saveProgress();
	dice_ = {};
}

Player const* Game::winner() const
{
	for (auto& player : players_)
	{
		if (board_.wins(*player))
			return player.get();
	}
	return nullptr;
}
