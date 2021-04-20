#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#define PIECE_SIZE 56
#define TOTAL_PIECES 24

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
int size = 56;

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

bool InvalidMove(sf::Vector2f from, sf::Vector2f to) {
  if (to.x/PIECE_SIZE >= 8 || to.x/PIECE_SIZE < 0 ||
      to.y/PIECE_SIZE >= 8 || to.y/PIECE_SIZE < 0)
    return true;

  for (int i = 0; i < TOTAL_PIECES; ++i) {
    if (pieces[i].getPosition() == to)
      return true;
  }

  return false;
}

bool MovePiece(sf::Vector2f from, sf::Vector2f to) {
  if (InvalidMove(from, to))
    return false;

  return true;
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

      if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
          movingPiece = false;
          sf::Vector2f p = pieces[currentPiece].getPosition() +
                           sf::Vector2f(PIECE_SIZE/2, PIECE_SIZE/2);
          newPiecePosition = sf::Vector2f(PIECE_SIZE * int(p.x/PIECE_SIZE),
                                          PIECE_SIZE * int(p.y/PIECE_SIZE));

          if (!MovePiece(oldPiecePosition, newPiecePosition))
            pieces[currentPiece].setPosition(oldPiecePosition);
          else
            pieces[currentPiece].setPosition(newPiecePosition);
        }
      }
    }

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
