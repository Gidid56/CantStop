#pragma once
#include "classes.h"

using Gain = Named<double, struct Gain_, Comparable, Negatable, Scalable, Summable>;
using EV = Gain;

enum class Decision : size_t { CastDice, EndTurn, };

using Die = Named<size_t, struct Die_, Comparable, Summable, Printable>;
namespace die { constexpr std::array<Die, 6> faces{ 1,2,3,4,5,6 }; }

namespace dice{ constexpr auto number{ 4 }; }
using Dice = std::array<Die, dice::number>;

using Row = Named<size_t, struct Row_, Comparable, Negatable, Summable, Incrementable, Decrementable>;
using Column = Named<size_t, struct Column_, Comparable, Printable>;
using Columns = std::vector<Column>;

namespace board
{
	namespace column { constexpr auto number{ 11 }; }

	namespace row { constexpr Row top{ 0 }; }

	constexpr std::array<Column, column::number> columns{
		2,3,4,5,6,7,8,9,10,11,12
	};
	constexpr std::array<Row, column::number> rows {
		3,5,7,9,11,13,11,9,7,5,3
	};

	constexpr gsl::index index(Column column) { return column() - 2; }

	constexpr Row startRow(Column column) { return rows.at(index(column)); }
}

namespace player
{
	enum class Type : size_t { Human, AI, };

	namespace max { constexpr auto number{ 4 }; }
}

namespace piece
{
	enum class Type : size_t { Temporary, Fixed };
	enum class Behavior : size_t { Stack, Block, Jump };

	constexpr std::array<Color, player::max::number> colors {
		Color::Blue, Color::Yellow, Color::Green, Color::Orange
	};
}

namespace move
{
	enum class Rule : size_t { Always, Optional };
}

namespace game
{
	struct State
	{
		Board const& board_;
		Player const& currentPlayer_;
	};
}