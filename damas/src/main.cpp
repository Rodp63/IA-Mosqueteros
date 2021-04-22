#include <iostream>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#define PIECE_SIZE 56
#define TOTAL_PIECES 24
#define INF 12345
#define EMPTY_SQUARE 0

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
bool playerColourChoice;
bool searchAlgorithm;
short int playerPiece = 1;
short int aiPiece = -1;
short int maxDepth;

std::pair<short int, short int> aiOpts[2] = {{1, -1}, {1, 1}};
std::pair<short int, short int> playerOpts[2] = {{-1, -1}, {-1, 1}};

void Copy(short int t1[8][8], short int t2[8][8]) {
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
      t1[i][j] = t2[i][j];
}

short int ValueFunction(short int table[8][8]) {
  short int ans = 0;

  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
      ans += -table[i][j];

  return ans;
}

struct Node {
  short int best_board[8][8];
  short int current_board[8][8];
  short int depth;
  short int alpha, beta;
  bool minimize;

  Node(short int c[8][8], short  int d, short int a = -INF,
       short int b = INF) : depth(d), alpha(a), beta(b) {
    Copy(current_board, c);
    minimize = depth & 1;
  }

  short int AlphaBeta() {
    if (depth == maxDepth)
      return ValueFunction(current_board);

    // recursion
    short int best = minimize ? INF : -INF;
    short int player = minimize ? 1 : -1;
    std::pair<short int, short int>* options = minimize ? playerOpts : aiOpts;

    // visit children
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        if (current_board[i][j] == player) {
          for (int k = 0; k < 2; ++k) {
            short int a = i + options[k].first;
            short int b = j + options[k].second;

            if (a >= 0 && a < 8 && b >= 0 && b < 8 && current_board[a][b] == 0) {
              // simple move
              current_board[i][j] = 0;
              current_board[a][b] = player;
              Node child(current_board, depth + 1, alpha, beta);
              short int score = child.AlphaBeta();

              if (minimize) {
                if (score < best) {
                  best = score;
                  Copy(best_board, current_board);
                }

                beta = std::min(beta, best);

                if (alpha >= beta)
                  return best;
              }
              else {
                if (score > best) {
                  best = score;
                  Copy(best_board, current_board);
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
              Node child(current_board, depth + 1, alpha, beta);
              short int score = child.AlphaBeta();

              if (minimize) {
                if (score < best) {
                  best = score;
                  Copy(best_board, current_board);
                }

                beta = std::min(beta, best);

                if (alpha >= beta)
                  return best;
              }
              else {
                if (score > best) {
                  best = score;
                  Copy(best_board, current_board);
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

  short int MinMax() {
    if (depth == maxDepth)
      return ValueFunction(current_board);

    // recursion
    short int best = minimize ? INF : -INF;
    short int player = minimize ? 1 : -1;
    std::pair<short int, short int>* options = minimize ? playerOpts : aiOpts;

    // visit children
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        if (current_board[i][j] == player) {
          for (int k = 0; k < 2; ++k) {
            short int a = i + options[k].first;
            short int b = j + options[k].second;

            if (a >= 0 && a < 8 && b >= 0 && b < 8 && current_board[a][b] == 0) {
              // simple move
              current_board[i][j] = 0;
              current_board[a][b] = player;
              Node child(current_board, depth + 1);
              short int score = child.MinMax();

              if (minimize && score < best) {
                best = score;
                Copy(best_board, current_board);
              }

              if (!minimize && score > best) {
                best = score;
                Copy(best_board, current_board);
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
              Node child(current_board, depth + 1);
              short int score = child.MinMax();

              if (minimize && score < best) {
                best = score;
                Copy(best_board, current_board);
              }

              if (!minimize && score > best) {
                best = score;
                Copy(best_board, current_board);
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
};

void LoadPosition() {
  int k = 0;
  int square, y;

  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      square = board[i][j];

      if (square == EMPTY_SQUARE)
        continue;

      if (playerColourChoice == 0)
        y = (square > 0) ? 1 : 0;
      else
        y = (square > 0) ? 0 : 1;

      pieces[k].setTextureRect( sf::IntRect(0, PIECE_SIZE*y,
                                            PIECE_SIZE, PIECE_SIZE) );
      pieces[k].setPosition(PIECE_SIZE * j, PIECE_SIZE * i);
      ++k;
    }
  }

  for (; k < TOTAL_PIECES; ++k)
    pieces[k].setPosition(-100, 0);
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

  if (movementVector.x == 1 && movementVector.y == -1)
    return Right;

  if (movementVector.x == 2 && movementVector.y == -2)
    return JumpRight;

  if (movementVector.x == -1 && movementVector.y == -1)
    return Left;

  if (movementVector.x == -2 && movementVector.y == -2)
    return JumpLeft;

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

    for (int i = 0; i < TOTAL_PIECES; ++i) {
      const sf::Vector2f& piecePosition = pieces[i].getPosition();
      short int& square = board[int(piecePosition.y/PIECE_SIZE)][int(
                            piecePosition.x/PIECE_SIZE)];

      if (piecePosition == middleSquare
          && square == aiPiece) {
        square = 0;
        pieces[i].setPosition(-100, 0);
        capturedEnemy = true;
      }
    }

    if (!capturedEnemy)
      return false;
  }

  return true;
}

void DeclareResults() {
  short int result = ValueFunction(board);

  if (result < 0)
    std::cout << "PLAYER IS THE WINNER" << std::endl;
  else if (result == 0)
    std::cout << "GAME TIED. NOBODY WINS" << std::endl;
  else
    std::cout << "AI IS THE WINNER" << std::endl;
}

void CalculateMovement() {
  Node root(board, 0);
  short int result;

  if (searchAlgorithm == 0)
    result = root.MinMax();
  else
    result = root.AlphaBeta();

  if (result == -INF)
    DeclareResults();
  else
    Copy(board, root.best_board);
}

void PrintBoard() {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j)
      std::cout << board[i][j] << "\t";

    std::cout << std::endl;
  }

  std::cout << std::endl;
}

bool PlayerHasOptions() {
  std::pair<short int, short int>* options = playerOpts;

  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      if (board[i][j] == playerPiece) {
        for (int k = 0; k < 2; ++k) {
          short int a = i + options[k].first;
          short int b = j + options[k].second;

          if (a >= 0 && a < 8 && b >= 0 && b < 8 && board[a][b] == 0)
            return true;

          a += options[k].first;
          b += options[k].second;

          if (a >= 0 && a < 8 && b >= 0 && b < 8 && board[a][b] == 0 &&
              board[a-options[k].first][b-options[k].second] == aiPiece)
            return true;
        }
      }
    }
  }

  return false;
}

int main() {
  std::cout << "Do you want begin move first? (0 = no, 1 = yes) ";
  std::cin >> playerColourChoice;
  bool turn = playerColourChoice ? 0 : 1;

  std::cout << "Use MinMax or AlphaBeta? (0 = MinMax, 1 = AlphaBeta) ";
  std::cin >> searchAlgorithm;

  std::cout << "Algorithm depth? (0 - 12) ";
  std::cin >> maxDepth;

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
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window) -
                                 sf::Vector2i(frameOffset);

    sf::Event event;

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
          for (int i = 0; i < TOTAL_PIECES; ++i) {
            const sf::Vector2f& piecePosition = pieces[i].getPosition();

            if (pieces[i].getGlobalBounds().contains(mousePosition.x, mousePosition.y)
                && board[int(piecePosition.y/PIECE_SIZE)][int(piecePosition.x/PIECE_SIZE)] ==
                playerPiece) {
              movingPiece = true;
              currentPiece = i;
              deltaX = mousePosition.x - piecePosition.x;
              deltaY = mousePosition.y - piecePosition.y;
              oldPiecePosition = piecePosition;
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

            board[int(oldPiecePosition.y/PIECE_SIZE)][int(oldPiecePosition.x/PIECE_SIZE)] =
              0;
            board[int(p.y/PIECE_SIZE)][int(p.x/PIECE_SIZE)] = playerPiece;

            turn ^= 1;

#ifndef NDEBUG
            PrintBoard();
#endif
          }
        }
      }
    }

    if (movingPiece)
      pieces[currentPiece].setPosition(mousePosition.x - deltaX,
                                       mousePosition.y - deltaY);

    if (turn) {
      CalculateMovement();
      LoadPosition();
      turn ^= 1;

#ifndef NDEBUG
      PrintBoard();
#endif
    }

    if (!PlayerHasOptions())
      DeclareResults();

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
