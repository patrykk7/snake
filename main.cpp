#include <deque>
#include "raylib.h"
#include "raymath.h"

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

float cellSize = 30;
int cellCount = 20;

double lastUpdate = 0;

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
    std::deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
    public:
    void Draw() {
        for (int i = 0; i < body.size(); i++) {
            Rectangle segment = Rectangle{body[i].x*cellSize,body[i].y*cellSize,cellSize, cellSize};
            DrawRectangleRounded(segment,0.5,6,darkGreen);
        }
    }
    void Update() {
        body.pop_back();
        body.push_front(Vector2Add(body[0],direction));
    }
    void Move() {
        if (IsKeyDown(KEY_UP) and direction.y != 1) direction = {0,-1};
        if (IsKeyDown(KEY_DOWN) and direction.y != -1) direction = {0,1};
        if (IsKeyDown(KEY_LEFT) and direction.x != 1) direction = {-1,0};
        if (IsKeyDown(KEY_RIGHT) and direction.x != -1) direction = {1,0};
    }
};

class Food {
    Vector2 position;
    Texture2D texture;

    public:
        Food() {
            Image image = LoadImage("apple.png");
            ImageResize(&image, cellSize, cellSize);
            texture = LoadTextureFromImage(image);
            UnloadImage(image);
            position = GetPosition();
        }

        ~Food() {
            UnloadTexture(texture);
        }

        void Draw() {
            DrawTexture(texture, position.x*cellSize, position.y*cellSize, WHITE);
        }

        Vector2 GetPosition() {
            float x = GetRandomValue(0, cellCount - 1);
            float y = GetRandomValue(0, cellCount - 1);
            return Vector2{x, y};
        }
};

class Game {
    public:
    Snake snake;
    Food food;
    void Draw() {
        food.Draw();
        snake.Draw();
    }
    void Update() {
        snake.Update();
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