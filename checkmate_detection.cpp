#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// Define the chess pieces
enum Piece {
    EMPTY = 0,
    PAWN = 1,
    BISHOP = 2,
    KNIGHT = 3,
    ROOK = 4,
    QUEEN = 5,
    KING = 6
};

enum Player {
    WHITE = 1,
    BLACK = -1
};

// Structure to represent a position on the board
struct Position {
    int x;
    int y;

    Position(int _x, int _y) : x(_x), y(_y) {}
};
// Function to check if a position is within the board
bool isValidPosition(int x, int y) {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}


bool isLegalMove(vector<vector<int>>& board, int startX, int startY, int endX, int endY) {
    int piece = board[startX][startY];
    int player = piece > 0 ? WHITE : BLACK;
    int directionX = (endX - startX) > 0 ? 1 : (endX - startX) == 0 ? 0 : -1;
    int directionY = (endY - startY) > 0 ? 1 : (endY - startY) == 0 ? 0 : -1;
    int pathX = startX + directionX;
    int pathY = startY + directionY;

    // Check for obstructions in the path
    while (pathX != endX || pathY != endY) {
        if (board[pathX][pathY] != EMPTY) {
            return false; // Obstruction detected
        }
        pathX += directionX;
        pathY += directionY;
    }

    // Check the final destination
    if (board[endX][endY] == EMPTY || board[endX][endY] * player < 0) {
        // For different pieces, ensure the move is legal
        switch (abs(piece)) {
            case ROOK:
                // Rooks move in straight lines
                return (startX == endX || startY == endY);
            case BISHOP:
                // Bishops move diagonally
                return abs(startX - endX) == abs(startY - endY);
            case QUEEN:
                // Queens move in straight lines or diagonally
                return (startX == endX || startY == endY) || (abs(startX - endX) == abs(startY - endY));
            case KNIGHT:
                // Knights move in L-shapes
                return (abs(startX - endX) == 2 && abs(startY - endY) == 1) || (abs(startX - endX) == 1 && abs(startY - endY) == 2);
            case KING:
                // Kings move one square in any direction
                return abs(startX - endX) <= 1 && abs(startY - endY) <= 1;
            default:
                return false;
        }
    }

    return false;
}


vector<pair<int, int>> getLegalMoves(vector<vector<int>>& board, int x, int y, Player player) {
    vector<pair<int, int>> moves;
    int piece = board[x][y];
    int maxDistance = (abs(piece) == KING || abs(piece) == KNIGHT) ? 1 : 7; // Knights and Kings have a max range of 1, others can go up to 7 squares.

    vector<pair<int, int>> directions;
    if (abs(piece) == ROOK || abs(piece) == QUEEN) {
        directions.push_back({1, 0}); // Down
        directions.push_back({-1, 0}); // Up
        directions.push_back({0, 1}); // Right
        directions.push_back({0, -1}); // Left
    }
    if (abs(piece) == BISHOP || abs(piece) == QUEEN) {
        directions.push_back({1, 1}); // Diagonal down-right
        directions.push_back({-1, -1}); // Diagonal up-left
        directions.push_back({1, -1}); // Diagonal down-left
        directions.push_back({-1, 1}); // Diagonal up-right
    }
    if (abs(piece) == KNIGHT) {
        directions = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
    }
    if (abs(piece) == KING) {
        directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
    }
    if (abs(piece) == PAWN) {
        int direction = (player == WHITE) ? -1 : 1; // For white, pawns move upwards (negative y direction), for black, downwards (positive y direction)
        int startY = (player == WHITE) ? 6 : 1; // Initial row for pawns based on player color

        // Check one step forward
        int newX = x + direction;
        int newY = y;
        if (isValidPosition(newX, newY) && board[newX][newY] == EMPTY) {
            moves.emplace_back(newX, newY);
        }

        // Check initial two steps forward if pawn is in its starting position
        if (x == startY && board[x + direction][y] == EMPTY) {
            newX = x + (2 * direction);
            newY = y;
            if (isValidPosition(newX, newY) && board[newX][newY] == EMPTY) {
                moves.emplace_back(newX, newY);
            }
        }

        // Check diagonal captures
        for (int dx = -1; dx <= 1; dx += 2) {
            newX = x + direction;
            newY = y + dx;
            if (isValidPosition(newX, newY) && board[newX][newY] * player < 0) {
                moves.emplace_back(newX, newY);
            }
        }
    }

    for (const auto& [dx, dy] : directions) {
        for (int i = 1; i <= maxDistance; ++i) {
            int newX = x + i * dx;
            int newY = y + i * dy;
            if (newX < 0 || newX >= 8 || newY < 0 || newY >= 8) break; // Stay within board limits
            if (isLegalMove(board, x, y, newX, newY)) {
                moves.emplace_back(newX, newY);
                if (board[newX][newY] != EMPTY) break; // Stop if it hits another piece
            } else {
                break; // Not a valid move, break the loop
            }
        }
    }

    return moves;
}


// Define the player colors



bool isKingInCheckByRook(vector<vector<int>>& board, int kingX, int kingY, Player player) {
    int opponent = -player;

    // Define directions: up, down, left, right
    vector<Position> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    // Perform backtracking for each direction
    for (const auto& dir : directions) {
        int dx = dir.x;
        int dy = dir.y;
        int newX = kingX + dx;
        int newY = kingY + dy;

        // Start backtracking from the current position
        while (isValidPosition(newX, newY) && board[newX][newY] == EMPTY) {
            newX += dx;
            newY += dy;
        }

        // Check if we found a rook or queen along the path
        if (isValidPosition(newX, newY) && (board[newX][newY] == opponent * ROOK || board[newX][newY] == opponent * QUEEN)) {
            return true; // King is in check by a rook or queen
        }
    }

    // King is not in check by a rook
    return false;
}


// Function to check if the king is in check by a bishop
bool isKingInCheckByBishop(vector<vector<int>>& board, int kingX, int kingY, Player player) {
    int opponent = -player;

    // Check for attacks from bishops along diagonals
    vector<Position> directions = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
    for (auto dir : directions) {
        for (int i = 1; i < 8; ++i) {
            int newX = kingX + i * dir.x;
            int newY = kingY + i * dir.y;
            if (!isValidPosition(newX, newY) || board[newX][newY] != EMPTY) {
                if (isValidPosition(newX, newY) && (board[newX][newY] == opponent * BISHOP || board[newX][newY] == opponent * QUEEN)) {
                    return true; // King is in check by a bishop or queen
                } else {
                    break; // No bishop or queen found along this direction
                }
            }
        }
    }

    return false; // King is not in check by a bishop
}
bool isKingInCheckFromPawn(vector<vector<int>>& board, int kingX, int kingY, Player player) {
    // For white player, black pawns attack from above (i.e., their 'x' coordinate increases)
    // For black player, white pawns attack from below (i.e., their 'x' coordinate decreases)
    int opponentPawnDirection = (player == WHITE) ? 1 : -1; // Corrected direction based on player
    int opponentPawn = (player == WHITE) ? -PAWN : PAWN; // Pawn of the opponent

    // Check diagonal squares for opponent pawns
    int attackX = kingX + opponentPawnDirection;
    int attackYLeft = kingY - 1;
    int attackYRight = kingY + 1;

    if ((isValidPosition(attackX, attackYLeft) && board[attackX][attackYLeft] == opponentPawn) ||
        (isValidPosition(attackX, attackYRight) && board[attackX][attackYRight] == opponentPawn)) {
        return true;
    }

    return false;
}


// Function to check if the king is in check by a knight
bool isKingInCheckByKnight(vector<vector<int>>& board, int kingX, int kingY, Player player) {
    int opponent = -player;

    // Check for attacks from knights
    vector<Position> knightMoves = { {2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2} };
    for (auto move : knightMoves) {
        int newX = kingX + move.x;
        int newY = kingY + move.y;
        if (isValidPosition(newX, newY) && board[newX][newY] == opponent * KNIGHT) {
            return true; // King is in check by knight
        }
    }

    return false; // King is not in check by a knight
}


bool isKingInCheckFromQueen(vector<vector<int>>& board, int kingX, int kingY, Player player) {
    int opponent = -player;

    // Define directions: up-right, up-left, down-right, down-left, up, down, right, left
    vector<Position> directions = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    // Perform check along each direction
    for (const auto& dir : directions) {
        int dx = dir.x;
        int dy = dir.y;
        int newX = kingX + dx;
        int newY = kingY + dy;

        // Keep moving along the direction until reaching the end of the board or encountering a piece
        while (isValidPosition(newX, newY)) {
            if (board[newX][newY] != EMPTY) {
                // Check if the piece is a queen belonging to the opponent
                if (board[newX][newY] == opponent * QUEEN) {
                    return true; // King is in check by a queen
                } else {
                    break; // Stop checking along this direction if another piece is encountered
                }
            }
            newX += dx;
            newY += dy;
        }
    }

    return false; // King is not in check by a queen
}


bool isKingInCheck(vector<vector<int>>& board, int kingX, int kingY, Player player){
    if(isKingInCheckFromQueen(board, kingX, kingY, player)){
        return true;
    }
    else if(isKingInCheckByRook(board, kingX,kingY,player)){
        return true;
    }
    else if(isKingInCheckByBishop(board, kingX,kingY, player)){
        return true;
    }
    else if(isKingInCheckByKnight(board, kingX,kingY,player)){
        return true;
    }
    else if(isKingInCheckFromPawn(board,kingX,kingY,player)){
        return true;
    }
    else{
        return false;
    }
}
bool canBlockCheck(vector<vector<int>>& board, Player player, int kingX, int kingY) {
    // Iterate over all pieces of the player
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            // Check if the piece belongs to the player
            if (board[x][y] * player > 0) {
                // Get all legal moves for this piece
                vector<pair<int, int>> legalMoves = getLegalMoves(board, x, y, player);

                // Try every legal move
                for (auto [newX, newY] : legalMoves) {
                    // Make the move
                    int originalPiece = board[x][y];
                    int capturedPiece = board[newX][newY];
                    board[newX][newY] = board[x][y];
                    board[x][y] = EMPTY;

                    // Check if the move blocks the check
                    if (!isKingInCheck(board, kingX, kingY, player)) {
                        // Undo the move
                        board[x][y] = originalPiece;
                        board[newX][newY] = capturedPiece;
                        cout<<player<<endl;
                        return true; // Found a move that blocks the check
                    }

                    // Undo the move
                    board[x][y] = originalPiece;
                    board[newX][newY] = capturedPiece;
                }
            }
        }
    }

    return false; // No piece can block the check
}




bool noLegalMovesAvailable(vector<vector<int>>& board, Player player, int kingX, int kingY) {
    int opponent = player == WHITE ? BLACK : WHITE;

    // Check if the king has any legal moves available
     for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx != 0 || dy != 0) {
                int newX = kingX + dx;
                int newY = kingY + dy;
                if (isValidPosition(newX, newY) && board[newX][newY] == EMPTY && !isKingInCheck(board, newX, newY, player)) {
                    return false; // Legal move available for the king
                }
            }
        }
     }

    // Check if any piece can block or capture attacking pieces
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (board[i][j] * player > 0) { // Piece belongs to the player
                // Check all pieces for potential to capture or block attacks
                if (isKingInCheckFromQueen(board, kingX, kingY, player) ||
                    isKingInCheckByBishop(board, kingX, kingY, player) ||
                    isKingInCheckByRook(board, kingX, kingY, player) ||
                    isKingInCheckByKnight(board, kingX, kingY, player)||
                    isKingInCheckFromPawn(board,kingX,kingY,player)) {
                    if (isLegalMove(board, i, j, kingX, kingY)) {
                        return false; // There's a legal move to capture/block the attacking piece
                    }
                }
            }
        }
    }

    return true; // No legal moves available
}



// Function to check if it's a checkmate
bool isCheckmate(vector<vector<int>>& board, Player player) {
    // Find the king's position
    int kingX = -1, kingY = -1;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (board[i][j] == (player == WHITE ? KING : -KING)) {
                kingX = i;
                kingY = j;
                break;
            }
        }
        if (kingX != -1) break;
    }

    // Check if the king is in check'
    if (isKingInCheck(board, kingX, kingY, player)) {
        // Check if there are any legal moves available
        cout<<"King in Check"<<endl;
        if (noLegalMovesAvailable(board, player,kingX,kingY)) {
            cout<<"Checking Legal Moves.."<<endl;
            if(!canBlockCheck(board,player,kingX,kingY)){
                return true;   
            }
            else{
                cout<<"Blocked by a piece"<<endl;
            }
        }
        else{
            cout<<"King can move"<<endl;
        }
    }
    return false; // Not a checkmate
}

int main() {
    // Initialize the board
    vector<vector<int>> board = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, -4},
        {6, 0, 0, 0, 0, 0, 0, -5},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {-1, -1, -1, -1, -1, -1, -1, -1},
        {-4, -3, -2, -5, -6, -2, -3, -4}
    };

    // Check if it's a checkmate for White
    if (isCheckmate(board, WHITE)) {
        cout << "Checkmate for White!" << endl;
    } else {
        cout << "Not a checkmate for White." << endl;
    }

    // // Check if it's a checkmate for Black
    // if (isCheckmate(board, BLACK)) {
    //     cout << "Checkmate for Black!" << endl;
    // } else {
    //     cout << "Not a checkmate for Black." << endl;
    // }

    return 0;
}