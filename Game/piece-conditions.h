#pragma once

namespace piece
{
	constexpr auto is(Type type) {
		return [=](Piece const& piece) {
			return piece.type() == type;
		};
	};

	constexpr auto is(Row row) {
		return [=](Piece const& piece) {
			return piece.row() == row;
		};
	};

	constexpr auto is(Column column) {
		return [=](Piece const& piece) {
			return piece.column() == column;
		};
	};

	constexpr auto is(Color color) {
		return [=](Piece const& piece) {
			return piece.color() == color;
		};
	};

	template<typename Property>
	constexpr auto is_not(Property property) {
		return [=](Piece const& piece) {
			return not is(property)(piece);
		};
	};

	template<typename First, typename ...Rest>
	constexpr auto is(First first, Rest&& ... rest) {
		return[first, ... rest = std::forward<Rest>(rest)](Piece const& piece) {
			return is(first)(piece) and is(rest...)(piece);
		};
	};
}