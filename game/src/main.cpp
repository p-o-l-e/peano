#include "raylib.h"
#include "raymath.h"
#include "frame.hpp"
#include "game.h"   // an external header in this project
#include "lib.h"	// an external header in the static lib project
#include "sequencer.h"
#include <cstdio>
#include <ctime>
#include <iostream>

network_t net;
frame<unsigned> canvas(W, H);

float input[LS]     = { -2.0f, -1.0f, 0.0f, 0.10f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.5f, 2.0f, 3.0f };
float target[LS]    = { 0.25f, 0.45f, 0.0f, 0.75f, 1.0f, 0.5f, 0.8f, 0.1f, 0.0f, 0.5f, 2.0f, 0.9f, 0.7f, 0.2f, 0.3f, 0.9f };
int   epoch = 0;

#define EPOCHS W
#define LEARNING_RATE 0.01f
#define VALIDATION_INTERVAL 5000

float adaptive_lr(int epoch, float base_lr) 
{
    return base_lr / (1.0f + 0.002f * epoch); // Slight decay factor
}

float noisy_input(float input, float noise_strength) 
{
    return input + noise_strength * ((float)rand() / RAND_MAX - 0.5f);
}

// Generate random input for training
void generate_random_input(float input[], int size, float noise_strength) 
{
    for (int i = 0; i < size; i++) 
    {
        input[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        input[i] = noisy_input(input[i], noise_strength);
    }
}

// Validate reconstruction quality
void validate_reconstruction(const float input[], const float reconstructed[], int size) 
{
    printf("Input vs. Reconstructed:\n");
    for (int i = 0; i < size; i++) 
    {
        printf("[%d] Input: %f | Reconstructed: %f | Error: %f\n",
               i, input[i], reconstructed[i], fabsf(input[i] - reconstructed[i]));
    }
}

// Validate latent space properties
void validate_latent_distribution(network_t *network) 
{
    printf("Latent Representation:\n");
    for (int i = 0; i < LS; i++) 
    {
        printf("[%d] Mean: %f | Deviation: %f\n", i, network->latent.mean[i], network->latent.deviation[i]);
    }
}

// Test generalization with unseen data
void test_unseen_input(network_t *network) 
{
    float unseen_input[LS];
    generate_random_input(unseen_input, LS, 0.01f);
    network_forward(network, unseen_input);

    printf("\nTesting Unseen Input...\n");
    validate_reconstruction(unseen_input, network->layer[HL - 1].memory, LS);
    validate_latent_distribution(network);
}

void test_network(frame<unsigned>* canvas, network_t* network, float input[], int width, int height) 
{
    network_forward(network, input);


        for (int i = 0; i < LS; i++) 
        {
            int activation_y = height / 2 - network->layer[HL - 1].memory[i] * (height / 2);
            int mean_y = height / 2 - network->latent.mean[i] * (height / 2);
            int deviation_y = height / 2 - network->latent.deviation[i] * (height / 2);
            int error_y = height / 2 - fabsf(input[i] - network->layer[HL - 1].memory[i]) * (height / 2);

            // Plot points with higher density across epochs
            canvas->set(epoch, activation_y, 0x00FF00FF);  // Green for activations
            canvas->set(epoch, mean_y, 0xFF0000FF);  // Red for latent mean
            canvas->set(epoch, deviation_y, 0x0000FFFF);  // Blue for latent deviation
            canvas->set(epoch, error_y, 0xFFFF00FF);  // Yellow for reconstruction error
        }
}


// Run training and integrate validation steps
void train_network_full(network_t *network, const bool d) 
{
    float input[LS], target[LS];

    generate_random_input(input, LS, 0.01f);

    for (int i = 0; i < LS; i++) 
    {
        target[i] = input[i];  
    }

    train_network(network, input, target, LEARNING_RATE, d);

    if (epoch % VALIDATION_INTERVAL == 0) 
    {
        printf("\nEpoch %d - Loss: %f\n", epoch, network->loss);
        validate_reconstruction(input, network->layer[HL - 1].memory, LS);
        validate_latent_distribution(network);
        test_unseen_input(network);


    }
            // **Plot network behavior for deeper analysis**
            test_network(&canvas, network, input, W, H);
}





void GameInit()
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(W, H, "Example");
    SetTargetFPS(144);
    canvas.clr(0x16161616);
    init_network(&net);
    srand(time(NULL));

    float output[LS];
    printf("Starting Training...\n");
    for(int i = 0; i < EPOCHS; ++i)
    {
        // train_network(&net, input, target, adaptive_lr(i, 0.01f));
        train_network_full(&net, false);

        canvas.set(i, net.loss * 100.0f + H/2, 0xFFFFFFFF);
        for(int l = 0; l < HL; ++l)
        {
            for(int n = 0; n < HN; ++n)
            {
                float sum = 0.0f;
                for(int d = 0; d < LS; ++d)
                {
                    sum += net.layer[l].neuron[n].weight[d];
                    // canvas.set(i, net.layer[l].neuron[n].weight[d] * 300.0f + H/2, ColorToInt( Color {255/(l + 1), 255/(n + 1), 255/(d + 1), 255 }));
                }
                canvas.set(i, sum * 200.0f + H/2, ColorToInt( Color {255/(l + 1), 255/(n + 1), 128, 255 }));

            }
        }
        ++epoch;
    }
    for(int i = 0; i < 100000; ++i) 
    {
        train_network_full(&net, false);
        ++epoch;
    }

    printf("Training Completed!\n");
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