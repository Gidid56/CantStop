#pragma once

class Move
{
public:
	explicit Move(Columns&& columns) : columns_{ std::move(columns) } {}
	explicit Move(Dice const&);

	void removeBlocked(game::State);

	static Moves allMoves(Dice);
	static Moves possibleMoves(Moves const&, game::State);
	static Moves possibleMoves(Dice const&, game::State);

	bool isOptional(Board const&) const;
	bool isExclusive(Board const&) const;
	bool isPlayingAllColumns(Board const&) const;

	Gain gain(game::State) const;

	bool isPossible() const { return not std::empty(columns_); }

	std::string to_string() const;

	bool operator < (Move const& other) const {
		return this->columns_ < other.columns_;
	}

	auto& columns() const { return columns_; }

private:
	Columns columns_;
};
