#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 5000
#define BUFFER_SIZE 1024

using namespace std;

int main() {
    // Ініціалізація Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed! Error: " << WSAGetLastError() << endl;
        return 1;
    }

    // Створення UDP сокета
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Failed to create socket! Error: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // Заповнення структури адреси сервера
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Прив'язка сокета
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed! Error: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "UDP Server is running on port " << PORT << "..." << endl;

    while (true) {
        sockaddr_in clientAddr = {};
        int clientAddrSize = sizeof(clientAddr);
        char buffer[BUFFER_SIZE];
        ZeroMemory(buffer, BUFFER_SIZE);

        // Отримання типу даних
        int bytesReceived = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0,
                                     (sockaddr*)&clientAddr, &clientAddrSize);
        if (bytesReceived == SOCKET_ERROR) {
            cerr << "Failed to receive data type! Error: " << WSAGetLastError() << endl;
            continue;
        }

        if (bytesReceived == 0) {
            cout << "Client disconnected." << endl;
            continue;
        }

        char dataType = buffer[0];
        cout << "Received data type: " << dataType << endl;

        // Пауза між отриманнями даних
        Sleep(50);

        if (dataType == 'L') {
            long number = 0;

            bytesReceived = recvfrom(serverSocket, (char*)&number, sizeof(number), 0,
                                     (sockaddr*)&clientAddr, &clientAddrSize);
            if (bytesReceived == SOCKET_ERROR) {
                cerr << "Failed to receive long data! Error: " << WSAGetLastError() << endl;
            } else {
                cout << "Received " << bytesReceived << " bytes of data." << endl;
                cout << "Received long value: " << number << endl;
            }
        }
        else if (dataType == 'D') {
            double number = 0.0;

            bytesReceived = recvfrom(serverSocket, (char*)&number, sizeof(number), 0,
                                     (sockaddr*)&clientAddr, &clientAddrSize);
            if (bytesReceived == SOCKET_ERROR) {
                cerr << "Failed to receive double data! Error: " << WSAGetLastError() << endl;
            } else {
                cout << "Received " << bytesReceived << " bytes of data." << endl;
                cout << "Received double value: " << number << endl;
            }
        }
        else {
            cerr << "Unknown data type received: " << dataType << endl;
        }

        // Відправлення підтвердження клієнту
        const char* ack = "Data received successfully";
        sendto(serverSocket, ack, (int)strlen(ack), 0, (sockaddr*)&clientAddr, clientAddrSize);
    }

    // Закриття сокета
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}