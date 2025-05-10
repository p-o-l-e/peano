#pragma once
#include "lib.h"
#include "note.h"

typedef struct 
{ 
    float pitch; 
    float detune;
    float gate;
    float velocity;
    float latitude;

} transformer_t;

void train_sequence(network_t *network, float sequence[][NF], int seq_length, float learning_rate) 
{
    if (seq_length < LS) return;

    for (int i = 0; i < seq_length - LS + 1; i++)
    {
        float input[LS];
        float target[LS];

        for (int j = 0; j < LS; j++) 
        {
            input[j]  = sequence[i + j][j];
            target[j] = sequence[i + j][j];
        }

        train_network(network, input, target, learning_rate, false);
    }
}

void pad_sequence(float padded_seq[][NF], float original_seq[][NF], int seq_length) 
{
    for (int i = 0; i < LS; i++) 
    {
        if (i < seq_length) 
        {
            for (int j = 0; j < NF; j++) 
            {
                padded_seq[i][j] = original_seq[i][j];
            }
        } 
        else 
        {
            for (int j = 0; j < NF; j++) 
            {
                padded_seq[i][j] = 0.0f; // Zero-padding (adjust strategy if needed)
            }
        }
    }
}



void feed_dataset(network_t *net, const note_cluster_t dataset[], size_t size, const transformer_t *transformer, float lrate, bool debug) 
{
    for (size_t i = 0; i < size; ++i) 
    {
        float input[POLYPHONY * NF];
        float target[POLYPHONY * NF];

        for (size_t j = 0; j < POLYPHONY; ++j) 
        {
            for (size_t k = 0; k < NF; ++k) 
            {
                input[j * NF + k] = ((float*)&dataset[i].note[j])[k];
                float factor = ((float*)transformer)[k];
                target[j * NF + k] = input[j * NF + k] * factor;
            }
        }
        train_network(net, input, target, lrate, debug);
    }
}

