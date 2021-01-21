#pragma once

class RNG
{
	using Distribution = std::uniform_int_distribution<size_t>;

public:
	void cast(Dice& dice)
	{
		std::ranges::generate(dice, [this] { return static_cast<Die::type>(dieFaces_(rng_)); });
	}

	auto startPlayer(Players const& players)
	{
		return Distribution{ 0, std::size(players) - 1 }(rng_);
	}

private:
	std::mt19937 rng_{ std::random_device{}() };
	Distribution dieFaces_{ die::faces.front()(), die::faces.back()() };
};