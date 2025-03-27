#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstdio>

#pragma comment(lib, "Ws2_32.lib")

int main() {
    printf("Server starting...\n");
    fflush(stdout);

    // Ініціалізація Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }
    printf("WSA started\n");
    fflush(stdout);

    // Створення сокета
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    printf("Socket created\n");
    fflush(stdout);

    // Налаштування адреси
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(8888); // Змінюємо порт на інший

    // Прив'язка сокета
    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    printf("Socket bound\n");
    fflush(stdout);

    // Прослуховування
    if (listen(listenSocket, 1) == SOCKET_ERROR) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Server is running. Waiting for connection on port 8888...\n");
    fflush(stdout);

    // Прийняття з'єднання
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

    if (clientSocket == INVALID_SOCKET) {
        printf("Accept failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("Client connected!\n");
    fflush(stdout);

    // Просто отримуємо масив чисел для тесту
    double numbers[4];
    int bytesReceived = recv(clientSocket, (char*)numbers, sizeof(numbers), 0);

    if (bytesReceived > 0) {
        printf("Received %d bytes\n", bytesReceived);
        for (int i = 0; i < 4; i++) {
            printf("Number %d: %f\n", i+1, numbers[i]);
        }

        // Відправляємо назад
        send(clientSocket, (char*)numbers, sizeof(numbers), 0);
        printf("Sent response back\n");
    } else {
        printf("Receive failed with error: %d\n", WSAGetLastError());
    }

    fflush(stdout);

    // Закриття сокетів
    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();

    printf("Server shutdown.\n");
    fflush(stdout);

    system("pause");
    return 0;
}