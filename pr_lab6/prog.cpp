#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <windows.h>
#include <winsock.h>
#define PORT 1234

using namespace std;

int main(int argc, char** argv)
{
    WSADATA ws;
    struct sockaddr_in address;
    if (WSAStartup(MAKEWORD(2,2),&ws) < 0)
    {
        cout<<"WSAStartup failed"<<endl;
        return -1;
    }
    string s = argv[1];
    int arg = stoi(s);
    srand(arg);
    int status, client_fd, valread;
    struct sockaddr_in serv_addr;
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket creation error");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
    {
        printf("Connection Failed");
        return -1;
    }
    while(true)
    {
        int r = rand()%255;
        char buffer[1];
        buffer[0] = (char)r;
        send(client_fd,buffer,256,0);
        char answer[1];
        do
        {
            valread = recv(client_fd, answer, 4,0);
        } while (valread < 0);
        if (answer[0] == 1)
            cout<<r<<endl;
    }
    return 0;
}
