DEBUG_DIR="build/debug"
if [ ! -d "$DEBUG_DIR" ]; then
    cmake --preset debug
fi
cmake --build --preset debug
./build/debug/bin/app index -i index -c books.csv
python3 pythonscripts/main.py -i index