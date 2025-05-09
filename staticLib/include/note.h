#pragma once
#include "math.h"
#include "chromatic.h"

#define NF              5                                               // Note factors: [pitch, detune, gate, velocity, latitude]

#define CHROMA_MAX      (sizeof(chromatic) / sizeof(chromatic[0]))
#define DETUNE_MAX      16383.0f                                        // 0 |<----------- |8192| ----------->| 16384
#define VELOCITY_MAX    127.0f
#define GATE_MAX        5000.0f                                         // Maximum gate duration in milliseconds
#define LATITUDE_MAX    65535.0f                                        // Maximum time before next note (milliseconds)
#define POLYPHONY       8

typedef struct 
{ 
    float pitch; 
    float detune;
    float gate;
    float velocity;
    float latitude;

} note_t;

typedef struct 
{ 
    unsigned pitch; 
    unsigned detune;
    unsigned gate;
    unsigned velocity;
    unsigned latitude;

} midi_note_t;

typedef struct 
{
    note_t note[POLYPHONY];

} note_cluster_t;

typedef struct 
{
    midi_note_t note[POLYPHONY];

} midi_note_cluster_t;


float encode(float value, float min, float max) 
{
    return 2.0f * ((value - min) / (max - min)) - 1.0f;
}


float decode(float value, float min, float max) 
{
    return min + (value + 1.0f) * 0.5f * (max - min);
}

note_t encode_note(const midi_note_t* midi_note)
{
    note_t r;

    int pitch_index = midi_note->pitch;
    pitch_index = pitch_index < 0 ? 0 : pitch_index >= CHROMA_MAX ? CHROMA_MAX - 1 : pitch_index;

    r.pitch     = encode((float)pitch_index,            0.0f, (float)(CHROMA_MAX - 1));
    r.detune    = encode((float)midi_note->detune,      0.0f, DETUNE_MAX);
    r.velocity  = encode((float)midi_note->velocity,    0.0f, VELOCITY_MAX);
    r.gate      = encode((float)midi_note->gate,        0.0f, GATE_MAX);
    r.latitude  = encode((float)midi_note->latitude,    0.0f, LATITUDE_MAX);  

    return r;
}


midi_note_t decode_note(const note_t* encoded_note)
{
    midi_note_t r;

    int pitch_index = (int)decode(encoded_note->pitch, 0.0f, (float)(CHROMA_MAX - 1));
    pitch_index = pitch_index < 0 ? 0 : pitch_index >= CHROMA_MAX ? CHROMA_MAX - 1 : pitch_index;
    
    r.pitch     = pitch_index;
    r.detune    = (unsigned)lroundf(decode(encoded_note->detune,    0.0f, DETUNE_MAX));
    r.velocity  = (unsigned)lroundf(decode(encoded_note->velocity,  0.0f, VELOCITY_MAX));
    r.gate      = (unsigned)lroundf(decode(encoded_note->gate,      0.0f, GATE_MAX));  
    r.latitude  = (unsigned)lroundf(decode(encoded_note->latitude,  0.0f, LATITUDE_MAX));

    return r;
}


note_cluster_t encode_cluster(const midi_note_cluster_t* midi_cluster)
{
    note_cluster_t encoded_cluster;
    
    for (int i = 0; i < POLYPHONY; i++) 
    {
        encoded_cluster.note[i] = encode_note(&midi_cluster->note[i]);  // Encode each note separately
    }
    
    return encoded_cluster;
}

midi_note_cluster_t decode_cluster(const note_cluster_t* encoded_cluster)
{
    midi_note_cluster_t midi_cluster;
    
    for (int i = 0; i < POLYPHONY; i++) 
    {
        midi_cluster.note[i] = decode_note(&encoded_cluster->note[i]);  // Decode each note separately
    }
    
    return midi_cluster;
}
