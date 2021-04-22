#include <iostream>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#define PIECE_SIZE 56
#define TOTAL_PIECES 24
#define MAX_DEPTH 5
#define INF 12345

enum SquareOccupation {
  BLACK_PIECE,
  EMPTY_SQUARE,
  WHITE_PIECE
};

short int board[8][8] = {
  {0, -1, 0, -1, 0, -1, 0, -1},
  {-1, 0, -1, 0, -1, 0, -1, 0},
  {0, -1, 0, -1, 0, -1, 0, -1},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {1, 0, 1, 0, 1, 0, 1, 0},
  {0, 1, 0, 1, 0, 1, 0, 1},
  {1, 0, 1, 0, 1, 0, 1, 0}
};

sf::Sprite pieces[TOTAL_PIECES];
sf::Vector2f frameOffset(28, 28);
bool turn = 0;

std::pair<short int, short int> black[2] = {{1, -1}, {1, 1}};
std::pair<short int, short int> white[2] = {{-1, -1}, {-1, 1}};

void copy(short int t1[8][8], short int t2[8][8]) {
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
      t1[i][j] = t2[i][j];
}

short int func(short int table[8][8]) {
  short int ans = 0;
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
      ans += -table[i][j];
  return ans;
}

short int alphabeta(short int depth, short int current_board[8][8], short int best_board[8][8],
                   short int alpha, short int beta) {
  if (depth == MAX_DEPTH)
    return func(current_board);
  // recursion
  short int best = depth & 1 ? INF : -INF;
  short int player = depth & 1 ? 1 : -1;
  std::pair<short int, short int> *options = depth & 1 ? white : black;
   // visit children
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      if (current_board[i][j] == player) {
        for (int k = 0; k < 2; ++k) {
          short int a = i + options[k].first;
          short int b = i + options[k].second;
          if (a >= 0 && a < 8 && b >= 0 && b < 8 && current_board[a][b] == 0) {
            // simple move
            current_board[i][j] = 0;
            current_board[a][b] = player;
            short int score = alphabeta(depth + 1, current_board, best_board, alpha, beta);
            if (depth & 1) {
              // minimize
              if (score < best) {
                best = score;
                copy(best_board, current_board);
              }
              beta = std::min(beta, best);
              if (alpha >= beta)
                return best;
            }
            else {
              // maximize
              if (score > best) {
                best = score;
                copy(best_board, current_board);
              }
              alpha = std::max(alpha, best);
              if (alpha >= beta)
                return best;
            }
            current_board[i][j] = player;
            current_board[a][b] = 0;
          }
          a += options[k].first;
          b += options[k].second;
          if (a >= 0 && a < 8 && b >= 0 && b < 8 && current_board[a][b] == 0 &&
              current_board[a-options[k].first][b-options[k].second] == -player) {
            // eat a piece
            current_board[i][j] = 0;
            current_board[a-options[k].first][b-options[k].second] = 0;
            current_board[a][b] = player;
            short int score = alphabeta(depth + 1, current_board, best_board, alpha, beta);
            if (depth & 1) {
              // minimize
              if (score < best) {
                best = score;
                copy(best_board, current_board);
              }
              beta = std::min(beta, best);
              if (alpha >= beta)
                return best;
            }
            else {
              // maximize
              if (score > best) {
                best = score;
                copy(best_board, current_board);
              }
              alpha = std::max(alpha, best);
              if (alpha >= beta)
                return best;
            }
            current_board[i][j] = player;
            current_board[a-options[k].first][b-options[k].second] = -player;
            current_board[a][b] = 0;
          }
        }
      }
    }
  }
  return best;
}

short int minmax(short int depth, short int current_board[8][8], short int best_board[8][8]) {
  if (depth == MAX_DEPTH)
    return func(current_board);
  // recursion
  short int best = depth & 1 ? INF : -INF;
  short int player = depth & 1 ? 1 : -1;
  std::pair<short int, short int> *options = depth & 1 ? white : black;
  // visit children
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      if (current_board[i][j] == player) {
        for (int k = 0; k < 2; ++k) {
          short int a = i + options[k].first;
          short int b = i + options[k].second;
          if (a >= 0 && a < 8 && b >= 0 && b < 8 && current_board[a][b] == 0) {
            // simple move
            current_board[i][j] = 0;
            current_board[a][b] = player;
            short int score = minmax(depth + 1, current_board, best_board);
            if (depth & 1 && score < best) {
              best = score;
              copy(best_board, current_board);
            }
            if (!(depth & 1) && score > best) {
              best = score;
              copy(best_board, current_board);
            }
            current_board[i][j] = player;
            current_board[a][b] = 0;
          }
          a += options[k].first;
          b += options[k].second;
          if (a >= 0 && a < 8 && b >= 0 && b < 8 && current_board[a][b] == 0 &&
              current_board[a-options[k].first][b-options[k].second] == -player) {
            // eat a piece
            current_board[i][j] = 0;
            current_board[a-options[k].first][b-options[k].second] = 0;
            current_board[a][b] = player;
            short int score = minmax(depth + 1, current_board, best_board);
            if (depth & 1 && score < best) {
              best = score;
              copy(best_board, current_board);
            }
            if (!(depth & 1) && score > best) {
              best = score;
              copy(best_board, current_board);
            }
            current_board[i][j] = player;
            current_board[a-options[k].first][b-options[k].second] = -player;
            current_board[a][b] = 0;
          }
        }
      }
    }
  }
  return best;
}

void LoadPosition() {
  int k = 0;
  int square, y;

  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      square = board[i][j] + 1;

      if (square == EMPTY_SQUARE)
        continue;

      y = (square > 0) ? 1 : 0;
      pieces[k].setTextureRect( sf::IntRect(0, PIECE_SIZE*y,
                                            PIECE_SIZE, PIECE_SIZE) );
      pieces[k].setPosition(PIECE_SIZE * j, PIECE_SIZE * i);
      ++k;
    }
  }
}

bool InvalidMove(sf::Vector2f to) {
  if (to.x/PIECE_SIZE >= 8 || to.x/PIECE_SIZE < 0 ||
      to.y/PIECE_SIZE >= 8 || to.y/PIECE_SIZE < 0)
    return true;

  for (int i = 0; i < TOTAL_PIECES; ++i) {
    if (pieces[i].getPosition() == to)
      return true;
  }

  return false;
}

enum Direction {
  Right,
  Left,
  JumpRight,
  JumpLeft,
  Invalid
};

Direction GetDirection(sf::Vector2f from, sf::Vector2f to) {
  auto movementVector = sf::Vector2i(int(to.x/PIECE_SIZE) - int(
                                       from.x/PIECE_SIZE),
                                     int(to.y/PIECE_SIZE) - int(from.y/PIECE_SIZE));

  std::cout << "movementVector: " << movementVector.x << " " << movementVector.y
            << std::endl;

  if (turn == 0) {
    if (movementVector.x == 1 && movementVector.y == -1)
      return Right;

    if (movementVector.x == 2 && movementVector.y == -2)
      return JumpRight;

    if (movementVector.x == -1 && movementVector.y == -1)
      return Left;

    if (movementVector.x == -2 && movementVector.y == -2)
      return JumpLeft;
  }
  else {
    if (movementVector.x == 1 && movementVector.y == 1)
      return Right;

    if (movementVector.x == 2 && movementVector.y == 2)
      return JumpRight;

    if (movementVector.x == -1 && movementVector.y == 1)
      return Left;

    if (movementVector.x == -2 && movementVector.y == 2)
      return JumpLeft;
  }

  return Invalid;
}

bool MovePiece(sf::Vector2f from, sf::Vector2f to) {
  if (InvalidMove(to))
    return false;

  Direction direction = GetDirection(from, to);

  if (direction == Invalid)
    return false;

  if (direction == JumpLeft || direction == JumpRight) {
    auto middleSquare = (from + to) / 2.f;
    bool capturedEnemy = false;

    if (turn == 0) {
      for (int i = 0; i < TOTAL_PIECES/2; ++i) {
        if (pieces[i].getPosition() == middleSquare) {
          pieces[i].setPosition(-100, 0);
          capturedEnemy = true;
        }
      }
    }
    else {
      for (int i = TOTAL_PIECES/2; i < TOTAL_PIECES; ++i) {
        if (pieces[i].getPosition() == middleSquare) {
          pieces[i].setPosition(-100, 0);
          capturedEnemy = true;
        }
      }
    }

    if (!capturedEnemy)
      return false;
  }

  return true;
}

void calculate_movement() {
  short int new_board[8][8];
  short int result = minmax(0, board, new_board);
  //short int result = alphabeta(0, board, new_board, -INF, INF);
  if (result == -INF)
    std::cout << "GG" << std::endl;
  else
    copy(board, new_board);
}

int main() {
  sf::RenderWindow window(sf::VideoMode(504, 504), "Damas");

  sf::Texture boardTexture, piecesTexture;
  boardTexture.loadFromFile("../images/board.png");
  piecesTexture.loadFromFile("../images/pieces.png");

  for (int i = 0; i < TOTAL_PIECES; ++i)
    pieces[i].setTexture(piecesTexture);

  sf::Sprite boardSprite(boardTexture);

  LoadPosition();

  bool movingPiece = false;
  float deltaX, deltaY;
  sf::Vector2f oldPiecePosition, newPiecePosition;
  int currentPiece = 0;

  while (window.isOpen()) {
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window) - sf::Vector2i(
                                   frameOffset);

    sf::Event event;

    if (turn) {
      calculate_movement();
      turn ^= 1;
    }

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
          for (int i = 0; i < TOTAL_PIECES; ++i) {
            if (pieces[i].getGlobalBounds().contains(mousePosition.x, mousePosition.y)) {
              movingPiece = true;
              currentPiece = i;
              deltaX = mousePosition.x - pieces[i].getPosition().x;
              deltaY = mousePosition.y - pieces[i].getPosition().y;
              oldPiecePosition = pieces[i].getPosition();
            }
          }
        }
      }

      if (event.type == sf::Event::MouseButtonReleased && movingPiece) {
        if (event.mouseButton.button == sf::Mouse::Left) {
          movingPiece = false;
          sf::Vector2f p = pieces[currentPiece].getPosition() +
                           sf::Vector2f(PIECE_SIZE/2, PIECE_SIZE/2);
          newPiecePosition = sf::Vector2f(PIECE_SIZE * int(p.x/PIECE_SIZE),
                                          PIECE_SIZE * int(p.y/PIECE_SIZE));

          if (!MovePiece(oldPiecePosition, newPiecePosition))
            pieces[currentPiece].setPosition(oldPiecePosition);
          else {
            pieces[currentPiece].setPosition(newPiecePosition);
            turn ^= 1;
            // Actualizar board :v
          }
        }
      }
    }

    // Cargar graficos del board

    if (movingPiece)
      pieces[currentPiece].setPosition(mousePosition.x - deltaX,
                                       mousePosition.y - deltaY);

    window.clear();
    window.draw(boardSprite);

    for (int i = 0; i < TOTAL_PIECES; ++i) {
      pieces[i].move(frameOffset);
      window.draw(pieces[i]);
      pieces[i].move(-frameOffset);
    }

    window.display();
  }

  return 0;
}
