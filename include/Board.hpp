#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
using namespace std;

class Board
{
    public:
        static const int width = 10;
        static const int height = 20;
        static const int blockSize = 40;

    private:
        struct Cell
        {
            int x;
            int y;
            bool occupied;
            sf::Color color;
        };

    public:
        vector<vector<Cell>> grid;

        Board()
        {
            grid.resize(height, vector<Cell>(width));

                for(int i = 0; i < height; ++i)
                {
                    for(int j = 0; j < width; ++j)
                        grid[i][j] = {j, i, false, sf::Color::White};
                }
        }

        void draw(sf::RenderWindow& window)
        {
            for(int i = 0; i < height; ++i)
            {
                for(int j = 0; j < width; ++j)
                {
                    sf::RectangleShape cellShape(sf::Vector2f(blockSize, blockSize));
                    cellShape.setFillColor(grid[i][j].occupied ? grid[i][j].color : sf::Color::White);
                    cellShape.setOutlineColor(sf::Color::Black);
                    cellShape.setOutlineThickness(1);
                    cellShape.setPosition(sf::Vector2f(grid[i][j].x * blockSize, grid[i][j].y * blockSize));
                    window.draw(cellShape);
                }
            }
        }

        bool isValidPosition(int x, int y)
        {
            return x >= 0 && x < width && y >= 0 && y < height;
        }

        int clearLines()
        {
            int linesCleared = 0;
            for(int i = height - 1; i >= 0; --i)
            {
                bool fullLine = true;

                for(int j = 0; j < width; ++j)
                {
                    if(grid[i][j].occupied == false)
                    {
                        fullLine = false;
                        break;
                    }
                }

                if(fullLine)
                {
                    for(int k = i; k > 0; --k)
                    {
                        for(int j = 0; j < width; ++j)
                        {
                            grid[k][j].occupied = grid[k - 1][j].occupied;
                            grid[k][j].color = grid[k - 1][j].color;
                        }
                    }

                    for(int j = 0; j < width; ++j)
                    {
                        grid[0][j].occupied = false;
                        grid[0][j].color = sf::Color::White;
                    }

                    i++;
                    linesCleared++;
                }
            }

            return linesCleared;
        }
};  