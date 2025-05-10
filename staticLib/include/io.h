
#include "midifile.h"

void write_midi_file(const char* filename, const note_cluster_t dataset[], size_t dataset_size, unsigned tempo) 
{
    smf::MidiFile midi;
    midi.setTicksPerQuarterNote(960);  // Standard resolution

    for (size_t i = 0; i < dataset_size; ++i) 
    {
        midi_note_cluster_t midi_cluster = decode_cluster(&dataset[i]);  // Convert to MIDI-compatible format

        for (size_t j = 0; j < POLYPHONY; ++j) 
        {
            const midi_note_t *note = &midi_cluster.note[j];
            if (note->velocity == 0 || note->gate == 0) continue; 
            midi.addNoteOn(0, note->offset, 0, note->pitch, note->velocity);  // Note ON event for each note in the cluster
            midi.addNoteOff(0, note->offset + note->gate, 0, note->pitch);  // Note OFF event to end note
        }
    }

    midi.sortTracks();  // Ensure correct ordering of events
    midi.write(filename);
}
