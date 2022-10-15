#include "Piece.h"

//VARIABLE initialization

void Piece::initVariables(sf::Vector2f scale, unsigned pieceId, bool isBlack)
{
	this->squareSize = scale;

	//If it is a pawn enPassant is true
	if (pieceId == 5) this->doubleMove = true;
	else this->doubleMove = false;
	if (pieceId == 0) {
		unsigned yCoord = isBlack ? 0 : 7;
		this->castle["left"] = sf::Vector2u(2, yCoord);
		this->castle["right"] = sf::Vector2u(6, yCoord);
	}
		
	this->enPassant.x = -1;
	this->enPassant.y = -1;
	this->pieceId = pieceId;

	this->isBlack = isBlack;
	this->hasMoved = false;
	

}



//Gets the texture of the piece and sets scale
void Piece::initTexture()
{
	if (!this->Alltextures.loadFromFile("Textures/chess-pieces.png"))
		std::cout << "Loading all piece textures failed" << "\n";
	else
	{
		this->sprite.setTexture(this->Alltextures);

		//Find the correct texture
		this->textureSize.x = this->Alltextures.getSize().x / 6;
		this->textureSize.y = this->Alltextures.getSize().y / 2;

		sf::Vector2f pieceScale;
		pieceScale.x = this->squareSize.x / textureSize.x;
		pieceScale.y = this->squareSize.y / textureSize.y;

		//using texturePos finds the correct texture
		this->sprite.setTextureRect(sf::IntRect(textureSize.x * texturePos.x, textureSize.y * texturePos.y, textureSize.x, textureSize.y));

		this->sprite.setScale(pieceScale);
		//resize the sprite
	}
}



void Piece::initPos(sf::Vector2u position)
{

	this->sprite.setPosition(position.x * this->squareSize.x, position.y * this->squareSize.y);
	this->position = position;
}

sf::Vector2u Piece::getPosition()
{
	return this->position;
}

bool Piece::getIsBlack()
{
	return this->isBlack;
}

int Piece::getPieceId()
{
	return pieceId;
}

void Piece::setEnPassant(sf::Vector2i enPassant)
{
	this->enPassant = enPassant;
}

void Piece::removeCastle(std::string side)
{
	this->castle.erase(side);
}

void Piece::setHasMoved(bool hasMoved)
{
	this->hasMoved = hasMoved;
}

bool Piece::getHasMoved()
{
	return this->hasMoved;
}

char Piece::getPieceSymbol()
{
		char charPiece;
		//where X - Piece type(0 - King, 1 - Queen, 2 - Bishop, 3 - Knight, 4 - Rook, 5 - Pawn)
		switch (this->pieceId)
		{
		case 0:
			charPiece = 'k';
			break;
		case 1:
			charPiece = 'q';
			break;
		case 2:
			charPiece = 'b';
			break;
		case 3:
			charPiece = 'n';
			break;
		case 4:
			charPiece = 'r';
			break;
		case 5:
			charPiece = 'p';
			break;
		}
		//If is black using ASCII table capitlalize (-32 is the Uppercase version of the letter)
		if (this->isBlack) charPiece -= 32;
		return charPiece;
}



//Constructor //textureY 0 - white 1 - Black
Piece::Piece(unsigned textureX, unsigned textureY, sf::Vector2f scale, sf::Vector2u position)
{
	//gets the position of the required texture
	this->texturePos.x = textureX;
	this->texturePos.y = textureY;

	//init variables
	this->initVariables(scale, textureX, static_cast<bool>(textureY));

	//initializes everything
	this->initTexture();

	//put all piece textures on piece
	this->initPos(position);

}

//Destructor
Piece::~Piece()
{

}

//UPDATES ALL POSSIBLE MOVES AND SETS IT IN THE VARIABLE

bool Piece::charCompare(char val, char comp)
{
	if (val == comp || val == comp - 32 || val == comp + 32) return true;
	return false;
}

std::vector<sf::Vector2u> Piece::possibleMoves(std::vector<std::vector<char>>& board)
{
	std::vector<sf::Vector2u> moves = {};
	switch (pieceId)
	{
	//King
	case 0:
	{
		kingMoves(moves, board);

		//castling
		std::cout << "Starting castle loop" << "\n";
		for (auto &move : castle)
		{
			std::cout << "Move: " << "\n";
			int multiply = 1; //to negate the value or not
			int canCastle= 0;
			for (int i = 1; i <= 2; i++)
			{
				std::cout << "Loop " << i << " of loop" << "\n";
				if (move.second.x < position.x)  multiply = -1;
				if (board[position.y][position.x+(i*multiply)] != 'e') break;
				std::cout << "position is empty to castle" << "\n";
				if (isFKingAttacked(sf::Vector2u(position.x + (i*multiply), position.y), board)) break;
				std::cout << "King won't be attacked" << "\n";
				canCastle++;
			}
			if (canCastle == 2)
				moves.push_back(move.second);
		}

		break;
	}

	//Queen
	case 1:
		diagonalMoves(moves, board);
		straightMoves(moves, board);
		break;

	//Bishop
	case 2:
		diagonalMoves(moves, board);
		break;

	//Knight
	case 3:
	{
		knightMoves(moves, board);
		break;
	}

	//Rook
	case 4:
		straightMoves(moves, board);

		break;

	//Pawn
	case 5:
		// sets to current position, later will add x and y values to simulate moves;

		int moveAmount = 1;
		if (doubleMove == true)
			moveAmount++;
		sf::Vector2u move = this->position;
		char piece;
		sf::Vector2u size(board.size(), board[0].size());
		//Move forward moves + doubleMove
		for (int i = 1; i <= moveAmount; i++)
		{
			if (isBlack) move.y++;
			else move.y--;
			if (!isInBoard(move, size)) break;

			piece = board[move.y][move.x];
			if (piece != 'e') break;
			
			moves.push_back(move);
		}
		move = this->position;
		//Killing moves (diagonal)
		if (isBlack) move.y++;
		else move.y--;
		if (isInBoard(move, size))
		{
			for (int i = -1; i <= 1; i = i + 2)
			{
				move.x += i;
				if (isInBoard(move, size))
				{
					piece = board[move.y][move.x];
					if (isEnemy(piece)) 
						moves.push_back(move);
				}
				move.x = position.x;
			}
		}
		if (this->enPassant.x != -1) 
		{
			//Adding enPassant move
			moves.push_back(sf::Vector2u(enPassant.x, enPassant.y)); //CHECK IF ENPASSANT ISNT EMPTY
		}
		break;
	}
	for (auto move = moves.begin(); move != moves.end();)
	{
;		if (isFKingAttacked(*move, board))
		{
			move = moves.erase(move);
		}
			
		else
			move++;
	}
	return moves;
}

std::vector<sf::Vector2u> Piece::legalMoves(std::vector<std::vector<char>>& board)
{
	return std::vector<sf::Vector2u>{};
}

bool Piece::isLegalMove(sf::Vector2u move, std::vector<std::vector<char>>& board)
{
	
	if (!isInBoard(move, sf::Vector2u(board.size(), board[0].size()))) return false;
	
	if (board[move.y][move.x] != 'e' && !isEnemy(board[move.y][move.x])) return false;

	return true;
}

//if friendly king is being attacked (can use to check if the move is illegal or if the king is in check and need to protect)
//beacuse it checks if the friendly king is being attacked, not if you are attacking the enemy king
bool Piece::isFKingAttacked(sf::Vector2u move, std::vector<std::vector<char>>& board)
{
	std::vector < std::vector<char>> bCopy = board;//copies the board so can make the move and see if king would be in check
	
	bCopy[move.y][move.x] = getPieceSymbol(); 
	bCopy[position.y][position.x] = 'e';
	sf::Vector2i kingPos;
	for (int i = 0; i < bCopy.size(); i++)
	{
		for (int j = 0; j < bCopy.size(); j++)
		{
			if (charCompare(bCopy[i][j], 'k') && !isEnemy(bCopy[i][j]))
			{
				kingPos.x = j, kingPos.y = i;
			}
				
		}
	}
	
	std::vector<sf::Vector2u> moves;
	//Checks if a knight is attacking the king
	knightMoves(moves, bCopy, kingPos);
	char piece;
	for (auto &move : moves)
	{
		piece = bCopy[move.y][move.x];
		if (charCompare(piece, 'n') && isEnemy(piece))
		{
			return true;
		}
			
	}
	//if Bishop or queen or (pawn if one square next) is attacking on diagonals
	moves.clear();
	diagonalMoves(moves, bCopy, kingPos);

	for (auto &move : moves)
	{ 
		piece = bCopy[move.y][move.x];
		//if enemy Pawn is attacking the king
		if (((isBlack && move.y-kingPos.y == 1) || (!isBlack && move.y - kingPos.y == -1)) && charCompare(piece, 'p') && isEnemy(piece))
		{
			return true;
				
		}
		//if bishop or queen is attacking
		else if ((charCompare(piece, 'q') || charCompare(piece, 'b')) && isEnemy(piece))
		{
			return true;
		}
			
	}

	//if rook or queen is attacking on the straight lines
	moves.clear();
	straightMoves(moves, bCopy, kingPos);

	for (auto& move : moves)
	{
		piece = bCopy[move.y][move.x];
		if ((charCompare(piece, 'r') || charCompare(piece, 'q')) && isEnemy(piece))
		{
			return true;
		}
			
	}
	//if King is attacking
	moves.clear();
	kingMoves(moves, bCopy, kingPos);
	for (auto& move : moves)
	{
		piece = bCopy[move.y][move.x];
		if (charCompare(piece, 'k') && isEnemy(piece))
		{
			return true;
		}
			
	}
	return false;
}



std::vector<sf::Vector2u> Piece::straightMoves(std::vector<sf::Vector2u>& moves, std::vector<std::vector<char>>& board, sf::Vector2i pos)
{
	if (pos.x == -1) pos = sf::Vector2i(this->position.x, this->position.y);
	std::vector<sf::Vector2i> directions(4, sf::Vector2i(0, 0));

	//Initialize the direction map

	directions[0].y = -1; //Go up
	directions[1].y = 1;  //go Down
	directions[2].x = -1; // go left
	directions[3].x = 1; //go right

	while (directions.size())
	{
		auto it = directions.begin();
		while (it != directions.end())
		{
			if (isLegalMove(sf::Vector2u(pos.x + (*it).x, pos.y + (*it).y), board)) //is in board, and doesnt move on own piece
			{
				if (!isEnemy(board[pos.y + (*it).y][pos.x + (*it).x]))
				{
					moves.push_back(sf::Vector2u(pos.x + (*it).x, pos.y + (*it).y));
					(*it).x += (*it).x > 0 ? 1 : (*it).x < 0 ? -1 : 0; //increment x
					(*it).y += (*it).y > 0 ? 1 : (*it).y < 0 ? -1 : 0; //increment y
				}
				else
				{
					moves.push_back(sf::Vector2u(pos.x + (*it).x, pos.y + (*it).y));
					it = directions.erase(it);
				}
			}
			else
				it = directions.erase(it);
		
		}
	}
	return moves;
}

std::vector<sf::Vector2u> Piece::diagonalMoves(std::vector<sf::Vector2u>& moves, std::vector<std::vector<char>>& board, sf::Vector2i pos)
{
	if (pos.x == -1) pos = sf::Vector2i(this->position.x, this->position.y);
	std::vector<sf::Vector2i> directions(4, sf::Vector2i());

	//Initialize the direction map

	directions[0].y = 1, directions[0].x = 1; //Go down right
	directions[1].y = 1, directions[1].x = -1;  //go Down left
	directions[2].y = -1, directions[2].x = 1; // go up right
	directions[3].y = -1, directions[3].x = -1; //go up left

	while (directions.size())
	{
		auto it = directions.begin();
		while (it != directions.end())
		{
			if (isLegalMove(sf::Vector2u(pos.x + (*it).x, pos.y + (*it).y), board)) //is in board, and doesnt move on own piece
			{
				if (!isEnemy(board[pos.y + (*it).y][pos.x + (*it).x]))
				{
					moves.push_back(sf::Vector2u(pos.x + (*it).x, pos.y + (*it).y));
					(*it).x += (*it).x > 0 ? 1 : -1; //increment x
					(*it).y += (*it).y > 0 ? 1 : -1; //increment y
				}
				else
				{
					moves.push_back(sf::Vector2u(pos.x + (*it).x, pos.y + (*it).y));
					it = directions.erase(it);
				}
			}
			else
				it = directions.erase(it);

		}
	}
	return moves;
}

std::vector<sf::Vector2u> Piece::knightMoves(std::vector<sf::Vector2u>& moves, std::vector<std::vector<char>>& board, sf::Vector2i pos)
{
	if (pos.x == -1) pos = sf::Vector2i(this->position.x, this->position.y);
	std::vector<sf::Vector2i> directions(4, sf::Vector2i());

	//Initialize the direction map

	directions[0].x = 1, directions[0].y = -2; //Go up
	directions[1].x = 1, directions[1].y = 2;  //go Down
	directions[2].x = 2, directions[2].y = 1; // go left
	directions[3].x = 2, directions[3].y = -1; //go right

	for (int i = 0; i != 2; i++) //loop two times one time one side, other time mirror the moves
	{
		for (auto &dir : directions)
		{
			if (isLegalMove(sf::Vector2u(pos.x + dir.x, pos.y + dir.y), board)) //is in board, and doesnt move on own piece
				moves.push_back(sf::Vector2u(pos.x + dir.x, pos.y + dir.y));
			dir.x *= -1, dir.y *= -1; //mirrors the moves to the other side
		}
	}
	return moves;
}

std::vector<sf::Vector2u> Piece::kingMoves(std::vector<sf::Vector2u>& moves, std::vector<std::vector<char>>& board, sf::Vector2i pos)
{
	if (pos.x == -1) pos = sf::Vector2i(this->position.x, this->position.y);
	std::vector<sf::Vector2i> directions(4, sf::Vector2i());

	//Initialize the direction map

	directions[0].x = 0, directions[0].y = -1; //Go up
	directions[1].x = 1, directions[1].y = -1;  //go Down
	directions[2].x = 1, directions[2].y = 0; // go left
	directions[3].x = 1, directions[3].y = 1; //go right

	for (int i = 0; i != 2; i++) //loop two times one time one side, other time mirror the moves
	{
		for (auto &dir : directions)
		{
			if (isLegalMove(sf::Vector2u(pos.x + dir.x, pos.y + dir.y), board)) //is in board, and doesnt move on own piece
				moves.push_back(sf::Vector2u(pos.x + dir.x, pos.y + dir.y));
			dir.x *= -1, dir.y *= -1; //mirrors the moves to the other side
		}
	}

	

	return moves;
}

//Functions


bool Piece::isPieceClicked(sf::Vector2f mousePosView)
{
	//NEED TO CHECK IF IT IS THE PIECES TURN< THEN SELECT IT OTHERWISE CAN KILL IT

	if (this->sprite.getGlobalBounds().contains(mousePosView))
	{
		return true;
	}
	return false;
}

bool Piece::isInBoard(sf::Vector2u pos, sf::Vector2u boardSize)
{
	return (pos.x >= 0 && pos.x < boardSize.x && pos.y >= 0 && pos.y < boardSize.y);
}

bool Piece::isInBoard(unsigned x, unsigned y, sf::Vector2u boardSize)
{
	return (x >= 0 && x < boardSize.x && y >= 0 && y < boardSize.y);
}

bool Piece::isEnemy(char piece)
{
	//97 - 122 are non capitalized characters (white)
	if (isBlack && piece != 'e' && piece >= 97 && piece <= 122) return true;
	//65 - 90 are capitalized characters (black)
	else if (!isBlack && piece != 'e' && piece >= 65 && piece <= 90) return true;

	return false;
}

void Piece::promotePawn()
{
	//Make it a queen
	this->pieceId = 1;
	this->texturePos.x = 1;
	texturePos.y = static_cast<unsigned>(isBlack);

	this->sprite.setTextureRect(sf::IntRect(textureSize.x * texturePos.x, textureSize.y * texturePos.y, textureSize.x, textureSize.y));
}

bool Piece::movePiece(unsigned x, unsigned y, std::vector<std::vector<char>> &board)
{	
	std::vector<sf::Vector2u> possibleMoves = this->possibleMoves(board);
	for (auto move : possibleMoves)
	{
		if (move.x == x && move.y == y) {
			if (pieceId == 5)
			{
				if (doubleMove == true) doubleMove = false; //if moved cant go 2 squares
				if ((!isBlack && move.y == 0) || (isBlack && move.y == board.size() - 1)) //if reaches the emd
					this->promotePawn();
				if(enPassant.x != -1)
					enPassant.x = -1, enPassant.y = -1;
				
			}
			//updates piece and sprite position
			this->updatePiecePos(move);
			
			
			

			return true;
		}
	}
	return false;
	
}

void Piece::updatePiecePos(sf::Vector2u pos)
{
	this->position = pos;
	this->sprite.setPosition(pos.x*this->squareSize.x, pos.y*squareSize.y);
}

void Piece::update()
{
	
}

void Piece::render(sf::RenderTarget& target)
{
	target.draw(this->sprite);
}


