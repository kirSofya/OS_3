#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int server_socket;

void DieWithError(char *errorMessage) {
    perror(errorMessage);
    exit(1);
}

int main(int argc, char *argv[]) {
    // проверка на количество входных данных
    if (argc != 4) {
        printf("Usage: <%s> <port> <n> <id>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    int n = atoi(argv[2]);
    int id = atoi(argv[3]);
    
    // проверка на правильность id
    if (id != 3) {
        DieWithError("Incorrect id");
    }

    ssize_t for_read;
    int new_socket[id];
    struct sockaddr_in broadcast_addr;
    int addrlen = sizeof(broadcast_addr);

    // создание серверного сокета
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        DieWithError("socket() failed");
    }

    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = INADDR_ANY;
    broadcast_addr.sin_port = htons(port);

    // привязка серверного сокета к IP-адресу и порту
    if (bind(server_socket, (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
        DieWithError("blind() failed");
    }

    // прослушивание входящих подключений
    if (listen(server_socket, id) < 0) {
        DieWithError("listen() failed");
    }

    printf("Server is waiting for clients...\n");

    for (int i = 0; i < 3; ++i) {
        // подключение клиентов
        if ((new_socket[i] = accept(server_socket, (struct sockaddr *)&broadcast_addr, (socklen_t *)&addrlen)) < 0) {
            DieWithError("accept() failed");
        }
        printf("Client %d connected\n", i + 1);
    }

    int stolen_items = 0;
    int client_id = 0;
    int sum = 0;
    while (stolen_items < n) {
        for (int i = 0; i < 3; ++i) {
            send(new_socket[client_id], &stolen_items, sizeof(int), 0);
            int res;
            // получаем значение количества украденных предметов
            for_read = recv(new_socket[client_id], &res, sizeof(res), 0);
            if (for_read != -1) {
                stolen_items = res;
            }
            // получаем стоимость украденного прежмета
            for_read = recv(new_socket[client_id], &res, sizeof(res), 0);
            // добавляем в сумарную стоимость
            sum += res;
            if (res != 0) {
                printf("cost of %d item is %d\n", stolen_items, res);
            }
            // переходим к следующему клиенту
            client_id = (client_id + 1) % 3;
        }
    }
    
    printf("client_id = %d\n", client_id);
    printf("stolen_items = %d\n", stolen_items - 1);

    printf("TOTAL COST: %d\n", sum);
    int for_exit = -1;
    for (int i = 0; i < 3; ++i) {
        // завершение программы
        send(new_socket[i], &for_exit, sizeof(int), 0);
    }
    close(server_socket);
    exit(0);
}
