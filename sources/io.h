#pragma once

#include <stdio.h>
#include "dataset.h"
#include "midifile/include/MidiFile.h"
#include "midifile/include/MidiEvent.h"
#include "midifile/include/MidiMessage.h"
#include "note.h"



void write_midi_file(const char* filename, const note_cluster_t dataset[], size_t dataset_size, unsigned bpm) 
{
    smf::MidiFile midi;
    midi.setTicksPerQuarterNote(PPQ);  // Standard PPQ resolution
    midi.addTrack();  // Create track 0
    // Add Tempo Meta Event (Convert BPM to microseconds per quarter note)
    uint32_t tempo_mpqn = (60000000 / bpm);
    std::vector<smf::uchar> tempoData = {0xFF, 0x51, 0x03, 
                                    (tempo_mpqn >> 16) & 0xFF, 
                                    (tempo_mpqn >> 8) & 0xFF, 
                                    tempo_mpqn & 0xFF};
    smf::MidiEvent tempoEvent(0, 0, tempoData);  // Correct constructor usage
    midi.addEvent(0, tempoEvent);

    uint32_t time = 0;  // Running timestamp in ticks

    for (size_t i = 0; i < dataset_size; ++i) 
    {
        for (size_t j = 0; j < POLYPHONY; ++j) 
        {
            midi_pair_t midi_pair = convert_note_to_midi(&dataset[i], j, 0);
            if (midi_pair.on.data2 == 0 || midi_pair.off.delta <= midi_pair.on.delta) continue; 
            // Note ON
            std::vector<smf::uchar> noteOnData = {midi_pair.on.status, midi_pair.on.data1, midi_pair.on.data2};
            smf::MidiEvent noteOnEvent(time, 0, noteOnData);
            midi.addEvent(0, noteOnEvent);
            // Note OFF
            std::vector<smf::uchar> noteOffData = {midi_pair.off.status, midi_pair.off.data1, 0};
            smf::MidiEvent noteOffEvent(time + midi_pair.off.delta, 0, noteOffData);
            midi.addEvent(0, noteOffEvent);

            time += midi_pair.on.delta;
        }
    }
    midi.sortTracks();  // Ensure correct ordering of events
    midi.write(filename);
}




