#include <deque>
#include "raylib.h"
#include "raymath.h"

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

float cellSize = 30;
int cellCount = 20;

double lastUpdate = 0;

bool ElementInDeque(Vector2 element, std::deque<Vector2> deque) {
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
    bool addSegment = false;
    std::deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
    public:
    void Draw() {
        for (int i = 0; i < body.size(); i++) {
            Rectangle segment = Rectangle{body[i].x*cellSize,body[i].y*cellSize,cellSize, cellSize};
            DrawRectangleRounded(segment,0.5,6,darkGreen);
        }
    }
    void Update() {
        body.push_front(Vector2Add(body[0],direction));
        if (!addSegment) body.pop_back();
        else addSegment = false;


    }
    void Move() {
        if (IsKeyDown(KEY_UP) and direction.y != 1) direction = {0,-1};
        if (IsKeyDown(KEY_DOWN) and direction.y != -1) direction = {0,1};
        if (IsKeyDown(KEY_LEFT) and direction.x != 1) direction = {-1,0};
        if (IsKeyDown(KEY_RIGHT) and direction.x != -1) direction = {1,0};
    }

    std::deque<Vector2> getBody () {
        return body;
    }

    void addLength() {addSegment = true;}
};

class Food {
    Vector2 position;
    Texture2D texture;

    public:
        Food(std::deque<Vector2> body) {
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
            DrawTexture(texture, position.x*cellSize, position.y*cellSize, WHITE);
        }

        Vector2 genRandomCell() {
            float x = GetRandomValue(0, cellCount - 1);
            float y = GetRandomValue(0, cellCount - 1);
            return Vector2{x,y};
        }

        Vector2 randPosition(std::deque<Vector2> snakeBody) {
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
    void Draw() {
        food.Draw();
        snake.Draw();
    }
    void Update() {
        snake.Update();
        Eat();
    }
    void Eat () {
        if (Vector2Equals(snake.getBody()[0],food.getPosition())) {
            food.setPosition(food.randPosition(snake.getBody()));
            snake.addLength();
        }
    }
};

int main() {
    InitWindow(cellSize*cellCount, cellSize*cellCount, "Snake");
    SetTargetFPS(120);
    Game game;

    while (!WindowShouldClose()) {
        BeginDrawing();
        game.snake.Move();
        if (eventTriggered(0.2)) game.Update();
        ClearBackground(green);
        game.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}