#include <stdio.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <algorithm> 
#include <chrono>
#include <cmath>
#include<time.h>

/*
	to check the correctness
	to check flipping we just have to check from the marker placed at that point.
	override equals operator to easily check euqlity of game states

	maintain a start index where the pieces start and an end index so less search space.
*/
struct cord
{
	int row;
	int col;
};

using namespace std;
using namespace std::chrono;
class Game {
public:

	int numRow;
	int numColumn;
	int** board; // might need to change this 
	int valid[8][8];

	// methods
	void addMarker(int row, int column, int marker, int** who);
	void makeBoard(const int rows, const int columns);
	void displayBoard();
	void askForInput();
	void flipMarkersCheck(int row, int column, int marker, int** who);
	void flipMarkers(vector<cord> cordsMem, int** who);
	void generateValid(int color, int** who);
	bool validMove(int color, int rowShift, int colShift, int row, int col, int** who);
	bool checkRestOfLine(int color, int rowShift, int colShift, int row, int col, int** who);
	void makeCompPlay();
	int mctsPlayout(int row, int col);
};

void Game::makeBoard(const int rows, const int columns) {
	board = new int* [rows];
	numRow = rows;
	numColumn = columns;
	for (int i = 0; i < rows; i++) {
		board[i] = new int[columns];
		for (int k = 0; k < columns; k++) {
			if (i == 3 && k == 4) {
				board[i][k] = 0;
				continue;
			}
			else if (i == 4 && k == 3) {
				board[i][k] = 0;
				continue;
			}
			else if (i == 3 && k == 3) {
				board[i][k] = 1;
				continue;
			}
			else if (i == 4 && k == 4) {
				board[i][k] = 1;
				continue;
			}
			board[i][k] = -1;
		}
	}
	/*board[3][4] = 0;
	board[4][3] = 0;
	board[3][3] = 1;
	board[4][4] = 1;*/
}

void Game::displayBoard() {
	auto start = high_resolution_clock::now();
	cout << "  +---+---+---+---+---+---+---+---+";
	printf("\n");
	bool flag = true;
	for (int i = 0; i < numRow; i++) {
		for (int k = 0; k < numColumn; k++) {
			if (flag) {
				cout << i + 1 << " |";
				flag = false;
			}
			if (board[i][k] == 1) {
				cout << " X |";
			}
			else if (board[i][k] == 0) {
				cout << " O |";
			}
			else {
				cout << " - |";
			}
		}
		flag = true;
		printf("\n");
		cout << "  +---+---+---+---+---+---+---+---+";
		printf("\n");

	}
	cout << "    a   b   c   d   e   f   g   h  \n";
	auto stop = high_resolution_clock::now();
	auto duration1 = duration_cast<nanoseconds>(stop - start);
	cout << "dur print : " << duration1.count() << endl;
}

void Game::addMarker(int row, int column, int marker, int** who) {
	who[row][column] = marker;
	auto start = high_resolution_clock::now();
	flipMarkersCheck(row, column, marker, who);
	auto stop = high_resolution_clock::now();
	auto duration1 = duration_cast<nanoseconds>(stop - start);
	//cout << "dur add : " << duration1.count() << endl;
}

void Game::flipMarkersCheck(int row, int column, int marker, int** who) {
	//cout << " marker  " << marker;
	int opponentMarker = !marker;
	cord mem;
	vector <cord> cordsMemory;
	if (row > 0 && who[row - 1][column] == opponentMarker) {
		//cout << "vert nor" << endl;
		mem.col = column;
		mem.row = row - 1;
		int check = who[mem.row][mem.col];
		cordsMemory.push_back({ mem.row , mem.col });
		while (mem.row > 0) {
			// move check up by one in the vertical north
			mem.row = mem.row - 1;
			check = who[mem.row][mem.col];
			if (check == marker)
				break;
			// if it ends in an empty space then cant flip cause it needs to end with marker.
			else if (check == -1) {
				cordsMemory.clear();
				break;
				// add square to memory
				cordsMemory.push_back({ mem.row , mem.col });

			}
		}
		//cout << check << "cehck" << endl;
		if (cordsMemory.size() != 0) {
			flipMarkers(cordsMemory, who);
			cordsMemory.clear();
		}
	}
	if (row > 0 && column < 7 && who[row - 1][column + 1] == opponentMarker) {
		//cout << "dia nor e" << endl;
		mem.col = column + 1;
		mem.row = row - 1;
		int check = who[mem.row][mem.col];
		cordsMemory.push_back({ mem.row , mem.col });
		while (mem.row > 0 && mem.col < 7) {
			// move check up by one in the diagonal northeast
			mem.row = mem.row - 1;
			mem.col = mem.col + 1;
			check = who[mem.row][mem.col];
			if (check == marker)
				break;
			else if (check == -1) {
				cordsMemory.clear();
				break;
			}
			// add square to memory
			cordsMemory.push_back({ mem.row , mem.col });
		}
		if (cordsMemory.size() != 0) {
			flipMarkers(cordsMemory, who);
			cordsMemory.clear();
		}
	}
	if (column < 7 && who[row][column + 1] == opponentMarker) {
		//cout << "hor eas" << endl;
		mem.col = column + 1;
		mem.row = row;
		int check = who[mem.row][mem.col];
		cordsMemory.push_back({ mem.row , mem.col });
		while (mem.col < 7) {
			// move check up by one in the horizontal east
			mem.col = mem.col + 1;
			check = who[mem.row][mem.col];
			if (check == marker)
				break;
			else if (check == -1) {
				cordsMemory.clear();
				break;
			}
			// add square to memory
			cordsMemory.push_back({ mem.row , mem.col });
		}
		if (cordsMemory.size() != 0) {
			flipMarkers(cordsMemory, who);
			cordsMemory.clear();
		}
	}
	if (row < 7 && column < 7 && who[row + 1][column + 1] == opponentMarker) {
		//cout << "dia sou e" << endl;
		mem.col = column + 1;
		mem.row = row + 1;
		int check = who[mem.row][mem.col];
		cordsMemory.push_back({ mem.row , mem.col });
		while (mem.row < 7 && mem.col < 7) {
			// move check up by one in the diagonal south east
			mem.row = mem.row + 1;
			mem.col = mem.col + 1;
			check = who[mem.row][mem.col];
			if (check == marker)
				break;
			else if (check == -1) {
				cordsMemory.clear();
				break;
			}
			cordsMemory.push_back({ mem.row , mem.col });
		}
		if (cordsMemory.size() != 0) {
			flipMarkers(cordsMemory, who);
			cordsMemory.clear();
		}
	}
	if (row < 7 && who[row + 1][column] == opponentMarker) {
		//cout << "vert sou" << endl;
		mem.col = column;
		mem.row = row + 1;
		int check = who[mem.row][mem.col];
		cordsMemory.push_back({ mem.row , mem.col });
		while (mem.row < 7) {
			// move check up by one in the vertical south
			mem.row = mem.row + 1;
			check = who[mem.row][mem.col];
			if (check == marker)
				break;
			else if (check == -1) {
				cordsMemory.clear();
				break;
			}
			cordsMemory.push_back({ mem.row , mem.col });
		}
		if (cordsMemory.size() != 0) {
			flipMarkers(cordsMemory, who);
			cordsMemory.clear();
		}
	}
	if (row < 7 && column >0 && who[row + 1][column - 1] == opponentMarker) {
		//cout << "dia souw" << endl;
		mem.col = column - 1;
		mem.row = row + 1;
		int check = who[mem.row][mem.col];
		cordsMemory.push_back({ mem.row , mem.col });
		while (mem.row < 7 && mem.col > 0) {
			// move check up by one in the diagonal south west
			mem.row = mem.row + 1;
			mem.col = mem.col - 1;
			check = who[mem.row][mem.col];
			if (check == marker)
				break;
			else if (check == -1) {
				cordsMemory.clear();
				break;
			}
			cordsMemory.push_back({ mem.row , mem.col });
		}
		if (cordsMemory.size() != 0) {
			flipMarkers(cordsMemory, who);
			cordsMemory.clear();
		}
	}
	if (column > 0 && who[row][column - 1] == opponentMarker) {
		//cout << "hor we" << endl;
		mem.col = column - 1;
		mem.row = row;
		int check = who[mem.row][mem.col];
		cordsMemory.push_back({ mem.row , mem.col });
		while (mem.col > 0) {
			// add square to memory
			//cout << " came into while" << endl;
			// move check up by one in the horizontal west
			mem.col = mem.col - 1;
			check = who[mem.row][mem.col];
			if (check == marker)
				break;
			else if (check == -1) {
				cordsMemory.clear();
				break;
			}
			cordsMemory.push_back({ mem.row , mem.col });
		}
		if (cordsMemory.size() != 0) {
			//cout << cordsMemory[0].col << cordsMemory[0].row << endl;
			flipMarkers(cordsMemory, who);
			cordsMemory.clear();
		}
	}
	if (row > 0 && column > 0 && who[row - 1][column - 1] == opponentMarker) {
		//cout << "dia nor w" << endl;
		mem.col = column - 1;
		mem.row = row - 1;
		int check = who[mem.row][mem.col];
		cordsMemory.push_back({ mem.row , mem.col });
		while (mem.row > 0 && mem.col > 0) {
			// move check up by one in the diagonal north west
			mem.row = mem.row - 1;
			mem.col = mem.col - 1;
			check = who[mem.row][mem.col];
			if (check == marker)
				break;
			else if (check == -1) {
				cordsMemory.clear();
				break;
			}
			cordsMemory.push_back({ mem.row , mem.col });
			//cout << check << "check in while" << endl;
		}
		if (cordsMemory.size() != 0) {
			//cout << cordsMemory[0].col << cordsMemory[0].row << endl;;
			flipMarkers(cordsMemory, who);
			cordsMemory.clear();
		}
	}


}

void Game::flipMarkers(vector<cord> cordsMem, int** who) {
	for (int i = 0; i < cordsMem.size(); i++) {
		// maybe put if condition here 
		//cout << board[cordsMem[i].row][cordsMem[i].col] <<endl;
		//cout << !board[cordsMem[i].row][cordsMem[i].col] << endl;
		who[cordsMem[i].row][cordsMem[i].col] = !who[cordsMem[i].row][cordsMem[i].col];
	}
}

void Game::generateValid(int color, int** who) {

	bool nn, ne, nw, ww, ee, ss, se, sw;
	// maybe change this to direct initalization
	for (int i = 0; i < 8; i++) {
		for (int k = 0; k < 8; k++) {
			valid[i][k] = -1;
		}
	}

	for (int i = 0; i < 8; i++) {
		for (int k = 0; k < 8; k++) {
			if (board[i][k] == -1) {
				nn = validMove(color, -1, 0, i, k, who);
				nw = validMove(color, -1, -1, i, k, who);
				ne = validMove(color, -1, 1, i, k, who);

				ww = validMove(color, 0, -1, i, k, who);
				ee = validMove(color, 0, 1, i, k, who);

				sw = validMove(color, 1, -1, i, k, who);
				ss = validMove(color, 1, 0, i, k, who);
				se = validMove(color, 1, 1, i, k, who);

				if (nn || nw || ne || ww || ee || sw || ss || se) {
					valid[i][k] = color;
				}

			}
		}
	}


}

bool Game::validMove(int color, int rowShift, int colShift, int row, int col, int** who) {
	int opponentColor = !color;
	// if one away from empty space goes off board ret false.
	if (row + rowShift > 7 || row + rowShift < 0) {
		return false;
	}
	if (col + colShift > 7 || col + colShift < 0) {
		return false;
	}

	// if the adjacent cell doesn't have the opposing color, then ret false.
	if (who[row + rowShift][col + colShift] != opponentColor) {
		return false;
	}

	// if 2 away from empty space goes off the board then ret false.
	if (row + (rowShift * 2) > 7 || row + (rowShift * 2) < 0) {
		return false;
	}
	if (col + (colShift * 2) > 7 || col + (colShift * 2) < 0) {
		return false;
	}

	// if above conditions satisfied, then go along that direction to check if 
	// its a line of opp. color and ends in same color.
	return checkRestOfLine(color, rowShift, colShift, (row + (rowShift * 2)), (col + (colShift * 2)), who);
}

bool Game::checkRestOfLine(int color, int rowShift, int colShift, int row, int col, int** who) {
	// if this func is called then we know that the adjacent cell to the empty space above is of
	// opposite color.

	// if two away from the empty space is the same color then return true. its a valid line.
	if (who[row][col] == color) {
		return true;
	}

	// otherwise we need to continue but if three away from empty space goes off board
	// then no space to finish the line. so ret false.
	if (row + rowShift > 7 || row + rowShift < 0) {
		return false;
	}
	if (col + colShift > 7 || col + colShift < 0) {
		return false;
	}

	// then call this func again but pass in the row/col as one next to current row/col.
	return checkRestOfLine(color, rowShift, colShift, (row + rowShift), (col + colShift), who);
}

void Game::askForInput() {
	cout << "Please enter the coordinates of where you want to place your marker. \n Eg. top left corner is 1a \n";
	char input[2];
	//char yesNo = 'y';
	// could append a 0 to the beginning and have a direct 1-1 acces of the array from input
	char rows[8] = { '1','2','3','4','5','6','7','8' };
	char cols[8] = { 'a','b','c','d','e','f','g','h' };
	cin >> input;
	if (strlen(input) > 2) {
		while (strlen(input) > 2) {
			cout << "Oops! Please try again. \n Eg. top left corner is 1a \n";
			cin >> input;
		}
	}

	generateValid(1, board);
	int colVal = -1;
	int rowVal = -1;
	bool hasValid = false;
	while (!hasValid && (rowVal == -1 || colVal == -1)) {
		for (int i = 0; i < 8; i++) {
			if (cols[i] == input[1]) {
				colVal = i;
				break;
			}
		}

		for (int i = 0; i < 8; i++) {
			if (rows[i] == input[0]) {
				rowVal = i;
				break;
			}
		}
		if (rowVal == -1 || colVal == -1) {
			cout << "Oops! Please try again. \n Eg. top left corner is 1a \n";
			cin >> input;
			if (strlen(input) > 2) {
				while (strlen(input) > 2) {
					cout << "Oops! Please try again. \n Eg. top left corner is 1a \n";
					cin >> input;
				}
			}
		}
		//for (int a = 0; a < 8; a++) {
		//	for (int b = 0; b < 8; b++) {
		//		if (valid[a][b] == 1) {
		//			cout << "F" << "  ";
		//		}
		//		else {
		//			cout << valid[a][b] << "  ";
		//		}
		//	}
		//	cout << "\n";
		//}
		if (rowVal != -1 && colVal != -1 && valid[rowVal][colVal] == 1) {
			hasValid = true;
			break;
		}
		else if (rowVal != -1 && colVal != -1 && valid[rowVal][colVal] != 1) {
			cout << "Invalid Move!! Please try again. \n Eg. top left corner is 1a \n";
			cin >> input;
			if (strlen(input) > 2) {
				while (strlen(input) > 2) {
					cout << "Oops! Please try again. \n Eg. top left corner is 1a \n";
					cin >> input;
				}
			}
		}
	}
	if (hasValid)
		addMarker(rowVal, colVal, 1, board);

}

void Game::makeCompPlay() {
	int sum = 0;
	bool flag = false;
	cord play;
	generateValid(0, board);
	vector<cord> validMoves;
	//save a copy of the current valid moves the comp can make.
	int boardValid[8][8];
	for (int i = 0; i < 8; i++) {
		for (int k = 0; k < 8; k++) {
			boardValid[i][k] = valid[i][k];
		}
	}
	// here are all the valid moves the comp can make
	for (int i = 0; i < 8; i++) {
		for (int k = 0; k < 8; k++) {
			//cout << boardValid[i][k] << "  ";
			if (boardValid[i][k] == 0) {
				validMoves.push_back({ i,k });
			}
		}
		//cout << "\n";
	}
	// set the return value to this in case something goes wrong below. This is basically
	// the comp making a random move.
	play.row = validMoves.back().row;
	play.col = validMoves.back().col;
	cout << "play row,col" << play.row << play.col << endl;

	// now go through each move and simulate X number of playouts.
	int temp;
	
	while (validMoves.size() > 0) {
		int tempSum = 0;
		for (int x = 1; x < 10; x++) {
			temp = mctsPlayout(validMoves.back().row, validMoves.back().col);
			if (temp == 0)
				tempSum += 5;
			else if (temp == 1)
				tempSum += 10;
			else
				tempSum -= 10;
		}
		if (flag == false) {
			sum = tempSum;
			flag = true;
		}
		if (tempSum > sum) {
			play.row = validMoves.back().row;
			play.col = validMoves.back().col;
		}
		validMoves.pop_back();
	}

	addMarker(play.row, play.col, 0, board);
}
// generate the indices above, then for each index pass the cords in so this can make that move then simulate.
int Game::mctsPlayout(int r, int c) {
	int remainingIndices = 0; // keeps track of if the game is finished.
	int xNum = 0;
	int oNum = 0;
	int** boardCopy;
	boardCopy = new int* [8];
	int randRow;
	int randCol;
	for (int i = 0; i < 8; i++) {
		boardCopy[i] = new int[8];
		for (int k = 0; k < 8; k++) {
			if (board[i][k] == -1) {
				//remaining.push_back({ i,k });
				remainingIndices++;
			}
			boardCopy[i][k] = board[i][k];
		}
	}
	//cout << "rem index  " << remainingIndices << endl;
	// add this marker to the copy of the current board, to start things off.
	addMarker(r, c, 0, boardCopy);
	remainingIndices--;

	while (remainingIndices > 0) {
		// make the comp simulate the players move
		randRow = rand() % 8;
		randCol = rand() % 8;
		generateValid(1, boardCopy);
		if (valid[randRow][randCol] == 1) {
			addMarker(randRow, randCol, 1, boardCopy);
		}
		else {
			while (valid[randRow][randCol] != 1) {
				randRow = rand() % 8;
				randCol = rand() % 8;
				if (valid[randRow][randCol] == 1) {
					addMarker(randRow, randCol, 1, boardCopy);					
				}
			}
		}
		remainingIndices--;
		// then have the comp go
		randRow = rand() % 8;
		randCol = rand() % 8;
		generateValid(0, boardCopy);

		if (valid[randRow][randCol] == 0) {
			addMarker(randRow, randCol, 0, boardCopy);
		}
		else {
			while (valid[randRow][randCol] != 0) {
				randRow = rand() % 8;
				randCol = rand() % 8;
				if (valid[randRow][randCol] == 0) {
					addMarker(randRow, randCol, 0, boardCopy);
				}
			}
		}
		remainingIndices--;
	}
	for (int i = 0; i < 8; i++) {
		for (int k = 0; k < 8; k++) {
			if (boardCopy[i][k] == 1) {
				//cout << " X " << "  ";
				xNum++;
			}
			else if (boardCopy[i][k] == 0) {
				//cout << " O " << "  ";
				oNum++;
			}
			//else {
			//	cout << " - " << "  ";
			//}
		}
		//cout << "\n";
	}
	//cout << "\n";
	int ret = 0;
	if (oNum > xNum)
		ret = 1;
	else
		ret = -1;
	return ret;
}

int main() {
	srand(time(NULL));
	Game trial;
	trial.makeBoard(8, 8);
	trial.displayBoard();
	for (int i = 0; i < 10; i++) {
		trial.askForInput();
		trial.displayBoard();
		trial.makeCompPlay();
		trial.displayBoard();
	}
}