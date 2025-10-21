#include "ConnectFour.h"
#include "Log.h"

ConnectFour::ConnectFour() : Game() {
    _grid = new Grid(7, 6);
    _redPieces = 0;
    _yellowPieces = 0;
    Log::initialize(WARNING,"log");
}

ConnectFour::~ConnectFour() {
    delete _grid;
}

Bit* ConnectFour::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit *bit = new Bit();
    // should possibly be cached from player class?
    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    return bit;
}

void ConnectFour::setUpBoard() {
    // connect 4 is 2 players with a 7x6 board
    setNumberOfPlayers(2);
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;

    // Initialize all squares
    _grid->initializeSquares(80, "boardsquare.png");

    // make sure game is not ended
    gameEnded = false;

    // enable AI
    Log::log(INFO, "Game has started!");
    startGame();
}

Bit* ConnectFour::createPiece(int pieceType) {
    Bit* bit = new Bit();
    bool isRed = (pieceType == RED_PIECE);
    bit->LoadTextureFromFile(isRed ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(isRed ? RED_PLAYER : YELLOW_PLAYER));
    bit->setGameTag(pieceType);
    return bit;
}

bool ConnectFour::actionForEmptyHolder(BitHolder &holder) {
    if(!gameEnded){
        // cast bitholder as chess square to use getcolumn()
        ChessSquare* selected = static_cast<ChessSquare*>(&holder);
        int selectedColumn = selected->getColumn();

        // find the lowest point in the row
        int dropToRow = -1;
        for(int i = _gameOptions.rowY- 1; i >= 0; i--){
            ChessSquare* target = _grid->getSquare(selectedColumn,i);
            if(target && !target->bit()){
                dropToRow = i;
                break;
            }
        }
        // create piece depending on whos turn it is
        Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber() == 0 ? HUMAN_PLAYER : AI_PLAYER);
        Log::log(INFO, "Bit Created!");
        // get the lowest empty holder and set all the good stuff
        ChessSquare* dropHolder = _grid->getSquare(selectedColumn,dropToRow);
        bit->setPosition(dropHolder->getPosition());
        dropHolder->setBit(bit);
        Log::log(INFO, "Bit Set!");

        // end turn methods
        endTurn();
        Player* winner = checkForWinner();
        if(winner){
            Log::log(INFO, "WINNER HAS BEEN FOUND!");
            gameEnded = true;
        }
        if(checkForDraw()){
            Log::log(INFO, "DRAW!");
            gameEnded = true;
        }
        return true;
    }
}
// winner logic
Player* ConnectFour::checkForWinner() {
    // directions on board 
    const int direction[4][2] = {
        {1,0},{0,1},{1,1},{-1,1} // horizontal, vertical, diagnol right, diagnol left
    };
    // loop through grid
    for(int y = 0; y < _gameOptions.rowY; y++){
        for(int x = 0; x < _gameOptions.rowX; x++){
            ChessSquare* square = _grid->getSquare(x,y);
            Bit* bit = square->bit();
            // check if null
            if(!bit) continue;
            Player* owner = bit->getOwner();
            if(!owner) continue;
            // check each direction
            for (auto& dir : direction){
                int dirX = dir[0];
                int dirY = dir[1];
                int count = 1;

                // check next 3 chess squares
                for(int step = 1; step < 4; step++){
                    int nextX = x + dirX * step;
                    int nextY = y + dirY * step;
                    if(nextX < 0 || nextX >= _gameOptions.rowX || nextY < 0 || nextY >= _gameOptions.rowY) break;
                    // get next square
                    ChessSquare* nextSquare = _grid->getSquare(nextX,nextY);
                    if(!nextSquare || !nextSquare->bit()) break;
                    if(nextSquare->bit()->getOwner() == owner){
                        count++;
                    } else{
                        break;
                    }
                    // if count is 4 return winner
                    if(count >= 4){
                        return owner;
                    }
                }
            }
        }
    }
    return nullptr;
}
// check for tie (rare)
bool ConnectFour::checkForDraw() {
    bool boardFull = true;
    _grid->forEachSquare([&boardFull](ChessSquare* square, int,int){
        if (!square->bit()){
            boardFull = false;
        }
    });
    return boardFull && !checkForWinner();
}
// stops the game
void ConnectFour::stopGame() {
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
    gameEnded = false;
    Log::log(INFO, "GAME STOPPED!");
}

std::string ConnectFour::initialStateString() {
    return "0000000000000000000";
}
// thanks othello
std::string ConnectFour::stateString() {
    std::string state;
    _grid->forEachSquare([&state, this](ChessSquare* square, int x, int y) {
        Bit* bit = square->bit();
        if (!bit) {
            state += '0';
        } else if (bit->getOwner() == getPlayerAt(AI_PLAYER)) {
            state += '1';
        } else {
            state += '2';
        }
    });
    return state;
}
void ConnectFour::setStateString(const std::string &s) {
    if (s.length() != 64) return;
    int index = 0;
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        if (index < s.length()) {
            char pieceType = s[index++];
        }
    });
}
// please actually update in github
// ai update
void ConnectFour::updateAI() {
    int bestVal = -1000;
    int bestColumn = -1;
    std::string state = stateString();
    // check for lowest row in the column
    for(int col = 0; col< 7; col++){
        int row = getLowestEmptyRow(state, col);
        if(row == -1) continue;
        // make move
        setCell(state, col, row, '1');
        int moveVal = -negamax(state, 0, -1000, 1000, -1);
        // undo the move
        setCell(state, col, row, '0');
        if(moveVal > bestVal){
            bestVal = moveVal;
            bestColumn = col;
        }
    }
    // make best move
    if(bestColumn != -1){
        ChessSquare* square = _grid->getSquare(bestColumn,0);
        actionForEmptyHolder(*square);
        Log::log(INFO, "AI MADE A MOVE");
        // temp don't forget to remove
        std::cout << "Column: " + square->getColumn() << std::endl;
    }
}
// negamax but with alpha beta prning
int ConnectFour::negamax(std::string& state, int depth, int alpha, int beta, int playerColor){
    // get score
    int score = evaluateAiBoard(state);
    // 
    if(depth >= 6 || abs(score) == 1000 || isAIBoardFull(state)){
        return playerColor * score;
    }
    // min valued
    int bestVal = -1000;
    for(int col = 0; col < 7; col++){
        int row = getLowestEmptyRow(state,col);
        if(row == -1) continue;
        // temporarly change state string
        setCell(state, col, row, (playerColor == 1 ? '1' : '2'));
        int val = -negamax(state, depth + 1, -beta, -alpha, -playerColor);
        // undo change to state
        setCell(state, col, row, '0');
        bestVal = std::max(bestVal, val);
        // alpha beta pruning
        alpha = std::max(alpha, val);
        if(alpha >= beta) break; 
    }
    return bestVal;
}
// helper to get lowest empty row for AIs
int ConnectFour::getLowestEmptyRow(const std::string& state, int col){
    for(int row = _gameOptions.rowY - 1; row >=0; row--){
        if(state[row * _gameOptions.rowX + col] == '0') return row;
    }
    return -1;
}
// helper function for setting the value in the state string
void ConnectFour::setCell(std::string& state, int x, int y, char value){
    state[y * _gameOptions.rowX + x] = value;
}
// evaluates the score for the board
int ConnectFour::evaluateAiBoard(const std::string& state){
    // don't want to spam this long thing all over
    int width = _gameOptions.rowX;
    int height = _gameOptions.rowY;

    // helper to get the cell from the state string
    auto getCell = [&](int x, int y) -> char{
        return state[y * width + x];
    };

    int score = 0;
    // generate scores based off win lines
    auto possibleWinLine = [&](char a, char b, char c, char d) {
        int aiCount = 0, humanCount = 0;
        char arr[4] = {a,b,c,d};
        for (char cell : arr) {
            if (cell == '1') aiCount++;
            else if (cell == '2') humanCount++;
        }

        // AI possible scores
        if (aiCount > 0 && humanCount == 0) {
            if (aiCount == 4) score += 1000;
            else if (aiCount == 3) score += 100;
            else if (aiCount == 2) score += 10;
        }
        // human possible scores (bad for AI hence the minus)
        if (humanCount > 0 && aiCount == 0) {
            if (humanCount == 4) score -= 1000;
            else if (humanCount == 3) score -= 100;
            else if (humanCount == 2) score -= 10;
        }
    };
    // check all 4 types of winning
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // horizontal win
            if (x + 3 < width){
                possibleWinLine(getCell(x,y), getCell(x+1,y), getCell(x+2,y), getCell(x+3,y));
            }
            // vertical win
            if (y + 3 < height) {
                possibleWinLine(getCell(x,y), getCell(x,y+1), getCell(x,y+2), getCell(x,y+3));
            }
            // diagnol
            if (x + 3 < width && y + 3 < height) {
                possibleWinLine(getCell(x,y), getCell(x+1,y+1), getCell(x+2,y+2), getCell(x+3,y+3));
            }
            // diagnol right (crashing definetly due to out of bounds)
            //if (x - 3 < width && y - 3 < height) {possibleWinLine(getCell(x,y), getCell(x-1,y+1), getCell(x-2,y+2), getCell(x-3,y+3));}
        }
    }
    return score;
}
// check if board is full (thanks othello)
bool ConnectFour::isAIBoardFull(const std::string& state){
    return state.find("0") == std::string::npos;
}
//unused
bool ConnectFour::canBitMoveFrom(Bit &, BitHolder &){return false;}
bool ConnectFour::canBitMoveFromTo(Bit &, BitHolder &, BitHolder &){return false;}
void ConnectFour::bitMovedFromTo(Bit &, BitHolder &, BitHolder &){}