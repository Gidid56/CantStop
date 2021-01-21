#include "qt.pc.h"
#include "MainWindow.h"

#include "game.h"

void MainWindow::updateAll()
{
	this->setUpdatesEnabled(false);
	this->removeRuntimeLabels();

	if (game_) this->update(game_->dice());
	if (game_) this->update(game_->board());

	if (auto moves{ UI::moves() }) this->update(*moves);
	if (auto player{ UI::player() }) this->update(*player);

	this->updateControls();
	this->updateVisibility();

	this->setUpdatesEnabled(true);
}

void MainWindow::update(Dice const& dice)
{
	for (auto&& [index, die] : dice | enumerate)
	{
		dice_.at(index)->setPixmap(image::path(die));
	}
}

void MainWindow::update(Board const& board)
{
	for (auto& piece : board.pieces())
	{
		auto label{ new QLabel{ ui_.label_Board } };
		label->setPixmap(image::path(piece, imageSize_));
		label->move(image::position(piece.row(), piece.column(), imageSize_));
		label->show();
		if (piece.isTemporary())
			label->raise();

		pieces_.emplace_back(label);
	}
	ui_.label_Board->lower();
}

void MainWindow::updateStats(game::State state)
{
	auto start{ Clock::now() };

	if (move_.isObserving())
	{
		if (auto selectedMove{ this->selectedMove() })
		{
			auto ev{ engine::moveEV(*selectedMove, state) };

			ui_.label_EV->setText(qt::format(ev(), "Lines"));
		}
	}
	else
	{
		auto node{ engine::castDice(state) };

		ui_.label_Success->setText(qt::format(node.successProb_));
		ui_.label_Progress->setText(qt::format(node.currentGain_(), "Lines"));
		ui_.label_EV->setText(qt::format(node.ev()(), "Lines"));
	}
	ui_.label_Time->setText(qt::format<microseconds>(Clock::now() - start));
}

void MainWindow::update(Moves const& moves)
{
	ui_.listWidget_Moves->clear();

	for (auto& move : moves)
	{
		auto item{ new QListWidgetItem{ qt::format(move.to_string()) } };
		item->setTextAlignment(Qt::AlignCenter);
		ui_.listWidget_Moves->addItem(item);
	}

	ui_.listWidget_Moves->setFixedHeight(
		2 * ui_.listWidget_Moves->frameWidth()
		+ moves.size() * ui_.listWidget_Moves->sizeHintForRow(0)
	);
}

void MainWindow::update(Player const& player)
{
	ui_.label_PlayerName->setText(
		qt::format(player.name()) + (player.isAI() ? " (AI)" : ""));

	ui_.widget_PlayerColor->setPalette(qt::format(color::rgb(player.color())));
}

void MainWindow::updateControls()
{
	auto numberPlayers{ ui_.slider_Players->value() };

	ui_.spinBox_ColumnsToWin->setMaximum(1 + 10 / numberPlayers);

	for (auto&& [index, playerFrame] : playerFrames_ | enumerate)
	{
		playerFrame->setVisible(index < numberPlayers);
	}
}

void MainWindow::updateVisibility()
{
	auto player{ UI::player() };
	auto moves{ UI::moves() };

	auto isGameOver{ game_ and game_->over() };
	auto hasDice{ game_ and game_->dice() != Dice{} };

	auto canStartGame{ this->canStartGame() };
	auto canCastDice{ this->canCastDice(player::Type::Human) };
	auto canEndTurn{ this->canEndTurn(player::Type::Human) };
	auto canMovePieces{ this->canMovePieces(player::Type::Human) };

	auto showEV{ canCastDice or canMovePieces };

	ui_.pushButton_NewGame->setVisible(canStartGame);
	ui_.pushButton_CastDice->setVisible(canCastDice);
	ui_.pushButton_EndTurn->setVisible(canEndTurn);
	ui_.listWidget_Moves->setEnabled(canMovePieces);

	ui_.frame_Player->setVisible(player or isGameOver);
	ui_.label_Winner->setVisible(isGameOver);
	ui_.groupBox_Dice->setVisible(hasDice);
	ui_.groupBox_Moves->setVisible(moves);
	ui_.groupBox_Players->setVisible(canStartGame);
	ui_.groupBox_Rules->setVisible(canStartGame);

	ui_.frame_Success->setVisible(showStats_ and canCastDice);
	ui_.frame_Progress->setVisible(showStats_ and canCastDice);
	ui_.frame_Time->setVisible(showStats_ and showEV);
	ui_.frame_EV->setVisible(showStats_ and showEV);

	if (game_ and player_ and showStats_ and showEV)
		this->updateStats({ game_->board(), *player_ });
}

void MainWindow::removeRuntimeLabels()
{
	for (auto piece : pieces_)
	{
		piece->deleteLater();
	}
	pieces_.clear();
}
