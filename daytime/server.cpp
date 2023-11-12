#include <iostream>
#include <string>
#include <cstdlib> //exit()
#include <unistd.h> //close()
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctime> // для работы с датой и временем
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
	setlocale(LC_ALL, "ru_RU.utf8");
	sockaddr_in * selfAddr = new (sockaddr_in);
	selfAddr -> sin_family = AF_INET; // интернет протокол IPv4
	selfAddr -> sin_port = htons(atoi(argv[1]));  //Только порт из ком.строки
	selfAddr -> sin_addr.s_addr = INADDR_ANY;   // любой IP

	char *buffer = new char[256];
	int msgLen = 256;      //максимальный размер пакета 

	int mySocket = socket(AF_INET, SOCK_DGRAM, 0); //udp протокол
	if (mySocket == -1) {
		close(mySocket);
		errHanding( "Ошибка открытия сокета", 2);
	}

	int rc = bind(mySocket, (const sockaddr*) selfAddr, sizeof(sockaddr_in));
	if (rc == -1) {
		close(mySocket);
		errHanding("Ошибка связывания сокета адресом",3);
	}
	
    sockaddr_in remaddr; // структура для адреса клиента
    socklen_t addrlen = sizeof(remaddr); // размер структуры
	while (true){
    rc = recvfrom(mySocket, buffer, msgLen, 0, (struct sockaddr *)&remaddr, &addrlen);
    if (rc == -1) {
        close(mySocket);
        errHanding("Ошибка с получением данных от клиента",4);
    }
    buffer[rc] = '\0'; // конец принятой строки
    cout << "Пользователь подключился. Мы получили: " << buffer << endl; // вывод полученного сообщения от клиента

    time_t now = time(0); // получаем текущее время
	tm *ltm = localtime(&now);

	// массив для форматированной строки времени
	char time_str[256];

	// Форматируем время как полную дату и время на русском языке.
	strftime(time_str, sizeof(time_str), "%A %d %B %Y %T", ltm);

	string data(time_str); // преобразуем время в строку
	data += "\n";
    rc = sendto(mySocket, data.c_str(), data.size(), 0, (struct sockaddr *)&remaddr, addrlen); // отправляем ответ
    if (rc == -1) {
        close(mySocket);
        errHanding("Ошибка отправки ответа",5);
    }
	}	
    delete selfAddr;
    delete[] buffer;
    return 0;
}
