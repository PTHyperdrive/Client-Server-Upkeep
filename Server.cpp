#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <winsock2.h>  // For Windows sockets
#include <ws2tcpip.h>  // For getaddrinfo()

#pragma comment(lib, "Ws2_32.lib") // Link with the Winsock library

using namespace std;

const int SERVER_PORT = 1234;
const string IP_FILE = "last_ip.txt";

string load_last_ip() {
    ifstream file(IP_FILE);
    string ip;
    if (getline(file, ip)) {   
        return ip;
    } else {
        return ""; 
    }
}

void save_ip(const string& ip) {
    ofstream file(IP_FILE);
    file << ip; 
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed!" << endl;
        return 1;
    }

    // Create socket
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Socket creation failed!" << endl;
        WSACleanup();
        return 1;
    }

    // Bind socket
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cerr << "Socket bind failed!" << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Main loop
    char buffer[1024];
    string last_ip = load_last_ip();

    while (true) {
        sockaddr_in client_addr;
        int client_addr_size = sizeof(client_addr);
        int bytes_received = recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr*)&client_addr, &client_addr_size);

        if (bytes_received > 0) {
            buffer[bytes_received] = '\0'; // Null-terminate
            string payload(buffer);

            // Simple parsing (adjust based on your format)
            vector<string> parts = split(payload, ", "); 

            cout << "Received from " << inet_ntoa(client_addr.sin_addr) << ":" << endl;
            for (const string& part : parts) {
                cout << part << endl; // Print each part
            }

            if (parts.size() >= 1 && parts[0] != last_ip) {
                cout << "IP Address Changed!" << endl;
                save_ip(parts[0]);
                last_ip = parts[0];
            }
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
