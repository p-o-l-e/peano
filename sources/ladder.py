
# ladder.py - Generates dataset.h with time-shifted chromatic arpeggio

import numpy as np

# Constants
DATASET_SIZE = 6
POLYPHONY = 8
CHROMA_MAX = 127.0
DETUNE_MAX = 16383.0
VELOCITY_MAX = 127.0
GATE_MAX = 5000.0
OFFSET_MAX = 960 * 2

# Encode function
def encode(value, max_val):
    return 2.0 * (value / max_val) - 1.0

# Generate dataset
base_pitch_C3 = 48  # MIDI pitch for C3
dataset = []
timestamp = 0
time_step = 240  # Progressive offset step

for cluster_index in range(DATASET_SIZE):
    cluster = []
    for voice_index in range(POLYPHONY):
        pitch = encode(base_pitch_C3 + (cluster_index * POLYPHONY + voice_index), CHROMA_MAX)
        detune = encode(0, DETUNE_MAX)
        gate = encode(240, GATE_MAX)
        velocity = encode(120, VELOCITY_MAX)
        offset = encode(timestamp, OFFSET_MAX)  # Time-shifted offsets
        timestamp += time_step
        cluster.append((pitch, detune, gate, velocity, offset))
    dataset.append(cluster)

# Generate 'dataset.h'
header = """#pragma once\n\n#include "note.h"\n\n#define DATASET_SIZE {}\n\nnote_cluster_t dataset[DATASET_SIZE] = {{\n""".format(DATASET_SIZE)

clusters_str = ",\n".join(["{{\n" + ",\n".join(["{" + ", ".join([f"{value:.3f}" for value in note]) + "}" for note in cluster]) + "\n}}" for cluster in dataset])

footer = "};\n"

with open("dataset.h", "w") as f:
    f.write(header + clusters_str + footer)

print("Generated dataset.h with time-shifted notes successfully.")

