#include <deque>
#include "raylib.h"
#include "raymath.h"

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

float cellSize = 30;
int cellCount = 20;
int  offset = 75;

bool running = true;

int points = 0;
double lastUpdate = 0;

bool ElementInDeque(Vector2 element, const std::deque<Vector2>& deque) {
    for (unsigned int i = 0; i < deque.size(); i++) {
        if (Vector2Equals(deque[i], element)) return true;
    }
    return false;
}

bool eventTriggered (double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdate >= interval) {
        lastUpdate = currentTime;
        return true;
    }
    return false;
}

class Snake {
    Vector2 direction = {1,0};
    Vector2 runningDirection = {1,0};
    bool addSegment = false;
    std::deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
    public:
    Snake() = default;
    ~Snake() = default;

    void Draw() {
        for (int i = 0; i < body.size(); i++) {
            Rectangle segment = Rectangle{offset+body[i].x*cellSize,offset+body[i].y*cellSize,cellSize, cellSize};
            DrawRectangleRounded(segment,0.5,6,darkGreen);
        }
    }

    void Update() {
        body.push_front(Vector2Add(body[0],direction));
        runningDirection = direction;
        if (!addSegment) body.pop_back();
        else addSegment = false;
    }

    void Move() {
        if (IsKeyDown(KEY_UP) and runningDirection.y != 1) direction = {0,-1};
        if (IsKeyDown(KEY_DOWN) and runningDirection.y != -1) direction = {0,1};
        if (IsKeyDown(KEY_LEFT) and runningDirection.x != 1) direction = {-1,0};
        if (IsKeyDown(KEY_RIGHT) and runningDirection.x != -1) direction = {1,0};
    }

    std::deque<Vector2> getBody () {
        return body;
    }

    void addLength() {addSegment = true;}

    void Reset() {
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
        Food(const std::deque<Vector2> &body) {
            Image image = LoadImage("apple.png");
            ImageResize(&image, cellSize, cellSize);
            texture = LoadTextureFromImage(image);
            UnloadImage(image);
            position = randPosition(body);
        }

        ~Food() {
            UnloadTexture(texture);
        }

        void Draw() {
            DrawTexture(texture, offset+position.x*cellSize, offset+position.y*cellSize, WHITE);
        }

        Vector2 genRandomCell() {
            float x = GetRandomValue(0, cellCount - 1);
            float y = GetRandomValue(0, cellCount - 1);
            return Vector2{x,y};
        }

        Vector2 randPosition(const std::deque<Vector2> &snakeBody) {
            Vector2 position = genRandomCell();
            while (ElementInDeque(position, snakeBody)) {
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

class Game {
    public:
    Snake snake;
    Food food = Food(snake.getBody());
    Sound eatApple;
    Sound hitWall;

    Game () {
        InitAudioDevice();
        eatApple = LoadSound("apple.mp3");
        hitWall = LoadSound("wall.mp3");
    }
    ~Game() {
        UnloadSound(eatApple);
        UnloadSound(hitWall);
        CloseAudioDevice();
    }
    void Draw() {
        food.Draw();
        snake.Draw();
    }
    void Update() {
        if (running) {
            snake.Update();
            checkCollision();
            Eat();
        }
    }
    void Eat () {
        if (Vector2Equals(snake.getBody()[0],food.getPosition())) {
            food.setPosition(food.randPosition(snake.getBody()));
            snake.addLength();
            PlaySound(eatApple);
            points++;
        }
    }

    void gameOver() {
        snake.Reset();
        food.setPosition(food.randPosition(snake.getBody()));
        running = false;
    }

    void DrawEnding(int screenWidth) {

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
        game.snake.Move();
        if (eventTriggered(0.2)) game.Update();
        ClearBackground(green);
        if (running) {
            const char *score = TextFormat("%i", points);
            int fontSize = 40;
            int textWidth = MeasureText(score, fontSize);
            int posX = screenWidth - offset - textWidth + 5;
            DrawRectangleLinesEx(Rectangle{(float) offset - 5, (float) offset - 5, cellCount * cellSize + 10,
                cellCount * cellSize + 10}, 5, darkGreen);
            DrawText("Snake Game", offset - 5, 20, fontSize, darkGreen);
            DrawText(TextFormat("%i", points), posX, 20, fontSize, darkGreen);
            game.Draw();
        } else game.DrawEnding(screenWidth);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
