#!/usr/bin/env bash
EMSDK_ROOT="./emsdk"
if [ ! -d "$EMSDK_ROOT" ]; then echo "Error: EMSDK_ROOT ($EMSDK_ROOT) not found."; exit 1; fi
echo "Using EMSDK_ROOT: $EMSDK_ROOT"

if ! command -v emcc &> /dev/null; then
    echo "Warning: emcc not found in your current PATH. Attempting to use full path."
    EMCC_CMD="$EMSDK_ROOT/upstream/bin/emcc"
    if [ ! -x "$EMCC_CMD" ]; then echo "Error: emcc executable not found at $EMCC_CMD. Please verify your EMSDK_ROOT."; exit 1; fi
else
    EMCC_CMD="emcc"
fi

EMSDK_PATH="$EMSDK_ROOT" "$EMCC_CMD" \
    src/Candle.c \
    src/Client.c \
    src/Integrity.c \
    src/Order.c \
    src/OrderFactory.c \
    src/PairConvertor.c \
    src/Uuid.c \
    -I./src \
    -I./src/include \
    -I/usr/local/include \
    -o sfix.js \
    -s WASM=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MODULARIZE=1 \
    -s EXPORT_ES6=1 \
    -s EXPORTED_FUNCTIONS="[ \
        '_SFIX_estimateMessageSize', \
        '_SFIX_compose', \
        '_SFIX_listLength', \
        '_SFIX_parse', \
        '_SFIX_print', \
        '_SFIX_acknowledge', \
        '_SFIX_unknown', \
        '_SFIX_strategy', \
        '_SFIX_error', \
        '_malloc', \
        '_free' \
    ]" \
    -s EXTRA_EXPORTED_RUNTIME_METHODS="['stringToUTF8', 'UTF8ToString']" 

if [ $? -eq 0 ]; then
    echo "Compilation successful! Output: my_library.js and my_library.wasm"
else
    echo "Compilation failed!"
    exit 1
fi
