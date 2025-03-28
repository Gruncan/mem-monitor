
# User manual 


## MemoryMonitor
Can be run with the following arguements:
```
-t -> The time delay between reads
-f -> Output to filename to record the information
-p -> The command to, fork, execute and and pull process specific data
-i -> The already started PID of a process to pull specific data for
-n -> The already started process name to pull specific data for (this requires pgrep)
```

## MemoryTracker
Tracking of a binary can be done with the following
```
$ LD_PRELOAD=./lib_mem_tracker.so ./program_to_track
```
this log will be named by default `memory_tracker1.tmtc` where `1` will auto increment if the file number is found.

This name is configurable at compilation with env variables 
- `FPATH` - relative filename
- `INDEX` - the index of which to increment the value of

## MemorySimulator
Simulation of a tracking file can be done with the following
```
./memorysimulator <filename>.tmtc <nodelay|real> <single|repeat>
```
order of parameter is required, no parameter is optional

## MemoryGUI
The GUI is simple to run, annoying to compile :)
```
./MemoryGUI
```

## Decoder library
Utilisation this requires normal C linkage, see components/common/mtc/tests for example of how this was done and used.

