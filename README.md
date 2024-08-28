### Rect pack

Library for packing rectangles in circle segments.

Tested on: 
- Ubuntu 24.04 
- emcc 3.1.64 
- gcc 13.2
- cmake 3.28.3

### Compile native code
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --target rp_native -j`nproc`
```

### Run tests
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build --target rp_test -j`nproc`
```

### Compile wasm module
```bash
emcmake cmake -S . -B build_em -DCMAKE_BUILD_TYPE=Release && cmake --build build_em --target rp_wasm -j`nproc`
```

Used to create: https://rkest.github.io/skillscatter/

