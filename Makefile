transfer:
	./transfer.sh

execute:
	g++ -shared -fPIC -o interception.so interception.cpp -I/usr/local/cuda-12.6/include -L/usr/local/cuda-12.6/lib64/stubs -lcuda -std=c++17
	LD_PRELOAD="./interception.so" ./firecracker_test

source:
	g++ -o firecracker_test firecracker_test.cpp -I/usr/local/cuda-12.6/include -L/usr/local/cuda-12.6/lib64/stubs -lcuda -std=c++17

test:
	LD_PRELOAD="./interception.so" ./firecracker_test

interception:
	g++ -shared -fPIC -o interception.so interception.cpp -I/usr/local/cuda-12.6/include -L/usr/local/cuda-12.6/lib64/stubs -lcuda -std=c++17

interception_runtime:
	g++ -shared -fPIC -o interception_runtime.so interception_runtime.c -I/usr/local/cuda-12.6/include -L/usr/local/cuda-12.6/lib64/stubs -lcuda -std=c++17
