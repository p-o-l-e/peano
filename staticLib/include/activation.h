#include <math.h>

float sigmoid(const float x)
{
    return 1.0f / (1.0f + exp(-x));
}

float swish(const float x) 
{
    return x / (1.0f + expf(-x));
}

float LReLU(const float x, const float leak = 0.01f) 
{
    return (x > 0.0f) ? x : x * leak;
}

float TANH(const float x) 
{
    return tanhf(x);
}

float ReLU(const float x) 
{
    return x > 0.0f ? x : 0.0f;
}

float eLU(const float x, const float alpha) 
{
    return x > 0 ? x : alpha * (expf(x) - 1.0f);
}

float softplus(const float x) 
{
    return logf(1.0f + expf(x));
}

float gelu(const float x) 
{
    return 0.5f * x * (1.0f + tanhf(0.79788456f * (x + 0.044715f * x * x * x)));
}