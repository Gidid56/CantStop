#include "std.pc.h"
#include "engine.h"

#include "game.h"
#include <fstream>

void engine::printMoves()
{
	Board board{ Rules{} };
	Human player{ "dummy", Color::Blue };
	game::State state{ board, player };

	std::ofstream file{ "Moves.csv", std::ofstream::trunc };

	file << "move" << ',' << "EV" << '\n';

	for (auto const& permutation : cartesian_product(
			board::columns, board::columns))
	{
		if (std::get<1>(permutation) >= std::get<0>(permutation))
		{
			auto columns{ util::to_array(permutation) };

			Move move{ Columns{ begin(columns), end(columns) } };
			auto ev{ engine::moveEV(move, state) };

			file << move.to_string() << ',' << ev() << '\n';
		}
	}
	file << std::endl;
}

void engine::printCombos()
{
	Human player{ "dummy", Color::Blue };

	std::ofstream file{ "Combos.csv", std::ofstream::trunc };

	file << "combo" << ',' << "success" << ',' << "EV" << '\n';

	for (auto const& permutation : cartesian_product(
			board::columns, board::columns, board::columns))
	{
		auto columns{ util::to_array(permutation) };

		if (columns.at(2) > columns.at(1)
			and columns.at(1) > columns.at(0))
		{
			Board board{ Rules{} };
			Move move{ Columns{ begin(columns), end(columns) } };
			game::State state{ board, player };

			auto ev{ engine::moveEV(move, state) };
			board.move(move, player);
			auto success{ engine::castDice(state).successProb_ };

			file << move.to_string()
				<< ',' << success()
				<< ',' << ev() 
				<< '\n';
		}
	}
	file << std::endl;
}
