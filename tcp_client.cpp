#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstdio>
#include <chrono>

#pragma comment(lib, "Ws2_32.lib")

int main() {
    printf("Client starting...\n");
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
    SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectSocket == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    printf("Socket created\n");
    fflush(stdout);

    // Налаштування адреси сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888); // Використовуємо той самий порт, що й на сервері

    // Перетворення IP-адреси
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) != 1) {
        printf("inet_pton failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }
    printf("Server address set\n");
    fflush(stdout);

    // Підключення до сервера
    printf("Connecting to server...\n");
    fflush(stdout);

    if (connect(connectSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Unable to connect to server: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    printf("Connected to server!\n");
    fflush(stdout);

    // Підготовка даних - три double і один long (у масиві double для простоти)
    double numbers[4] = {3.14159, 2.71828, 1.41421, 12345.0}; // Останнє число представляє long

    // Вимірювання часу
    auto start = std::chrono::high_resolution_clock::now();

    // Надсилання всіх чисел одразу
    int bytesSent = send(connectSocket, (char*)numbers, sizeof(numbers), 0);

    if (bytesSent == SOCKET_ERROR) {
        printf("Send failed with error: %d\n", WSAGetLastError());
    } else {
        printf("Sent %d bytes\n", bytesSent);

        // Отримання відповіді
        double response[4];
        int bytesReceived = recv(connectSocket, (char*)response, sizeof(response), 0);

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        if (bytesReceived > 0) {
            printf("Received response, %d bytes\n", bytesReceived);
            for (int i = 0; i < 4; i++) {
                printf("Number %d: %f\n", i+1, response[i]);
            }
            printf("Total time: %lld ms\n", elapsed);
        } else {
            printf("Receive failed with error: %d\n", WSAGetLastError());
        }
    }

    fflush(stdout);

    // Закриття сокету
    closesocket(connectSocket);
    WSACleanup();

    printf("Client shutdown.\n");
    fflush(stdout);

    system("pause");
    return 0;
}