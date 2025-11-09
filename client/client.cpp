
// ==========================================================
// Network File Sharing Client (Windows / C++)
// Author : Bibek Mohanty_2241013232
// ==========================================================

#define _WIN32_WINNT 0x0600  // <-- Add this line (important)
#include <bits/stdc++.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;
#define PORT 8080
#define BUF_SIZE 1024
#define XOR_KEY 'K'

void xorData(char *data, int len) {
    for (int i = 0; i < len; i++) data[i] ^= XOR_KEY;
}

// Download file from server
void downloadFile(SOCKET sock, const string &filename) {
    string cmd = "GET " + filename;
    send(sock, cmd.c_str(), (int)cmd.size(), 0);

    FILE *fp = fopen(filename.c_str(), "wb");
    if (!fp) return;

    char buffer[BUF_SIZE];
    int bytes;
    while ((bytes = recv(sock, buffer, BUF_SIZE, 0)) > 0) {
        xorData(buffer, bytes);
        fwrite(buffer, 1, bytes, fp);
        if (bytes < BUF_SIZE) break;
    }
    fclose(fp);
    cout << "📥 File downloaded: " << filename << endl;
}

// Upload file to server
void uploadFile(SOCKET sock, const string &filename) {
    FILE *fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        cout << "❌ File not found.\n";
        return;
    }

    string cmd = "PUT " + filename;
    send(sock, cmd.c_str(), (int)cmd.size(), 0);

    char buffer[BUF_SIZE];
    size_t n;
    while ((n = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
        xorData(buffer, (int)n);
        send(sock, buffer, (int)n, 0);
    }
    fclose(fp);
    cout << "📤 File uploaded: " << filename << endl;
}

void showMenu() {
    cout << "\n============================\n";
    cout << "1) LIST\n2) GET <filename>\n3) PUT <filename>\n4) QUIT\n";
    cout << "============================\n";
    cout << "Enter command: ";
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    // compatible with every MinGW version
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "❌ Connection failed.\n";
        return 0;
    }
    cout << "✅ Connected to server.\n";

    // Authentication
    send(sock, "user:password", 13, 0);
    char auth[100] = {0};
    recv(sock, auth, 100, 0);
    if (string(auth).find("AUTH_OK") == string::npos) {
        cout << "❌ Authentication failed.\n";
        closesocket(sock);
        WSACleanup();
        return 0;
    }
    cout << "🔐 Authentication success.\n";

    while (true) {
        showMenu();
        string cmd;
        getline(cin, cmd);
        if (cmd.empty()) getline(cin, cmd);

        if (cmd == "LIST") {
            send(sock, cmd.c_str(), (int)cmd.size(), 0);
            char buffer[4096] = {0};
            int bytes = recv(sock, buffer, sizeof(buffer), 0);
            cout << string(buffer, bytes) << endl;
        }
        else if (cmd.rfind("GET ", 0) == 0) {
            string filename = cmd.substr(4);
            downloadFile(sock, filename);
        }
        else if (cmd.rfind("PUT ", 0) == 0) {
            string filename = cmd.substr(4);
            uploadFile(sock, filename);
        }
        else if (cmd == "QUIT") {
            send(sock, cmd.c_str(), (int)cmd.size(), 0);
            cout << "👋 Disconnected.\n";
            break;
        }
        else {
            cout << "Invalid command.\n";
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
