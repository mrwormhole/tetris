#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include <iostream>
#include "BlockCreator.hpp"
#include "GameManager.hpp"

using namespace std;
using namespace sf;

/*
Few honest words from a programmer;
I NEED TO SOMEHOW MERGE THE METHODS IN BLOCKCREATOR AND GAMEMANAGER
Tick method from blockcreator and IterateBlockValuesOnCMatrix are doing the same function
Why do i need to merge them?Because i want to not draw when Matrix value of the block gets 0
This totally sucks i created a matrix where i don't have full control over it.Dammit it looks like an useless map.I wanna kill myself
*/

void drawGrid(RenderWindow& window) {
	RectangleShape line(Vector2f(1.0f, 704.0f));
	Color color(150, 150, 150);
	line.setFillColor(color);
	//vertical
	for (int i = 1; i < 12; i++) {
		line.setPosition(12.0f + (32 * i), 00.0f);
		window.draw(line);
	}
	//horizontal
	line.rotate(-90.0f);
	line.setSize(Vector2f(1.0f, 321.0f));
	for (int i = 1; i < 24; i++) {
		line.setPosition(44.0f, 737.0f - (32 * i));
		window.draw(line);
	}
}

int main() {
	srand(time(0));
	RenderWindow window(VideoMode(600, 800), "Fruit Tetris", Style::Close | Style::Titlebar); //not sure about Titlebar.Docs say it provides fixed border
	window.setFramerateLimit(60);

	BlockCreator _blockCreator;
	_blockCreator.createBlock();

	GameManager _gameManager;
	_gameManager.getNewBlockValuesForCMatrix(_blockCreator);

	float timer = 0.0f;
	float delay = 0.4f;
	Clock clock;

	while (window.isOpen()) {
		float time = clock.restart().asSeconds(); //returns delta time and resets it
		timer += time;

		if (time > 1.0f / 20.0f) {
			time = 1.0f / 20.0f; //avoid fps drops
		}



		Event _event;

		while (window.pollEvent(_event)) {
			if (_event.type == _event.Closed) {
				window.close();
			}
			if (_event.type == _event.Resized) {
				cout << "Warning!!!Window size has been changed" << endl;
				cout << "Width is: " << _event.size.width << " " << "Height is: " << _event.size.height << endl;
			}
			if (_event.type == _event.KeyPressed) {
				if (_event.key.code == Keyboard::A && _gameManager.checkBelow()) {
					_blockCreator.moveBlockLeft();
					_blockCreator.decrementJ();
				}
				if (_event.key.code == Keyboard::D && _gameManager.checkBelow()) {
					_blockCreator.moveBlockRight();
					_blockCreator.incrementJ();
				}
				if (_event.key.code == Keyboard::R && _gameManager.checkBelow()) {
					_blockCreator.movePartsDown();
					_gameManager.movePartsDownOnCMatrix();
				}
				if (_event.key.code == Keyboard::T && _gameManager.checkBelow()) {
					_blockCreator.movePartsUp();
					_gameManager.movePartsUpOnCMatrix();
				}
				if (_event.key.code == Keyboard::S && _gameManager.checkBelow()) {
					delay = 0.0f;
				}
				if (_event.key.code == Keyboard::Q && _gameManager.checkBelow()) {
					delay = 1000.0f;
				}
			}
		}


		if (timer > delay) {
			_gameManager.getVNumber(_blockCreator);

			if (_gameManager.checkBelow()) {
				_blockCreator.tick(delay);
			}
			_gameManager.iterateBlockValuesOnCMatrix(_blockCreator);
			_gameManager.debugCMatrix();

			if ( _gameManager.askPermissionForNewBlock()) {
				_gameManager.addTempCMatrixToCMatrix(); 
				//_gameManager.processCMatrix(window);
				_blockCreator.incrementBlockNumber(); 
				_blockCreator.setTimeNormal(delay); 
				_blockCreator.createBlock(); 
				_gameManager.getNewBlockValuesForCMatrix(_blockCreator); 
			}
			timer = 0;
		}

		window.clear();
		drawGrid(window);
		_blockCreator.drawBlocks(window);
		_gameManager.processCMatrix(window);
		//_gameManager.makeInvisible(window, 20, 5);
		//_gameManager.makeInvisible(window, 21, 5); //we should draw after drawing blocks to cover it
		window.display();
	}
	
	return 0;
}
