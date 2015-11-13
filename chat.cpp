/* 
 * Name			: chat.cpp
 * Auther		: sameragi
 * Copyright	: Copyright sameragi 2015
 * License		: 
 */

#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <mutex>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define log_file_name "log"
#define port_number 40000
//#define target_ip "192.168.1.26"
//#define target_ip "127.0.0.1"

using namespace std;

mutex mtx1, mtx2;


class receiver {
	int re_sock = 0;
	int re_new_sock = 0;
	sockaddr_in read_addr;
	sockaddr_in write_addr;
	socklen_t write_addr_len = sizeof(write_addr);

public:
	receiver() {
		bzero((char *)&read_addr, sizeof(read_addr));
		bzero((char *)&write_addr, sizeof(write_addr));
		re_sock = socket(AF_INET, SOCK_STREAM, 0);
		read_addr.sin_family = AF_INET;
		read_addr.sin_port = htons(port_number);
		inet_aton("INADDR_ANY", &read_addr.sin_addr);

		bind(re_sock, (sockaddr *)&read_addr, sizeof(read_addr));
		listen(re_sock, 1);
	}

	void operator() () {
		char buff[32] = "";
		char stock[32];
		strcpy(stock, buff);

		while(1) { 
			if((re_new_sock = accept(re_sock, (sockaddr *)&write_addr, &write_addr_len)) < 0) {
				cerr << "accept error" << endl;
			}

			if((recv(re_new_sock, buff, sizeof(buff), 0)) < 0) {
				cout << "recv error" << endl;
			}

			if(strcmp(stock, buff) != 0) {
				cout << endl;
				cout << "receive message >> " << buff << endl;
				strcpy(stock, buff);
			}
		}
	}

	~receiver() { close(re_sock); close(re_new_sock); }
};


class transmitter {
	int tr_sock = 0;
	sockaddr_in tr_addr;

public:
	transmitter(string target_ip) {
		bzero((char *)&tr_addr, sizeof(tr_addr));
		tr_sock = socket(AF_INET, SOCK_STREAM, 0);
		tr_addr.sin_family = AF_INET;
		tr_addr.sin_port = htons(port_number);
		//tr_addr.sin_addr.s_addr = inet_addr(target_ip);
		tr_addr.sin_addr.s_addr = inet_addr(target_ip.c_str());
	}

	void operator () () {
		char tr_message[32];
		//cout << endl;
		//cout << "input message >> ";
		cin >> tr_message;

		if((connect(tr_sock, (sockaddr *)&tr_addr, sizeof(tr_addr))) < 0) {
			cerr << "connect error" << endl;
		}

		if((send(tr_sock, tr_message, sizeof(tr_message), 0)) < 0) {
			cerr << "send error" << endl;
		}
	}

	~transmitter() { close(tr_sock); }
};


int main()
{
	string target;
	cout << "input target ip >> ";
	cin >> target;

	receiver re_obj;
	thread th1(ref(re_obj));

	cout << "chat start" << endl;

	while(1) {
		transmitter tr_obj(target);
		tr_obj();
	}

	th1.join();
}

