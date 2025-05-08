#pragma once

#define LS      16      // Dimensions (Latent Size)
#define HN      16      // Hidden neurons
#define HL       3      // Hidden layers

typedef struct
{
    float weight[LS];
    float bias;
    float alpha;  // Learnable leakage factor

} neuron_t;

typedef struct
{
    neuron_t neuron[HN];
    float memory[LS];

} layer_t;

typedef struct
{
    float mean[LS];  // Mean values for latent representation
    float deviation[LS]; // Standard deviation for controlled variation

} latent_t;

typedef struct
{
    layer_t layer[HL];  // Stacked layers for deeper learning
    latent_t latent;

    float loss;

} network_t;

void init_neuron(neuron_t*);
void init_layer(layer_t*);
void init_network(network_t*);
float neuron_forward(const neuron_t*, const float*);
void layer_forward(const layer_t*, const float*, float*);
void compute_latent_mean(network_t*);
void compute_latent_deviation(network_t*); 
void network_forward(network_t*, const float*);
float mse_loss(const float*, const float*, const int);
void update_weights(neuron_t*, const float*, const float, const float, const float, const float);
void train_network(network_t*, const float*, const float*, const float, const bool); 
