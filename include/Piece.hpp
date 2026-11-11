#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
using namespace std;
class Board;
#include "Board.hpp"


class Piece
{
    public:
        enum class Type
        {
            I, J, L, O, S, T, Z
        };

        vector<vector<int>> shape;
        int x, y;
        sf::Color color;


        Piece(Type type)
        {
            x = 3;
            y = 0;

            switch(type)
            {
                case Type::I:
                    shape = {{1, 1, 1, 1},
                             {0, 0, 0, 0},
                             {0, 0, 0, 0},
                             {0, 0, 0, 0}};
                    color = sf::Color::Cyan;
                    break;

                case Type::J:
                    shape = {{1, 0, 0},
                             {1, 1, 1},
                             {0, 0, 0}};
                    color = sf::Color::Blue;
                    break;

                case Type::L:
                    shape = {{0, 0, 1},
                             {1, 1, 1},
                             {0, 0, 0}};
                    color = sf::Color(255, 165, 0);
                    break;

                case Type::O:
                    shape = {{1, 1, 0},
                             {1, 1, 0},
                             {0, 0, 0}};
                    color = sf::Color::Yellow;
                    break;

                case Type::S:
                    shape = {{0, 1, 1},
                             {1, 1, 0},
                             {0, 0, 0}};
                    color = sf::Color::Green;
                    break;

                case Type::T:
                    shape = {{0, 1, 0},
                             {1, 1, 1},
                             {0, 0, 0}};

                    color = sf::Color::Magenta;
                    break;

                case Type::Z:
                    shape = {{1, 1, 0},
                             {0, 1, 1},
                             {0, 0, 0}};

                    color = sf::Color::Red;
                    break;
            }
        }

        void moveLeft(const Board& board)
        {
            for (size_t i = 0; i < shape.size(); ++i)
            {
                for (size_t j = 0; j < shape[i].size(); ++j)
                {
                    if (shape[i][j] == 1)
                    {
                        int nextX = x + j - 1;
                        int nextY = y + i;

                        if (nextX < 0 || board.grid[nextY][nextX].occupied)
                            return; 
                    }
                }
            }

            x--;
        }

        void moveRight(const Board& board)
        {
            for (size_t i = 0; i < shape.size(); ++i)
            {
                for (size_t j = 0; j < shape[i].size(); ++j)
                {
                    if (shape[i][j] == 1)
                    {
                        int nextX = x + j + 1;
                        int nextY = y + i;

                        if (nextX >= board.width || board.grid[nextY][nextX].occupied)
                            return;
                    }
                }
            }

            x++;
        }

        bool moveDown(const Board& board)
        {
            for (size_t i = 0; i < shape.size(); ++i)
            {
                for (size_t j = 0; j < shape[i].size(); ++j)
                {
                    if (shape[i][j] == 1)
                    {
                        int nextX = x + j;
                        int nextY = y + i + 1;

                        if (nextY >= board.height || board.grid[nextY][nextX].occupied)
                            return false;
                    }
                }
            }

            y++;
            return true;
        }

        void rotate(const Board &board)
        {
            vector<vector<int>> rotatedShape(shape[0].size(), vector<int>(shape.size()));

            for (size_t i = 0; i < shape.size(); ++i)
            {
                for (size_t j = 0; j < shape[i].size(); ++j)
                    rotatedShape[j][shape.size() - 1 - i] = shape[i][j];
            }
        
            auto isValidRotation = [&](int testX) {
                for (size_t i = 0; i < rotatedShape.size(); ++i)
                {
                    for (size_t j = 0; j < rotatedShape[i].size(); ++j)
                    {
                        if (rotatedShape[i][j] == 1)
                        {
                            int nextX = testX + j;
                            int nextY = y + i;

                            if (nextX < 0 || nextX >= board.width || nextY < 0 || nextY >= board.height || board.grid[nextY][nextX].occupied)
                                return false; 
                        }
                    }
                }
                return true;
            };

            if (isValidRotation(x))
            {
                shape = rotatedShape;
                return;
            }

            if (isValidRotation(x + 1))
            {
                x += 1;
                shape = rotatedShape;
                return;
            }

            if (isValidRotation(x + 2))
            {
                x += 2;
                shape = rotatedShape;
                return;
            }

            if (isValidRotation(x - 1))
            {
                x -= 1;
                shape = rotatedShape;
                return;
            }

            if (isValidRotation(x - 2))
            {
                x -= 2;
                shape = rotatedShape;
                return;
            }
        }

        void lockToBoard(Board &board)
        {
            for (size_t i = 0; i < shape.size(); ++i)
            {
                for (size_t j = 0; j < shape[i].size(); ++j)
                {
                    if(shape[i][j] == 1)
                    {
                        int boardX = x + j;
                        int boardY = y + i;
                        
                        if(board.isValidPosition(boardX, boardY))
                        {
                            board.grid[boardY][boardX].occupied = true;
                            board.grid[boardY][boardX].color = color;
                        }
                    }
                }
            }
        }

        void hardDrop(Board &board)
        {
            while(moveDown(board)){}
        }

        void draw(sf::RenderWindow &window)
        {
            for (size_t i = 0; i < shape.size(); ++i)
            {
                for (size_t j = 0; j < shape[i].size(); ++j)
                {
                    if(shape[i][j] == 1)
                    {
                        sf::RectangleShape block(sf::Vector2f(Board::blockSize, Board::blockSize));
                        block.setPosition(sf::Vector2f((x + j) * Board::blockSize, (y + i) * Board::blockSize));
                        block.setFillColor(color);
                        block.setOutlineColor(sf::Color::Black);
                        block.setOutlineThickness(-1.0f);
                        window.draw(block);
                    }
                }
            }
        }

        void drawGhost(sf::RenderWindow &window, const Board &board)
        {
            int ghostY = y;
            
            while (true)
            {
                bool collision = false;

                for (size_t i = 0; i < shape.size(); ++i)
                {
                    for (size_t j = 0; j < shape[i].size(); ++j)
                    {
                        if(shape[i][j] == 1)
                        {
                            int nextX = x + j;
                            int nextY = ghostY + i + 1;

                            if (nextY >= board.height || board.grid[nextY][nextX].occupied)
                            {
                                collision = true;
                                break;
                            }
                        }
                    }

                    if(collision)
                        break;
                }

                if(collision)
                    break;

                ghostY++;
            }
        
            for (size_t i = 0; i < shape.size(); ++i)
            {
                for (size_t j = 0; j < shape[i].size(); ++j)
                {
                    if(shape[i][j] == 1)
                    {
                        sf::RectangleShape block(sf::Vector2f(Board::blockSize, Board::blockSize));
                        block.setPosition(sf::Vector2f((x + j) * Board::blockSize, (ghostY + i) * Board::blockSize));
                        sf::Color ghostColor = color;
                        ghostColor.a = 80; 
                        block.setFillColor(ghostColor);
                        block.setOutlineColor(sf::Color::Black);
                        block.setOutlineThickness(-1.0f);
                        window.draw(block);
                    }
                }
            }
        }
};