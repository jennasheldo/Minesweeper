#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "TextureManager.h"
#include "RandomNum.h"

using namespace std;


/* game plan!
    class for board
    class for each tile
    create a vector to hold each tile, an obj of the board
    make it work!
    map of resources??
    things it should do/ functions to make:
    - restart the board
    - right click (set flags, clear tiles of flags)
    - set or clear mines, single or large quantity
    - recalculate num adj mines
*/


// class to represent each tile spot!
struct tileSpot {

    sf::Sprite tileHidden;
    sf::Sprite tileRevealed;
    sf::Sprite flagSpr;
    sf::Sprite mineSpr;
    sf::Sprite number1;
    sf::Sprite number2;
    sf::Sprite number3;
    sf::Sprite number4;
    sf::Sprite number5;
    sf::Sprite number6;
    sf::Sprite number7;
    sf::Sprite number8;
    
    bool mine;
    bool flag;
    bool hidden;
    bool showNum ;

    int numAdjTiles;
    int numAdjMines;
    
    bool corner ;
    bool topEdge ;
    bool bottomEdge ;
    bool leftEdge ;
    bool rightEdge;

    int adjLocs[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
    vector <sf::Sprite> numbers;

    tileSpot() {

        this->corner = false;
        this->topEdge = false;
        this->bottomEdge = false;
        this->leftEdge = false;
        this->rightEdge = false;

        this->numAdjTiles = 0;
        this->numAdjMines = 0;

        this->mine = false;
        this->flag = false;
        this->hidden = true;
        this->showNum = false;

        tileHidden.setTexture(TextureManager::GetTexture("tile_hidden"));
        tileRevealed.setTexture(TextureManager::GetTexture("tile_revealed"));

        flagSpr.setTexture(TextureManager::GetTexture("flag"));
        mineSpr.setTexture(TextureManager::GetTexture("mine"));

        number1.setTexture(TextureManager::GetTexture("number_1"));
        number2.setTexture(TextureManager::GetTexture("number_2"));
        number3.setTexture(TextureManager::GetTexture("number_3"));
        number4.setTexture(TextureManager::GetTexture("number_4"));
        number5.setTexture(TextureManager::GetTexture("number_5"));
        number6.setTexture(TextureManager::GetTexture("number_6"));
        number7.setTexture(TextureManager::GetTexture("number_7"));
        number8.setTexture(TextureManager::GetTexture("number_8"));

        numbers.push_back(number1);
        numbers.push_back(number2);
        numbers.push_back(number3);
        numbers.push_back(number4);
        numbers.push_back(number5);
        numbers.push_back(number6);
        numbers.push_back(number7);
        numbers.push_back(number8);
    }
};

// class to represent the board!
struct board {
    int windowSize;
    int tileCount;
    int numColumns;
    int numRows;

    int tilesRemain;
    int minesRemain;

    vector <tileSpot> boardOfTiles;

    sf::Sprite happyFace;
    sf::Sprite loserFace;
    sf::Sprite winnerFace;
    sf::Sprite debug;
    sf::Sprite digits;
    sf::Sprite test1;
    sf::Sprite test2;
    sf::Sprite test3;

    bool happy = true;
    bool loser = false;
    bool winner = false;
    bool debugMode = false;

    void reset (int numRows, int numColumns, int tileCount, int windowSize,int  mineCount);
    void LoadTestBoard(string fileName);
    void initializeTiles();
    void RevealTiles(int n);
    void AssignAdjacent();

    board (int numRows, int numColumns, int tileCount, int windowSize, int mineCount) {
        reset(numRows, numColumns, tileCount, windowSize, mineCount);
    }
        
};

void board :: reset(int numRows, int numColumns, int tileCount, int windowSize, int mineCount) {
    this->windowSize = windowSize;
    this->tileCount = tileCount;
    this->winner = false;
    this->loser = false;
    this->happy = true;

    this->numColumns = numColumns;
    this->numRows = numRows;


    this-> minesRemain = mineCount;
    this->tilesRemain = tileCount - mineCount;

    happyFace.setTexture(TextureManager::GetTexture("face_happy"));
    loserFace.setTexture(TextureManager::GetTexture("face_lose"));
    winnerFace.setTexture(TextureManager::GetTexture("face_win"));
    debug.setTexture(TextureManager::GetTexture("debug"));
    digits.setTexture(TextureManager::GetTexture("digits"));
    test1.setTexture(TextureManager::GetTexture("test_1"));
    test2.setTexture(TextureManager::GetTexture("test_2"));
    test3.setTexture(TextureManager::GetTexture("test_3"));

    // clear previous board
    boardOfTiles.clear();

    //initialize all the tiles to the vector
    initializeTiles();

    // mine setup randomly
    int minesShown = 0;
    for (int i = 0; i < mineCount; i++) {
        int cur = RandomNum::Int(0, tileCount-1);
        if (boardOfTiles[cur].mine == false) {
            boardOfTiles[cur].mine = true;
            minesShown += 1;
        }
        else while (boardOfTiles[cur].mine == true) {
            cur = RandomNum::Int(0, tileCount - 1);
        }
        if (boardOfTiles[cur].mine == false) {
            boardOfTiles[cur].mine = true;
            minesShown += 1;
        }
    }

    //go through and assign each tile with its adjacent tiles
    AssignAdjacent();

    //set positions of fixed buttons
    happyFace.setPosition((numColumns * 32 / 2) - 32, numRows * 32);
    loserFace.setPosition((numColumns * 32 / 2) - 32, numRows * 32);
    winnerFace.setPosition((numColumns * 32 / 2) - 32, numRows * 32);
    debug.setPosition((numColumns * 32 / 2) + 96, numRows * 32);
    test1.setPosition((numColumns * 32 / 2) + 160, numRows * 32);
    test2.setPosition((numColumns * 32 / 2) + 224, numRows * 32);
    test3.setPosition((numColumns * 32 / 2) + 288, numRows * 32);
}

void board::RevealTiles(int n) {
    if (this->boardOfTiles[n].hidden && this->boardOfTiles[n].flag == false) {
        this->boardOfTiles[n].hidden = false;
        this->boardOfTiles[n].showNum = true;
        this->tilesRemain = tilesRemain - 1;
        if (boardOfTiles[n].numAdjMines == 0) {
            for (int i = 0; i < 8; i++) {
                if (boardOfTiles[n].adjLocs[i] != -1) {
                    RevealTiles(boardOfTiles[n].adjLocs[i]);
                }
            }
        }
    }
}

void board::initializeTiles() {
    for (int i = 0; i < tileCount; i++) {
        tileSpot currentTile = tileSpot();
        if (i == 0 || i == numColumns - 1 || i == tileCount - 1 || i == tileCount - numColumns) {
            currentTile.corner = true;
            currentTile.numAdjTiles = 3;
        }
        if (i % numColumns == 0) {
            currentTile.leftEdge = true;
            if (currentTile.corner == false) {
                currentTile.numAdjTiles = 5;
            }
        }
        if (i % numColumns == numColumns - 1) {
            currentTile.rightEdge = true;
            if (currentTile.corner == false) {
                currentTile.numAdjTiles = 5;
            }
        }
        if (i < numColumns) {
            currentTile.topEdge = true;
            if (currentTile.corner == false) {
                currentTile.numAdjTiles = 5;
            }
        }
        if (i >= tileCount - numColumns) {
            currentTile.bottomEdge = true;
            if (currentTile.corner == false) {
                currentTile.numAdjTiles = 5;
            }
        }
        boardOfTiles.push_back(currentTile);
    }
}

void board::AssignAdjacent() {
    for (int i = 0; i < tileCount; i++) {
        if (boardOfTiles[i].topEdge == false && boardOfTiles[i].leftEdge == false) {
            int topLeft = i - numColumns - 1;
            if (boardOfTiles[topLeft].mine == false) {
                boardOfTiles[i].adjLocs[0] = topLeft;
            }
            else {
                boardOfTiles[i].numAdjTiles -= 1;
                boardOfTiles[i].numAdjMines += 1;
            }
        }
        if (boardOfTiles[i].topEdge == false) {
            int top = i - numColumns;
            if (boardOfTiles[top].mine == false) {
                boardOfTiles[i].adjLocs[1] = top;
            }
            else {
                boardOfTiles[i].numAdjTiles -= 1;
                boardOfTiles[i].numAdjMines += 1;
            }
        }
        if (boardOfTiles[i].topEdge == false && boardOfTiles[i].rightEdge == false) {
            int topRight = i - numColumns + 1;
            if (boardOfTiles[topRight].mine == false) {
                boardOfTiles[i].adjLocs[2] = topRight;
            }
            else {
                boardOfTiles[i].numAdjTiles -= 1;
                boardOfTiles[i].numAdjMines += 1;
            }
        }
        if (boardOfTiles[i].leftEdge == false) {
            int left = i - 1;
            if (boardOfTiles[left].mine == false) {
                boardOfTiles[i].adjLocs[3] = left;
            }
            else {
                boardOfTiles[i].numAdjTiles -= 1;
                boardOfTiles[i].numAdjMines += 1;
            }
        }
        if (boardOfTiles[i].rightEdge == false) {
            int right = i + 1;
            if (boardOfTiles[right].mine == false) {
                boardOfTiles[i].adjLocs[4] = right;
            }
            else {
                boardOfTiles[i].numAdjTiles -= 1;
                boardOfTiles[i].numAdjMines += 1;
            }
        }
        if (boardOfTiles[i].bottomEdge == false && boardOfTiles[i].leftEdge == false) {
            int bottomLeft = i + numColumns - 1;
            if (boardOfTiles[bottomLeft].mine == false) {
                boardOfTiles[i].adjLocs[5] = bottomLeft;
            }
            else {
                boardOfTiles[i].numAdjTiles -= 1;
                boardOfTiles[i].numAdjMines += 1;
            }
        }
        if (boardOfTiles[i].bottomEdge == false) {
            int bottom = i + numColumns;
            if (boardOfTiles[bottom].mine == false) {
                boardOfTiles[i].adjLocs[6] = bottom;
            }
            else {
                boardOfTiles[i].numAdjTiles -= 1;
                boardOfTiles[i].numAdjMines += 1;
            }
        }
        if (boardOfTiles[i].bottomEdge == false && boardOfTiles[i].rightEdge == false) {
            int bottomRight = i + numColumns + 1;
            if (boardOfTiles[bottomRight].mine == false) {
                boardOfTiles[i].adjLocs[7] = bottomRight;
            }
            else {
                boardOfTiles[i].numAdjTiles -= 1;
                boardOfTiles[i].numAdjMines += 1;
            }
        }
    }
}

void board::LoadTestBoard(string fileName) {

    this->loser = false;
    this->winner = false;
    this->happy = true;
    this->minesRemain = 0;
    this->tilesRemain = 0;

    // clear and initialize like regular constructor
    boardOfTiles.clear();
    initializeTiles();

    //read the file
    ifstream testBoard;
    testBoard.open(fileName);

    // check for that working
    if (!testBoard.is_open()) {
        cout << "test board not open :(" << endl;
    }
    cout << "test board open! :)" << endl;

    //read the file to find the mines
    unsigned char c;
    for (int i = 0; i < tileCount; i++) {
        testBoard >> c;
        if (c == '0') {
            boardOfTiles[i].mine = false;
            this-> tilesRemain += 1;
        }
        if (c == '1') {
            boardOfTiles[i].mine = true;
            this-> minesRemain += 1;
        }
    }
    // assigne the connected mines for recursion algorithm
    AssignAdjacent();
}

// this is where the magic happens
int main()
{
    // open the config file and gather initial info for setup
    ifstream openFile;
    openFile.open("boards/config.cfg");

    if (!openFile.is_open()) {
        cout << "not open :(" << endl;
    }
    cout << "config open! :)" << endl;

    string columns;
    string rows;
    string mines;

    getline(openFile, columns);
    getline(openFile, rows);
    getline(openFile, mines);

    int columnCount = stoi(columns);
    int rowCount = stoi(rows);
    int mineCount = stoi(mines);
    int windowSize = (columnCount * 32) * ((rowCount * 32) + 88);
    int tileCount = columnCount * rowCount;

    //create the board with everything we need
    board curBoard = board(rowCount, columnCount, tileCount, windowSize, mineCount);

    // "load a texture, create one or more sprites from that texture, and draw them to the screen"

    sf::RenderWindow window(sf::VideoMode((columnCount * 32), ((rowCount * 32) + 88)), "Minesweeper");

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i position = sf::Mouse::getPosition(window);

                    //if the happy face is pressed
                    if (curBoard.happyFace.getGlobalBounds().contains(position.x, position.y)) {
                        curBoard.reset(rowCount, columnCount, tileCount, windowSize, mineCount);
                        cout << "new board!" << endl;
                    }
                    // if the testbutton1 clicked
                     //if the happy face is pressed
                    if (curBoard.test1.getGlobalBounds().contains(position.x, position.y)) {
                        curBoard.LoadTestBoard("boards/testboard1.brd");
                        cout << "test board 1!" << endl;
                    }
                    // testbutton2 clicked
                    if (curBoard.test2.getGlobalBounds().contains(position.x, position.y)) {
                        curBoard.LoadTestBoard("boards/testboard2.brd");
                        cout << "test board 2!" << endl;
                    }
                    // testbutton 3 clicked
                    if (curBoard.test3.getGlobalBounds().contains(position.x, position.y)) {
                        curBoard.LoadTestBoard("boards/testboard3.brd");
                        cout << "test board 3!" << endl;
                    }
                    //if the board is in play mode basically
                    if (!curBoard.loser && !curBoard.winner) {

                        // if the debug button is pressed
                        if (curBoard.debug.getGlobalBounds().contains(position.x, position.y)) {
                            if (curBoard.debugMode == false) {
                                curBoard.debugMode = true;
                                cout << "debug mode" << endl;
                            }
                            else if (curBoard.debugMode == true) {
                                curBoard.debugMode = false;
                                cout << "debug mode off" << endl;
                            }
                        }
                        // if a hidden tile button is pressed, not a flag and not a mine
                        for (int i = 0; i < tileCount; i++) {
                            if (curBoard.boardOfTiles[i].tileHidden.getGlobalBounds().contains(position.x, position.y) && curBoard.boardOfTiles[i].mine == false && curBoard.boardOfTiles[i].flag == false) {
                                curBoard.RevealTiles(i);  
                                
                            }

                        }
                            // if a hidden tile is pressed, not a flag but is a mine
                            for (int i = 0; i < tileCount; i++) {
                                if (curBoard.boardOfTiles[i].tileHidden.getGlobalBounds().contains(position.x, position.y) && curBoard.boardOfTiles[i].mine == true && curBoard.boardOfTiles[i].flag == false) {
                                    curBoard.loser = true;
                                    for (int j = 0; j < tileCount; j++) {
                                        curBoard.boardOfTiles[j].flag = false;
                                        if (curBoard.boardOfTiles[j].mine == true) {
                                            curBoard.boardOfTiles[j].hidden = false;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // if instead a right click, controls the flags
                    if (event.mouseButton.button == sf::Mouse::Right && !curBoard.loser && !curBoard.winner) {
                        sf::Vector2i position = sf::Mouse::getPosition(window);
                        for (int i = 0; i < tileCount; i++) {
                            if (curBoard.boardOfTiles[i].tileHidden.getGlobalBounds().contains(position.x, position.y) && curBoard.boardOfTiles[i].hidden == true) {
                                if (curBoard.boardOfTiles[i].flag == true) {
                                    curBoard.boardOfTiles[i].flag = false;
                                    curBoard.minesRemain += 1;
                                }
                                else if (curBoard.boardOfTiles[i].flag == false) {
                                    curBoard.boardOfTiles[i].flag = true;
                                    curBoard.minesRemain -= 1;
                                }
                            }
                        }
                    }
                }
            }
        
            window.clear();

            // prints everything that needs to be printed onto the screen
            int k = 0;
            for (int i = 0; i < (rowCount * 32); i += 32) {
                for (int j = 0; j < columnCount; j++) {
                    curBoard.boardOfTiles[k].tileHidden.setPosition(32 * j, i);
                    for (int m = 0; m < 8; m++) {
                        curBoard.boardOfTiles[k].numbers[m].setPosition(32 * j, i);
                    }
                    if (curBoard.boardOfTiles[k].hidden == true) {
                        window.draw(curBoard.boardOfTiles[k].tileHidden);
                    }
                    curBoard.boardOfTiles[k].tileRevealed.setPosition(32 * j, i);
                    if (curBoard.boardOfTiles[k].hidden == false) {
                        window.draw(curBoard.boardOfTiles[k].tileRevealed);
                        if (curBoard.boardOfTiles[k].showNum == true && curBoard.boardOfTiles[k].numAdjMines != 0) {
                            window.draw(curBoard.boardOfTiles[k].numbers[(curBoard.boardOfTiles[k].numAdjMines) - 1]);
                        }
                    }
                    curBoard.boardOfTiles[k].flagSpr.setPosition(32 * j, i);
                    if (curBoard.boardOfTiles[k].flag == true || (curBoard.winner && curBoard.boardOfTiles[k].mine == true)) {
                        window.draw(curBoard.boardOfTiles[k].flagSpr);
                    }
                    curBoard.boardOfTiles[k].mineSpr.setPosition(32 * j, i);
                    if (curBoard.boardOfTiles[k].mine == true && (curBoard.debugMode == true || curBoard.loser == true)) {
                        window.draw(curBoard.boardOfTiles[k].mineSpr);
                    }
                    k += 1;
                }
            }

            if (curBoard.tilesRemain == 0) {
                curBoard.winner = true;
                curBoard.happy = false;
                curBoard.loser = false;
                curBoard.debugMode = false;
                curBoard.minesRemain = 0;
            }

           int h = curBoard.minesRemain / 100;
           int r = curBoard.minesRemain % 100;
           r = r / 10;
           int o = curBoard.minesRemain % 10;

           if (curBoard.minesRemain < 0) {
                curBoard.digits.setTextureRect(sf::IntRect(10 * 21, 0, 21, 32));
                curBoard.digits.setPosition(0, 32 * rowCount);
                window.draw(curBoard.digits);
                h = h * (-1);
                r = r * (-1);
                o = o * (-1);
            }

           //100s place
           for (int i = 0; i < 10; i++) {
               if (i == h) {
                   curBoard.digits.setTextureRect(sf::IntRect(i*21, 0, 21, 32));
                   curBoard.digits.setPosition(21, 32 * rowCount);
                   window.draw(curBoard.digits);
               }
           }
           //10s place
           for (int i = 0; i < 10; i++) {
               if (i == r) {
                   curBoard.digits.setTextureRect(sf::IntRect(i * 21, 0, 21, 32));
                   curBoard.digits.setPosition(42, 32 * rowCount);
                   window.draw(curBoard.digits);
               }
           }
           //1s place
           for (int i = 0; i < 10; i++) {
               if (i == o) {
                   curBoard.digits.setTextureRect(sf::IntRect(i * 21, 0, 21, 32));
                   curBoard.digits.setPosition(63, 32 * rowCount);
                   window.draw(curBoard.digits);
               }
           }

            if (curBoard.happy) {
                window.draw(curBoard.happyFace);
            }
            if (curBoard.loser) {
                window.draw(curBoard.loserFace);
            }
            
            if (curBoard.winner) {
                window.draw(curBoard.winnerFace);
            }
            window.draw(curBoard.debug);
            window.draw(curBoard.test1);
            window.draw(curBoard.test2);
            window.draw(curBoard.test3);

            window.display();
        }

        TextureManager::Clear();
        return 0;
    }

