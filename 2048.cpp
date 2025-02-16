#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <unistd.h>  // for sleep()

using namespace std;

class Game2048 {
public:
    vector<vector<int>> board;
    int score;

    Game2048() : score(0) {
        board = vector<vector<int>>(4, vector<int>(4, 0));
        addNewTile();
        addNewTile();
    }

    void addNewTile() {
        vector<pair<int, int>> emptyPositions;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (board[i][j] == 0) emptyPositions.emplace_back(i, j);
            }
        }
        if (!emptyPositions.empty()) {
            auto [x, y] = emptyPositions[rand() % emptyPositions.size()];
            board[x][y] = (rand() % 10 < 9) ? 2 : 4;
        }
    }

    bool canMove(int direction) {
        Game2048 temp = *this;
        temp.move(direction);
        return temp.board != board;
    }

    void move(int direction) {
        vector<vector<int>> original = board;
        if (direction == 0) moveUp();
        else if (direction == 1) moveDown();
        else if (direction == 2) moveLeft();
        else if (direction == 3) moveRight();
        if (board != original) addNewTile();
    }

    void moveLeft() {
        for (auto &row : board) row = merge(row);
    }

    void moveRight() {
        for (auto &row : board) {
            reverse(row.begin(), row.end());
            row = merge(row);
            reverse(row.begin(), row.end());
        }
    }

    void moveUp() {
        transpose();
        moveLeft();
        transpose();
    }

    void moveDown() {
        transpose();
        moveRight();
        transpose();
    }

    vector<int> merge(vector<int> row) {
        vector<int> newRow;
        for (int val : row) if (val) newRow.push_back(val);
        for (size_t i = 0; i + 1 < newRow.size(); ++i) {
            if (newRow[i] == newRow[i + 1]) {
                newRow[i] *= 2;
                score += newRow[i];
                newRow[i + 1] = 0;
            }
        }
        newRow.erase(remove(newRow.begin(), newRow.end(), 0), newRow.end());
        while (newRow.size() < 4) newRow.push_back(0);
        return newRow;
    }

    void transpose() {
        for (int i = 0; i < 4; ++i) {
            for (int j = i + 1; j < 4; ++j) {
                swap(board[i][j], board[j][i]);
            }
        }
    }

    int getScore() const {
        return score;
    }

    bool isGameOver() {
        for (int move = 0; move < 4; ++move) {
            if (canMove(move)) return false;
        }
        return true;
    }

    void displayBoard() {
        system("clear");
        cout << "Score: " << score << "\n";
        for (const auto &row : board) {
            for (int num : row) {
                if (num == 0) cout << " . ";
                else cout << num << " ";
            }
            cout << "\n";
        }
        cout << "--------------------\n";
        usleep(200000); // Pause for visualization
    }
};

class Genetic2048 {
public:
    vector<vector<vector<double>>> population;
    int populationSize;
    double mutationRate;

    Genetic2048(int size = 20000, double mutation = 0.1) : populationSize(size), mutationRate(mutation) {
        srand(time(0));
        for (int i = 0; i < size; ++i) {
            population.push_back(randomWeights());
        }
    }

    vector<vector<double>> randomWeights() {
        vector<vector<double>> weights(4, vector<double>(4));
        for (auto &row : weights) {
            for (auto &val : row) val = (double)rand() / RAND_MAX;
        }
        return weights;
    }

    int chooseBestMove(vector<vector<int>> &board, vector<vector<double>> &agent) {
        vector<int> scores(4, -1);
        Game2048 tempGame;
        tempGame.board = board;
        for (int move = 0; move < 4; ++move) {
            if (tempGame.canMove(move)) {
                Game2048 temp = tempGame;
                temp.move(move);
                scores[move] = computeScore(temp.board, agent);
            }
        }
        return max_element(scores.begin(), scores.end()) - scores.begin();
    }

    int computeScore(vector<vector<int>> &board, vector<vector<double>> &agent) {
        int score = 0;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                score += board[i][j] * agent[i][j];
            }
        }
        return score;
    }

    int evaluateFitness(vector<vector<double>> &agent) {
        Game2048 game;
        while (!game.isGameOver()) {
            int move = chooseBestMove(game.board, agent);
            game.move(move);
            game.displayBoard();
        }
        return game.getScore();
    }
};

int main() {
    Genetic2048 ga;
    ga.evaluateFitness(ga.population[0]);
    return 0;
}
