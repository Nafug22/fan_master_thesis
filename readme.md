To run this project:

1. on the host
- clone the repo
- `mkdir build` & `cd build`
- `cmake ..` to configure
- `cd interception`
- `make cuda_server` to compile the cuda server
- `./cuda_server` to run the executable (the server should be run before the execution on guest)

2. on the guest
- run `ln -sf /usr/local/cuda-12.6/lib64/stubs/libcuda.so /usr/lib/x86_64-linux-gnu/libcuda.so` for subsitution
- run `export LD_LIBRARY_PATH=/usr/local/cuda-12.6/lib64/stubs:$LD_LIBRARY_PATH` for runtime alias
- clone the repo
- `mkdir build` & `cd build`
- `cmake ..` to configure
- `cd interception`
- `make interception` to get the interception library
- in the `{PROJECT_DIR}/tests/test_firecracker/`, run `g++ -o firecracker_test firecracker_test.cpp -I/usr/local/cuda-12.6/include -L/usr/local/cuda-12.6/lib64/stubs -lcuda -std=c++17`
- run `LD_PRELOAD="{PROJECT_DIR}/build/interception/interception.so" ./firecracker_test` to observe the execution