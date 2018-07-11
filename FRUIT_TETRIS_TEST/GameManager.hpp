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

public:
	//Delete this parameter and see how it crashes like idiotic
	GameManager() {
		clearTempCMatrix();
		clearCMatrix();
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

	/*
	///Match 3 algorihm
	void processCMatrix() {
		//horizontal [22][10]
		for (int i = 0; i < 22;i++) {
			for (int j = 1; j < 10; j++) {

			}
		}



		//vertical

	}*/

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
};

