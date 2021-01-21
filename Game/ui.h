#pragma once
#include "request.h"

// UI is the interface for communication between a game instance
// and its user interface window
// The game requests actions through a Request object

// The mainwindow thread can stop excecution of the game through a stop_token,
// causing a Stop exception to be thrown

class UI
{
public:
	class Stop : public std::exception {};

	virtual ~UI() = default;
	void resetUI();

	std::future<Decision> requestDecision(Board const&);
	std::future<Move const&> requestMove(Moves const&);

	void send(Decision);
	void send(Move const&);
	void select(Player const& player) { player_ = &player; }

	void checkForStop() const;

	virtual void updateUI() = 0;
	virtual void castDice(player::Type) = 0;
	virtual void endTurn(player::Type) = 0;
	virtual void selectMove(gsl::index, player::Type) = 0;
	virtual void movePieces(player::Type) = 0;
	virtual Duration animationTime() const = 0;
	virtual Rules rules() const = 0;
	virtual Players players() const = 0;

	bool isPlayer(player::Type) const;

	void bustPlayer(bool busted) { busted_ = busted; }

	auto player() const { return player_; }
	bool playerBusted() const { return busted_; }

	auto moves() const { return move_.view<Moves>(); }
	auto board() const { return decision_.view<Board>(); }

protected:
	Player const* player_{ nullptr };
	std::atomic<bool> busted_{ false };

	Request<Decision, Board> decision_;
	Request<Move const&, Moves> move_;

	std::stop_token stopToken_;
};
