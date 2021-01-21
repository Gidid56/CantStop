#include "std.pc.h"
#include "moves.h"

#include "board.h"

Moves Move::allMoves(Dice dice)
{
	std::set<Move> allMoves;

	std::ranges::sort(dice);

	do allMoves.emplace(dice);
	while (next_permutation(dice).found);

	return { begin(allMoves), end(allMoves) };
}

Moves Move::possibleMoves(Moves const& allMoves, game::State state)
{
	Moves possibleMoves;

	for (Move move : allMoves)
	{
		move.removeBlocked(state);

		if (move.isPossible())
		{
			if (move.isExclusive(state.board_))
			{
				for (auto& column : move.columns())
					possibleMoves.emplace_back(Columns{ column });
			}
			else
			{
				if (move.isOptional(state.board_))
					for (auto& column : move.columns())
						possibleMoves.emplace_back(Columns{ column });
				possibleMoves.push_back(move);
			}
		}
	}
	return possibleMoves;
}

Moves Move::possibleMoves(Dice const& dice, game::State state)
{
	return possibleMoves(allMoves(dice), state);
}

Move::Move(Dice const& dice)
{
	for (auto die{ begin(dice) }; die < end(dice); advance(die, 2))
	{
		columns_.emplace_back(Column{ (*die + *next(die))() });
	}
	std::ranges::sort(columns_);
}

void Move::removeBlocked(game::State state)
{
	std::map<Column, size_t> rowCounter;

	auto isBlocked = [&state,&rowCounter](Column column)
	{
		++rowCounter[column];

		return not state.board_.canMove(column)
			or rowCounter[column] > state.board_.rowsRemaining(column, state.currentPlayer_);
	};
	std::erase_if(columns_, isBlocked);
}

bool Move::isOptional(Board const& board) const
{
	auto isPlaying = [&board](Column column)
	{
		return board.isPlaying(column);
	};
	return board.rules().moveOptional()
		and ranges::all_are_unique(columns_)
		and size(columns_) > 1;
}

bool Move::isExclusive(Board const& board) const
{
	auto isPlaying = [&board](Column column)
	{
		return board.isPlaying(column);
	};
	return ranges::all_are_unique(columns_)
		and board.temporaryMarkers() == 2
		and none_of(columns_, isPlaying);
}

bool Move::isPlayingAllColumns(Board const& board) const
{
	auto isPlaying = [&board](Column column)
	{
		return board.isPlaying(column);
	};
	return all_of(columns_, isPlaying);
}

Gain Move::gain(game::State state) const
{
	auto gain = [&](Column column)
	{
		auto row{ state.board_.currentRow(state.currentPlayer_, column) };
		return engine::gain(row - Row{ 1 }, column)
			- engine::gain(row, column);
	};
	return ranges::sum(columns_, gain);
}

std::string Move::to_string() const
{	
	return ranges::accumulate(columns_
		| ranges::views::transform(&Column::to_string)
		| ranges::views::intersperse(" | "s), ""s);
}
