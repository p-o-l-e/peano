#include "raylib.h"
#include "raymath.h"
#include "frame.hpp"
#include "game.h"   // an external header in this project
#include "lib.h"	// an external header in the static lib project
#include <cstdio>

network_t net;
frame<unsigned> canvas(W, H);

float input[LS]     = { -2.0f, -1.0f, 0.0f, 0.10f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.5f, 2.0f, 3.0f };
float target[LS]    = { 0.25f, 0.45f, 0.0f, 0.75f, 1.0f, 0.5f, 0.8f, 0.1f, 0.0f, 0.5f, 2.0f, 0.9f, 0.7f, 0.2f, 0.3f, 0.9f };
int epoch = 0;

float adaptive_lr(int epoch, float base_lr) 
{
    return base_lr / (1.0f + 0.002f * epoch); // Slight decay factor
}

void GameInit()
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(W, H, "Example");
    SetTargetFPS(144);
    canvas.clr(0x16161616);
    init_network(&net);

    float output[LS];
    for(int i = 0; i < 200000; ++i)
    {
        train_network(&net, input, target, adaptive_lr(i, 0.01f));
        // canvas.set(i, net.loss * 100.0f + H/2, 0xFFFFFFFF);
        // for(int l = 0; l < HL; ++l)
        // {
        //     for(int n = 0; n < HN; ++n)
        //     {
        //         for(int d = 0; d < LS; ++d)
        //         {
        //             net.layer[l].neuron[n].weight[d];
        //             canvas.set(i, net.layer[l].neuron[n].weight[d] * 1000.0f + H/2, ColorToInt( Color {255/(l + 1), 255/(n + 1), 255/(d + 1), 255 }));
        //         }
        //     }
        // }
    }

}

void GameCleanup()
{
    CloseWindow();
}

bool GameUpdate()
{
    return true;
}

void GameDraw()
{
    for(int i = 0; i < W; ++i)
    {
        
    }

    BeginDrawing();
    ClearBackground(Color { 16, 16, 16, 16 });
    for(int x = 0; x < W; ++x)
    {
        for(int y = 0; y < H; ++y)
        {
            DrawPixel(x, y, GetColor(canvas.get(x, y)));
        }
    }
    EndDrawing();
}

int main()
{
    GameInit();

    while (!WindowShouldClose())
    {
        if (!GameUpdate())
            break;

        GameDraw();
    }
    GameCleanup();

    return 0;
}