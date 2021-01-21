#pragma once
#include "ui_mainWindow.h"
#include "ui.h"

#include "images.h"

class MainWindow : public QMainWindow, public UI
{
	Q_OBJECT

signals:
	void startUpdate();
	void startShakeDice();
	void error(QString const&);

public slots:
	void newGame();
	void abortGame();
	void shakeDice();
	void castDice(player::Type = player::Type::Human) override;
	void endTurn(player::Type = player::Type::Human) override;
	void selectMove(gsl::index, player::Type = player::Type::Human) override;
	void movePieces(player::Type = player::Type::Human) override;

	void updateAll();
	void update(Dice const&);
	void update(Board const&);
	void updateStats(game::State);
	void update(Moves const&);
	void update(Player const&);
	void updateControls();
	void updateVisibility();
	void removeRuntimeLabels();

	void switchSound();

	void showError(QString const&);
	void keyPressEvent(QKeyEvent*) override;

public:
	explicit MainWindow();
	~MainWindow();

	void drawBoard();

	void load(Rules const&);

	void updateUI() override;

	Duration animationTime() const override;

	bool canStartGame() const;
	bool canCastDice(player::Type) const;
	bool canEndTurn(player::Type) const;
	bool canMovePieces(player::Type) const;

	Move const* selectedMove() const;
	size_t numberPlayers() const;
	Players players() const;
	Rules rules() const;

private:
	Ui::MainWindowClass ui_;

	std::unique_ptr<Game> game_{ nullptr };
	std::jthread gameThread_, animationThread_;
	std::mutex gameMutex_;

	std::vector<QLabel*> pieces_, squares_;
	std::array<QLabel*, dice::number> dice_;
	std::array<QWidget*, player::max::number> playerColors_;
	std::array<QFrame*, player::max::number> playerFrames_;
	std::array<QLineEdit*, player::max::number> playerNames_;
	std::array<QRadioButton*, player::max::number> playerAIswitches_;
	EnumComboBox<piece::Behavior>* pieceBehavior_;
	EnumComboBox<move::Rule>* moveRule_;
	EnumComboBox<image::Size>* imageSizes_;

	QMediaPlayer* mediaPlayer_{ new QMediaPlayer };
	QMediaPlaylist* mediaPlaylist_{ new QMediaPlaylist };

	bool showStats_{ false }, cheatMode_{ false };

	Duration const frameTime_{ 80ms };

	image::Size imageSize_{ image::Size::Small };
};
