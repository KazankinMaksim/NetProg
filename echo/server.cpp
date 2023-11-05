#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
  if (argc==1)
  {
     cout<<"Пожалуйста, укажите номер порта, на котором вы хотите, чтобы сервер прослушивал."<<endl;
     return 1;
  }
  
  int sock_fd;
  sockaddr_in server;

  if ((sock_fd= socket(AF_INET, SOCK_STREAM, 0))==-1)
  {
     perror("socket: ");
     exit(-1);
  }

 server.sin_family= AF_INET;
 server.sin_port= htons(atoi(argv[1]));
 server.sin_addr.s_addr= INADDR_ANY;
 bzero(&server.sin_zero, 8);

 int len=sizeof(sockaddr_in);
 
 if (bind(sock_fd, (sockaddr*) &server, len)==-1)
 {
      perror("bind:");
      exit(-1);
 }

 if (listen(sock_fd, 10)==-1)
 {
       perror("listen:");
       exit(-1);
 }

 while (true)
 {
   sockaddr_in client; // структура для адреса клиента
   len=sizeof(sockaddr_in);
	 
   int client_fd;
   if ((client_fd=accept(sock_fd, (sockaddr*) &client, (socklen_t*) &len) )==-1)
   {
	perror("accept:");
	exit(-1);
   }
   
   char clientInput[100]="";
   
   while (strcmp(clientInput, "exit")!=0)
   {
   	memset(clientInput, 0, sizeof(clientInput)); // Очистка буфера перед чтением данных
     int bytes_recv= recv(client_fd, clientInput, 100, 0);
     if (bytes_recv==-1)
     {
        perror("recv");
        break;
     }
 
     cout<<"Данные, полученные от клиента ("<<inet_ntoa(client.sin_addr)<<"):  "<<clientInput<<endl<<endl;
     
     int bytes_sent=send(client_fd, clientInput,  strlen(clientInput)+1, 0);
     if (bytes_sent==-1)
     {
        perror("send");
        break;
     }

   }
  
  cout<<"Клиент ("<<inet_ntoa(client.sin_addr)<<") отключен."<<endl<<endl;
  close(client_fd);

 }

}
