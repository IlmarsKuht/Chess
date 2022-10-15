#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>


class Piece
{
private:
	sf::Vector2f squareSize;
	//piece object
	sf::Sprite sprite;
	//textures
	sf::Texture Alltextures;
	sf::Vector2u texturePos;
	sf::Vector2i textureSize;

	//piece variables
	sf::Vector2u position;

	
	int pieceId;
	bool isBlack;
	bool doubleMove;
	sf::Vector2i enPassant;
	std::unordered_map<std::string, sf::Vector2u> castle;
	bool hasMoved;


	//Private functions

	void initVariables(sf::Vector2f scale, unsigned pieceId, bool isBlack);
	void initTexture();
	void initPos(sf::Vector2u position);	

public:
	//Getters and setters
	sf::Vector2u getPosition();
	bool getIsBlack();
	int getPieceId();
	void setEnPassant(sf::Vector2i enPassant);
	void removeCastle(std::string side);
	void setHasMoved(bool hasMoved);
	bool getHasMoved();


	char getPieceSymbol();


	//Constructor / Destructor
	//TextureX - pieceId textureY - isBlack scale - piece size   position - position on 0 initialized board
	Piece(unsigned textureX, unsigned textureY, sf::Vector2f scale, sf::Vector2u position);
	virtual ~Piece();

	//Functions

	bool charCompare(char val, char comp);

	//all Possible piece moves depending on the pieceId
	std::vector<sf::Vector2u> possibleMoves(std::vector<std::vector<char>>& board);
	//all  legal moves according to chess rules
	std::vector<sf::Vector2u> legalMoves(std::vector<std::vector<char>>& board);
	bool isLegalMove(sf::Vector2u move, std::vector<std::vector<char>>& board);
	//returns true if the friendly king is being attacked after the move
	bool isFKingAttacked(sf::Vector2u move, std::vector<std::vector<char>>& board);
	//functions to help calculate moves
	std::vector<sf::Vector2u> straightMoves(std::vector<sf::Vector2u>& moves, std::vector<std::vector<char>>& board, sf::Vector2i pos = sf::Vector2i(-1, -1));
	std::vector<sf::Vector2u> diagonalMoves(std::vector<sf::Vector2u>& moves, std::vector<std::vector<char>>& board, sf::Vector2i pos = sf::Vector2i(-1, -1));
	std::vector<sf::Vector2u> knightMoves(std::vector<sf::Vector2u>& moves, std::vector<std::vector<char>>& board, sf::Vector2i pos = sf::Vector2i(-1, -1));
	std::vector<sf::Vector2u> kingMoves(std::vector<sf::Vector2u>& moves, std::vector<std::vector<char>>& board, sf::Vector2i pos = sf::Vector2i(-1, -1));


	
	//returns true if piece iscliecked else false
	bool isPieceClicked(sf::Vector2f mousePosView);
	bool isInBoard(sf::Vector2u pos, sf::Vector2u boardSize);
	bool isInBoard(unsigned x, unsigned y, sf::Vector2u boardSize);
	bool isEnemy(char piece);
	void promotePawn();

	//tells if piece can be moved or not
	bool movePiece(unsigned x, unsigned y, std::vector<std::vector<char>> &board);

	//updates piece and sprite position
	void updatePiecePos(sf::Vector2u pos);


	void update();
	void render(sf::RenderTarget& target);
	
};

