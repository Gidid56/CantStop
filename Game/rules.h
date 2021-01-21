#pragma once

class Rules
{
public:
	void setWinCondition(int columsToWin) { columsToWin_ = columsToWin; }
	void set(piece::Behavior pieceBehavior) { pieceBehavior_ = pieceBehavior; }
	void set(move::Rule moveRule) { moveRule_ = moveRule; }

	bool piecesBlock() const { return pieceBehavior_ == piece::Behavior::Block; }
	bool piecesJump() const { return pieceBehavior_ == piece::Behavior::Jump; }

	bool moveAlways() const { return moveRule_ == move::Rule::Always; }
	bool moveOptional() const { return moveRule_ == move::Rule::Optional; }

	auto columsToWin() const { return columsToWin_; }
	auto pieceBehavior() const { return pieceBehavior_; }
	auto moveRule() const { return moveRule_; }

private:
	int columsToWin_{ 3 };

	piece::Behavior pieceBehavior_{ piece::Behavior::Stack };
	move::Rule moveRule_{ move::Rule::Always };
};
