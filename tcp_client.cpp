#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <limits>
#include <sstream>
#include <iomanip> // Для форматування виводу

#pragma comment(lib, "Ws2_32.lib")

#define PORT 5000
#define SERVER "127.0.0.1"

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

// Функція для перевірки, чи можна безпечно конвертувати рядок у long
bool isValidLong(const string& str) {
    try {
        size_t pos;
        long value = stol(str, &pos);
        return pos == str.length(); // Перевірка, чи весь рядок було конвертовано
    } catch (const std::out_of_range&) {
        return false; // Число надто велике для long
    } catch (const std::invalid_argument&) {
        return false; // Рядок не є числом
    }
    return true;
}

// Функція для перевірки, чи можна безпечно конвертувати рядок у double
bool isValidDouble(const string& str) {
    try {
        size_t pos;
        double value = stod(str, &pos);
        return pos == str.length(); // Перевірка, чи весь рядок було конвертовано
    } catch (const std::out_of_range&) {
        return false; // Число надто велике для double
    } catch (const std::invalid_argument&) {
        return false; // Рядок не є числом
    }
    return true;
}

int main() {
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed!" << endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed!" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER, &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Connection failed! Make sure the server is running." << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "Connected to server!" << endl;

    Message message;
    int choice;
    string input;

    while (true) {
        cout << "Select data type (1 for double, 2 for long, 0 to exit): ";
        cin >> choice;

        // Очищення буфера введення
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 0) break;

        if (choice == TYPE_DOUBLE) {
            message.type = TYPE_DOUBLE;

            while (true) {
                cout << "Enter double value: ";
                getline(cin, input);

                if (isValidDouble(input)) {
                    message.data.doubleValue = stod(input);
                    break;
                } else {
                    cout << "Warning: Invalid double value or number too large. Please try again." << endl;
                }
            }

        } else if (choice == TYPE_LONG) {
            message.type = TYPE_LONG;

            while (true) {
                cout << "Enter long value: ";
                getline(cin, input);

                if (isValidLong(input)) {
                    message.data.longValue = stol(input);
                    break;
                } else {
                    cout << "Warning: Invalid long value or number too large. Please try again." << endl;
                }
            }

        } else {
            cout << "Invalid choice. Try again." << endl;
            continue;
        }

        send(clientSocket, (char*)&message, sizeof(Message), 0);

        Message response;
        int bytesReceived = recv(clientSocket, (char*)&response, sizeof(Message), 0);

        if (bytesReceived > 0) {
            if (response.type == TYPE_DOUBLE) {
                // Використовуємо fixed і setprecision для красивого виведення double без наукового формату
                cout << "Server response (DOUBLE): " << fixed << setprecision(0);

                // Якщо число дійсно має дробову частину, показуємо до 6 знаків після коми
                if (response.data.doubleValue != (long)response.data.doubleValue) {
                    cout << setprecision(6);
                }

                cout << response.data.doubleValue << endl;
            } else if (response.type == TYPE_LONG) {
                cout << "Server response (LONG): " << response.data.longValue << endl;
            }
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}