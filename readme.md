To run this project:

1. on the host
- `mkdir build` & `cd build`
- `cmake ..` to configure
- `cd interception`
- `make cuda_server` to compile the cuda server
- `./cuda_server` to run the executable

2. on the guest
- run `ln -sf /usr/local/cuda-12.6/lib64/stubs/libcuda.so /usr/lib/x86_64-linux-gnu/libcuda.so` for subsitution
- `mkdir build` & `cd build`
- `cmake ..` to configure
- `cd interception`
- `make interception` to get the interception library
- compile the cuda application