#pragma once
#include "network.h"
#include "note.h"

typedef struct 
{ 
    float pitch; 
    float detune;
    float gate;
    float velocity;
    float latitude;

} transform_t;


void feed_dataset(network_t *net, const note_cluster_t dataset[], size_t size, const transform_t *transform, float lrate, bool debug) 
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
                float factor = ((float*)transform)[k];
                target[j * NF + k] = input[j * NF + k] * factor;
            }
        }
        train_network(net, input, target, lrate, debug);
    }
}

