#include <cstdlib>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
using namespace std;

void errHanding (const char * why, const int exitCode = 1)
{
	cerr<< why << endl;
	exit(exitCode);
}

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
     errHanding("Ошибка открытия сокета:", 2);
  }

 server.sin_family= AF_INET;
 server.sin_port= htons(atoi(argv[1]));
 server.sin_addr.s_addr= INADDR_ANY;
 int len=sizeof(sockaddr_in);
 
 if (bind(sock_fd, (sockaddr*) &server, len)==-1)
 {
      errHanding("Ошибка связывания сокета адресом:", 3);
 }

 if (listen(sock_fd, 10)==-1)
 {
       errHanding("Сообщение о получении ошибки:", 4);
 }

while (true)
{
  sockaddr_in client; // структура для адреса клиента
  len=sizeof(sockaddr_in);
  
  int client_fd;
  if ((client_fd=accept(sock_fd, (sockaddr*) &client, (socklen_t*) &len) )==-1)
  {
    errHanding("Ошибка при попытке принять соединение от клиента:", 5);
  }
  
  char clientInput[100]="";
  int bytes_recv;
  while ((bytes_recv = recv(client_fd, clientInput, 100, 0)) > 0)
  {
    cout<<"Данные, полученные от клиента ("<<inet_ntoa(client.sin_addr)<<"):  "<<clientInput<<endl<<endl;
    
    int bytes_sent=send(client_fd, clientInput,  strlen(clientInput)+1, 0);
    if (bytes_sent==-1)
    {
      errHanding("Ошибка при попытке отправить данные клиенту", 6);
      break;
    }

    memset(clientInput, 0, sizeof(clientInput)); // Очистка буфера перед чтением данных
  }
  
  if (bytes_recv == -1)
  {
    errHanding("Ошибка при попытке получить данные от клиента", 7);
  }
  
  cout<<"Клиент ("<<inet_ntoa(client.sin_addr)<<") отключен."<<endl<<endl;
  close(client_fd);
}

}
