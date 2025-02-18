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

echo "Cleaned all artifacts"
