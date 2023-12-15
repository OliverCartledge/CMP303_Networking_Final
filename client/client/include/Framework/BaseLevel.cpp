#include "BaseLevel.h"

BaseLevel::BaseLevel()
{
	window = nullptr;
	input = nullptr;
	gameState = nullptr;
	audio = nullptr;
}

BaseLevel::~BaseLevel()
{
}

// Begins rendering to the back buffer. Background colour set to light blue.
void BaseLevel::beginDraw()
{
	window->clear(sf::Color(97, 83, 109));
}

// Ends rendering to the back buffer, and swaps buffer to the screen.
void BaseLevel::endDraw()
{
	window->display();
}