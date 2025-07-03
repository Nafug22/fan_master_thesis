execute:
	g++ -shared -fPIC -o interception.so interception.cpp -I/usr/local/cuda/include -L/usr/local/cuda-12.2/compat -lcuda -std=c++17
	LD_PRELOAD="./interception.so" ./firecracker_test

source:
	g++ -o firecracker_test firecracker_test.cpp -I/usr/local/cuda/include -L/usr/local/cuda-12.2/compat -lcuda -std=c++17

test:
	LD_PRELOAD="./interception.so" ./firecracker_test