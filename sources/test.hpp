#pragma once
#include "raylib.h"
#include "raymath.h"
#include "frame.hpp"
#include "network.h"
#include "io.h"
#include "sequencer.h"
#include <cstdio>
#include <ctime>
#include <vector>
#include <iostream>
#include <map>
#include <string>

float sample_input[LS]     = { -2.0f, -1.0f, 0.0f, 0.10f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.5f, 2.0f, 3.0f };
float sample_target[LS]    = { 0.25f, 0.45f, 0.0f, 0.75f, 1.0f, 0.5f, 0.8f, 0.1f, 0.0f, 0.5f, 2.0f, 0.9f, 0.7f, 0.2f, 0.3f, 0.9f };

#define LEARNING_RATE 0.01f
#define VALIDATION_INTERVAL 5000

float noisy_input(float input, float noise_strength) 
{
    return input + noise_strength * ((float)rand() / RAND_MAX - 0.5f);
}

void generate_random_input(float input[], int size, float noise_strength) 
{
    for (int i = 0; i < size; i++) 
    {
        input[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        input[i] = noisy_input(input[i], noise_strength);
    }
}

void validate_reconstruction(const float input[], const float reconstructed[], int size) 
{
    printf("Input vs. Reconstructed:\n");
    for (int i = 0; i < size; i++) 
    {
        printf("[%d] Input: %f | Reconstructed: %f | Error: %f\n",
               i, input[i], reconstructed[i], fabsf(input[i] - reconstructed[i]));
    }
}

void validate_latent_distribution(network_t *network) 
{
    printf("Latent Representation:\n");
    for (int i = 0; i < LS; i++) 
    {
        printf("[%d] Mean: %f | Deviation: %f\n", i, network->latent.mean[i], network->latent.deviation[i]);
    }
}

void test_unseen_input(network_t *network) 
{
    float unseen_input[LS];
    generate_random_input(unseen_input, LS, 0.1f);
    network_forward(network, unseen_input);

    printf("\nTesting Unseen Input...\n");
    validate_reconstruction(unseen_input, network->layer[HL - 1].memory, LS);
    validate_latent_distribution(network);
}

typedef enum LAYER { ACTIVATION, MEAN, DEVIATION, ERROR, WEIGHT, LOSS, COUNT };

const std::map<LAYER, std::string> layer {
    { LAYER::ACTIVATION, "Activation" },
    { LAYER::MEAN,       "Mean      " },
    { LAYER::DEVIATION,  "Deviation " },
    { LAYER::ERROR,      "Error     " },
    { LAYER::WEIGHT,     "Weights   " },
    { LAYER::LOSS,       "Loss      " }
};

typedef enum TFLAG
{
    FDEFAULT = 0,
    AVERAGE = 1 << 0,
    DATASET = 1 << 1

};

void test_network(std::vector<frame<unsigned>*>* canvas, network_t* network, float input[], int width, int height, int epoch, int flags = TFLAG::FDEFAULT) 
{
    network_forward(network, input);
    if(flags & TFLAG::AVERAGE)
    {
        float activation = 0.0f, mean = 0.0f, deviation = 0.0f, error = 0.0f, sum = 0.0f;

        for (int i = 0; i < LS; i++) 
        {
            activation += network->layer[HL - 1].memory[i];
            mean += network->latent.mean[i];
            deviation += network->latent.deviation[i];
            error += fabsf(input[i] - network->layer[HL - 1].memory[i]);
        }
        for(int l = 0; l < HL; ++l)
        {
            for(int n = 0; n < HN; ++n)
            {
                for (int i = 0; i < LS; i++) 
                {
                    sum += network->layer[l].neuron[n].weight[i];
                }
                sum /= (float)LS;
                canvas->at(LAYER::WEIGHT)->set(epoch, height/2 - sum * height, ColorToInt( Color {255/(l + 1), 255/(n + 1), 128, 255 }));
            }
        }

        int activation_y = height / 2 - (activation / LS) * (height / 2);
        int mean_y = height / 2 - (mean / LS) * (height / 2);
        int deviation_y = height / 2 - (deviation / LS) * (height / 2);
        int error_y = height / 2 - (error / LS) * (height / 2);

        // Plot points with higher density across epochs
        canvas->at(LAYER::ACTIVATION)->set(epoch, activation_y, ColorToInt(Color { 0, 255, 0, 255 }));  // Green for activations
        canvas->at(LAYER::MEAN)->set(epoch, mean_y, ColorToInt(Color { 255, 0, 0, 255 }));  // Red for latent mean
        canvas->at(LAYER::DEVIATION)->set(epoch, deviation_y, ColorToInt(Color { 0, 0, 255, 255 }));  // Blue for latent deviation
        canvas->at(LAYER::ERROR)->set(epoch, error_y, ColorToInt(Color { 255, 255, 0, 255 }));  // Yellow for reconstruction error

    }
    else
    {
        for (int i = 0; i < LS; i++) 
        {
            int activation_y = height / 2 - network->layer[HL - 1].memory[i] * (height / 2);
            int mean_y = height / 2 - network->latent.mean[i] * (height / 2);
            int deviation_y = height / 2 - network->latent.deviation[i] * (height / 2);
            int error_y = height / 2 - fabsf(input[i] - network->layer[HL - 1].memory[i]) * (height / 2);

            // Plot points with higher density across epochs
            canvas->at(LAYER::ACTIVATION)->set(epoch, activation_y, ColorToInt(Color { 255/(LS - i), 255/(i + 1), 255/(i + 1), 255 }));  // Green for activations
            canvas->at(LAYER::MEAN)->set(epoch, mean_y, ColorToInt(Color { 255/(i + 1), 255/(LS - i), 255/(i + 1), 255 }));  // Red for latent mean
            canvas->at(LAYER::DEVIATION)->set(epoch, deviation_y, ColorToInt(Color { 255/(i + 1), 255/(i + 1), 255/(LS - i), 255 }));  // Blue for latent deviation
            canvas->at(LAYER::ERROR)->set(epoch, error_y, ColorToInt(Color { 255/(LS - i), 255/(LS - i), 255/(LS - i), 255 }));  // Yellow for reconstruction error
            for(int l = 0; l < HL; ++l)
            {
                for(int n = 0; n < HN; ++n)
                {
                    canvas->at(LAYER::WEIGHT)->set(epoch, height/2 - network->layer[l].neuron[n].weight[i] * height/2, ColorToInt( Color {255/(l + 1), 255/(n + 1), 255/(i + 1), 255 }));

                }
            }
        }
    }

    canvas->at(LAYER::LOSS)->set(epoch, height/2 - network->loss * (height / 2), 0xFFFFFFFF);
}


void train_network_full(std::vector<frame<unsigned>*>* canvas, network_t *network, const bool d, int width, int height, int epoch, int flags = TFLAG::FDEFAULT) 
{
    if(flags & TFLAG::DATASET)
    {
        const transform_t tr { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
        feed_dataset(network, dataset, DATASET_SIZE, &tr, LEARNING_RATE, false); 
    }
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
    test_network(canvas, network, input, width, height, epoch, flags);
}



