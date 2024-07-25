#pragma once
#include <unordered_map>
#include <vector>

#include "AI.h"

class SmartAI : public AI{
    int player;
    int enemy;

    const float maMulti = 6;     //Move advantage
    const float ddMulti = 4;     //Disc difference
    const float cMulti = 10;     //Corners
    const int MAXDEPTH = 7;
public:
    long long totalTime = 0.0f;
    long long copying = 0.0f;
    long long evaling = 0.0f;

    void printTimings() const {
        std::cout << "Total time: "<< totalTime / 1000000 << "ms" << std::endl;
        std::cout << "Copying: "<< copying / 1000000 << "ms / " << (float) copying / (float) totalTime << "%" << std::endl;
        std::cout << "Evaluating: "<< evaling / 1000000 << "ms / " << (float) evaling / (float) totalTime << "%" << std::endl;
    }

    std::pair<int, int> MakeDecition(const GameState &gameState) override {
        player = gameState.currentPlayer;
        enemy = player == 1 ? 2 : 1;
        auto v = MaxValue(
            gameState,
            -std::numeric_limits<float>::infinity(),
            std::numeric_limits<float>::infinity(),
            0,
            {}
        ).second;
        return v;
    }

    std::pair<float, std::pair<int, int>> MaxValue(
        const GameState &gameState,
        float a,
        const float b,
        const int depth,
        std::unordered_map<std::string, float> checked
        ) {
        // If max depth is hit or there are no longer any legal moves, then evaluates game state
        const auto &legalMoves = gameState.legalMoves(player);
        if (depth == MAXDEPTH || legalMoves.empty()) return {eval(gameState), {-1, -1}};

        float v = -std::numeric_limits<float>::infinity();
        std::pair<int, int> move{};

        for (const auto &pos : legalMoves) {
            auto *game = new GameState(gameState);
            game->insertToken(pos);
            auto hash = game->hash(player);
            float value;
            if (const auto it = checked.find(hash); it != checked.end()) {
                value = checked[hash];
            } else {
                value = MinValue(*game, a, b, depth + 1,checked);
                checked[hash] = value;
            }
            delete game;
            if(value > v){
                v = value;
                move = pos;         // updates move to the best possible move (highest value).
                a = std::max(a,v);  // updates alpha.
            }
            if(v >= b) break;       // performs a beta cut if v is greater or equal to beta.
        }
        return {v, move};
    }

    // Same as MaxValue except returns the lowest value and no move
    float MinValue(
        const GameState &gameState,
        const float a,
        float b,
        const int depth,
        std::unordered_map<std::string, float> checked
        ) {
        if (depth == MAXDEPTH) return eval(gameState);

        float v = std::numeric_limits<float>::infinity();

        for (std::pair<int, int> pos :gameState.legalMoves(enemy)) {
            auto *game = new GameState(gameState);
            game->insertToken(pos);
            auto hash = game->hash(enemy);
            float value;
            if (const auto it = checked.find(hash); it != checked.end()) {
                value = checked[hash];
            } else {
                value = MaxValue(*game,a,b,depth+1,checked).first;
                checked[hash] = value;
            }
            delete game;
            if(value < v) {
                v = value;
                b = std::min(b,v);
            }
            if(v <= a) break;
        }
        return v;
    }

    // Evaluation function.
    float eval(const GameState &gameState) {
        if (gameState.finished())
            return discAdvantage(gameState, true);
        const float ma = getMovesAdvantage(gameState);
        const float dd = discAdvantage(gameState, false);
        const float cc = cornerAdvantage(gameState);
        const auto v = maMulti * ma + ddMulti * dd + cMulti * cc;

        return v;
    }

    // Returns how many moves Max has compared to Min taken into account the total amount of moves.
    float getMovesAdvantage(const GameState& gameState) const {

        const auto playerMoves = gameState.legalMoves(player).size();
        const auto enemyMoves = gameState.legalMoves(enemy).size();
        return static_cast<float>(playerMoves - enemyMoves) / static_cast<float>(enemyMoves + playerMoves);
    }

    // Returns how many disc Max has compared to Min taken into account the total amount of discs.
    float discAdvantage(const GameState& gameState, const bool gameOver) const{
        auto [playerDiscs, enemyDiscs] = gameState.countTokens();
        if (player == 2) {
            const auto temp = playerDiscs;
            playerDiscs = enemyDiscs;
            enemyDiscs = temp;
        }
        if (gameOver) return playerDiscs > enemyDiscs ? std::numeric_limits<float>::max() : std::numeric_limits<float>::min();
        return static_cast<float>(playerDiscs - enemyDiscs) / static_cast<float>(enemyDiscs + playerDiscs);
    }

    // Returns how many corners Max has compared to Min taken into account the total amount of corners.
    float cornerAdvantage(const GameState& gameState) const {
        int myCorners = 0, opCorners = 0;
        for (const auto g: {0, 56, 7, 63}) {
            if (const auto cell = gameState.get(g); cell == player) ++myCorners;
            else if (cell == enemy) ++opCorners;
        }
        if (myCorners + opCorners == 0) return 0.0f;
        const auto _myCorners = static_cast<float>(myCorners), _opCorners = static_cast<float>(opCorners);
        return (_myCorners - _opCorners) / (_myCorners + _opCorners + 1.0f);
    }
};
