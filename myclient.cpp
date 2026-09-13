#include "myOSI.h"
int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    
    connect(sock,(sockaddr*)&serverAddr,sizeof(serverAddr));
    std::cout << "[Client] Connected:\n";

    std::string received = applicationReceive(sock);
    applicationSend(sock,"Hello from Client" ,1,2,8080,80,7);


    closesocket(sock);
    WSACleanup();
    return 0;
}