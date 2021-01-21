#include "std.pc.h"
#include "player.h"

#include "game.h"
#include "ui.h"

Decision Human::decision(Game& game) const
{
	auto decision{ game.ui().requestDecision(game.board()) };
	
	game.ui().updateUI();

	while (decision.wait_for(10ms) != std::future_status::ready)
		game.ui().checkForStop();

	return decision.get();
}

Move const& Human::move(Moves const& moves, Game& game) const
{
	auto move{ game.ui().requestMove(moves) };

	while (move.wait_for(10ms) != std::future_status::ready)
		game.ui().checkForStop();

	return move.get();
}

void Human::endTurn(Game& game) const
{
	game.ui().bustPlayer(true);
	this->decision(game);
	game.ui().bustPlayer(false);
}

Decision AI::decision(Game& game) const
{
	game.ui().checkForStop();

	auto decision{ game.ui().requestDecision(game.board()) };

	auto start{ Clock::now() };

	while (decision.wait_for(10ms) != std::future_status::ready)
	{
		if (engine::castDice({ game.board(), *this }).ev() > 0)
			game.ui().castDice(player::Type::AI);
		else 
			game.ui().endTurn(player::Type::AI);
	}
	std::this_thread::sleep_until(start + game.ui().animationTime());

	return decision.get();
}

Move const& AI::move(Moves const& moves, Game& game) const
{
	game.ui().checkForStop();

	auto move{ game.ui().requestMove(moves) };

	auto start{ Clock::now() };

	auto moveEV = [this, &game](Move const& move)
	{
		return engine::moveEV(move, { game.board(), *this });
	};
	auto moveIndex{ max_element(moves, {}, moveEV) - begin(moves) };

	std::this_thread::sleep_until(start + game.ui().animationTime() / 2);

	while (move.wait_for(10ms) != std::future_status::ready)
	{
		game.ui().selectMove(moveIndex, player::Type::AI);
		game.ui().movePieces(player::Type::AI);
	}
	std::this_thread::sleep_until(start + game.ui().animationTime());

	return move.get();
}

void AI::endTurn(Game& game) const
{
	game.ui().checkForStop();
	game.ui().updateUI();

	std::this_thread::sleep_for(game.ui().animationTime());
}

std::unique_ptr<Player> Player::create(player::Type type,
	std::string_view name, Color color)
{
	switch (type)
	{
	case player::Type::Human:
		return std::make_unique<Human>(name, color);
	case player::Type::AI:
		return std::make_unique<AI>(name, color);
	default:
		return nullptr;
	}
}
