#pragma once

#include "Piece.h"


class Game
{
private:
	sf::RenderWindow* window;

	sf::Event ev;
	sf::VideoMode windowSize;
	bool gameEnd;
	//Mouse positions

	sf::Vector2i mousePosWindow;
	sf::Vector2f mousePosView;

	//Board
	std::vector<std::vector<char>> board;
	unsigned boardWidth, boardHeight;
	sf::Color primaryColor;
	sf::Color secondaryColor;
	sf::Color highlightColor;
	sf::RectangleShape square;

	//Pieces
	Piece* whiteKing;
	Piece* blackKing;
	std::vector<Piece*> pieces;
	Piece* pieceToMove;
	bool enPassant;


	bool blacksTurn;

	//Private Functions
	void initVariables();
	void initWindow();
	void initPieces();
	void initBoard();

	void drawBoard();


public:
	//Constructor / Destructor
	Game();
	~Game();

	//Functions

	//part of update function
	void pollEvents();

	//Part of main function for updating and rendernig

	//Checks if piece is clicked
	void isPieceClicked();
	void movePiece();


	bool isValidEvent();
	
	void updateMousePositions();
	void updateBoard(sf::Vector2u prevPos);
	void highLightMoves();
	void clearEnPassant();
	void update();

	//draws all the pieces
	void drawPieces(sf::RenderTarget& target);

	void render();


	//MAIN RUN FUNCTION
	void run();
};

