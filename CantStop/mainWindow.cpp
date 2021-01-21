#include "qt.pc.h"
#include "MainWindow.h"

#include "game.h"

MainWindow::MainWindow() : QMainWindow{}
{
	ui_.setupUi(this);

	dice_ = { ui_.die_0, ui_.die_1, ui_.die_2, ui_.die_3 };
	playerColors_ = { ui_.color_0, ui_.color_1, ui_.color_2, ui_.color_3 };
	playerFrames_ = { ui_.player_0, ui_.player_1, ui_.player_2, ui_.player_3 };
	playerNames_ = { ui_.name_0, ui_.name_1, ui_.name_2, ui_.name_3 };
	playerAIswitches_ = { ui_.type_0, ui_.type_1, ui_.type_2, ui_.type_3 };

	imageSizes_ = new EnumComboBox<image::Size>;
	ui_.horizontalLayout_imageSize->addWidget(imageSizes_);

	moveRule_ = new EnumComboBox<move::Rule>;
	ui_.horizontalLayout_moveRule->addWidget(moveRule_);

	pieceBehavior_ = new EnumComboBox<piece::Behavior>;
	ui_.horizontalLayout_pieceBehavior->addWidget(pieceBehavior_);

	for (auto&& [widget, color] : zip(playerColors_, piece::colors))
		widget->setPalette(qt::format(color::rgb(color)));

	mediaPlaylist_->addMedia(QUrl{ "qrc:Audio/Dice/Shake.mp3" });
	mediaPlaylist_->addMedia(QUrl{ "qrc:Audio/Dice/Roll.mp3" });
	mediaPlayer_->setPlaylist(mediaPlaylist_);

	connect(imageSizes_, qOverload<int>(&QComboBox::currentIndexChanged),
		this, &MainWindow::drawBoard);
	connect(this, &MainWindow::startUpdate, this, &MainWindow::updateAll);
	connect(this, &MainWindow::startShakeDice, this, &MainWindow::shakeDice,
		Qt::BlockingQueuedConnection);
	connect(this, &MainWindow::error, this, &MainWindow::showError);

	this->load(Rules{});
	this->drawBoard();
	this->grabKeyboard();
	this->adjustSize();
}

MainWindow::~MainWindow() {}

void MainWindow::drawBoard()
{
	for (auto square : squares_)
	{
		square->deleteLater();
	}
	squares_.clear();
	imageSize_ = imageSizes_->current();

	for (auto& column : board::columns)
	{
		for (auto row{ board::row::top }; row < board::startRow(column); ++row)
		{
			auto square{ new QLabel{ ui_.label_Board } };
			square->move(image::position(row, column, imageSize_));
			square->setPixmap(row == board::row::top
				? image::path(column, imageSize_)
				: image::pathSquare(imageSize_));
			square->show();

			squares_.emplace_back(square);
		}
	}
	QPixmap pixmap{ image::pathBoard(imageSize_) };
	ui_.label_Board->setPixmap(pixmap);
	ui_.label_Board->setFixedSize(pixmap.size());
	ui_.frame_Board->setFixedSize(pixmap.size() + frame::board);
	ui_.frame_BottomRight->move(ui_.frame_Board->rect().bottomRight() - position::bottomRight);
	ui_.frame_TopRight->move(ui_.frame_Board->rect().topRight() - position::topRight);

	this->updateAll();
}

void MainWindow::updateUI()
{
	emit startUpdate();
}

void MainWindow::shakeDice()
{
	auto start{ Clock::now() };

	mediaPlaylist_->setCurrentIndex(0);
	mediaPlayer_->play();

	while (Clock::now() - start < this->animationTime())
	{
		game_->castDice();
		emit startUpdate();

		std::this_thread::sleep_for(frameTime_);
	}
	mediaPlaylist_->setCurrentIndex(1);
}

void MainWindow::switchSound()
{
	mediaPlayer_->setMuted(not mediaPlayer_->isMuted());

	ui_.pushButton_Sound->setIcon(QIcon{ image::path(mediaPlayer_) });
}

Duration MainWindow::animationTime() const
{
	return frameTime_ * ui_.dial_AnimationTime->value();
}

size_t MainWindow::numberPlayers() const
{
	return ui_.slider_Players->value();
}

Players MainWindow::players() const
{
	Players players{ this->numberPlayers() };
	
	for (auto&& [index, player] : players | enumerate)
	{
		auto type{ playerAIswitches_.at(index)->isChecked()
			? player::Type::AI : player::Type::Human };
		auto name{ playerNames_.at(index)->text().toStdString() };
		auto color{ piece::colors.at(index) };

		player = Player::create(type, name, color);
	}
	return players;
}

void MainWindow::load(Rules const& rules)
{
	ui_.spinBox_ColumnsToWin->setValue(rules.columsToWin());
	pieceBehavior_->setCurrent(rules.pieceBehavior());
	moveRule_->setCurrent(rules.moveRule());
}

Rules MainWindow::rules() const
{
	Rules rules;
	rules.setWinCondition(ui_.spinBox_ColumnsToWin->value());
	rules.set(pieceBehavior_->current());
	rules.set(moveRule_->current());
	return rules;
}

void MainWindow::showError(QString const& error)
{
	QMessageBox::critical(this, "Error", error);
}
