#include "std.pc.h"
#include "engine.h"

#include "game.h"

namespace engine
{
	const auto moveProbabilities_{ []
	{
		std::map<Moves, Probability> moveProbabilities;

		constexpr auto permutations{ math::power(std::size(die::faces), dice::number) };

		constexpr Probability probability{ 1.0 / permutations };

		for (auto const& dice : cartesian_product(
				die::faces, die::faces, die::faces, die::faces))
		{
			moveProbabilities[Move::allMoves(Dice{ util::to_array(dice) })]
				+= probability;
		}
		return moveProbabilities;
	}() };
}

engine::Node engine::castDice(game::State state)
{
	Node node;
	node.currentGain_ = state.board_.currentGain(state.currentPlayer_);

	for (auto& [moves, probability] : moveProbabilities_)
	{
		auto possibleMoves{ Move::possibleMoves(moves, state) };

		if (not possibleMoves.empty())
		{
			auto gain = [&](Move const& move)
			{
				return move.gain(state);
			};
			auto gains{ possibleMoves | ranges::views::transform(gain) };

			node.successEV_ += probability * *max_element(gains);
			node.successProb_ += probability;
		}
	}
	return node;
}

EV engine::moveEV(Move const& move, game::State state, Gain gainOffset)
{
	Board futureBoard{ state.board_ };
	futureBoard.move(move, state.currentPlayer_);
	return boardEV({ futureBoard, state.currentPlayer_ }, gainOffset);
}

EV engine::boardEV(game::State state, Gain gainOffset)
{
	if (state.board_.temporaryMarkers() == 3)
		return fixedComboEV(state, gainOffset);
#ifdef _DEBUG
	return{};
#else
	return recursiveComboEV(state, gainOffset);
#endif
}

EV engine::fixedComboEV(game::State state, Gain gainOffset)
{
	auto node{ castDice(state) };
	node.currentGain_ += gainOffset;

	constexpr Probability overshoot{ 0.5 };
	auto gainPerStep{ node.successGain() };
	auto successPerFail{ node.successProb_ / node.failProb() };
	auto gainLimit{ gainPerStep * (overshoot + successPerFail) };
	auto steps{ (gainLimit - node.currentGain_) / gainPerStep };
	return gainLimit * std::pow(node.successProb_(), steps);
}

EV engine::recursiveComboEV(game::State state, Gain gainOffset)
{
	auto stayInColumns{ engine::stayInColumns(state) };
	gainOffset += stayInColumns.successEV_ / stayInColumns.failProb();

	auto possibleMoves = [&state](auto const& moveProbability)
	{
		auto& [moves, probability] { moveProbability };

		return std::make_pair(Move::possibleMoves(moves, state), probability);
	};
	auto isPlayingCurrentColumns = [&state](auto const& possibleMove)
	{
		auto& [moves, probability] { possibleMove };

		auto isPlayingAllColumns = [&state](Move const& move)
		{
			return move.isPlayingAllColumns(state.board_);
		};
		return any_of(moves, isPlayingAllColumns);
	};
	auto maxMoveEV = [&](auto const& possibleMove)
	{
		auto& [moves, probability] { possibleMove };

		if (moves.empty())
			return EV{};

		auto calcEV = [&](Move const& move)
		{
			return moveEV(move, state, gainOffset);
		};
		auto moveEVs{ moves | ranges::views::transform(calcEV) };

		return probability * *max_element(moveEVs);
	};

	auto evs{ moveProbabilities_
		| ranges::views::transform(possibleMoves)
		| ranges::views::remove_if(isPlayingCurrentColumns)
		| ranges::views::transform(maxMoveEV)
	};
	return std::reduce(std::execution::par_unseq, begin(evs), end(evs), EV{})
		/ stayInColumns.failProb();
}

engine::Node engine::stayInColumns(game::State state)
{
	Node node;

	for (auto& [moves, probability] : moveProbabilities_)
	{
		auto possibleMoves{ Move::possibleMoves(moves, state) };

		auto isStayingInColumns = [&state](Move const& move)
		{
			return move.isPlayingAllColumns(state.board_);
		};
		auto gain = [&](Move const& move)
		{
			return move.gain(state);
		};
		auto gains{ possibleMoves
			| filter(isStayingInColumns)
			| ranges::views::transform(gain)
		};
		if (auto maxGain{ max_element(gains) };
			maxGain != end(gains))
		{
			node.successEV_ += probability * *maxGain;
			node.successProb_ += probability;
		}
	}
	return node;
}
