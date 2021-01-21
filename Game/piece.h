#pragma once
#include "engine.h"

class Piece
{
public:
	explicit Piece(Column column, Color color)
		:
		type_{ piece::Type::Temporary },
		row_{ board::startRow(column) },
		column_{ column },
		color_{ color }
	{}

	void moveUpOnce() { if (row_ != board::row::top) --row_; }
	void moveTo(Row row) { row_ = row; }

	void makeTemporary() { type_ = piece::Type::Temporary; }
	void makeFixed() { type_ = piece::Type::Fixed; }

	Gain gain() const { return engine::gain(row_, column_); }

	bool isTemporary() const { return type_ == piece::Type::Temporary; }
	bool isFixed() const { return type_ == piece::Type::Fixed; }

	auto type() const { return type_; }
	auto row() const { return row_; }
	auto column() const { return column_; }
	auto color() const { return color_; }

private:
	piece::Type type_;
	Row row_;
	Column column_;
	Color color_;
};

#include "piece-conditions.h"
