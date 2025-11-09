// ==========================================================
// Network File Sharing Server (Windows / C++)
// Author : Bibek Mohanty_2241013232
// ==========================================================

#include <bits/stdc++.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;
#define PORT 8080
#define BUF_SIZE 1024
#define XOR_KEY 'K'

// XOR Encryption/Decryption
void xorData(char *data, int len) {
    for (int i = 0; i < len; i++) data[i] ^= XOR_KEY;
}

// Send a file to the client
void sendFile(SOCKET client, const string &filename) {
    FILE *fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        string msg = "ERROR: File not found\n";
        send(client, msg.c_str(), msg.size(), 0);
        return;
    }

    char buffer[BUF_SIZE];
    size_t n;
    while ((n = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
        xorData(buffer, (int)n);
        send(client, buffer, (int)n, 0);
    }
    fclose(fp);
    cout << "📤 File sent successfully: " << filename << endl;
}

// Receive a file from the client
void receiveFile(SOCKET client, const string &filename) {
    FILE *fp = fopen(filename.c_str(), "wb");
    if (!fp) return;
    char buffer[BUF_SIZE];
    int bytes;
    while ((bytes = recv(client, buffer, BUF_SIZE, 0)) > 0) {
        xorData(buffer, bytes);
        fwrite(buffer, 1, bytes, fp);
        if (bytes < BUF_SIZE) break;
    }
    fclose(fp);
    cout << "📥 File uploaded successfully: " << filename << endl;
}

// List files in current directory (ANSI version)
string listFiles() {
    string list = "Available files:\n";
    WIN32_FIND_DATAA fd;
    HANDLE hFind = FindFirstFileA(".\\*", &fd);
    if (hFind == INVALID_HANDLE_VALUE) return "No files found.\n";

    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            list += " - " + string(fd.cFileName) + "\n";
    } while (FindNextFileA(hFind, &fd));
    FindClose(hFind);
    return list;
}

// Authentication
bool authenticate(SOCKET client) {
    char creds[100] = {0};
    recv(client, creds, sizeof(creds), 0);
    string input(creds);
    if (input == "user:password") {
        send(client, "AUTH_OK", 7, 0);
        return true;
    }
    send(client, "AUTH_FAIL", 9, 0);
    return false;
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    bind(server_fd, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(server_fd, 3);
    cout << "📡 Server listening on port " << PORT << "...\n";

    SOCKET client;
    sockaddr_in clientAddr{};
    int c = sizeof(clientAddr);
    client = accept(server_fd, (sockaddr*)&clientAddr, &c);
    cout << "✅ Client connected.\n";

    if (!authenticate(client)) {
        cout << "❌ Authentication failed.\n";
        closesocket(client);
        closesocket(server_fd);
        WSACleanup();
        return 0;
    }
    cout << "🔐 Authentication successful.\n";

    while (true) {
        char cmd[256] = {0};
        int valread = recv(client, cmd, sizeof(cmd), 0);
        if (valread <= 0) break;
        string command(cmd);

        if (command == "LIST") {
            string files = listFiles();
            send(client, files.c_str(), (int)files.size(), 0);
        } 
        else if (command.rfind("GET ", 0) == 0) {
            string filename = command.substr(4);
            sendFile(client, filename);
        } 
        else if (command.rfind("PUT ", 0) == 0) {
            string filename = command.substr(4);
            receiveFile(client, filename);
        } 
        else if (command == "QUIT") {
            cout << "👋 Client disconnected.\n";
            break;
        } 
        else {
            string msg = "Unknown command.\n";
            send(client, msg.c_str(), msg.size(), 0);
        }
    }

    closesocket(client);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
