#include <iostream>

using namespace std;

class GameManager {

private:
	int collision_matrix[22][10]; //all records
	int temp_collision_matrix[22][10];  //just for movement of the new block
	int fruitRandomNumsINCREMENTED_M_VALUES[3]; //incremented values because 0 means empty in CMatrix
	int vNumber; //vertical number of the created block
	int i = -2; //iterator value of the created block
	int pointerForCheckingBelow = 0; //pointer of the created block
	bool permission = false; //To determine when the new block is needed
	int score;

public:
	Music sceneMusic; 
	GameManager() {
		clearMatrix("bothmatrices");
	}

	void drawGrid(RenderWindow& window) {
		RectangleShape line(Vector2f(1.0f, 704.0f));
		Color color(150, 150, 150);
		line.setFillColor(color);
		for (int i = 1; i < 12; i++) { //vertical drawing
			line.setPosition(12.0f + (32 * i), 00.0f);
			window.draw(line);
		}
		line.rotate(-90.0f);
		line.setSize(Vector2f(1.0f, 321.0f));
		for (int i = 1; i < 24; i++) { //horizontal drawing
			line.setPosition(44.0f, 737.0f - (32 * i));
			window.draw(line);
		}
	}

	void runMainGameLoop(BlockCreator &b,float &delay,float &timer) {
		if (timer > delay) {
			getVNumber(b);
			iterateBlockOnVisualSide(b);
			iterateBlockValuesOnTempCMatrix();
			debugMatrix("cmatrix"); //or debugMatrix("tempcmatrix");
			if (askPermissionForNewBlock()) {
				allowBlockCreatorToCreateAnotherBlock(b, delay);
			}
			timer = 0;
		}
	}

	//utilies start here
	void startMusic() {
		if (!sceneMusic.openFromFile("../Sounds/letTheGamesBegin.ogg")) {
			cout << "error";
		}
		sceneMusic.setVolume(80);
		sceneMusic.setLoop(true);
		sceneMusic.play();
	}

	void debugMatrix(string name) {
		if (name == "cmatrix") {
			cout << endl;
			for (int i = 0; i < 22; i++) {
				for (int j = 0; j < 10; j++) {
					cout << collision_matrix[i][j] << " ";
				}
				cout << endl;
			}
		}
		else if (name == "tempcmatrix") {
			cout << endl;
			for (int i = 0; i < 22; i++) {
				for (int j = 0; j < 10; j++) {
					cout << temp_collision_matrix[i][j] << " ";
				}
				cout << endl;
			}
		}
	}

	void clearMatrix(string name) {
		if (name == "cmatrix") {
			for (int i = 0; i < 22; i++) {
				for (int j = 0; j < 10; j++) {
					collision_matrix[i][j] = 0;
				}
			}
		}
		else if (name == "tempcmatrix") {
			for (int i = 0; i < 22; i++) {
				for (int j = 0; j < 10; j++) {
					temp_collision_matrix[i][j] = 0;
				}
			}
		}
		else if (name == "bothmatrices") {
			clearMatrix("cmatrix");
			clearMatrix("tempcmatrix");
		}
	}

	void getVNumber(BlockCreator &a) {
		vNumber = a.getJ(); //getting the j value for matrices(a[i][j])
	}

	void addTempCMatrixToCMatrix() {
		for (int i = 0; i < 22; i++) {
			for (int j = 0; j < 10; j++) {
				collision_matrix[i][j] += temp_collision_matrix[i][j];
			}
		}
	}

	bool checkBelow() {
		if (temp_collision_matrix[pointerForCheckingBelow][vNumber] == 0 && collision_matrix[pointerForCheckingBelow][vNumber] == 0) {
			return true;
		}
		permission = true;
		return false;
	}

	bool askPermissionForNewBlock() {
		return permission;
	}
	
	void resetPointerAndPermission() {
		i = -2; //resetting it before new block
		pointerForCheckingBelow = 0; //resetting it before new block
		permission = false; //resetting it before new block
	}

	void setTimeNormal(float &d) {
		d = 0.4f; //for ticking
	}

	int getScore() {
		return score;
	}

	void getNewBlockValuesForCMatrix(BlockCreator &b) {
		fruitRandomNumsINCREMENTED_M_VALUES[0] = b.returnRandomNumsOfFruits()[0] + 1;
		fruitRandomNumsINCREMENTED_M_VALUES[1] = b.returnRandomNumsOfFruits()[1] + 1;
		fruitRandomNumsINCREMENTED_M_VALUES[2] = b.returnRandomNumsOfFruits()[2] + 1;
	}
	//utilities end here

	void movePartsDownForCMatrix() {
		//move the block values down
		// 1 -> 0 , 2 -> 1 ,  0 -> 2
		int temp = fruitRandomNumsINCREMENTED_M_VALUES[0];
		// 1 -> 0
		fruitRandomNumsINCREMENTED_M_VALUES[0] = fruitRandomNumsINCREMENTED_M_VALUES[1];
		// 2 -> 1	
		fruitRandomNumsINCREMENTED_M_VALUES[1] = fruitRandomNumsINCREMENTED_M_VALUES[2];
		// 0 -> 2
		fruitRandomNumsINCREMENTED_M_VALUES[2] = temp;
	}

	void movePartsUpForCMatrix() {
		//move the block values up
		//1 -> 2 , 0 -> 1 , 2 -> 0 
		int temp = fruitRandomNumsINCREMENTED_M_VALUES[2];
		// 1 -> 2
		fruitRandomNumsINCREMENTED_M_VALUES[2] = fruitRandomNumsINCREMENTED_M_VALUES[1];
		// 0 -> 1
		fruitRandomNumsINCREMENTED_M_VALUES[1] = fruitRandomNumsINCREMENTED_M_VALUES[0];
		// 2 -> 0
		fruitRandomNumsINCREMENTED_M_VALUES[0] = temp;
	}

	void iterateBlockValuesOnTempCMatrix() {
		if (i < 20 && checkBelow()) {
			clearMatrix("tempcmatrix");
			if (i == -2) {
				temp_collision_matrix[i + 2][vNumber] = fruitRandomNumsINCREMENTED_M_VALUES[0];
			}
			else if (i == -1) {
				temp_collision_matrix[i + 1][vNumber] = fruitRandomNumsINCREMENTED_M_VALUES[1];
				temp_collision_matrix[i + 2][vNumber] = fruitRandomNumsINCREMENTED_M_VALUES[0];
			}
			else if (i >= 0) {
				temp_collision_matrix[i][vNumber] = fruitRandomNumsINCREMENTED_M_VALUES[2];
				temp_collision_matrix[i + 1][vNumber] = fruitRandomNumsINCREMENTED_M_VALUES[1];
				temp_collision_matrix[i + 2][vNumber] = fruitRandomNumsINCREMENTED_M_VALUES[0];
			}
			i++;
			pointerForCheckingBelow++;
		}
	}

	void iterateBlockOnVisualSide(BlockCreator &b) {
		if (checkBelow()) {
			b.tick();
		}
	}

	void allowBlockCreatorToCreateAnotherBlock(BlockCreator &b,float &delay) {
		addTempCMatrixToCMatrix();
		setTimeNormal(delay);
		b.createBlock();
		resetPointerAndPermission();
		getNewBlockValuesForCMatrix(b);
	}

	///Match algorihm
	void processCMatrix() {
		int n = 1;
		int m = 1;
		//horizontal
		for (int i = 0; i < 22; i++) {
			for (int j = 1; j < 10; j++) {
				if (collision_matrix[i][j] == collision_matrix[i][j - 1] && collision_matrix[i][j] != 0) {
					n += 1;
				}
				else if (n < 3) {
					n = 1;
				}
				else if (n >= 3) {
					for (int u = 1; u < n + 1; u++) {
						collision_matrix[i][j - u] = 0;
						score += n * 100;
					}
					n = 1;
				}
			}
		}
		//vertical
		for (int j = 0; j < 10; j++) {
			for (int i = 1; i < 22; i++) {
				if (collision_matrix[i][j] == collision_matrix[i - 1][j] && collision_matrix[i][j] != 0) {
					m += 1;
				}
				else if (m < 3) {
					m = 1;
				}
				else if (m >= 3) {
					for (int u = 1; u < m + 1; u++) {
						collision_matrix[i - u][j] = 0;
						score += m * 100;
					}
					m = 1;
				}
			}
		}

	}

};
