#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 5000
#define SERVER "127.0.0.1"
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
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Failed to create socket! Error: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // Заповнення структури сервера
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER, &serverAddr.sin_addr) != 1) {
        cerr << "Invalid server address! Error: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Налаштування timeout для recvfrom
    DWORD timeout = 5000;  // 5 секунд
    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
        cerr << "Failed to set timeout! Error: " << WSAGetLastError() << endl;
    }

    char choice;

    while (true) {
        cout << "\nChoose data type (L - long, D - double, Q - quit): ";
        cin >> choice;
        cin.ignore();

        choice = toupper(choice);
        if (choice == 'Q') {
            break;
        }

        if (choice != 'L' && choice != 'D') {
            cout << "Invalid option. Please try again." << endl;
            continue;
        }

        // Надсилання типу даних
        if (sendto(clientSocket, &choice, 1, 0, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            cerr << "Failed to send data type! Error: " << WSAGetLastError() << endl;
            continue;
        }

        // Пауза між відправками
        Sleep(50);

        if (choice == 'L') {
            long number;
            cout << "Enter a long integer: ";
            cin >> number;
            cin.ignore();

            int bytesSent = sendto(clientSocket, (const char*)&number, sizeof(number), 0,
                                   (sockaddr*)&serverAddr, sizeof(serverAddr));

            if (bytesSent == SOCKET_ERROR) {
                cerr << "Failed to send data! Error: " << WSAGetLastError() << endl;
            } else {
                cout << "Sent " << bytesSent << " bytes of data." << endl;
            }
        }
        else if (choice == 'D') {
            double number;
            cout << "Enter a double: ";
            cin >> number;
            cin.ignore();

            int bytesSent = sendto(clientSocket, (const char*)&number, sizeof(number), 0,
                                   (sockaddr*)&serverAddr, sizeof(serverAddr));

            if (bytesSent == SOCKET_ERROR) {
                cerr << "Failed to send data! Error: " << WSAGetLastError() << endl;
            } else {
                cout << "Sent " << bytesSent << " bytes of data." << endl;
            }
        }

        // Отримання відповіді
        char buffer[BUFFER_SIZE] = {0};
        sockaddr_in fromAddr = {};
        int fromAddrSize = sizeof(fromAddr);

        int bytesReceived = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0,
                                     (sockaddr*)&fromAddr, &fromAddrSize);
        if (bytesReceived == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAETIMEDOUT) {
                cerr << "Timeout waiting for server response" << endl;
            } else {
                cerr << "Failed to receive response! Error: " << WSAGetLastError() << endl;
            }
        } else {
            buffer[bytesReceived] = '\0';
            cout << "Server response: " << buffer << endl;
        }
    }

    // Закриття сокета
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}