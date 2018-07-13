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
	bool permission; //To determine when the new block is needed
	int score;

public:
	Music sceneMusic;
	GameManager() {
		clearTempCMatrix();
		clearCMatrix();
		startMusic();
	}

	//utilities start here
	void debugCMatrix() {
		cout << endl;
		for (int i = 0; i < 22; i++) {
			for (int j = 0; j < 10; j++) {
				cout << collision_matrix[i][j] << " ";

			}
			cout << endl;
		}
	}

	void debugTempCMatrix() {
		cout << endl;
		for (int i = 0; i < 22; i++) {
			for (int j = 0; j < 10; j++) {
				cout << temp_collision_matrix[i][j] << " ";

			}
			cout << endl;
		}
	}

	void clearCMatrix() {
		for (int i = 0; i < 22; i++) {
			for (int j = 0; j < 10; j++) {
				collision_matrix[i][j] = 0;
			}
		}
	}

	void clearTempCMatrix() {
		for (int i = 0; i < 22; i++) {
			for (int j = 0; j < 10; j++) {
				temp_collision_matrix[i][j] = 0;
			}
		}
	}
	
	bool checkBelow() {
		if (temp_collision_matrix[pointerForCheckingBelow][vNumber] == 0 && collision_matrix[pointerForCheckingBelow][vNumber] == 0) {
				return true;
		}
		else {
			permission = true;
			return false;
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

	bool askPermissionForNewBlock() {
		return permission;
	}

	void startMusic() {
		if (!sceneMusic.openFromFile("../Sounds/letTheGamesBegin.ogg")) {
			cout << "error";
		}
		sceneMusic.setVolume(80);
		sceneMusic.setLoop(true);
		sceneMusic.play();
	}

	int getScore() {
		return score;
	}
	//utilities end here

	void getNewBlockValuesForCMatrix(BlockCreator &a) {
		i = -2; //resetting it before new block
		pointerForCheckingBelow = 0; //resetting it before new block
		permission = false; //resetting it before new block

		fruitRandomNumsINCREMENTED_M_VALUES[0] = a.returnRandomNumsOfFruits()[0] + 1;
		fruitRandomNumsINCREMENTED_M_VALUES[1] = a.returnRandomNumsOfFruits()[1] + 1;
		fruitRandomNumsINCREMENTED_M_VALUES[2] = a.returnRandomNumsOfFruits()[2] + 1;
	}

	void iterateBlockValuesOnCMatrix(BlockCreator& a) {
		
		if (pointerForCheckingBelow == 22) {
			pointerForCheckingBelow = 0;
		}
	
		while (i > -3 && i < 20 && checkBelow()) {	
			if (i == -2 ) {
				clearTempCMatrix();
				temp_collision_matrix[i + 2][vNumber] = fruitRandomNumsINCREMENTED_M_VALUES[0];
				i++;
				if (pointerForCheckingBelow < 22) {
					pointerForCheckingBelow++;
				}
				break;
			}
			else if (i == -1 ) {
				clearTempCMatrix();
				temp_collision_matrix[i + 1][vNumber] = fruitRandomNumsINCREMENTED_M_VALUES[1];
				temp_collision_matrix[i + 2][vNumber] = fruitRandomNumsINCREMENTED_M_VALUES[0];
				i++;
				if (pointerForCheckingBelow < 22) {
					pointerForCheckingBelow++;
				}
				break;
			}
			else if (i >= 0 ) {
				clearTempCMatrix();
				temp_collision_matrix[i][vNumber] = fruitRandomNumsINCREMENTED_M_VALUES[2];
				temp_collision_matrix[i + 1][vNumber] = fruitRandomNumsINCREMENTED_M_VALUES[1];
				temp_collision_matrix[i + 2][vNumber] = fruitRandomNumsINCREMENTED_M_VALUES[0];
				i++;
				if (pointerForCheckingBelow < 22) {
					pointerForCheckingBelow++;
				}
				break;
			}
		}
	}

	///Match algorihm
	void processCMatrix(RenderWindow &r) {
		int n = 1;
		int m = 1;
		//horizontal
		for (int i = 0; i < 22;i++) {
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
						makeInvisible(r, i, j - u);
						score += n * 100;
					}
					n = 1;
				}
			}
		}
		//vertical
		for (int j = 0; j < 10; j++) {
			for (int i = 1; i < 22; i++) {
				if (collision_matrix[i][j] == collision_matrix[i-1][j] && collision_matrix[i][j] != 0) {
					m += 1;
				}
				else if (m < 3) {
					m = 1;
				}
				else if (m >= 3) {
					for (int u = 1; u < m + 1; u++) {
						collision_matrix[i-u][j] = 0;
						makeInvisible(r, i - u, j);
						score += m * 100;
					}
					m = 1;
				}
			}
		}

	}

	void movePartsDownOnCMatrix() {
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

	void movePartsUpOnCMatrix() {
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

	//WRONG APPROACH
	void makeInvisible(RenderWindow &r , int i , int j) {
		RectangleShape blackObj(Vector2f(31.0f, 31.0f));
		blackObj.setPosition(Vector2f(45.0f + (j * 32), 1.0f + (i * 32)));
		blackObj.setFillColor(Color(255, 0, 0, 255));
		r.draw(blackObj);
	}
	//WRONG APPROACH
	void make0MatrixValuesInvisible(RenderWindow &r) {
		for (int i = 0; i < 22; i++) {
			for (int j = 0; j < 10; j++) {
				if (collision_matrix[i][j] = 0) {
					makeInvisible(r, i, j);
				}
			}
		}
	}
};

