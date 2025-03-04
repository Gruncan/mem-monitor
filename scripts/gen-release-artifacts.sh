cd ../components

cd memory-tracker
make FPATH=/home/instrument/fw/memory_tracker1.tmtc INDEX=34
cd -

cd memory-simulator
make
cd -

cd memory-monitor
make
cd -

cd common/mtc
make
cd -


mv memory-tracker/lib_mem_tracker.so .
mv memory-simulator/memorysimulator .
mv memory-monitor/mem_monitor .
mv common/mtc/libmtc_decoder.a .

echo "All artifacts built"