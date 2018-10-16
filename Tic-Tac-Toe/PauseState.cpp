#include "pch.h"
#include <sstream>
#include <iostream>
#include "DEFINITIONS.hpp"
#include "PauseState.hpp"
#include "MainMenuState.hpp"
#include "GameState.hpp"

namespace GameSpace
{
	PauseState::PauseState(GameDataRef data) : _data(data)
	{

	}

	void PauseState::Init()
	{
		this->_data->assets.LoadTexture("Pause Background", GENERAL_BACKGROUND_FILEPATH);
		this->_data->assets.LoadTexture("Resume Button", RESUME_BUTTON_FILENAME);
		this->_data->assets.LoadTexture("Home Button", HOME_BUTTON_FILENAME);

		_background.setTexture(this->_data->assets.GetTexture("Pause Background"));
		_resumeButton.setTexture(this->_data->assets.GetTexture("Resume Button"));
		_homeButton.setTexture(this->_data->assets.GetTexture("Home Button"));

		_resumeButton.setPosition(
			(this->_data->window.getSize().x / 2) - (this->_resumeButton.getGlobalBounds().width / 2),
			(this->_data->window.getSize().y / 3) - (this->_resumeButton.getGlobalBounds().height / 2)
		);
		_homeButton.setPosition(
			(this->_data->window.getSize().x / 2) - (this->_homeButton.getGlobalBounds().width / 2),
			(this->_data->window.getSize().y / 3 * 2) - (this->_homeButton.getGlobalBounds().height / 2)
		);
	}

	void PauseState::HandleInput()
	{
		sf::Event event;

		while (this->_data->window.pollEvent(event))
		{
			if (sf::Event::Closed == event.type)
			{
				this->_data->window.close();
			}

			if (this->_data->input.IsSpriteClicked(this->_resumeButton, sf::Mouse::Left, this->_data->window))
			{
				//Resume Game by Popping of the Current State (Pause State)
				this->_data->machine.RemoveState();
			}

			if (this->_data->input.IsSpriteClicked(this->_homeButton, sf::Mouse::Left, this->_data->window))
			{
				//Remove the Pause State off the Stack
				this->_data->machine.RemoveState();
				//Switch to Main Menu State by Replacing the Game State
				this->_data->machine.AddState(StateRef(new MainMenuState(_data)), true);
			}
		}
	}

	void PauseState::Update(float dt)
	{

	}

	void PauseState::Draw(float dt)
	{
		this->_data->window.clear(sf::Color::Red);
		
		this->_data->window.draw(this->_background);
		this->_data->window.draw(this->_resumeButton);
		this->_data->window.draw(this->_homeButton);

		this->_data->window.display();
	}
}