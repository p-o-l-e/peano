
local midi = require("midi")

-- Configuration
local OUTPUT_MIDI_FILE = "generated.mid"
local POLYPHONY = 8  -- Max voices per cluster

-- Reverse normalization function
local function decode(value, min, max)
    return (value + 1) * 0.5 * (max - min) + min
end

-- Example neural network output (note_cluster_t)
local network_output = {
    { { {0.300, 0.000, 0.500, 0.650, 0.100}, {0.500, 0.000, 0.550, 0.500, 0.150} } },
    { { {0.700, 0.000, 0.600, 0.550, 0.200}, {0.400, 0.000, 0.450, 0.600, 0.250} } },
}

-- Function to convert network output into MIDI events

local function convert_to_midi(note_clusters)
    local midi_events = {}


for _, cluster in ipairs(note_clusters or {}) do
    for _, note in ipairs(cluster or {}) do

                print("Processing Note:", note)  -- Debugging line
                if note[1] then
                    local midi_note = math.floor(decode(note[1], 0, 127))
                    local velocity = math.floor(decode(note[4], 0, 127))
                    local duration = math.floor(decode(note[3], 100, 5000))
                    table.insert(midi_events, { "noteOn", 1, midi_note, velocity })
                    table.insert(midi_events, { "noteOff", 1, midi_note, velocity, duration })
                end
            end
        end
    -- end

    return midi_events
end


-- Function to write MIDI file using `midi.process()`

local function write_midi_file(midi_events)
    local file = assert(io.open(OUTPUT_MIDI_FILE, "wb"))
    assert(file, "Failed to open MIDI output file.")

    -- Write Header Chunk
    file:write("MThd")
    file:write(string.pack(">I4", 6))  -- Header Length
    file:write(string.pack(">I2", 1))  -- Format Type (Format 1 for multiple tracks)
    file:write(string.pack(">I2", 1))  -- **Only One Track** (Fixed to 1)
    file:write(string.pack(">I2", 480))  -- Division (Ticks per quarter note)

    -- Write Track Chunk
    file:write("MTrk")
    local track_data = {}  -- Store MIDI events

    -- Process Events

    for _, event in ipairs(midi_events) do
        print("Event Type:", event[1], "Note:", event[3], "Velocity:", event[4])
        if event[1] == "noteOn" then
            file:write(string.pack("BBB", 0x90, event[3], event[4]))
        elseif event[1] == "noteOff" then
            file:write(string.pack("BBB", 0x80, event[3], event[4]))
        end
    end


    -- Write Track Length
    file:write(string.pack(">I4", #table.concat(track_data)))

    -- Write Events
    for _, data in ipairs(track_data) do
        file:write(data)
    end

    -- End Track Event
    file:write(string.pack("BBB", 0xFF, 0x2F, 0x00))

    file:close()
    print("Generated MIDI saved to " .. OUTPUT_MIDI_FILE)
end


-- for i, cluster in ipairs(network_output) do
--     for j, note in ipairs(cluster) do
--         print(string.format("Cluster %d, Note %d: Pitch %.3f, Velocity %.3f, Gate %.3f",
--             i, j, note[1], note[4], note[3]))
--     end
-- end
-- Convert network output & save as MIDI file

print("Network Output:", network_output)
local midi_events = convert_to_midi(network_output)
print("MIDI Events:", midi_events)
write_midi_file(midi_events)

