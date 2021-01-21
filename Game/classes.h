#pragma once

class Board;
class Engine;
class Game;
class Move;
class Piece;
class Player;
class Rules;
class UI;

using Moves = std::vector<Move>;
using Pieces = std::vector<Piece>;
using Players = std::vector<std::unique_ptr<Player>>;
