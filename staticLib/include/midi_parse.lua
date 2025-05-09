local midi = require("midi")

-- Configuration
local POLYPHONY = 8  -- Max voices per cluster
local OUTPUT_FILE = "dataset.h"  -- Header file output
local note_clusters = {}

-- Function to normalize MIDI values
local function encode(value, min, max)
    return 2.0 * ((value - min) / (max - min)) - 1.0
end

-- Callback function for parsing MIDI events
local function parse_midi_event(eventType, channel, note, velocity)
    if eventType == "noteOn" then
        local note_entry = {
            pitch = encode(note, 0, 127),
            velocity = encode(velocity, 0, 127),
            gate = encode(500, 0, 5000),  -- Placeholder gate duration
            latitude = encode(channel, 0, 15),  -- Basic timing information
            detune = encode(0, 0, 16383)  -- Default detune
        }

        -- Add notes to clusters
        if not note_clusters[#note_clusters] or #note_clusters[#note_clusters].note >= POLYPHONY then
            table.insert(note_clusters, {note = {}})
        end
        table.insert(note_clusters[#note_clusters].note, note_entry)
    end
end

-- Process the MIDI file
local function process_midi(midi_file_path)
    local file = assert(io.open(midi_file_path, "rb"))
    midi.process(file, parse_midi_event)  -- Use callback for MIDI note extraction
    file:close()
end

-- Write the dataset to a `.h` file
local function write_header_file()
    local file = io.open(OUTPUT_FILE, "w")

    file:write("#pragma once\n\n")
    file:write("#define DATASET_SIZE " .. #note_clusters .. "\n\n")
    file:write("typedef struct {\n    float pitch;\n    float detune;\n    float gate;\n    float velocity;\n    float latitude;\n} note_t;\n\n")
    file:write("typedef struct {\n    note_t note[" .. POLYPHONY .. "];\n} note_cluster_t;\n\n")
    file:write("note_cluster_t dataset[DATASET_SIZE] = {\n")

    for _, cluster in ipairs(note_clusters) do
        file:write("    { { ")
        for _, note in ipairs(cluster.note) do
            file:write(string.format("{ %.3f, %.3f, %.3f, %.3f, %.3f }",
                note.pitch, note.detune, note.gate, note.velocity, note.latitude))
            if _ < #cluster.note then file:write(", ") end
        end
        file:write(" } },\n")
    end

    file:write("};\n")
    file:close()
end

-- Example usage
local midi_file_path = "file.mid"
process_midi(midi_file_path)
write_header_file()
print("Dataset saved to " .. OUTPUT_FILE)
