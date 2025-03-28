cd ../components

cd common/mtc
make
cd -

cd memory-tracker
make
cd -

cd memory-simulator
make
cd -

cd memory-monitor
make
cd -


mv memory-tracker/lib_mem_tracker.so .
mv memory-simulator/memorysimulator .
mv memory-monitor/mem_monitor .
mv common/mtc/libmtc_decoder.a .

echo "All artifacts built"