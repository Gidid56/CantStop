#pragma once

namespace frame { constexpr QSize board{ 20, 20 }; }

namespace position
{
	constexpr QPoint topRight{ 120, 0 };
	constexpr QPoint bottomRight{ 100, 120 };
}

namespace image
{
	enum class Size { Small, Medium, Large };

	constexpr auto suffix{ ".png" };
	
	QString path(image::Size);

	QString pathBoard(image::Size);

	QString path(Die);

	QString path(Column, image::Size);

	QString pathSquare(image::Size);

	QString path(Piece const&, image::Size);

	QString path(QMediaPlayer const*);

	QPoint position(Row, Column, image::Size);
}
