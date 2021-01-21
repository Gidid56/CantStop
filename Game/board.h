#pragma once
#include "rules.h"
#include "piece.h"

class Board
{
public:
	explicit Board(Rules const& rules) : rules_{ rules } {}

	void move(Move const&, Player const&);
	void move(Column, Player const&);

	void saveProgress();
	void loseProgress();
	void removeLosingPieces();

	size_t temporaryMarkers() const;

	bool isOpen(Column) const;
	bool isPlaying(Column) const;
	bool canMove(Column) const;
	size_t rowsRemaining(Column, Player const&) const;
	bool isStacked(Piece const&) const;
	bool canStop() const;

	bool wins(Player const&) const;
	bool winsAfterTurn(Player const& player) const;

	Gain currentGain(Player const&) const;
	Row currentRow(Player const&, Column) const;

	auto& rules() const { return rules_; }
	auto& pieces() const { return pieces_; }

private:
	Rules rules_;
	Pieces pieces_;
};
