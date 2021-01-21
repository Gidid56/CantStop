#include "std.pc.h"
#include "ui.h"

#include "moves.h"
#include "player.h"

void UI::resetUI()
{
	decision_.reset();
	move_.reset();

	busted_ = {};
	player_ = {};
	stopToken_ = {};
}

std::future<Decision> UI::requestDecision(Board const& board)
{
	auto future{ decision_.request(board) };

	this->updateUI();

	return future;
}

void UI::send(Decision decision)
{
	decision_.try_send(decision);
}

std::future<Move const&> UI::requestMove(Moves const& moves)
{
	auto future{ move_.request(moves) };

	this->updateUI();

	return future;
}

void UI::send(Move const& move)
{
	move_.try_send(move);
}

void UI::checkForStop() const
{
	if (stopToken_.stop_requested())
		throw Stop{};
}

bool UI::isPlayer(player::Type type) const
{
	return player_ and player_->type() == type;
}
