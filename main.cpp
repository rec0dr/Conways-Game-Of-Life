#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>

// color codes

std::string reset   = "\033[0m";
std::string red     = "\033[31m";
std::string green   = "\033[32m";
std::string yellow  = "\033[33m";
std::string blue    = "\033[34m";

class Cell {
public:
    bool alive = true;
    bool killed = false;
    bool born = false;


    void setAlive(bool value) {
        alive = value;
    }

    Cell(bool state = false) {
        alive = state;
    }
};

class Board {

private:
    int rows;
    int cols;

public:
    // initial declaration
    std::vector<std::vector<std::vector<int>>> history;
    int max_depth;
    std::vector<std::vector<Cell>> grid;
    Board(int r, int c, int max_depth = 300) {
        rows = r;
        cols = c;
        grid = std::vector<std::vector<Cell>>(rows, std::vector<Cell>(cols));
        this->max_depth = max_depth;
    }
    
    // board logic
    std::vector<std::vector<int>> intArray() {
        std::vector<std::vector<int>> res;
        res = std::vector<std::vector<int>>(grid.size(), std::vector<int>(grid[0].size()));

        for (int i = 0; i < grid.size(); i++) {
            for (int j = 0; j < grid[i].size(); j++) {
                Cell curCell = grid[i][j];
                if (curCell.alive) {
                    res[i][j] = 1;
                }
                else {
                    res[i][j] = 0;
                }
            }
        }

        return res;
    }

    bool isequal(Board board2) {
        return (intArray() == board2.intArray());
    }

    void addHistory() {
        history.push_back(intArray());
    }

    int isStagnant() {
        int i = 0;
        int history_size = history.size();

        if (history_size > max_depth) {
            i = history_size - max_depth;
        }
        
        while (i < history_size) {
            if (intArray() == history[i]) {
                return i;
            }
            i++;
        }

        return -1;
    }

    // neighbor logic
    std::vector<Cell> neighbors(int r, int c) {
        std::vector<Cell> result;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i != 0 || j != 0) {
                    if (r+i >= 0 && r+i < rows && c+j >= 0 && c+j < cols) {
                        // std::cout << "Cell at " << r+i << "," << c+j << " is a neighbor of cell at " << r << "," << c << std::endl;
                        result.push_back(grid[r+i][c+j]);
                    }
                }
            }
        }

        return result;
    }

    std::vector<Cell> neighborsAlive(int r, int c) {
        std::vector<Cell> result;
        for (Cell i : neighbors(r,c)) {
            if (i.alive) {
                result.push_back(i);
            }
        }
        return result;
    }

    int numNeighborsAlive(int r, int c) {
        return neighborsAlive(r,c).size();
    }

    // stats

    std::vector<Cell> cellsKilled() {
        std::vector<Cell> count = {};
        for (int i = 0; i < grid.size(); i++) {
            for (int j = 0; j < grid[i].size(); j++) {
                Cell& curCell = grid[i][j];
                if (curCell.alive) {
                    if (numNeighborsAlive(i,j) != 2 && numNeighborsAlive(i,j) != 3) {
                        count.push_back(curCell);
                    } 
                }
            }
        }
        return count;
    }

    std::vector<std::vector<int>> posKilled() {
        std::vector<std::vector<int>> count;
        for (int i = 0; i < grid.size(); i++) {
            for (int j = 0; j < grid[i].size(); j++) {
                Cell& curCell = grid[i][j];
                if (curCell.alive) {
                    if (numNeighborsAlive(i,j) != 2 && numNeighborsAlive(i,j) != 3) {
                        count.push_back({i,j});
                    } 
                }
            }
        }
        return count;
    }

    int numKilled() {
        return cellsKilled().size();
    }

    int population() {
        int count = 0;
        for (std::vector<Cell> i : grid) {
            for (Cell j : i) {
                if (j.alive) {
                    count++;
                }
            }
        }
        
        return count;
    }

    std::vector<Cell> cellsBorn() {
        std::vector<Cell> count = {};
        for (int i = 0; i < grid.size(); i++) {
            for (int j = 0; j < grid[i].size(); j++) {
                Cell& curCell = grid[i][j];
                if (!curCell.alive) {
                    if (numNeighborsAlive(i,j) == 3) {
                        count.push_back(curCell);
                    }
                }
            }
        }
        return count;
    }

    std::vector<std::vector<int>> posBorn() {
        std::vector<std::vector<int>> count;
        for (int i = 0; i < grid.size(); i++) {
            for (int j = 0; j < grid[i].size(); j++) {
                Cell& curCell = grid[i][j];
                if (!curCell.alive) {
                    if (numNeighborsAlive(i,j) == 3) {
                        count.push_back({i,j});
                    }
                }
            }
        }
        return count;
    }

    int numBorn() {
        return cellsBorn().size();
    }

    // actually doing a turn

    void doTurn() {
        std::vector<std::vector<Cell>> newGrid = grid;

        for (int i = 0; i < grid.size(); i++) {
            for (int j = 0; j < grid[i].size(); j++) {

                int aliveNeighbors = numNeighborsAlive(i,j);

                if (grid[i][j].alive) {
                    if (aliveNeighbors != 2 && aliveNeighbors != 3) {
                        newGrid[i][j].alive = false;
                        newGrid[i][j].killed = true;
                    } else {
                        newGrid[i][j].killed = false;
                        newGrid[i][j].born = false;
                    }
                }
                else {
                    if (aliveNeighbors == 3) {
                        newGrid[i][j].alive = true;
                        newGrid[i][j].born = true;
                    } else {
                        newGrid[i][j].killed = false;
                        newGrid[i][j].born = false;
                    }
                }
            }
        }

        grid = newGrid;
    }

};

char getNthDigitLeft(long long number, int n) {
    // Convert to string and handle negative sign
    std::string numStr = std::to_string(std::abs(number));
    
    // Check if n is within valid bounds (1-indexed)
    if (n < 1 || n > numStr.length()) {
        return -1; // Error code for invalid index
    }
    
    // Convert char back to integer ('0' ascii trick)
    return numStr[n - 1];
}

void print2DArray(std::vector<std::vector<Cell>> array) {

    int offset = std::log10(array.size());
    int offset2 = std::log10(array[0].size());
    offset2++;
    for (int i = 0; i < offset2; i++) {
        for (int k = 0; k < offset; k++) {
            std::cout << "0";
        }

        std::cout << "0 : ";
        for (int j = 0; j < array[0].size(); j++) {
            int res = (((j+1)/(pow(10,(offset2-i-1)))));
            int res2 = res % 10;

            std::cout << res2 << " ";
        }
        std::cout << ": " << std::endl;
    }

    for (int i = 0; i < array.size(); i++) {
        int in = i + 1;
        int curset = offset - (int)std::log10(in);
        for (int k = 0; k < curset; k++) {
            std::cout << "0";
        }
        std::cout << in << " : ";
        for (int j = 0; j < array[i].size(); j++) {
            if (array[i][j].alive) {
                if (array[i][j].born) {
                    std::cout << blue << "c" << reset;
                } else {
                    std::cout << green << "o" << reset;
                }
            } else {
                if (array[i][j].killed) {
                    std::cout << yellow << "/" << reset;
                } else {
                    std::cout << red << "-" << reset;
                }
            }
            if (j != array[i].size()-1) {
                 std::cout << " ";
            }
        }
        std::cout << " : " << std::endl;
    }
}

void print2DInt(std::vector<std::vector<int>> array) {
    int offset = std::log10(array.size());
    int offset2 = std::log10(array[0].size());
    offset2++;
    for (int i = 0; i < offset2; i++) {
        for (int k = 0; k < offset; k++) {
            std::cout << "0";
        }

        std::cout << "0 : ";
        for (int j = 0; j < array[0].size(); j++) {
            int res = (((j+1)/(pow(10,(offset2-i-1)))));
            int res2 = res % 10;

            std::cout << res2 << " ";
        }
        std::cout << ": " << std::endl;
    }

    for (int i = 0; i < array.size(); i++) {
        int in = i + 1;
        int curset = offset - (int)std::log10(in);
        for (int k = 0; k < curset; k++) {
            std::cout << "0";
        }
        std::cout << in << " : ";
        for (int j = 0; j < array[i].size(); j++) {
            if (array[i][j] == 0) {
                std::cout << red << "-" << reset;
            } else {
                std::cout << green << "o" << reset;
            }
            if (j != array[i].size()-1) {
                 std::cout << " ";
            }
        }
        std::cout << " : " << std::endl;
    }
}

std::vector<std::vector<int>> randArray(int r, int c, double fertility) {
    std::vector<std::vector<int>> ex1;
    ex1 = std::vector<std::vector<int>>(r, std::vector<int>(c));
    // Optional random initialization
    for (std::vector<int>& i : ex1) {
        for (int& j : i) {
            double randNum = ((double)rand()/RAND_MAX);
            if (randNum <= fertility) {
                j = 1;
            } else {
                j = 0;
            }
        }
    }
    return ex1;
}

int startGame(int rows, int cols, std::vector<std::vector<int>> gridCopy, bool auton, bool print0, int waitMS) {
    Board game(rows,cols, 10);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (gridCopy[i][j] == 0) {
                game.grid[i][j].setAlive(false);
            }
            else {
                game.grid[i][j].setAlive(true);
            }
        }
    }

    game.addHistory();

    char pause;
    int numKilled = 0;
    int numBorn = 0;
    int gens = 0;
    bool stagnant = false;
    double percentLiving = (game.population()/(rows*cols))*100;

    if (!print0) {
        print2DArray(game.grid);
        std::cout << std::endl;
    }


    while (true) {

        // std::cout << game.history.size() << std::endl;

        // if printing board is enabled
        if (print0) {
            print2DArray(game.grid);
        
            std::cout << "Generation: " << gens << std::endl;
            if (gens != 0) {
                std::cout << yellow << "Number of cells killed: " << numKilled << reset << std::endl;
                std::cout << blue << "Number of cells born: " << numBorn << reset << std::endl;
            }

            int population = game.population();
            double pop_percent = 100.0 * population / (rows*cols);

            std::cout << green << "Population: " << population << "/" << rows*cols << " or " << pop_percent << "%" << reset << std::endl;
            std::cout << std::endl;
        }

        // automatic handling
        if (!auton) {
            std::cout << "Press any key to continue, or 'q' to quit: ";
            std::cin >> pause;
        } else {
            pause = ' ';
            std::this_thread::sleep_for(std::chrono::milliseconds(waitMS));
        }

        game.doTurn();
        int matched_gen = game.isStagnant();
        // std::cout << matched_gen << std::endl;
        stagnant = (matched_gen != -1);

        // check for quit
        if (pause != 'q' && !stagnant) {
            numKilled = game.numKilled();
            numBorn = game.numBorn();
            
            game.addHistory();
            if (print0) {
                std::cout << std::endl;
            }
            gens++;

        } else if (stagnant) {
            std::cout << "Stagnant! Gen " << gens+1 << " matches with gen " << matched_gen << std::endl;
            print2DArray(game.grid);
            break;
        } else {
            std::cout << "Game quit!" << std::endl;
            break;
        }
        
        // check for milestone
        if (gens % 100 == 0 && !print0 && auton) {
            std::cout << gens << " generations checked..." << std::endl;
        }

        // check for overflow
        if (gens > 10000) {
            break;
        }
    }
    
    return gens;
}

void regMain() {
    
    int r;
    int c;
    std::vector<std::vector<int>> gridCopy;
    std::string freeRunText;
    std::cout << "Full grid or probability? (G/P) ";
    std::cin >> freeRunText;
    if (freeRunText == "q") {
        return;
    }

    bool exampleRun = (freeRunText == "P");
    std::cout << "Enter number of rows: ";
    std::cin >> r;
    std::cout << "Enter number of columns: ";
    std::cin >> c;
    if (r == 0 || c == 0) {
        return;
    }

    gridCopy = std::vector<std::vector<int>>(r, std::vector<int>(c));

    if (!exampleRun) {
        print2DInt(gridCopy);
        std::cout << "Enter all positions of alive cells (x y || 0 0 to quit): " << std::endl;
        int curX = 1;
        int curY = 1;
        int curPosNum = 1;
        while (curX > 0 && curX <= c && curY > 0 && curY <= r) {
            std::cout << "Pos " << curPosNum << ": ";
            std::cin >> curX;
            std::cin >> curY;
            curPosNum++;
            if (curX > 0 && curX <= c && curY > 0 && curY <= r) {
                gridCopy[curY-1][curX-1] = 1;
                std::cout << std::endl;
                print2DInt(gridCopy);
                std::cout << std::endl;
            } else {
                break;
            }
        }
        
        
    } else {
        double fertility;
        std::cout << "Enter fertility rate: ";
        std::cin >> fertility;
        gridCopy = randArray(r, c, fertility);
    }

    std::cout << "Auto? (Y/N) ";
    std::string autonString;
    std::cin >> autonString;
    bool auton = (autonString == "Y" || autonString == "y");
    int waitMS;
    if (auton) {
        std::cout << "MS Wait: ";
        std::cin >> waitMS;
    }
    std::cout << "Print Board? (Y/N) ";
    std::string printString;
    std::cin >> printString;
    bool print0 = (printString == "Y" || printString == "y");

    // opt ex
    startGame(r, c, gridCopy, auton, print0, waitMS);
}

void iterMain() {
    int r;
    std::cin >> r;
    int c;
    std::cin >> c;
    int iter;
    std::cin >> iter;
    int a = 0;

    double f;
    std::cin >> f;
    for (int i = 0; i < iter; i++)
    {
        std::cout << "Iteration " << i+1 << ":" << std::endl;
        a += startGame(r,c, randArray(r,c, f), true, false, 0);
        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Iterations: " << iter << std::endl;
    double avg = (a/iter);
    std::cout << "Avg Gens: " << avg << std::endl;
}

int main() {
    srand(time(NULL));
    // Normal initialization
    regMain();
    
}