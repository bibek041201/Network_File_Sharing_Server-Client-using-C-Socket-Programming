# Network_File_Sharing_Server-Client-using-C-Socket-Programming
# 🚀 Network File Sharing System (C/C++ – Socket Programming)
A fully functional **TCP-based Client–Server File Sharing System** built using **C++**, **POSIX sockets**, and **Winsock2 (Windows)**. Designed for fast, reliable file upload/download across machines.

---

## ✨ Key Features
- 📡 **Reliable TCP communication** between client and server
- 📂 **File upload & download** with buffered streaming
- 🗂 **List server directory contents**
- ⚙️ **Cross-platform architecture** (Linux server, Windows client)
- 🔒 Graceful connection handling
- 📦 Easy to extend & modular design

---

## 🛠 Technology Stack
### **Server (Linux)**
- C++17
- POSIX Socket API

### **Client (Windows)**
- C++17
- Winsock2
- `ws2_32.lib`

---

## 📁 Project Structure
```
Network_File_Sharing_Server-Client-using-C-Socket-Programming/
│── server/
│   └── server.cpp
│
│── client/
│   └── client.cpp
│
└── README.md
```
---

## 🔧 How the System Works
### ✅ 1. Server Startup
Server binds to port **8080**, listens for connections, and processes commands.

### ✅ 2. Client Connection
Client (Windows) initializes Winsock, then connects to server IP:PORT.

### ✅ 3. Supported Commands
| Command | Description |
|---------|-------------|
| `LIST` | Get list of files stored on server |
| `GET <file>` | Download file from server |
| `PUT <file>` | Upload file to server |
| `EXIT` | Close connection |

### ✅ 4. File Transfer Logic
- Buffer size: **1024 bytes**
- File size sent first
- Data streamed until full transfer completes

---

## 🚀 Build & Run Instructions
### **Linux – Server**
```
g++ -std=c++17 server.cpp -o server
./server
```

### **Windows – Client**
```
g++ -std=c++17 client.cpp -lws2_32 -o client.exe
client.exe
```
Required lines:
```
#define _WIN32_WINNT 0x0600
#pragma comment(lib, "ws2_32.lib")
```
---

## ⚠️ Notes & Issues
- File paths must be valid on both systems
- Ensure port **8080** allowed in Firewall
- Works for binary & text files
- CRLF/LF differences do not affect transfer

---

## 🔮 Future Improvements
- Multi-client concurrency
- Transfer progress bar
- Encryption & file checksum (MD5/SHA256)
- GUI-based client
- Resume interrupted downloads

---

## 👤 Author
**Bibek Mohanty**  
Socket Programming Project (2024–25)

---

## 📜 License
Free for academic and educational use.
