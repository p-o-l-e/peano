#pragma once
#include <math.h>

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

float smooth_l1_loss(const float original[], const float reconstructed[], const int size) 
{
    float loss = 0.0f;
    for (int i = 0; i < size; ++i) 
    {
        float diff = fabsf(original[i] - reconstructed[i]);
        loss += (diff < 1.0f) ? 0.5f * diff * diff : (diff - 0.5f);
    }
    return loss / size;
}

float huber_loss(const float original[], const float reconstructed[], const int size, const float delta = 0.01f) 
{
    float loss = 0.0f;
    for (int i = 0; i < size; i++) 
    {
        float diff = original[i] - reconstructed[i];
        loss += (fabsf(diff) <= delta) ? 0.5f * diff * diff : delta * (fabsf(diff) - 0.5f * delta);
    }
    return loss / size;
}

float log_cosh_loss(const float original[], const float reconstructed[], const int size) 
{
    float loss = 0.0f;
    for (int i = 0; i < size; i++) 
    {
        float diff = original[i] - reconstructed[i];
        loss += logf(coshf(diff));
    }
    return loss / size;
}
