#pragma once
#include "GameState.h"

class AI {
public:
	virtual ~AI()=default;
	virtual std::pair<int, int> MakeDecition(const GameState &gameState) = 0;
};
