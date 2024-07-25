#include <iostream>
#include <vector>

#include "AI.h"
#include "DumAI.h"
#include "GameState.h"
#include "SmartAI.h"

GameState currentGameState;

std::shared_ptr<AI> player1;
std::shared_ptr<AI> player2;


void repaint() {
	system("clear");
	currentGameState.print();
}

int main() {
	currentGameState.mainBoard = true;
	player1 = std::make_shared<DumAI>();
	player2 = std::make_shared<SmartAI>();
	repaint();
	while (!currentGameState.finished()) {
		auto nextToken = currentGameState.currentPlayer == 1
		? player1->MakeDecition(currentGameState)
		: player2->MakeDecition(currentGameState);

		if (!currentGameState.insertToken(nextToken))
		{
			std::cout << "The AI for player " << currentGameState.currentPlayer <<
				(currentGameState.currentPlayer == 1 ? " (black)" : " (white)") <<
				" chose an invalid move (position " << nextToken.first << ", " << nextToken.second << "). Please debug!";
			exit(1);

		}
		repaint();
	}
	auto [black, white] = currentGameState.countTokens();
	std::cout << "Black: " << black << std::endl;
	std::cout << "White: " << white << std::endl;
	std::cout << (black > white == 1 ? "Black" : "White") << " wins" << std::endl;
}
