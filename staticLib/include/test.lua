local midi = require("midi")

local file = assert(io.open("file.mid", "rb"))
midi.process(file, print)  -- Should print all parsed MIDI events
file:close()

