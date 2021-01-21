#include "qt.pc.h"
#include "images.h"

#include "piece.h"

QString image::path(image::Size imageSize)
{
	return ":/Images/" + qt::format(imageSize) + "/";
}

QString image::pathBoard(image::Size imageSize)
{
	return path(imageSize) + "Board" + suffix;
}

QString image::path(Die die)
{
	return ":/Images/Dice/" + qt::format(die()) + suffix;
}

QString image::pathSquare(image::Size imageSize)
{
	return path(imageSize) + "Squares/Empty" + suffix;
}

QString image::path(Column column, image::Size imageSize)
{
	return path(imageSize) + "Squares/" + qt::format(column()) + suffix;
}

QString image::path(Piece const& piece, image::Size imageSize)
{
	return path(imageSize) + "Pieces/" + qt::format(
		piece.isTemporary() ? Color::White : piece.color()) + suffix;
}

QString image::path(QMediaPlayer const* mediaPlayer)
{
	if (mediaPlayer->isMuted())
		return QString{ ":/Icons/Muted" } + suffix;
	else
		return QString{ ":/Icons/Sound" } + suffix;
}

namespace board
{
	constexpr std::array<QPoint, column::number> positions{
		QPoint{ 75, 295 },
		QPoint{ 145, 225 },
		QPoint{ 215, 155 },
		QPoint{ 285, 105 },
		QPoint{ 355, 75 },
		QPoint{ 425, 65 },
		QPoint{ 495, 75 },
		QPoint{ 565, 105 },
		QPoint{ 635, 155 },
		QPoint{ 705, 225 },
		QPoint{ 775, 295 },
	};

	constexpr std::array<size_t, column::number> deltas{
		130,100,90,80,70,60,70,80,90,100,130
	};
}

QPoint image::position(Row row, Column column, image::Size imageSize)
{
	auto index{ board::index(column) };

	auto position{ board::positions.at(index) - QPoint{ 1, 1 } };

	position.ry() += row() * board::deltas.at(index);

	switch (imageSize)
	{
	case image::Size::Small:
		position *= 3;
		position /= 4;
		break;
	case image::Size::Large:
		position *= 5;
		position /= 4;
		break;
	}

	return position;
}
