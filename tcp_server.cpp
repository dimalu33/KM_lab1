#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <iomanip> // Для форматування виводу

#pragma comment(lib, "Ws2_32.lib")

#define PORT 5000

using namespace std;

enum DataType {
    TYPE_DOUBLE = 1,
    TYPE_LONG = 2
};

struct Message {
    DataType type;
    union {
        double doubleValue;
        long longValue;
    } data;
};

int main() {
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed!" << endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed!" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed! Try running as Administrator." << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server is running on port " << PORT << ". Waiting for a client..." << endl;

    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Accept failed!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Client connected!" << endl;

    Message message;
    while (true) {
        int bytesReceived = recv(clientSocket, (char*)&message, sizeof(Message), 0);
        if (bytesReceived <= 0) break;

        if (message.type == TYPE_DOUBLE) {
            // Використовуємо fixed і setprecision для красивого виведення double без наукового формату
            cout << "Received DOUBLE: " << fixed << setprecision(0);

            // Якщо число дійсно має дробову частину, показуємо до 6 знаків після коми
            if (message.data.doubleValue != (long)message.data.doubleValue) {
                cout << setprecision(6);
            }

            cout << message.data.doubleValue << endl;
        } else if (message.type == TYPE_LONG) {
            cout << "Received LONG: " << message.data.longValue << endl;
        } else {
            cout << "Received unknown data type!" << endl;
        }

        // Echo the message back to client
        send(clientSocket, (char*)&message, sizeof(Message), 0);
    }

    cout << "Client disconnected." << endl;
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}