#define _POSIX_C_SOURCE 200809L

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "./my-socket"

void *thread_run(void *args) {
    for (;;) {
        sleep(10);
        printf(
            "Press 1 to continue, 2 to stop, 3 to continue without asking\n");

        size_t len = 2;
        char *buf = nullptr;
        getline(&buf, &len, stdin);
        int32_t i = buf[0];

        if (i == '2') {
            unlink(SOCKET_NAME);
            exit(0);
        } else if (i == '3') {
            free(buf);
            return nullptr;
        }
    }
}

int32_t main() {
    pthread_t handle;
    pthread_create(&handle, nullptr, thread_run, nullptr);
    // pthread_join(handle, nullptr);

    int32_t connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connection_socket == -1) {
        perror("socket\n");
        return 1;
    }
    const struct sockaddr_un addr = {.sun_family = AF_UNIX,
                                     .sun_path = SOCKET_NAME};
    int32_t bind_err =
        bind(connection_socket, (const struct sockaddr *)&addr, sizeof(addr));
    if (bind_err == -1) {
        perror("bind\n");
        close(connection_socket);
        return 1;
    }
    int32_t listen_err = listen(connection_socket, 2);
    if (listen_err == -1) {
        perror("listen\n");
        close(connection_socket);
        unlink(SOCKET_NAME);
        return 1;
    }

    struct sockaddr_un peer_addr = {};
    socklen_t peer_addr_len = sizeof(peer_addr);
    int32_t results[2] = {};
    for (size_t i = 0; i < 2; i++) {
        int32_t data_socket = accept(
            connection_socket, (struct sockaddr *)&peer_addr, &peer_addr_len);
        if (data_socket == -1) {
            perror("accept\n");
            close(connection_socket);
            unlink(SOCKET_NAME);
            return 1;
        }
        int32_t read_err = read(data_socket, &results[i], sizeof(results[i]));
        if (read_err == -1) {
            perror("read\n");
            close(data_socket);
            close(connection_socket);
            unlink(SOCKET_NAME);
            return 1;
        }
        printf("value read from socket: %d\n", results[i]);
        if ((bool)results[i] == true) {
            printf("f(x) || g(x) == true\n");
            close(data_socket);
            close(connection_socket);
            unlink(SOCKET_NAME);
            return 0;
        }
        close(data_socket);
    }

    printf("f(x) || g(x) == false\n");
    close(connection_socket);
    unlink(SOCKET_NAME);

    exit(0);
}
