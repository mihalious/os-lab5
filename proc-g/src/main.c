#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int32_t g(int32_t x) {
    sleep(5);
    return x;
}

#define SOCKET_NAME "../my-socket"
int main() {
    int32_t g_res = g(10);

    int32_t connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connection_socket == -1) {
        perror("socket\n");
        return 1;
    }

    const struct sockaddr_un addr = {.sun_family = AF_UNIX,
                                     .sun_path = SOCKET_NAME};
    int32_t connect_err = connect(connection_socket,
                                  (const struct sockaddr *)&addr, sizeof(addr));
    if (connect_err == -1) {
        perror("connect\n");
        close(connection_socket);
        return 1;
    }
    int32_t write_err = write(connection_socket, &g_res, sizeof(g_res));
    if (write_err == -1) {
        perror("write\n");
        close(connection_socket);
        return 1;
    }
    close(connection_socket);
}
