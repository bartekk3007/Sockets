#include <iostream>
#include <fstream>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <winsock.h>
#include <pthread.h>
#include <semaphore.h>
#define PORT 1234

using namespace std;

sem_t sem;

typedef struct node node;
typedef struct node {
	node* next;
	node* prev;
	int data;
};

node* listbeg;
node* listend;

int add(int data) {
	sem_wait(&sem);
	node* current = listbeg->next;
	while (current != listend)
	{
		if (current->data == data)
		{
			sem_post(&sem);
			return 0;
		}
		if (current->data < data)
		{
			break;
		}
		current = current->next;
	}
	node* newnode = (node*) malloc(sizeof(node));
	newnode->data = data;
	newnode->next = current;
	newnode->prev = current->prev;
	current->prev->next = newnode;
	current->prev = newnode;
	if (listbeg->next == listend)
	{
		listbeg->next = newnode;
	}
	sem_post(&sem);
	return 1;
}

void* handleProg(void *arg)
{
    int cur_socket = *(int*)arg;
    while(true)
    {
        char buffer[1];
        int valread;
        do
        {
            valread = recv(cur_socket, buffer, 1,0);
        } while (valread < 0);
        int data = (int)buffer[0];
        char message[1];
        if (add(data))
        {
            message[0] = 1;
        }
        else
        {
            message[0] = 0;
        }
        send(cur_socket,message,4,0);
    }
}

int main()
{
    sem_init(&sem,0,1);
	listend = (node*) malloc(sizeof(node));
	listbeg = (node*) malloc(sizeof(node));
	listbeg->prev = NULL;
	listbeg->next = listend;
	listend->prev = listbeg;
	listend->next = NULL;
	int i;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
    const char *cmd[2] = {"prog 101", "prog 127"};
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	printf(" Uruchomienie programu \ n ");
	for (i = 0; i < 2; i++)
	{

		bool creationResult = CreateProcess(
			NULL,                   // No module name (use command line)
			(LPSTR) cmd[i],                // Command line
			NULL,                   // Process handle not inheritable
			NULL,                   // Thread handle not inheritable
			FALSE,                  // Set handle inheritance to FALSE
			NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP, // creation flags
			NULL,                   // Use parent's environment block
			NULL,                   // Use parent's starting directory
			&si,           // Pointer to STARTUPINFO structure
			&pi);   // Pointer to PROCESS_INFORMATION structure
	}
	int server_fd;
    WSADATA ws;
    struct sockaddr_in address;
    if (WSAStartup(MAKEWORD(2,2),&ws) < 0)
    {
        cout<<"WSAStartup failed"<<endl;
        exit(EXIT_FAILURE);
    }
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout<<"socket failed"<<endl;
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr*)&address,sizeof(address))< 0)
    {
        cout<<"bind failed"<<endl;
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
        {
        cout<<"listen failed"<<endl;
        exit(EXIT_FAILURE);
    }
	pthread_t th[2];
	int new_socket[2];
	int test[2] = {4,5};
	int sockets = 0;
	int addrlen = sizeof(address);
	while (true)
    {
        if ((new_socket[sockets] = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0)
        {
            cout<<"accept failed"<<endl;
            exit(EXIT_FAILURE);
        }
        pthread_create(&th[sockets],NULL,&handleProg,&new_socket[sockets]);
        sockets++;
    }
	Sleep(20000);
	return 0;
}
