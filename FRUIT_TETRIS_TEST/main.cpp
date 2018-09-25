#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include "BlockCreator.hpp"
#include "GameManager.hpp"
#include <iostream>

using namespace std;
using namespace sf;

//3 recent bugs(1 major bug,2 minor bugs)
//major bug: sprites don't show correctly [Couldn't find a proper way to destroy old blocks' sprites]
//minor bug: process doesn't work on CMatrix[21][9],CMatrix[21][8],CMatrix[21][7] (algorithm defect)
//minor bug: A and D keys can ruin CMatrix if they touch other blocks so fast from horizantal side (timer or keyboard buffer defect)

int main() {
	srand(time(0));
	RenderWindow window(VideoMode(600, 800), "Fruit Tetris", Style::Close | Style::Titlebar);
	window.setFramerateLimit(60);
	float timer = 0.0f;
	float delay = 0.4f;
	Clock clock;

	GameManager _gameManager;
	BlockCreator _blockCreator;
	_gameManager.allowBlockCreatorToCreateAnotherBlock(_blockCreator, delay);

	while (window.isOpen()) {
		Event _event;

		float time = clock.restart().asSeconds(); //returns delta time and resets it
		timer += time;

		if (time > 1.0f / 20.0f) {
			time = 1.0f / 20.0f; //avoid fps drops
		}

		while (window.pollEvent(_event)) {
			if (_event.type == _event.Closed) {
				window.close();
			}
			if (_event.type == _event.Resized) {
				cout << "Warning!!!Window size has been changed" << endl;
				cout << "Width is: " << _event.size.width << " " << "Height is: " << _event.size.height << endl;
			}
			if (_event.type == _event.KeyPressed) {
				if (_event.key.code == Keyboard::A ) {
					_blockCreator.moveBlockLeft();
				}
				if (_event.key.code == Keyboard::D ) {
					_blockCreator.moveBlockRight();
				}
				if (_event.key.code == Keyboard::R) {
					_blockCreator.movePartsDown(); 
					_gameManager.movePartsDownForCMatrix(); 
				} 
				if (_event.key.code == Keyboard::T) {
					_blockCreator.movePartsUp(); 
					_gameManager.movePartsUpForCMatrix(); 
				}
				if (_event.key.code == Keyboard::S) {
					delay = 0.0f; 
				}
				if (_event.key.code == Keyboard::Q) {
					delay = 1000.0f; 
				}
			}
		}

		_gameManager.runMainGameLoop(_blockCreator,delay,timer);

		window.clear();
		_gameManager.drawGrid(window);
		_blockCreator.drawBlocks(window); 
		_gameManager.processCMatrix();
		window.display();
	}
	return 0;
}
