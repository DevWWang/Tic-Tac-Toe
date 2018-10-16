#include "pch.h"
#include <sstream>
#include <iostream>
#include "DEFINITIONS.hpp"
#include "GameState.hpp"
#include "PauseState.hpp"
#include "GameOverState.hpp"

namespace GameSpace
{
	GameState::GameState(GameDataRef data) :_data(data)
	{

	}

	void GameState::Init()
	{
		gameState = STATE_PLAYING;
		turn = PLAYER_PIECE;

		this->_data->assets.LoadTexture("Pause Button", PAUSE_BUTTON_FILEPATH);
		this->_data->assets.LoadTexture("Grid Sprite", GRID_SPRITE_FILEPATH);
		this->_data->assets.LoadTexture("X Piece", X_PIECE_FILEPATH);
		this->_data->assets.LoadTexture("O Piece", O_PIECE_FILEPATH);
		this->_data->assets.LoadTexture("X Winning Piece", X_WINNING_PIECE_FILEPATH);
		this->_data->assets.LoadTexture("O Winning Piece", O_WINNING_PIECE_FILEPATH);

		_background.setTexture(this->_data->assets.GetTexture("Background"));
		_pauseButton.setTexture(this->_data->assets.GetTexture("Pause Button"));
		_gridSprite.setTexture(this->_data->assets.GetTexture("Grid Sprite"));

		_pauseButton.setPosition(
			this->_data->window.getSize().x - this->_pauseButton.getGlobalBounds().width,
			this->_pauseButton.getPosition().y
		);
		_gridSprite.setPosition(
			(SCREEN_WIDTH / 2) - (this->_gridSprite.getGlobalBounds().width / 2),
			(SCREEN_HEIGHT / 2) - (this->_gridSprite.getGlobalBounds().height / 2)
		);

		InitGridPieces();

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				_gridArray[i][j] = EMPTY_PIECE;
			}
		}
	}

	void GameState::HandleInput()
	{
		sf::Event event;

		while (this->_data->window.pollEvent(event))
		{
			if (sf::Event::Closed == event.type)
			{
				this->_data->window.close();
			}

			if (this->_data->input.IsSpriteClicked(this->_pauseButton, sf::Mouse::Left, this->_data->window))
			{
				//Switch to Game State
				std::cout << "Pause the Game" << std::endl;
				this->_data->machine.AddState(StateRef(new PauseState(_data)), false);
			}
			else if (this->_data->input.IsSpriteClicked(this->_gridSprite, sf::Mouse::Left, this->_data->window))
			{
				if (gameState == STATE_PLAYING)
				{
					CheckAndPlacePieces();
				}
			}
		}
	}

	void GameState::Update(float dt)
	{
		if (gameState == STATE_DRAW || gameState == STATE_LOSE || gameState == STATE_WON)
		{
			if (this->_clock.getElapsedTime().asSeconds() > TIME_BEFORE_SHOWING_GAMEOVER)
			{
				this->_data->machine.AddState(StateRef(new GameOverState(_data)), true);
			}
		}
	}

	void GameState::Draw(float dt)
	{
		this->_data->window.clear();

		this->_data->window.draw(this->_background);
		this->_data->window.draw(this->_pauseButton);
		this->_data->window.draw(_gridSprite);

		for (int i = 0; i < 3; i ++)
		{
			for (int j = 0; j < 3; j++)
			{
				this->_data->window.draw(this->_gridPieces[i][j]);
			}
		}

		this->_data->window.display();
	}

	void GameState::InitGridPieces()
	{
		sf::Vector2u tempSpriteSize = this->_data->assets.GetTexture("X Piece").getSize();

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				_gridPieces[i][j].setTexture(this->_data->assets.GetTexture("X Piece"));
				_gridPieces[i][j].setPosition(
					_gridSprite.getPosition().x + (tempSpriteSize.x * i) - 7,
					_gridSprite.getPosition().y + (tempSpriteSize.y * j) - 7
				);
				//Make the X pieces invisible
				_gridPieces[i][j].setColor(sf::Color(255, 255, 255, 0));
			}
		}
	}

	void GameState::CheckAndPlacePieces()
	{
		sf::Vector2i touchPoint = this->_data->input.GetMousePosition(this->_data->window);
		sf::FloatRect gridSize = _gridSprite.getGlobalBounds();
		sf::Vector2f gapOutsideOfGird = sf::Vector2f((SCREEN_WIDTH - gridSize.width) / 2, (SCREEN_HEIGHT - gridSize.height) / 2);
		
		sf::Vector2f gridLocalTouchPosition = sf::Vector2f(touchPoint.x - gapOutsideOfGird.x, touchPoint.y - gapOutsideOfGird.y);
		std::cout << gridLocalTouchPosition.x << ", " << gridLocalTouchPosition.y << std::endl;
		
		sf::Vector2f gridSectionSize = sf::Vector2f(gridSize.width / 3, gridSize.height / 3);

		int column, row;
		if (gridLocalTouchPosition.x < gridSectionSize.x)
		{
			column = 1;
		}
		else if (gridLocalTouchPosition.x < gridSectionSize.x * 2)
		{
			column = 2;
		}
		else if (gridLocalTouchPosition.x < gridSize.width)
		{
			column = 3;
		}

		if (gridLocalTouchPosition.y < gridSectionSize.y)
		{
			row = 1;
		}
		else if (gridLocalTouchPosition.y < gridSectionSize.y * 2)
		{
			row = 2;
		}
		else if (gridLocalTouchPosition.y < gridSize.height)
		{
			row = 3;
		}

		if (_gridArray[column - 1][row - 1] == EMPTY_PIECE)
		{
			_gridArray[column - 1][row - 1] = turn;
			
			if (turn == PLAYER_PIECE)
			{
				_gridPieces[column - 1][row - 1].setTexture(this->_data->assets.GetTexture("X Piece"));
				CheckHasPlayerWon(turn);
				turn = AI_PIECE;
			}
			else if (turn == AI_PIECE)
			{
				_gridPieces[column - 1][row - 1].setTexture(this->_data->assets.GetTexture("O Piece"));
				CheckHasPlayerWon(turn);
				turn = PLAYER_PIECE;
			}

			_gridPieces[column - 1][row - 1].setColor(sf::Color(255, 255, 255, 255));
		}
	}

	void GameState::CheckHasPlayerWon(int player)
	{
		Check3PiecesForMatch(0, 0, 1, 0, 2, 0, player);
		Check3PiecesForMatch(0, 1, 1, 1, 2, 1, player);
		Check3PiecesForMatch(0, 2, 1, 2, 2, 2, player);
		Check3PiecesForMatch(0, 0, 0, 1, 0, 2, player);
		Check3PiecesForMatch(1, 0, 1, 1, 1, 2, player);
		Check3PiecesForMatch(2, 0, 2, 1, 2, 2, player);
		Check3PiecesForMatch(0, 0, 1, 1, 2, 2, player);
		Check3PiecesForMatch(0, 2, 1, 1, 2, 0, player);

		int emptyNum = 9;

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (_gridArray[i][j] != EMPTY_PIECE)
				{
					emptyNum--;
				}
			}
		}

		// check if the game is draw
		if (emptyNum == 0 && gameState != STATE_WON && gameState != STATE_LOSE)
		{
			gameState == STATE_DRAW;
		}

		// check if the game is over
		if (gameState == STATE_DRAW || gameState == STATE_LOSE || gameState == STATE_WON)
		{
			this->_clock.restart();
		}

		std::cout << gameState << std::endl;
	}

	void GameState::Check3PiecesForMatch(int x1, int y1, int x2, int y2, int x3, int y3, int pieceToCheck)
	{
		if (pieceToCheck == _gridArray[x1][y1] && pieceToCheck == _gridArray[x2][y2] && pieceToCheck == _gridArray[x3][y3])
		{
			std::string winningPieceStr;

			if (pieceToCheck == O_PIECE)
			{
				winningPieceStr = "O Winning Piece";
			}
			else
			{
				winningPieceStr = "X Winning Piece";
			}

			_gridPieces[x1][y1].setTexture(this->_data->assets.GetTexture(winningPieceStr));
			_gridPieces[x2][y2].setTexture(this->_data->assets.GetTexture(winningPieceStr));
			_gridPieces[x3][y3].setTexture(this->_data->assets.GetTexture(winningPieceStr));

			if (pieceToCheck == PLAYER_PIECE)
			{
				gameState = STATE_WON;
			}
			else if (pieceToCheck == AI_PIECE)
			{
				gameState = STATE_LOSE;
			}
		}
	}
}