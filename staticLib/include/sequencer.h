#pragma once
#include "lib.h"
#include "note.h"


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
