#include "qt.pc.h"
#include "MainWindow.h"

#include "game.h"

bool MainWindow::canStartGame() const
{
	return not game_ or game_->over();
}

bool MainWindow::canCastDice(player::Type playerType) const
{
	return decision_.isRequesting()
		and UI::isPlayer(playerType)
		and game_->dice() == Dice{};
}

bool MainWindow::canMovePieces(player::Type playerType) const
{
	return move_.isRequesting()
		and UI::isPlayer(playerType);
}

bool MainWindow::canEndTurn(player::Type playerType) const
{
	return (canCastDice(playerType)
		and game_ and game_->board().canStop())
		or UI::playerBusted();
}

void MainWindow::newGame()
{
	std::unique_lock lock{ gameMutex_, std::try_to_lock };

	if (lock.owns_lock() and canStartGame())
	{
		gameThread_ = {};
		game_ = std::make_unique<Game>(*this);

		auto runGame = [this](auto stop_token)
		{
			stopToken_ = stop_token;

			try { game_->run(); }
			catch (std::exception const& exception)
			{
				if (not dynamic_cast<UI::Stop const*>(&exception))
					emit error(exception.what());
			}
		};
		gameThread_ = std::jthread{ runGame };
	}
}

void MainWindow::abortGame()
{
	std::unique_lock lock{ gameMutex_, std::try_to_lock };

	if (lock.owns_lock() and game_)
	{
		gameThread_ = {};
		animationThread_ = {};
		game_ = {};
		this->resetUI();
		this->updateAll();
	}
}

void MainWindow::castDice(player::Type playerType)
{
	std::unique_lock lock{ gameMutex_, std::try_to_lock };

	if (lock.owns_lock() and canCastDice(playerType))
	{
		auto runAnimation = [this]
		{
			this->shakeDice();
			this->send(Decision::CastDice);
		};
		animationThread_ = std::jthread{ runAnimation };
	}
}

void MainWindow::endTurn(player::Type playerType)
{
	std::unique_lock lock{ gameMutex_, std::try_to_lock };

	if (lock.owns_lock() and canEndTurn(playerType))
	{
		this->send(Decision::EndTurn);
	}
}

Move const* MainWindow::selectedMove() const
{
	auto index{ ui_.listWidget_Moves->currentRow() };
	auto moves{ move_.view<Moves>() };

	if (move_.isRequesting()
		and moves and index >= 0 and index < std::size(*moves))
		return &moves->at(index);

	return nullptr;
}

void MainWindow::selectMove(gsl::index index, player::Type playerType)
{
	std::unique_lock lock{ gameMutex_, std::try_to_lock };

	if (lock.owns_lock() and canMovePieces(playerType))
	{
		ui_.listWidget_Moves->setCurrentRow(index);
	}
}

void MainWindow::movePieces(player::Type playerType)
{
	std::unique_lock lock{ gameMutex_, std::try_to_lock };

	if (lock.owns_lock() and canMovePieces(playerType))
	{
		if (auto selectedMove{ this->selectedMove() })
			this->send(*selectedMove);
	}
}

void MainWindow::keyPressEvent(QKeyEvent* keyEvent)
{
	switch (keyEvent->key())
	{
	case Qt::Key::Key_Enter:
		this->endTurn();
		break;

	case Qt::Key::Key_Space:
	case Qt::Key::Key_0:
		this->castDice();
		break;

	case Qt::Key::Key_1:
	case Qt::Key::Key_2:
	case Qt::Key::Key_3:
	case Qt::Key::Key_4:
	case Qt::Key::Key_5:
	case Qt::Key::Key_6:
	case Qt::Key::Key_7:
	case Qt::Key::Key_8:
	case Qt::Key::Key_9:
		this->selectMove(keyEvent->key() - Qt::Key::Key_1);
		this->movePieces();
		break;

	case Qt::Key::Key_F1:
		showStats_ = not showStats_;
		this->updateAll();
		break;

	case Qt::Key::Key_F2:
		this->newGame();
		break;

	case Qt::Key::Key_F4:
		this->abortGame();
		break;

	case Qt::Key::Key_F5:
		this->updateAll();
		break;

	case Qt::Key::Key_Minus:
		ui_.dial_AnimationTime->triggerAction(
			QAbstractSlider::SliderSingleStepAdd);
		break;

	case Qt::Key::Key_Plus:
		ui_.dial_AnimationTime->triggerAction(
			QAbstractSlider::SliderSingleStepSub);
		break;

	case Qt::Key::Key_M:
		this->switchSound();
		break;

	case Qt::Key::Key_K:
		if (keyEvent->modifiers() == Qt::ControlModifier)
			cheatMode_ = not cheatMode_;
		break;

	default:
		QMainWindow::keyPressEvent(keyEvent);
	}
}