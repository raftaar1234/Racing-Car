#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <ctime>
#include <fstream>

using namespace std;
using namespace sf;


int readHighScoreFromFile()
{
    ifstream inputFile("highscore.txt");
    int highScore = 0;
    if (inputFile.is_open())
    {
        inputFile >> highScore;
        inputFile.close();
    }
    return highScore;
}


void writeHighScoreToFile(int highScore)
{
    ofstream outputFile("highscore.txt");
    if (outputFile.is_open())
    {
        outputFile << highScore;
        outputFile.close();
    }
}

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);


    RenderWindow mainWindow(VideoMode(800, 600), "Car Racing Game");
    mainWindow.setFramerateLimit(80);


    Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        cerr << "Error loading font" << "\n";
        return -1;
    }



    Text welcomeText;
    welcomeText.setFont(font);
    welcomeText.setString("Welcome to 2D Car Racing Game!");
    welcomeText.setCharacterSize(40);
    welcomeText.setFillColor(Color::White);
    welcomeText.setPosition(100.f, 200.f);



    Text instructionsText;
    instructionsText.setFont(font);
    instructionsText.setString("Press ENTER to start\nUse A/D to steer left/right\nPress P to pause/unpause\nAvoid the obstacles!");
    instructionsText.setCharacterSize(24);
    instructionsText.setFillColor(Color::White);
    instructionsText.setPosition(100.f, 300.f);


    bool gameStarted = false;
    bool isPaused = false;


    Texture carTexture;
    if (!carTexture.loadFromFile("car.png"))
    {
        cerr << "Error loading car texture" << "\n";
        return -1;
    }


    Sprite car;
    car.setTexture(carTexture);
    car.setScale(0.2f, 0.2f);


    Texture roadTexture;
    if (!roadTexture.loadFromFile("road.png"))
    {
        cerr << "Error loading road texture" << "\n";
        return -1;
    }


    Sprite road;
    road.setTexture(roadTexture);
    road.setScale(10.f, 10.f);



    const int laneCount = 3;
    vector<RectangleShape> roadLines(laneCount);
    vector<RectangleShape> laneBorders;
    float laneWidth = mainWindow.getSize().x / laneCount;


    for (int i = 0; i < laneCount; i++)
    {
        roadLines[i] = RectangleShape(Vector2f(10.f, 80.f));
        roadLines[i].setFillColor(Color::White);
        roadLines[i].setPosition(i * laneWidth + (laneWidth / 2), 0.f);
    }

    for (int i = 0; i <= laneCount; i++)
    {
        RectangleShape border(Vector2f(5.f, 600.f));
        border.setFillColor(Color::Yellow);
        border.setPosition(i * laneWidth, 0.f);
        laneBorders.push_back(border);
    }


    int currentLaneIndex = 1;
    car.setPosition((currentLaneIndex * laneWidth) + (laneWidth / 2 - car.getGlobalBounds().width / 2), 450.f);


    vector<Texture> obstacleTextures(3);
    if (!obstacleTextures[0].loadFromFile("obstacle1.png") ||
            !obstacleTextures[1].loadFromFile("obstacle2.png") ||
            !obstacleTextures[2].loadFromFile("obstacle3.png"))
    {
        cerr << "Error loading obstacle textures" << "\n";
        return -1;
    }


    vector<Sprite> obstacles;
    float obstacleSpeed = 5.0f;
    Clock obstacleSpawnClock;
    float minSpawnInterval = 1.0f;
    float maxSpawnInterval = 3.0f;
    float currentSpawnInterval = minSpawnInterval;


    int score = 0;


    int highScore = readHighScoreFromFile();  // High score variable
    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::Blue);
    scoreText.setPosition(10, 10);


    Text highScoreText;
    highScoreText.setFont(font);
    highScoreText.setCharacterSize(24);
    highScoreText.setFillColor(Color::Green);
    highScoreText.setPosition(10, 40);
    highScoreText.setString("High Score: " + to_string(highScore));


    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setString("GAME OVER!");
    gameOverText.setPosition(60, 100);
    bool gameOver = false;


    srand(static_cast<unsigned>(time(0)));


    Text pauseText;
    pauseText.setFont(font);
    pauseText.setCharacterSize(50);
    pauseText.setFillColor(Color::Yellow);
    pauseText.setString("PAUSED");
    pauseText.setPosition(200, 250);


    while (mainWindow.isOpen())
    {

        Event event;
        while (mainWindow.pollEvent(event))
        {
            if (event.type == Event::Closed)
                mainWindow.close();

            if (!gameStarted && event.type == Event::KeyPressed && event.key.code == Keyboard::Enter)
            {
                gameStarted = true;
            }


            if (gameStarted && event.type == Event::KeyPressed && event.key.code == Keyboard::P)
            {
                isPaused = !isPaused;
            }
        }


        if (!gameStarted)
        {
            mainWindow.clear(Color::Black);


            mainWindow.draw(welcomeText);
            mainWindow.draw(instructionsText);

            mainWindow.display();
            continue;
        }


        if (isPaused)
        {
            mainWindow.clear(Color::Black);


            mainWindow.draw(pauseText);
            mainWindow.display();
            continue;
        }

        if (!gameOver)
        {

            if (Keyboard::isKeyPressed(Keyboard::A))
            {
                if (currentLaneIndex > 0)
                {
                    currentLaneIndex--;
                    car.setPosition((currentLaneIndex * laneWidth) + ((laneWidth / 2) - (car.getGlobalBounds().width / 2)), car.getPosition().y);
                }
            }
            if (Keyboard::isKeyPressed(Keyboard::D))
            {
                if (currentLaneIndex < laneCount - 1)
                {
                    currentLaneIndex++;
                    car.setPosition((currentLaneIndex * laneWidth) + (laneWidth / 2 - car.getGlobalBounds().width / 2), car.getPosition().y);
                }
            }


            for (auto& obstacle : obstacles)
            {
                obstacle.move(0.f, obstacleSpeed);
            }

            for (auto& obstacle : obstacles)
            {
                if (car.getGlobalBounds().intersects(obstacle.getGlobalBounds()))
                {
                    gameOver = true;
                }
            }


            obstacles.erase(remove_if(obstacles.begin(), obstacles.end(),
                                      [&](Sprite& obs)
            {
                return obs.getPosition().y > 600;
            }),
            obstacles.end());


            if (obstacleSpawnClock.getElapsedTime().asSeconds() > currentSpawnInterval)
            {
                Sprite obstacle;
                int randomTextureIndex = rand() % obstacleTextures.size();
                obstacle.setTexture(obstacleTextures[randomTextureIndex]);
                obstacle.setScale(0.4f, 0.4f);
                int randomLane = rand() % laneCount;
                obstacle.setPosition(randomLane * laneWidth + (laneWidth / 2 - obstacle.getGlobalBounds().width / 2), -100.f);  // Center in lane
                obstacles.push_back(obstacle);
                obstacleSpawnClock.restart();


                currentSpawnInterval = minSpawnInterval + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxSpawnInterval - minSpawnInterval)));
            }


            score++;
            scoreText.setString("Score: " + to_string(score));




            if (score > highScore)
            {
                highScore = score;
                highScoreText.setString("High Score: " + to_string(highScore));
            }

            for (auto& line : roadLines)
            {
                line.move(0.f, 5.f);
                if (line.getPosition().y > 600)
                    line.setPosition(line.getPosition().x, -80.f);
            }
        }


        mainWindow.clear(Color::Black);


        mainWindow.draw(road);


        for (const auto& border : laneBorders)
        {
            mainWindow.draw(border);
        }


        for (const auto& line : roadLines)
        {
            mainWindow.draw(line);
        }
        mainWindow.draw(car);

        for (const auto& obstacle : obstacles)
        {
            mainWindow.draw(obstacle);
        }

        mainWindow.draw(scoreText);
        mainWindow.draw(highScoreText);

        if (gameOver)
        {
            mainWindow.draw(gameOverText);
            mainWindow.display();


            writeHighScoreToFile(highScore);


            mainWindow.close();
        }
        else
        {

            mainWindow.display();
        }
    }


    RenderWindow scoreWindow(VideoMode(400, 300), "GAME OVER");

    while (scoreWindow.isOpen())
    {
        Event event;
        while (scoreWindow.pollEvent(event))
        {
            if (event.type == Event::Closed)
                scoreWindow.close();
        }

        scoreWindow.clear(Color::White);
        scoreWindow.draw(gameOverText);
        scoreWindow.draw(scoreText);
        scoreWindow.draw(highScoreText);
        scoreWindow.display();
    }

    return 0;
}
