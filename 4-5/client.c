#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int client_socket;

void DieWithError(char *errorMessage) {
    perror(errorMessage);
    exit(1);
}

int main(int argc, char *argv[]) {
    // проверка на количество входных данных
    if (argc != 4) {
        printf("Usage: <%s> <server ip> <sever port> <client id>\n", argv[0]);
        exit(1);
    }
    int id = atoi(argv[3]);
    struct sockaddr_in server_address;
    
    // создание клиентского сокета
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        DieWithError("socket() failed");
    }
    
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(argv[2]));
    
    // Преобразование IP-адреса
    if (inet_pton(AF_INET, argv[1], &server_address.sin_addr) <= 0) {
        DieWithError("inet_pton() failed");
    }
    
    // подключение к серверу
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        DieWithError("connect() failed");
    }
    
    printf("Connection to the server: success\n");
    int stolen_items;
    
    while (1) {
        // получение количества украденных предметов
        if (recv(client_socket, &stolen_items, sizeof(int), 0) > 0) {
            if (stolen_items == -1) {
                break;
            }
            int cost = 0;
            if (id == 0) {
                printf("Ivanov stole\n");
            } else if (id == 1) {
                printf("Petrov loaded\n");
            } else if (id == 2) {
                printf("Necheporuk on the lookout\n");
                ++stolen_items;
                cost = rand() % 1000;
                printf("cost of item: %d\n", cost);
            }
            
            // отправка количества украденных предметов
            send(client_socket, &stolen_items, sizeof(stolen_items), 0);
            printf("stolen_items: %d, sent to server\n", stolen_items);
            
            // отправка стоимсоти предмета
            send(client_socket, &cost, sizeof(cost), 0);
        }
    }
    printf("finishing...");
    exit(0);
}
