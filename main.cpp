#include <deque>
#include "raylib.h"
#include "raymath.h"

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};
Color pulsating;

double lastUpdate = 0;
double lastPause = 0;
double lastAnimation = 0;

float cellSize = 30;

int cellCount = 20;
int offset = 75;
int spawned;
int points = 0;
bool running = true;
bool isPaused = false;
bool isFading = false;

bool elementInDeque(Vector2 element, const std::deque<Vector2>& deque) {
    for (unsigned int i = 0; i < deque.size(); i++) {
        if (Vector2Equals(deque[i], element)) return true;
    }
    return false;
}

bool eventTriggered (double interval, double &lastUpdate) {
    double currentTime = GetTime();
    if (currentTime - lastUpdate >= interval) {
        lastUpdate = currentTime;
        return true;
    }
    return false;
}

void chooseColor() {
    if (spawned == 0) pulsating = {221, 21, 51, 255};
    else if (spawned == 1) pulsating = {254, 254, 51, 255};
    else if (spawned == 2) pulsating = {79, 134, 247, 255};
    else if (spawned == 3) pulsating = {111, 45, 168, 255};
    else if (spawned == 4) pulsating = {156, 239, 67, 255};
    else if (spawned == 5) pulsating = {255, 165, 0, 255};
}

void drawPause(int screenWidth) {
    const char* scoreText = TextFormat("GAME PAUSED");
    int fontSize = 100;
    int textWidth = MeasureText(scoreText,fontSize);
    int posX = (screenWidth / 2) - (textWidth / 2);

    DrawText(scoreText,posX,screenWidth/2 - fontSize/2,fontSize,pulsating);
    if (pulsating.a == 255) isFading = true;
    else if (pulsating.a < 150) isFading = false;
    if (eventTriggered(0.05,lastAnimation)) {
        if (pulsating.a > 0 and isFading) pulsating.a -= 10;
        else pulsating.a += 10;
    }
}

void drawPoints(int screenWidth) {
    const char *score = TextFormat("%i", points);
    const char *spacebar = "press spacebar to pause/unpause";
    int fontSize = 40;
    int fontSize2 = 20;
    int textWidth = MeasureText(score, fontSize);
    int textWidth2 = MeasureText(spacebar, fontSize2);
    int posX = screenWidth - offset - textWidth + 5;
    DrawRectangleLinesEx(Rectangle{(float) offset - 5, (float) offset - 5, cellCount * cellSize + 10,
        cellCount * cellSize + 10}, 5, darkGreen);
    DrawText("Snake Game", offset - 5, 20, fontSize, darkGreen);
    DrawText(TextFormat("%i", points), posX, 20, fontSize, darkGreen);
    DrawText(spacebar,screenWidth/2 - textWidth2/2,screenWidth - offset + fontSize2/2,fontSize2, darkGreen);
}

class Snake {
    Vector2 direction = {1,0};
    Vector2 runningDirection = {1,0};
    bool addSegment = false;
    std::deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
    public:
    Snake() = default;
    ~Snake() = default;

    void draw() {
        for (int i = 0; i < body.size(); i++) {
            Rectangle segment = Rectangle{offset+body[i].x*cellSize,offset+body[i].y*cellSize,cellSize, cellSize};
            DrawRectangleRounded(segment,0.5,6,darkGreen);
        }
    }

    void update() {
        body.push_front(Vector2Add(body[0],direction));
        runningDirection = direction;
        if (!addSegment) body.pop_back();
        else addSegment = false;
    }

    void move() {
        if (IsKeyDown(KEY_UP) and runningDirection.y != 1) direction = {0,-1};
        if (IsKeyDown(KEY_DOWN) and runningDirection.y != -1) direction = {0,1};
        if (IsKeyDown(KEY_LEFT) and runningDirection.x != 1) direction = {-1,0};
        if (IsKeyDown(KEY_RIGHT) and runningDirection.x != -1) direction = {1,0};
    }

    std::deque<Vector2> getBody () {
        return body;
    }

    void addLength() {addSegment = true;}

    void reset() {
        addSegment = false;
        body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        direction = {1,0};
        runningDirection = {1,0};
    }
};

class Food {
    Vector2 position;
    Texture2D texture;

    public:
        Food(const std::deque<Vector2> &body, const char* path) {
            Image image = LoadImage(path);
            ImageResize(&image, cellSize, cellSize);
            texture = LoadTextureFromImage(image);
            UnloadImage(image);
            position = randPosition(body);
        }

        ~Food() {
            UnloadTexture(texture);
        }

        void draw() {
            DrawTexture(texture, offset+position.x*cellSize, offset+position.y*cellSize, WHITE);
        }

        Vector2 genRandomCell() {
            float x = GetRandomValue(0, cellCount - 1);
            float y = GetRandomValue(0, cellCount - 1);
            return Vector2{x,y};
        }

        Vector2 randPosition(const std::deque<Vector2> &snakeBody) {
            Vector2 position = genRandomCell();
            while (elementInDeque(position, snakeBody)) {
                position = genRandomCell();
            }
            return position;
        }

        Vector2 getPosition() {
            return position;
        }

        void setPosition(Vector2 position) {
            this->position = position;
        }
};

class Apple : public Food {
    public:
    Apple(const std::deque<Vector2> &body, const char *path = "assets/apple.png") : Food(body, path) {}
    ~Apple() = default;
};

class Grape : public Food {
    public:
    Grape(const std::deque<Vector2> &body, const char *path = "assets/grape.png"): Food(body, path) {}
    ~Grape() = default;
};

class Banana : public Food {
    public:
    Banana(const std::deque<Vector2> &body, const char *path = "assets/banana.png"): Food(body, path) {}
    ~Banana() = default;
};

class Blueberry : public Food {
public:
    Blueberry(const std::deque<Vector2> &body, const char *path = "assets/blueberry.png"): Food(body, path) {}
    ~Blueberry() = default;
};

class Kiwi : public Food {
    public:
    Kiwi(const std::deque<Vector2> &body, const char *path = "assets/kiwi.png"): Food(body, path) {}
    ~Kiwi() = default;
};

class Orange : public Food {
public:
    Orange(const std::deque<Vector2> &body, const char *path = "assets/orange.png"): Food(body, path) {}
    ~Orange() = default;
};

class Game {
    public:
    bool gameWon = false;
    Snake snake;
    Apple apple = Apple(snake.getBody());
    Banana banana = Banana(snake.getBody());
    Blueberry blueberry = Blueberry(snake.getBody());
    Orange orange = Orange(snake.getBody());
    Kiwi kiwi = Kiwi(snake.getBody());
    Grape grape = Grape(snake.getBody());
    Food* food [6] = {&apple, &banana,&blueberry,&grape,&kiwi,&orange};
    Sound eatApple;
    Sound hitWall;

    Game () {
        InitAudioDevice();
        eatApple = LoadSound("assets/apple.mp3");
        hitWall = LoadSound("assets/wall.mp3");
        spawned = GetRandomValue(0,5);
        chooseColor();
    }
    ~Game() {
        UnloadSound(eatApple);
        UnloadSound(hitWall);
        CloseAudioDevice();
    }

    void draw() {
        food[spawned]->draw();
        snake.draw();
    }

    void update() {
        if (running) {
            snake.update();
            checkCollision();
            eat();
        }
    }

    void eat () {
        if (Vector2Equals(snake.getBody()[0],food[spawned]->getPosition())) {
            spawned = GetRandomValue(0,5);
            chooseColor();
            food[spawned]->setPosition(food[spawned]->randPosition(snake.getBody()));
            snake.addLength();
            PlaySound(eatApple);
            points++;
        }
    }

    void gameOver() {
        snake.reset();
        spawned = GetRandomValue(0, 5);
        chooseColor();
        food[spawned]->setPosition(food[spawned]->randPosition(snake.getBody()));
        running = false;
    }

    void drawEnding(int screenWidth) {

        ClearBackground(green);

        const char* text1 = "YOU LOST";
        int fontSize1 = 50;
        int textWidth1 = MeasureText(text1, fontSize1);
        int posX1 = (screenWidth / 2) - (textWidth1 / 2);
        DrawText(text1,posX1,screenWidth/2 - 50,fontSize1,darkGreen);

        const char* text2 = "press spacebar to continue";
        int fontSize2 = 25;
        int textWidth2 = MeasureText(text2, fontSize2);
        int posX2 = (screenWidth / 2) - (textWidth2 / 2);
        DrawText(text2,posX2,screenWidth/2,fontSize2,darkGreen);

        const char* scoreText = TextFormat("score: %i", points);
        int textWidth3 = MeasureText(scoreText,fontSize2);
        int posX3 = (screenWidth / 2) - (textWidth3 / 2);
        DrawText(scoreText,posX3,screenWidth/2 + 30,fontSize2,darkGreen);

        if (IsKeyDown(KEY_SPACE)) {
            running = true;
            points = 0;
        }
    }

    void checkCollision() {
        std::deque<Vector2> body = snake.getBody();
        int snake_x = body.front().x;
        int snake_y = body.front().y;
        if (snake_x < 0 or snake_x >= cellCount) {
            PlaySound(hitWall);
            WaitTime(0.2);
            gameOver();
        }
        if (snake_y < 0 or snake_y >= cellCount) {
            PlaySound(hitWall);
            WaitTime(0.2);
            gameOver();
        }
        for (int i = 1; i < body.size(); i++) {
            if (snake_x == body[i].x && snake_y == body[i].y) {
                PlaySound(hitWall);
                WaitTime(0.2);
                gameOver();
            }
        }
    }
};

int main() {
    int screenWidth = 2*offset+cellSize*cellCount;
    InitWindow(screenWidth, screenWidth, "Snake");
    SetTargetFPS(120);
    Game game;

    while (!WindowShouldClose()) {
        BeginDrawing();

        if (IsKeyPressed(KEY_SPACE) and running and eventTriggered(0.5,lastPause)) {
            isPaused = !isPaused;
        }
        if (!isPaused) {
            game.snake.move();
            if (eventTriggered(0.2,lastUpdate)) game.update();
        }

        ClearBackground(green);
        if (running) {
            drawPoints(screenWidth);
            game.draw();
            if (isPaused) drawPause(screenWidth);
        } else game.drawEnding(screenWidth);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}