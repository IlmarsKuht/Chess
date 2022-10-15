// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "Game.h"

//Initialize all the variables
void Game::initVariables()
{
	//window inits
	this->windowSize = sf::VideoMode(800, 800);

	//Board inits
	this->boardWidth = 8;
	this->boardHeight = 8;
	this->primaryColor = sf::Color(228, 249, 180);
	this->secondaryColor = sf::Color(92, 183, 95);
	this->highlightColor = sf::Color(88, 245, 255, 150);
	this->square.setSize(sf::Vector2f(this->windowSize.width / float(this->boardWidth), this->windowSize.height / float(this->boardHeight)));
	//Piece inits
	this->pieces = {};
	this->blacksTurn = false;
	this->enPassant = false;
	this->gameEnd = false;

}

//Private functions//
void Game::initWindow()
{

	this->window = new sf::RenderWindow(this->windowSize, "Improved Chess", sf::Style::Close | sf::Style::Titlebar);
	this->window->setFramerateLimit(60);
	this->window->setVerticalSyncEnabled(false);
}

/*
	new Piece(textureX, textureY)
	where X - Piece type (0 - King, 1 - Queen, 2 - Bishop, 3 - Knight, 4 - Rook, 5 - Pawn)
	Y - Color (0 - white, 1 - Black)

*/
void Game::initPieces()
{
	sf::Vector2f scale;
	scale.x = this->windowSize.width / float(this->boardWidth);
	scale.y = this->windowSize.height / float(this->boardHeight);


	//Puts all white and black paws on their second ranks, pwnPos is the x
	for (int pwnPos = 7; pwnPos >= 0; pwnPos--) 
	{
		this->pieces.push_back(new Piece(5, 1, scale, sf::Vector2u(pwnPos, 1)));
		this->pieces.push_back(new Piece(5, 0, scale, sf::Vector2u(pwnPos, 6)));
	}
	//Rook initialize
	for (int rookPos = 0; rookPos <= 7; rookPos = rookPos + 7) {
		this->pieces.push_back(new Piece(4, 1, scale, sf::Vector2u(rookPos, 0)));
		this->pieces.push_back(new Piece(4, 0, scale, sf::Vector2u(rookPos, 7)));
	}
	//Knight initialze
	for (int knightPos = 1; knightPos <= 6; knightPos = knightPos + 5) {
		this->pieces.push_back(new Piece(3, 1, scale, sf::Vector2u(knightPos, 0)));
		this->pieces.push_back(new Piece(3, 0, scale, sf::Vector2u(knightPos, 7)));
	}
	//Bishop initialize
	for (int bishPos = 2; bishPos <= 5; bishPos = bishPos + 3) {
		this->pieces.push_back(new Piece(2, 1, scale, sf::Vector2u(bishPos, 0)));
		this->pieces.push_back(new Piece(2, 0, scale, sf::Vector2u(bishPos, 7)));
	}
	//Queen
	this->pieces.push_back(new Piece(1, 1, scale, sf::Vector2u(3, 0)));
	this->pieces.push_back(new Piece(1, 0, scale, sf::Vector2u(3, 7)));
	//King
	blackKing = new Piece(0, 1, scale, sf::Vector2u(4, 0));
	whiteKing = new Piece(0, 0, scale, sf::Vector2u(4, 7));
	this->pieces.push_back(blackKing);
	this->pieces.push_back(whiteKing);
	
	

}


void Game::initBoard()
{
	//Puts all values as "e" indicating empty and initializes the grid to its size
	for (int i = 0; i < this->boardHeight; i++) {
		board.emplace_back(std::vector<char>(boardWidth, 'e'));
	}
	//loads all the piece positions
	for (auto piece : this->pieces)
	{
		int positionX = piece->getPosition().x;
		int positionY = piece->getPosition().y;
		this->board[positionY][positionX] = piece->getPieceSymbol();
		
	}
}


//Initialize the 8x8 playing board and draw it
void Game::drawBoard()
{
	//Calculate how many units each tile is wide and tall
	float tileHeight = square.getSize().y;
	float tileWidth = square.getSize().x;
	//to start each row with first color or second color
	bool isSecondaryColor;
	
	//Renders the squares one by one, change their color accordingly
	for (unsigned i = 0; i < this->boardHeight; i++) //for rows
	{
		//Row 0 false, row 1 true, row 2 false
	

		for (unsigned j = 0; j < this->boardWidth; j++) //for columns
		{
			isSecondaryColor = static_cast<bool>((i+j) % 2);
			//sets the color of the square
			if (isSecondaryColor) square.setFillColor(secondaryColor);
			else square.setFillColor(primaryColor);

			//sets the position of the square
			square.setPosition(tileWidth*i, tileHeight*j);
			this->window->draw(square);

		}
	}
	this->highLightMoves();
}


//GAME Constructor
Game::Game()
{
	this->initVariables();
	this->initWindow();
	this->initPieces();
	this->initBoard();
}

//GAME Destructor
Game::~Game()
{
	delete this->window;

	for (auto piece : pieces)
	{
		delete piece;
	}
}


void Game::pollEvents()
{
	
	switch (this->ev.type)
	{
		// Pressing X button and Pressing Escape  closes the window (2 cases)
	case sf::Event::Closed:
		this->window->close();
		break;

	case sf::Event::KeyPressed:
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
		{
			this->window->close();
			break;
		}


	case sf::Event::MouseButtonPressed:
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) //Check if piece is clicked and update piece to mvoe
		{
			this->isPieceClicked();
		}
		break;

	case sf::Event::MouseButtonReleased:

		this->movePiece();

		break;

	}
}

void Game::isPieceClicked()
{
	for (auto &piece : this->pieces)
	{
		if (piece->getIsBlack() == this->blacksTurn && piece->isPieceClicked(mousePosView) )
		{
			this->pieceToMove = piece;
			
		}
	}
}

void Game::movePiece()
{
	if (pieceToMove)
	{
		
		for (unsigned i = 0; i < this->boardWidth; i++)
		{
			for (unsigned j = 0; j < this->boardHeight; j++)
			{
				//loops over each square i = rows j - columns
				this->square.setPosition(this->square.getSize().x * i, this->square.getSize().y * j);
				if (this->square.getGlobalBounds().contains(this->mousePosView)) 
				{
					sf::Vector2u prevPos = this->pieceToMove->getPosition();
					if (this->pieceToMove->movePiece(i, j, board)) {
						//EnPassant
	
						for (auto &piece : pieces)
						{
							if (std::abs(int(j) - int(prevPos.y)) == 2 && pieceToMove->getPieceId() == 5)//enPassant possibility (pawn moved 2 squares)
							{
								//finds if there are pawns to kill the passed pawn, if there is, it gives them a move enpassant
								if (piece->getPosition().y == j &&
									(piece->getPosition().x - 1 == i || piece->getPosition().x + 1 == i))
								{
									int yCoord = pieceToMove->getIsBlack() ? j - 1 : j + 1; //checks to kill up or down with enpassant
									piece->setEnPassant(sf::Vector2i(i, yCoord));
									this->enPassant = true;
								}
							}
							else if (this->enPassant == true) //removes the enpassant one move later
							{
								this->enPassant = false;
								this->clearEnPassant();
							}
							
						}

						//takes away castle
						if (pieceToMove->getPieceId() == 4 && !pieceToMove->getHasMoved())
						{
							if (prevPos.x == 0)
							{
								std::cout << "Removing castle left" << "\n";
								if (pieceToMove->getIsBlack())
									blackKing->removeCastle("left");
								else whiteKing->removeCastle("left");
							}
							else if (prevPos.x == 7)
							{
								std::cout << "Removing castle right" << "\n";
								if (pieceToMove->getIsBlack())
									blackKing->removeCastle("right");
								else whiteKing->removeCastle("right");
							}
						}
						if (pieceToMove->getPieceId() == 0 && !pieceToMove->getHasMoved())
						{
							if (pieceToMove->getIsBlack())
							{
								blackKing->removeCastle("left");
								blackKing->removeCastle("right");
							}
							else {
								whiteKing->removeCastle("left");
								whiteKing->removeCastle("right");
							}
								

						}
						


						pieceToMove->setHasMoved(true);
						


						//Updates board variable and in game board, deletes piece if needed
						this->updateBoard(prevPos);
						blacksTurn = !blacksTurn;
						pieceToMove = nullptr;

						for (int i = 0; i < this->boardHeight; i++) {
							for (int j = 0; j < this->boardWidth; j++) {
								std::cout << board[i][j] << " ";
							}
							std::cout << "\n";
						}

						return;
					}
				}
			}
		}
	}
}




bool Game::isValidEvent()
{
	switch (this->ev.type)
	{
	case sf::Event::Closed:
		return true;

	case sf::Event::KeyPressed:
		return true;

	case sf::Event::MouseButtonPressed:
		return true;

	case sf::Event::MouseButtonReleased:
		return true;

	default:
		return false;
	}
}

void Game::updateMousePositions()
{
	this->mousePosWindow = sf::Mouse::getPosition(*this->window);
	this->mousePosView = this->window->mapPixelToCoords(this->mousePosWindow);
}

void Game::updateBoard(sf::Vector2u prevPos)
{
	board[prevPos.y][prevPos.x] = 'e';
	sf::Vector2u pos = pieceToMove->getPosition();

	bool killedPiece = board[pos.y][pos.x] != 'e';
	bool enPassanted = pieceToMove->getPieceId() == 5 && prevPos.y != pos.y;
	bool castled = pieceToMove->getPieceId() == 0 && std::abs(static_cast<int>(prevPos.x - pos.x)) == 2; //check if casled then move rook too

		for (int i = 0; i < pieces.size(); i++)
		{
			if (killedPiece) //delete piece if the new piece moved onto it
			{
				if (pieces[i]->getPosition() == pos && pieces[i] != pieceToMove)
					pieces.erase(pieces.begin() + i);
			}
			else if (enPassanted)
			{
				int yToRemove = pieceToMove->getIsBlack() ? pos.y - 1 : pos.y + 1;
				if (pieces[i]->getPosition().x == pos.x && pieces[i]->getPosition().y == yToRemove)
				{ //removes killed enpassant piece in board and from vector
					board[yToRemove][pos.x] = 'e';
					pieces.erase(pieces.begin() + i);
				}
				
			}
			else if (castled)
			{
				unsigned xCoord;
				xCoord = pos.x < 4 ?  0 : 7;
				sf::Vector2u rookPos = sf::Vector2u(xCoord, pos.y);
				if (pieces[i]->getPieceId() == 4 && pieces[i]->getPosition() == rookPos)
				{
					board[rookPos.y][rookPos.x] = 'e';
					rookPos.x = pos.x < 4 ? pos.x + 1 : pos.x - 1;
					pieces[i]->updatePiecePos(rookPos);
					board[rookPos.y][rookPos.x] = pieces[i]->getPieceSymbol();
					//update the board
				}
			}
			


		}
	board[pos.y][pos.x] = this->pieceToMove->getPieceSymbol();
}

void Game::highLightMoves()
{
	if (this->pieceToMove == nullptr) return;

	std::vector<sf::Vector2u> highlights = pieceToMove->possibleMoves(this->board);

	for (auto &light : highlights)
	{
		square.setPosition(light.x*square.getSize().x, light.y * square.getSize().y);
		square.setFillColor(highlightColor);
		this->window->draw(square);

	}
}

void Game::clearEnPassant()
{
	for (auto &piece : pieces)
	{
		if (piece->getPieceId() != 5) continue;
		else {
			piece->setEnPassant(sf::Vector2i(-1, -1)); //NEED To CHANGE TO INT NOT U
		}
	}
}



//MAIN UPDATE FUNCTION
void Game::update()
{
	this->updateMousePositions();
	this->pollEvents();
	this->highLightMoves();

}
//Draws pieces

void Game::drawPieces(sf::RenderTarget& target)
{
	for (auto &piece : this->pieces)
	{
		piece->render(target);
	}

}

//MAIN RENDER FUNCTION
void Game::render()
{
	//clear frame
	this->window->clear();

	//draw board needs to be first, then pieces on top
	              //DONT REDRAW IF NOTHING IS HAPPENING
	this->drawBoard();
	this->drawPieces(*this->window);

	//display
	this->window->display();
}

//MAIN RUN FUNCTION//
void Game::run()
{
	//Draw the initial board and pieces
	this->render();
	
	while (this->window->waitEvent(ev))
	{
		
		if (this->isValidEvent() && !gameEnd) //checks if the event is useful and will update anything
		{
			
			this->update();
			this->render();
		}
	} 

}
