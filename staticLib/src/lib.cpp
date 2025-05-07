#include "lib.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
// #include "network.h"
#include "activation.h"

float xavier_init(const int size) 
{
    return ((float)rand() / RAND_MAX) * 2.0f * sqrtf(1.0f / size) - sqrtf(1.0f / size);
}

void init_neuron(neuron_t *neuron) 
{
    for (int i = 0; i < LS; ++i) 
    {
        neuron->weight[i] = xavier_init(LS);
    }
    neuron->bias = ((float)rand() / RAND_MAX) * 0.1f - 0.05f;
    neuron->alpha = 0.01f;
}

void init_layer(layer_t *layer) 
{
    for (int i = 0; i < HN; i++) 
    {
        init_neuron(&layer->neuron[i]);
    }
    for (int i = 0; i < LS; i++) 
    {
        layer->memory[i] = 0.0f;
    }
}

void init_network(network_t *network) 
{
    for (int i = 0; i < HL; i++) 
    {
        init_layer(&network->layer[i]);
    }
    for (int i = 0; i < LS; i++) 
    {
        network->latent.mean[i] = 0.0f;
        network->latent.deviation[i] = 1.0f;
    }
}

float neuron_forward(const neuron_t *neuron, const float input[]) 
{
    float sum = neuron->bias;
    for (int i = 0; i < LS; i++) 
    {
        sum += neuron->weight[i] * input[i];
    }
    return sigmoid(sum);
    // return LReLU(sum);
}

void layer_forward(const layer_t *layer, const float input[], float output[]) 
{
    for (int i = 0; i < HN; i++) 
    {
        output[i] = neuron_forward(&layer->neuron[i], input);
    }
}

void compute_latent_mean(network_t *network) 
{
    for (int i = 0; i < LS; i++) 
    {
        float sum = 0.0f;
        for (int j = 0; j < HL; j++) 
        {
            sum += network->layer[j].memory[i];
        }
        network->latent.mean[i] = sum / HL;
    }
}

void compute_latent_deviation(network_t *network) 
{
    for (int i = 0; i < LS; ++i) 
    {
        float variance = 0.0f;
        for (int j = 0; j < HL; ++j) 
        {
            float diff = network->layer[j].memory[i] - network->latent.mean[i];
            variance += diff * diff;
        }
        network->latent.deviation[i] = sqrt(variance / HL);
    }
}

void network_forward(network_t *network, const float inputs[]) 
{
    float buffer[LS];

    for (int i = 0; i < LS; i++) buffer[i] = inputs[i];

    for (int i = 0; i < HL; i++) 
    {
        layer_forward(&network->layer[i], buffer, network->layer[i].memory);
        for (int j = 0; j < LS; j++) 
        {
            buffer[j] = network->layer[i].memory[j];
        }
    }

    compute_latent_mean(network);
    compute_latent_deviation(network);
}

float mse_loss(const float original[], const float reconstructed[], const int size) 
{
    float loss = 0.0f;
    for (int i = 0; i < size; ++i) 
    {
        float diff = original[i] - reconstructed[i];
        loss += diff * diff;
    }
    return loss / size;
}

float mae_loss(const float original[], const float reconstructed[], const int size) 
{
    float loss = 0.0f;
    for (int i = 0; i < size; ++i) 
    {
        loss += fabsf(original[i] - reconstructed[i]);
    }
    return loss / size;
}


void update_weights(neuron_t *neuron, const float input[], const float output, const float target, const float learning_rate) 
{
    float error = target - output;
    
    for (int i = 0; i < LS; i++) 
    {
        neuron->weight[i] += learning_rate * error * input[i];
    }
    neuron->bias += learning_rate * error;
}

void train_network(network_t *network, const float input[], const float target[], const float learning_rate) 
{
    network_forward(network, input);
    float loss = mae_loss(target, network->layer[HL - 1].memory, LS);
    // float loss = mae_loss(input, network->layer[HL - 1].memory, LS);

    for (int i = 0; i < HL; i++) 
    {
        for (int j = 0; j < HN; j++) 
        {
            update_weights(&network->layer[i].neuron[j], input, network->layer[i].memory[j], target[j], learning_rate);
        }
    }
    network->loss = loss;
    printf("Loss: %f\n", loss);
}
