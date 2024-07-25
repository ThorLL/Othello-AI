#pragma once
#include <unordered_map>

struct GameState;

static std::unordered_map<std::string, std::vector<std::pair<int, int>>> globalLegalMoves;

struct GameState {

	int get(const int ij) const {
		const long long mask = 1LL << ij;
		return (bitBoard & mask) == 0 ? 0 : (chipBoard & mask) == 0 ? 1 : 2;
	}
	void set(const int ij, const int value) {
		const long long mask = 1LL << ij;
		bitBoard |= mask;

		if (value == 1)
			chipBoard &= ~mask;
		else
			chipBoard |= mask;
	}
	int get(const int i, const int j) const {return get(i * 8 + j);}
	void set(const int i, const int j, const int value) {set(i * 8 + j, value);}

	GameState() = default;
	GameState(const GameState &gameState) : currentPlayer(gameState.currentPlayer) {
		chipBoard = gameState.chipBoard;
		bitBoard = gameState.bitBoard;
	}
private:
	unsigned long long chipBoard = 34628173824;
	unsigned long long bitBoard = 103481868288;
public:
	int currentPlayer = 1;
	bool mainBoard = false;
	void print() const {
		auto start = std::chrono::high_resolution_clock::now();
		std::string top = "╔";
		std::string rowDivider = "╠";
		std::string bottom = "╚";
		for (int i = 0; i < 8; ++i) {
			top += "═══";
			rowDivider += "═══";
			bottom += "═══";
			if (i + 1 < 8) {
				top += "╦";
				rowDivider += "╬";
				bottom += "╩";
			}
		}
		top += "╗";
		rowDivider += "╣";
		bottom += "╝";

		std::cout << top << std::endl;
		for (int i = 0; i < 8; ++i) {
			std::string row = "║";
			for (int j = 0; j < 8; ++j) {
				const int cell = get(i, j);
				if (cell == 0) row += "   ";
				else if (cell == 1) row += " ░ ";
				else if (cell == 2) row += " ▓ ";
				else row += " F ";
				row += "║";
			}
			std::cout << row << std::endl;
			row.clear();
			std::cout << (i + 1 < 8 ? rowDivider : bottom) << std::endl;;
		}
	}

	int captureInDirection(const std::pair<int, int> &p, const int &deltaX, const int &deltaY, const int &player) const{
		auto start = std::chrono::high_resolution_clock::now();
		const auto opponent = player == 1 ? 2 : 1;

		int captured = 0;
		int cc = p.first;
		int rr = p.second;
		while ( 0 <= cc+deltaX && cc+deltaX < 8 && 0 <= rr+deltaY && rr+deltaY < 8
				&& get(cc+deltaX, rr+deltaY) == opponent ){
			cc = cc + deltaX;
			rr = rr + deltaY;
			captured++;
				}
		if ( 0 <= cc+deltaX && cc+deltaX < 8  && 0 <= rr+deltaY && rr+deltaY < 8
				&& get(cc+deltaX, rr+deltaY) == player && captured > 0 ) {
			return captured;
		}
		return 0;
	}

	std::vector<std::pair<int, int>> legalMoves(int player) const {
		std::vector<std::pair<int, int>> legalPlaces{};
		if (const auto it = globalLegalMoves.find(hash(player)); it != globalLegalMoves.end()) {
			legalPlaces = it->second;
		} else {
			for (int i = 0; i < 8; i++)
				for (int j = 0; j < 8; j++)
					if ( get(i, j) == 0 )
						for (int deltaX = -1; deltaX <= 1; deltaX++)
							for (int deltaY = -1; deltaY <= 1; deltaY++)
								if ( captureInDirection({i, j}, deltaX, deltaY, player) > 0 )
									legalPlaces.emplace_back(i, j);

			globalLegalMoves[hash(player)] = legalPlaces;
		}

		return legalPlaces;
	}

	bool finished() const {
		return legalMoves(1).empty() && legalMoves(2).empty();
	}

	bool insertToken(const std::pair<int, int> &place) {
		if ( place.first < 0 || place.second < 0 || place.first >= 8 || place.second >= 8 ) //not a position on the board
			return false;
		if ( get(place.first, place.second) != 0 ) // The position is not empty
			return false;

		bool capturesFound = false;
		// Capturing all possible opponents of the current player
		for (int deltaX = -1; deltaX <= 1; deltaX++){
			for (int deltaY = -1; deltaY <= 1; deltaY++){
				if (const int captives = captureInDirection(place, deltaX, deltaY, currentPlayer); captives > 0){
					capturesFound = true;
					for ( int i = 1; i <= captives; i++)
						set(place.first+deltaX*i, place.second+deltaY*i, currentPlayer);
				}
			}
		}

		if ( capturesFound ){
			// Place the token at the given place
			set(place.first, place.second, currentPlayer);
			int oponent = currentPlayer == 1 ? 2 : 1;
			if (!legalMoves(oponent).empty())
				currentPlayer = oponent;
		}
		return capturesFound;
	}

	std::pair<int, int> countTokens() const {
		int tokens1 = 0;
		int tokens2 = 0;
		for (int i = 0; i < 64; i++){
			auto token = get(i);
			if ( token == 1 )
				tokens1++;
			else if ( token == 2 )
				tokens2++;
		}
		return {tokens1, tokens2};
	}

	std::string hash(int player) const {
		return std::to_string(bitBoard) + "|" + std::to_string(chipBoard) + (player == 1 ? " black" : " white");
	}
};
