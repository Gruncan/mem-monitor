cd ../components

cd memory-tracker
make clean
cd -

cd memory-simulator
make clean
cd -

cd memory-monitor
make clean
cd -

cd common/mtc
make clean
cd -

rm lib_mem_tracker.so libmtc_decoder.a mem_monitor memorysimulator

echo "Cleaned all artifacts"
