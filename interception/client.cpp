#include <iostream>
#include <sys/socket.h>
#include <linux/vm_sockets.h>
#include <unistd.h>
#include <cstring>

#define VSOCK_HOST_CID 2
#define VSOCK_PORT 1234

int main() {
    int sock = socket(AF_VSOCK, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_vm sa = {};
    sa.svm_family = AF_VSOCK;
    sa.svm_cid = VSOCK_HOST_CID;
    sa.svm_port = VSOCK_PORT;

    std::cout << "Connecting to host..." << std::endl;
    if (connect(sock, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
        perror("connect");
        return 1;
    }

    const char* message = "Hello from guest!";
    send(sock, message, strlen(message), 0);

    char buffer[128] = {};
    recv(sock, buffer, sizeof(buffer), 0);
    std::cout << "Received from host: " << buffer << std::endl;

    close(sock);
    return 0;
}

