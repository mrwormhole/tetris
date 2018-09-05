#include <SFML\Graphics.hpp>
#include <iostream>
#include <vector> 

using namespace sf;
using namespace std;

struct Block {
	RectangleShape fruitObjs[3];
	int randomNums[3];
};

class BlockCreator {

private:
	Texture allFruitsTexture;
	IntRect fruits[7];
	int j;
	int currentBlockNumber = 0;
	Block b;
	vector<Block> blocks;

public:
	BlockCreator() {
		blocks.assign(100, b);
		allFruitsTexture.loadFromFile("../Images/fruits.png");
		Vector2u textureSize = allFruitsTexture.getSize();
		textureSize.x /= 4;
		textureSize.y /= 2;
		// apple , grape , lemonade , cherry
		// banana , watermelon , peach , NULL		
		int i = 0;
		for (int m = 0; m < 2; m++) {
			for (int n = 0; n < 4; n++) {
				if (m == 1 && n == 3) {
					break;
				}
				fruits[i] = IntRect(textureSize.x * n, textureSize.y * m, textureSize.x, textureSize.y);
				i++;
			}
		}
	}

public:
	void drawBlocks(RenderWindow& r) {
		for (int o = 1; o < 1 + currentBlockNumber; o++) {
			r.draw(blocks[o].fruitObjs[0]); //bottom
			r.draw(blocks[o].fruitObjs[1]); //mid 
			r.draw(blocks[o].fruitObjs[2]); //top	
		}
	}

	void createBlock() {
		currentBlockNumber++;
		int index0 = (rand() % 10) % 7;
		int index1 = (rand() % 10) % 7;
		int index2 = (rand() % 10) % 7;
		while (index0 == index1 && index1 == index2 && index2 == index0) {
			index0 = (rand() % 10) % 7;
			index1 = (rand() % 10) % 7;
			index2 = (rand() % 10) % 7;
		}
		blocks[currentBlockNumber].randomNums[0] = index0;
		blocks[currentBlockNumber].randomNums[1] = index1;
		blocks[currentBlockNumber].randomNums[2] = index2;
		j = rand() % 10;
		for (int k = 0; k < 3; k++) {
			RectangleShape fruitObj(Vector2f(32.0f, 32.0f));
			fruitObj.setPosition(Vector2f(44.0f + (j *32.0f), -32.0f - (k*32.0f)));
			fruitObj.setTexture(&allFruitsTexture);
			fruitObj.setTextureRect(IntRect(fruits[blocks[currentBlockNumber].randomNums[k]]));
			blocks[currentBlockNumber].fruitObjs[k] = fruitObj;
		}
		
	}

	void moveBlockLeft() {
		changeJ("dec");
		if (blocks[currentBlockNumber].fruitObjs[0].getPosition().x > 44 + 1) {
			for (int o = 0; o < 3; o++) {
				Vector2f tempPos = blocks[currentBlockNumber].fruitObjs[o].getPosition();
				tempPos.x -= 32;
				blocks[currentBlockNumber].fruitObjs[o].setPosition(tempPos);
			}
		}
	}

	void moveBlockRight() {
		changeJ("inc");
		if (blocks[currentBlockNumber].fruitObjs[0].getPosition().x < 332 - 1) {
			for (int o = 0; o < 3; o++) {
				Vector2f tempPos = blocks[currentBlockNumber].fruitObjs[o].getPosition();
				tempPos.x += 32;
				blocks[currentBlockNumber].fruitObjs[o].setPosition(tempPos);
			}
		}
	}

	void movePartsDown() {
		//move the blocks down
		// 1 -> 0 , 2 -> 1 ,  0 -> 2
		RectangleShape temp = blocks[currentBlockNumber].fruitObjs[0]; //we will need it in the last step
		Vector2f pos0 = blocks[currentBlockNumber].fruitObjs[0].getPosition();
		Vector2f pos1 = blocks[currentBlockNumber].fruitObjs[1].getPosition();
		Vector2f pos2 = blocks[currentBlockNumber].fruitObjs[2].getPosition();
		// 1 -> 0
		blocks[currentBlockNumber].fruitObjs[1].setPosition(pos0);
		blocks[currentBlockNumber].fruitObjs[0] = blocks[currentBlockNumber].fruitObjs[1];
		// 2 -> 1	
		blocks[currentBlockNumber].fruitObjs[2].setPosition(pos1);
		blocks[currentBlockNumber].fruitObjs[1] = blocks[currentBlockNumber].fruitObjs[2];
		// 0 -> 2
		temp.setPosition(pos2);
		blocks[currentBlockNumber].fruitObjs[2] = temp;
	}

	void movePartsUp() {
		//move the blocks up
		//1 -> 2 , 0 -> 1 , 2 -> 0 
		RectangleShape temp = blocks[currentBlockNumber].fruitObjs[2]; //we will need it in the last step
		Vector2f pos0 = blocks[currentBlockNumber].fruitObjs[0].getPosition();
		Vector2f pos1 = blocks[currentBlockNumber].fruitObjs[1].getPosition();
		Vector2f pos2 = blocks[currentBlockNumber].fruitObjs[2].getPosition();
		// 1 -> 2
		blocks[currentBlockNumber].fruitObjs[1].setPosition(pos2);
		blocks[currentBlockNumber].fruitObjs[2] = blocks[currentBlockNumber].fruitObjs[1];
		// 0 -> 1
		blocks[currentBlockNumber].fruitObjs[0].setPosition(pos1);
		blocks[currentBlockNumber].fruitObjs[1] = blocks[currentBlockNumber].fruitObjs[0];
		// 2 -> 0
		temp.setPosition(pos0);
		blocks[currentBlockNumber].fruitObjs[0] = temp;
	}

	void tick() { //for moving block down	
		if (blocks[currentBlockNumber].fruitObjs[0].getPosition().y < 672 - 1) {
			for (int o = 0; o < 3; o++) {
				Vector2f tempPos = blocks[currentBlockNumber].fruitObjs[o].getPosition();
				tempPos.y += 32;
				blocks[currentBlockNumber].fruitObjs[o].setPosition(tempPos);
			}
		}
	}

	//utilities start here
	RectangleShape *returnCurrentAvailableFruits() {
		return blocks[currentBlockNumber].fruitObjs;
	}

	int *returnRandomNumsOfFruits() {
		return blocks[currentBlockNumber].randomNums;
	}

	void changeJ(string a) {
		if (a == "inc" && j != 9) {
			j++; //inc
		}
		else if (a == "dec" && j != 0) {
			j--; //dec
		}
	}

	int getJ() {
		return j; //for controller
	}

	void incrementBlockNumber() {
		currentBlockNumber++; //for spawning
	}
	//utilities end here
};