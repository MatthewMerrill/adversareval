#ifndef MONTECARLO_HPP
#define MONTECARLO_HPP

#include "game.hpp"

GameState MCSelectRoot(GameState state);
float MCWinProb(GameState state, int numTrials, int searchDepth);

#endif
