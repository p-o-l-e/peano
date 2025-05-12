#pragma once
#include <stdint.h>
#include <math.h>

#define NF              5                                               // Note factors: [pitch, detune, gate, velocity, offset]
#define CHROMA_MAX      127.0f
#define DETUNE_MAX      16383.0f                                        // 0 |<----------- |8192| ----------->| 16384
#define VELOCITY_MAX    127.0f
#define GATE_MAX        5000.0f                                         // Maximum gate duration in milliseconds
#define PPQ             960                                             // Pulses Per Quarter
#define OFFSET_MAX      PPQ * 2                                         // 0 |<----------- |960| ----------->| 1920
#define POLYPHONY       8


typedef struct 
{ 
    float pitch; 
    float detune;
    float gate;
    float velocity;
    float offset;

} note_t;

typedef struct
{
    uint8_t status;                     // Status byte (Note On, Note Off, Control Change, etc.)
    uint8_t channel;                    // MIDI channel (0–15)
    uint8_t data1;                      // First data byte (e.g., pitch for Note On, control number for CC)
    uint8_t data2;                      // Second data byte (e.g., velocity for Note On, control value for CC)
    uint16_t delta;                     // Timing in MIDI ticks 

} midi_message_t;

typedef struct 
{
    note_t note[POLYPHONY];

} note_cluster_t;

typedef struct 
{
    midi_message_t on;
    midi_message_t off;

} midi_pair_t;


float encode(const float value, const float max) // [ -1.0f ... 1.0f ]
{
    return 2.0f * (value / max) - 1.0f;
}

float decode(const float value, const float max) // [ 0.0f ... max ]
{
    return (value + 1.0f) * 0.5f * max;
}


midi_pair_t convert_note_to_midi(const note_cluster_t* cluster, uint8_t voice, uint8_t channel)
{
    midi_pair_t msg = { 0 };
    if (voice >= POLYPHONY) return msg;
    const note_t* note = &cluster->note[voice];
    if (note->velocity == 0 || note->gate == 0) return msg;

    uint8_t  pitch         = (uint8_t) lroundf(decode(note->pitch,    CHROMA_MAX));
    uint8_t  velocity      = (uint8_t) lroundf(decode(note->velocity, VELOCITY_MAX));
    uint16_t offset_ticks  = (uint16_t)lroundf(decode(note->offset,   OFFSET_MAX));
    uint16_t gate_ticks    = (uint16_t)lroundf(decode(note->gate,     GATE_MAX));

    msg.on  = (midi_message_t){ .status = 0x90, .channel = channel, .data1 = pitch, .data2 = velocity, .delta = offset_ticks };
    msg.off = (midi_message_t){ .status = 0x80, .channel = channel, .data1 = pitch, .data2 = 0,        .delta = offset_ticks + gate_ticks };

    return msg;
}

note_t convert_note_from_midi(const midi_pair_t* midi_pair, uint16_t base_time)
{
    note_t note = { 0 };

    if (midi_pair->on.status != 0x90 || midi_pair->off.status != 0x80) return note;  // Validate MIDI Note On/Off

    note.pitch     = encode((float)midi_pair->on.data1, CHROMA_MAX);
    note.velocity  = encode((float)midi_pair->on.data2, VELOCITY_MAX);
    uint16_t gate  = midi_pair->off.delta - midi_pair->on.delta;
    note.gate      = encode((float)gate, GATE_MAX);
    int16_t offset = midi_pair->on.delta - base_time;
    note.offset    = encode((float)offset, OFFSET_MAX);

    return note;
}
