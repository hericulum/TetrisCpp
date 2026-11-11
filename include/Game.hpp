#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Board.hpp"
#include "Piece.hpp"
#include <algorithm>
#include <fstream>
using namespace std;

class Game
{
    private:
        sf::RenderWindow window;
        Board board;
        sf::Clock dropClock;
        float dropInterval = 0.8f;       
        int totalLinesCleared = 0;       
        bool isGameOver = false;
        sf::Font font;
        sf::Text gameOverText {font, "GAME OVER\nPress R to Restart", 24};
        int score = 0;
        int highscore = 0;
        sf::Text scoreText {font, "Score: 0", 20};
        Piece currentPiece = Piece(Piece::Type(rand() % 7));
        Piece nextPiece = Piece(Piece::Type(rand() % 7));

        // --- Audio Assets ---
        sf::Music backgroundMusic;
        sf::SoundBuffer dropBuffer;
        sf::Sound dropSound{dropBuffer};
        sf::SoundBuffer clearBuffer;
        sf::Sound clearSound{clearBuffer};
        sf::SoundBuffer rotateBuffer;
        sf::Sound rotateSound{rotateBuffer};

        void saveHighscore()
        {
            ofstream outFile("highscore.txt");
            if (outFile.is_open())
            {
                outFile << highscore;
                outFile.close();
            }
        }

        void processLineClear()
        {
            int linesCleared = board.clearLines();
            if (linesCleared > 0)
            {
                score += linesCleared * 100;
                totalLinesCleared += linesCleared;
                scoreText.setString("Score: " + to_string(score));

                if (score > highscore)
                {
                    highscore = score;
                    saveHighscore();
                }

                clearSound.play();
                dropInterval = max(0.08f, 0.5f - (totalLinesCleared / 5) * 0.04f);
            }
        }

    public:
        Game()
        {
            window.create(sf::VideoMode({Board::width * Board::blockSize + 160, Board::height * Board::blockSize}), "Tetris Board");

            if(!font.openFromFile("assets/ARIAL.TTF"))
            {
                window.close();
                throw runtime_error("Failed to load font!");
            }

            // --- Load Local Highscore from Disk ---
            ifstream inFile("highscore.txt");
            if (inFile.is_open())
            {
                inFile >> highscore;
                inFile.close();
            }

            // --- Load and Initialize Audio ---
            if (backgroundMusic.openFromFile("assets/theme.mp3"))
            {
                backgroundMusic.setLooping(true);
                backgroundMusic.setVolume(30.f); 
                backgroundMusic.play();
            }

            if (!dropBuffer.loadFromFile("assets/drop.wav")) {
                // Handle optional error loading drop sound
            }

            if (!clearBuffer.loadFromFile("assets/clear.mp3")) {
                // Handle optional error loading clear sound
            }

            if (!rotateBuffer.loadFromFile("assets/rotate.mp3")) {
                // Handle optional error loading rotate sound
            }
        };

        void spawnNewPiece()
        {
            currentPiece = nextPiece;
            nextPiece = Piece(Piece::Type(rand() % 7));
            
            for (size_t i = 0; i < currentPiece.shape.size(); ++i)
            {
                for (size_t j = 0; j < currentPiece.shape[i].size(); ++j)
                {
                    if(currentPiece.shape[i][j] == 1)
                    {
                        int boardX = currentPiece.x + j;
                        int boardY = currentPiece.y + i;

                        if(board.grid[boardY][boardX].occupied)
                        {
                            isGameOver = true;
                            backgroundMusic.stop(); 
                            return;
                        }
                    }
                }
            }
        }

        void run()
        {
            while(window.isOpen())
            {
                if(!isGameOver)
                {
                    while(const optional event = window.pollEvent())
                    {
                        if(event -> is<sf::Event::Closed>())
                            window.close();
                    
                        if(const auto* keyPressed = event -> getIf<sf::Event::KeyPressed>())
                        {
                            switch(keyPressed -> code)
                            {
                                case sf::Keyboard::Key::Left:
                                    currentPiece.moveLeft(board);
                                    break;

                                case sf::Keyboard::Key::Right:
                                    currentPiece.moveRight(board);
                                    break;

                                case sf::Keyboard::Key::Down:
                                    if(!currentPiece.moveDown(board))
                                    {
                                        currentPiece.lockToBoard(board);
                                        dropSound.play();
                                        processLineClear();
                                        spawnNewPiece();
                                    }
                                    break;

                                case sf::Keyboard::Key::Space: 
                                    {
                                        currentPiece.hardDrop(board);
                                        currentPiece.lockToBoard(board);
                                        dropSound.play();
                                        processLineClear();
                                        spawnNewPiece();
                                        dropClock.restart();
                                    }
                                    break;

                                case sf::Keyboard::Key::Up: 
                                    currentPiece.rotate(board);
                                    rotateSound.play();
                                    break;  
                            }
                        }
                    }
                    
                    if(dropClock.getElapsedTime().asSeconds() >= dropInterval)
                    {
                        if(!currentPiece.moveDown(board))
                        {
                            currentPiece.lockToBoard(board);
                            dropSound.play();
                            processLineClear();
                            spawnNewPiece();
                        }

                        dropClock.restart();
                    }
                    
                    window.clear(sf::Color::Black);
                    scoreText.setFillColor(sf::Color::Magenta);
                    scoreText.setPosition({10.0f, 10.0f});
                    board.draw(window);
                    currentPiece.drawGhost(window, board);
                    currentPiece.draw(window);
                    
                    sf::RectangleShape previewBox({120.0f, 120.0f});
                    float previewX = Board::width * Board::blockSize + 20.0f;
                    float previewY = 80.0f;
                    previewBox.setPosition({previewX, previewY});
                    previewBox.setFillColor(sf::Color(30, 30, 30, 220));
                    previewBox.setOutlineColor(sf::Color::White);
                    previewBox.setOutlineThickness(1.0f);
                    sf::Text nextLabel {font, "NEXT", 18};
                    nextLabel.setFillColor(sf::Color::White);
                    nextLabel.setPosition({previewX + 30.0f, previewY - 30.0f});
                    window.draw(nextLabel);
                    window.draw(previewBox);
                    int minX = 4, maxX = 0, minY = 4, maxY = 0;

                    for (size_t i = 0; i < nextPiece.shape.size(); ++i)
                    {
                        for (size_t j = 0; j < nextPiece.shape[i].size(); ++j)
                        {
                            if (nextPiece.shape[i][j] == 1)
                            {
                                if (j < minX) minX = j;
                                if (j > maxX) maxX = j;
                                if (i < minY) minY = i;
                                if (i > maxY) maxY = i;
                            }
                        }
                    }

                    float pieceWidth = (maxX - minX + 1) * Board::blockSize;
                    float pieceHeight = (maxY - minY + 1) * Board::blockSize;
                    float boxSize = 120.0f;
                    float offsetX = previewX + (boxSize - pieceWidth) / 2.0f - (minX * Board::blockSize);
                    float offsetY = previewY + (boxSize - pieceHeight) / 2.0f - (minY * Board::blockSize);

                    for (size_t i = 0; i < nextPiece.shape.size(); ++i)
                    {
                        for (size_t j = 0; j < nextPiece.shape[i].size(); ++j)
                        {
                            if (nextPiece.shape[i][j] == 1)
                            {
                                sf::RectangleShape block(sf::Vector2f(Board::blockSize, Board::blockSize));
                                block.setPosition(sf::Vector2f(offsetX + (j * Board::blockSize), offsetY + (i * Board::blockSize)));
                                block.setFillColor(nextPiece.color);
                                block.setOutlineColor(sf::Color::Black);
                                block.setOutlineThickness(-1.0f);
                                window.draw(block);
                            }
                        }
                    }

                    window.draw(scoreText);
                    window.display();
                }

                else
                {
                    window.clear(sf::Color::Black);
                    board.draw(window);
                    gameOverText.setFillColor(sf::Color::Magenta);
                    gameOverText.setCharacterSize(24);
                    gameOverText.setPosition({30.0f, 250.0f});
                    string gameOverString = "GAME OVER\nPress R to Restart\nScore: " + to_string(score) + "\nHighscore: " + to_string(highscore);
                    gameOverText.setString(gameOverString);
                    window.draw(gameOverText);
                    window.display();

                    while(const optional event = window.pollEvent())
                    {
                        if(event -> is<sf::Event::Closed>())
                            window.close();

                        if(const auto* keyPressed = event -> getIf<sf::Event::KeyPressed>())
                        {
                            if(keyPressed -> code == sf::Keyboard::Key::R)
                            {
                                for (int i = 0; i < Board::height; ++i)
                                {
                                    for (int j = 0; j < Board::width; ++j)
                                    {
                                        board.grid[i][j].occupied = false;
                                        board.grid[i][j].color = sf::Color::White;
                                    }
                                }

                                isGameOver = false;
                                score = 0;
                                totalLinesCleared = 0;
                                dropInterval = 0.8f; 
                                spawnNewPiece();
                                dropClock.restart();
                                backgroundMusic.play(); 
                            }
                        }
                    }
                }
            }
        }
};