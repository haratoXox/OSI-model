#include "myOSI.h"

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(9000);

    bind(listenSocket,(sockaddr*)&serverAddr,sizeof(serverAddr));
    listen(listenSocket,1);

    std::cout<<"[SERVER] waiting for a client...\n";
    SOCKET clientSocket = accept(listenSocket, nullptr , nullptr);
    std::cout<<"[SERVER] client connected:\n";

    applicationSend(clientSocket,"konichiwa mere server-san awaj aa rahi hai",2,1,80,8080,7);
    std::string received = applicationReceive(clientSocket);


    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}