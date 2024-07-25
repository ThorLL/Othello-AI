#pragma once
#include <vector>

#include "AI.h"

class DumAI final : public AI {
	std::pair<int, int> MakeDecition(const GameState &gameState) override {
		if (auto legalMoves = gameState.legalMoves(gameState.currentPlayer); !legalMoves.empty()) return legalMoves[0];
		return {-1, -1};
	}
};
