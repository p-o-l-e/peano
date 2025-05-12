#include <vector>
#include "network.h"
#include "raylib.h"
#include "io.h"
#include "frame.hpp"
#include "test.hpp"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#define RAYGUI_STANDALONE
#define SCREEN_WIDTH (1600)
#define SCREEN_HEIGHT (750)

#define GAP_X 2 
#define GAP_Y 2
#define FRAME_OX 90
#define FRAME_OY 2 

#define FRAME_W (SCREEN_WIDTH - FRAME_OX - GAP_X)
#define FRAME_H (SCREEN_HEIGHT - GAP_Y * 2)

#define WINDOW_TITLE "AE.0.0.1"



int main(void)
{
    int cycle = FRAME_W;
    int iteration = 0;

    srand(time(NULL));
    bool next_aion = true;

    std::vector<frame<unsigned>*> canvas;
    network_t net;
    init_network(&net);

    bool layer_active[LAYER::COUNT];
    for(int i = 0; i < LAYER::COUNT; ++i) 
    {
        canvas.push_back(new frame<unsigned>{ FRAME_W, FRAME_H });
        canvas.at(i)->clr(0);
        layer_active[i] = false;
    }


  
    write_midi_file("test.mid", dataset, DATASET_SIZE, 120);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    bool repaint = true;
    SetTargetFPS(30);


    Image img = GenImageColor(FRAME_W, FRAME_H, Color { 0, 0, 0, 255 }); 
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Texture2D tex = LoadTextureFromImage(img);
    Image imCopy = ImageCopy(img);

    while (!WindowShouldClose())
    {
        if(next_aion)
        {
            for(int i = 0; i < cycle; ++i)
            {
                train_network_full(&canvas, &net, false, FRAME_W, FRAME_H, i, TFLAG::FDEFAULT); 
            }
            iteration++;
            next_aion = false;
        }

        if(repaint)
        {
            UnloadImage(imCopy);                            // Unload image-copy data
            imCopy = ImageCopy(img);                        // Restore image-copy from image-origin
            for(int l = 0; l < LAYER::COUNT; ++l) 
            {
                if(!layer_active[l]) continue;
                for(int x = 0; x < FRAME_W; ++x)
                {
                    for(int y = 0; y < FRAME_H; ++y)
                    {  
                        auto c = canvas.at(l)->get(x, y);
                        if(c) ImageDrawPixel(&imCopy, x, y, GetColor(c));
                    }
                }
            }
            Color *pixels = LoadImageColors(imCopy);    // Load pixel data from image (RGBA 32bit)
            UpdateTexture(tex, pixels);
            UnloadImageColors(pixels);                  // Unload pixels data from RAM

            repaint = false;
        }
        BeginDrawing(); 
        ClearBackground(LIGHTGRAY);

        DrawTexture(tex, FRAME_OX, FRAME_OY, WHITE);
        for(int i = LAYER::ACTIVATION; i < LAYER::COUNT; ++i)
        {
            if(GuiCheckBox(Rectangle { 12, 12 + 14 * i, 12, 12 }, layer.at((LAYER)i).c_str(), &layer_active[i]))
            {
                repaint = true;
            }
        }
        if (GuiButton(Rectangle { GAP_X, SCREEN_HEIGHT - GAP_Y - 24, 80, 24 }, GuiIconText(ICON_PLAYER_NEXT, "Epoch"))) 
        { 
            next_aion = true; 
            for(int i = 0; i < LAYER::COUNT; ++i)                 
            {
                canvas.at(i)->clr(0);
            }
            repaint = true;
        };
        GuiLabel(Rectangle { GAP_X , SCREEN_HEIGHT - GAP_Y - 50, 80, 24 }, std::to_string(iteration * cycle).c_str());

        EndDrawing();
    }

    UnloadTexture(tex);
    UnloadImage(img);
    UnloadImage(imCopy);

    CloseWindow();
    for(int i = LAYER::COUNT - 1; i >= 0; --i) 
    {
        delete canvas.at(i);   
    }

    return 0;
}
