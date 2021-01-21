#include "std.pc.h"
#include "board.h"

#include "player.h"
#include "moves.h"
#include "rules.h"

using namespace piece;

void Board::move(Move const& move, Player const& player)
{
	for (auto& column : move.columns())
	{
		this->move(column, player);
	}
}

void Board::move(Column column, Player const& player)
{
	auto temp{ find_if(pieces_,
		is(column, player.color(), Type::Temporary)) };

	if (temp == std::end(pieces_))
	{
		auto fixed{ find_if(pieces_,
			is(column, player.color(), Type::Fixed)) };

		if (fixed == std::end(pieces_))
		{
			pieces_.emplace_back(column, player.color());
		}
		else
		{
			pieces_.emplace_back(*fixed).makeTemporary();
		}
		temp = std::end(pieces_) - 1;
	}

	do temp->moveUpOnce();
	while (rules_.piecesJump() and this->isStacked(*temp));
}

void Board::saveProgress()
{
	for (Piece& temp : pieces_ | filter(is(Type::Temporary)))
	{
		auto fixed{ find_if(pieces_,
			is(temp.column(), temp.color(), Type::Fixed)) };

		if (fixed != std::end(pieces_))
		{
			fixed->moveTo(temp.row());
		}
		else
		{
			temp.makeFixed();
		}
	}
	this->removeLosingPieces();
	this->loseProgress();
}

void Board::removeLosingPieces()
{
	for (auto const& piece : Pieces{ pieces_ })
	{
		if (piece.row() == board::row::top)
		{
			auto isLosing = [column = piece.column()](auto& piece)
			{
				return piece.column() == column
					and piece.row() != board::row::top;
			};
			std::erase_if(pieces_, isLosing);
		}
	}
}

void Board::loseProgress()
{
	std::erase_if(pieces_, is(Type::Temporary));
}

size_t Board::temporaryMarkers() const
{
	return count_if(pieces_, is(Type::Temporary));
}

bool Board::isOpen(Column column) const
{
	return none_of(pieces_, is(column, board::row::top));
}

bool Board::isPlaying(Column column) const
{
	return any_of(pieces_, is(column, Type::Temporary));
}

bool Board::canMove(Column column) const
{
	return isOpen(column)
		and (isPlaying(column) or temporaryMarkers() < 3);
}

size_t Board::rowsRemaining(Column column, Player const& player) const
{
	auto highestPiece{ find_if(std::rbegin(pieces_), std::rend(pieces_),
		is(column, player.color())) };

	if (highestPiece == std::rend(pieces_))
	{
		return board::startRow(column)();
	}
	return highestPiece->row()();
}

bool Board::isStacked(Piece const& piece) const
{
	return any_of(pieces_, is(piece.column(), piece.row(), Type::Fixed));
}

bool Board::canStop() const
{
	auto isStacked = [this](Piece const& piece)
	{
		return this->isStacked(piece);
	};
	return not rules_.piecesBlock()
		or none_of(pieces_ | filter(is(Type::Temporary)), isStacked);
}

bool Board::wins(Player const& player) const
{
	return count_if(pieces_, is(player.color(), board::row::top, Type::Fixed))
		>= rules_.columsToWin();
}

bool Board::winsAfterTurn(Player const& player) const
{
	return count_if(pieces_, is(player.color(), board::row::top))
		>= rules_.columsToWin();
}

Gain Board::currentGain(Player const& player) const
{
	if (winsAfterTurn(player)) return 9'000;

	Gain gain{ 0 };

	for (Piece const& temp : pieces_ | filter(is(Type::Temporary)))
	{
		auto fixed{ find_if(pieces_,
			is(temp.column(), temp.color(), Type::Fixed)) };

		Gain currentGain{ temp.gain() };
		Gain startGain{ fixed != end(pieces_) ? fixed->gain() : 0 };

		gain += currentGain - startGain;
	}
	return gain;
}

Row Board::currentRow(Player const& player, Column column) const
{
	auto reversePieces{ pieces_ | ranges::views::reverse };
	auto piece{ find_if(reversePieces, is(column, player.color())) };

	return piece != end(reversePieces) ? piece->row() : board::startRow(column);
}
