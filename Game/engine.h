#pragma once

namespace engine
{
	constexpr auto aggressionDecay{ 100 };
	constexpr auto lateAggression{ 0.33 };
	constexpr auto earlyAggression{ 1 - lateAggression };

	constexpr Probability step(Column column)
	{
		return std::pow(aggressionDecay, -1.0 / board::startRow(column)());
	}

	constexpr Gain gain(Row row, Column column)
	{
		const auto stepProbability{ step(column) };

		return (earlyAggression * (board::startRow(column) - row)()) / board::startRow(column)()
			+ lateAggression * std::pow(stepProbability(), row());
	}

	struct Node
	{
		EV successEV_;
		Probability successProb_;
		Gain currentGain_;

		auto failProb() const { return complementary(successProb_); }
		Gain successGain() const { return successEV_ / successProb_; }
		EV ev() const { return successEV_ - failProb() * currentGain_; }
	};

	Node castDice(game::State);
	Node stayInColumns(game::State);

	EV moveEV(Move const&, game::State, Gain = {});
	EV boardEV(game::State, Gain);

	EV fixedComboEV(game::State, Gain);
	EV recursiveComboEV(game::State, Gain);

	void printMoves();
	void printCombos();
};
