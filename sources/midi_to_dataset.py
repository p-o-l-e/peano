
# midi_to_dataset.py - Converts MIDI file to dataset.h format with proper timing

import sys
import mido

# Constants
CHROMA_MAX = 127.0
DETUNE_MAX = 16383.0
VELOCITY_MAX = 127.0
GATE_MAX = 5000.0
OFFSET_MAX = 960 * 2
POLYPHONY = 8

# Encoding function
def encode(value, max_val):
    return 2.0 * (value / max_val) - 1.0

def parse_midi(file_path):
    """Parses MIDI file and extracts structured note data."""
    mid = mido.MidiFile(file_path)
    dataset = []
    active_notes = {}
    absolute_time = 0

    for msg in mid:
        
        if msg.type == 'note_on' and msg.velocity > 0:
            absolute_time += encode(msg.time, OFFSET_MAX)  # Track absolute time correctly
            pitch = encode(msg.note, CHROMA_MAX)
            velocity = encode(msg.velocity, VELOCITY_MAX)
            offset = absolute_time

            active_notes[msg.note] = {'pitch': pitch, 'velocity': velocity, 'offset': offset, 'start_time': absolute_time}

        elif msg.type == 'note_off' and msg.note in active_notes:
            gate_duration = absolute_time - active_notes[msg.note]['start_time']  # Use real timing
            gate = encode(gate_duration, GATE_MAX)
            
            note_data = (
                active_notes[msg.note]['pitch'],
                encode(0, DETUNE_MAX),  # Detune remains fixed
                gate,
                active_notes[msg.note]['velocity'],
                active_notes[msg.note]['offset']
            )

            dataset.append(note_data)
            del active_notes[msg.note]  # Clean up processed note

    return dataset

def write_dataset(dataset, output_file):
    """Writes dataset to dataset.h file, matching structured arpeggio format."""
    dataset_size = len(dataset) // POLYPHONY
    header = f"#pragma once\n\n#include \"note.h\"\n\n#define DATASET_SIZE {dataset_size}\n\nnote_cluster_t dataset[DATASET_SIZE] = {{\n"

    clusters_str = ",\n".join([
        "{{\n" + ",\n".join(["{" + ", ".join([f"{value:.3f}" for value in note]) + "}" for note in dataset[i:i+POLYPHONY]]) + "\n}}"
        for i in range(0, len(dataset), POLYPHONY)
    ])

    footer = "};\n"

    with open(output_file, "w") as f:
        f.write(header + clusters_str + footer)

    print(f"Generated {output_file} successfully.")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python midi_to_dataset.py <input.mid>")
        sys.exit(1)

    midi_file = sys.argv[1]
    output_file = "dataset.h"
    
    dataset = parse_midi(midi_file)
    write_dataset(dataset, output_file)

