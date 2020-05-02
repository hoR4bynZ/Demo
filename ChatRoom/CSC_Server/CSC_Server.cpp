/*Server.c
 *功能：CSC Mod UDP_Chatting
*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#pragma warning (disable:4996)

#define BUF_SIZE 1024
#define PIECE 10

//客户端信息结构体
typedef struct _Client
{
	SOCKADDR_IN clntAddr;      //客户端套接字
	char buf[BUF_SIZE];       //数据缓冲区，因为是起转发作用，所以用一个缓冲区就够了
	char userName[32];   //客户端用户名
	int flag = 0;       //标记客户端，用来区分不同的客户端
}Client;

Client g_Client[2] = { 0 };                  //创建一个客户端结构体
Client* cp = g_Client;

int linkNum = 0;//全局变量，表示连接数量

void FileTrans_send(char*);
void FileTrans_recv(char*);
void FileTransIntnue_send(char*);

int main() {
	//加载socket动态链接库(dll)
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);    // 请求2.2版本的WinSock库

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return -1;			// 返回值为零的时候是表示成功申请WSAStartup
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		// 检查这个低字节是不是2，高字节是不是2以确定是否我们所请求的2.2版本
		// 否则的话，调用WSACleanup()清除信息，结束函数
		WSACleanup();
		return -1;
	}

	/*
	 *上面代码可简化为
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	*/

	SOCKET servSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (servSock == INVALID_SOCKET)
	{
		printf("Socket error!");
		return 0;
	}

	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(1234);
	servAddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(servSock, (SOCKADDR*)&servAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		printf("Bind error!");
		closesocket(servSock);
		return 0;
	}

	SOCKADDR_IN clntAddr;
	int clLen = sizeof(clntAddr);
	char recvBuf[BUF_SIZE];
	char ci = '0';//记录连接数量

	while (1) {
		/*连接阶段：测试过需要用while()循环一直接收请求否则后面进入注册登录函数发送请求会失败*/
		int ret = 0;//返回值
		//接收客户端的连接请求，成功接收请求就打印该客户端的ip,并且把这个地址信息赋给客户端信息结构体数组
		ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);
		recvBuf[ret] = '\0';
		printf("%s\n", recvBuf);
		//告诉客户端你成功连接了噢
		sendto(servSock, "You enter chatting room successfully!\n", strlen("You enter chatting room successfully!\n"), 0, (SOCKADDR*)&clntAddr, clLen);

		/*注册登录阶段*/
		//声明变量
		char unPath[BUF_SIZE];//用户名路径
		char temp[BUF_SIZE] = "";//临时数组暂存数据作对比用
		FILE* ufp;
		int i;//i计数值
		while (1) {

			/*这个while()循环处理登录注册*/

			//接收客户端功能请求:0,1,2
			memset(recvBuf, 0, BUF_SIZE);
			recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);
			sendto(servSock, "y", strlen("y"), 0, (SOCKADDR*)&clntAddr, clLen);
			sprintf(temp, "%s", recvBuf);

			if (strcmp(temp, "1") == 0) {
				/*注册模块*/

				/*检查用户名是否存在*/
				//客户端会发送用户名给服务器让服务器检查是否已注册
				memset(recvBuf, 0, BUF_SIZE);
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);
				recvBuf[ret] = '\0';
				//补充用户名路径
				sprintf(unPath, "%s%s", "E:\\SYSTEM\\Server\\UserData\\", recvBuf);
				//r打开一个已存在文件，文件不存在出错，只读
				ufp = fopen(unPath, "r");
				//判断用户名是否存在
				//fp!=NULL：假如成功打开了文件
				if (ufp != NULL) {
					sendto(servSock, "y", strlen("y"), 0, (SOCKADDR*)&clntAddr, clLen);
					continue;
				}
				else sendto(servSock, "n", strlen("n"), 0, (SOCKADDR*)&clntAddr, clLen);
				//fclose(ufp);

				/*记录密码*/
				memset(recvBuf, 0, BUF_SIZE);
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);

				//w打开/新建一个文件，只写
				ufp = fopen(unPath, "w");
				//以用户名命名的文件，密码作为文件文本内容
				if ((fputs(recvBuf, ufp)) != 0) printf("Password was failed to save\n");

				sendto(servSock, "password saved", strlen("password saved"), 0, (SOCKADDR*)&clntAddr, clLen);
				fclose(ufp);
			}
			else if (strcmp(temp, "2") == 0) {
				/*登录模块*/

				/*检查用户名*/
				memset(recvBuf, 0, BUF_SIZE);
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);

				//把客户端ip、port信息保存到数组成员
				g_Client[linkNum].clntAddr = clntAddr;
				//保存用户名
				recvBuf[ret] = '\0';
				sprintf(g_Client[linkNum].userName, "%s", recvBuf);

				//补充用户名路径
				sprintf(unPath, "%s", "E:\\SYSTEM\\Server\\UserData\\");
				sprintf(unPath, "%s%s", unPath, recvBuf);
				//r打开一个已存在文件，文件不存在出错，只读
				if ((ufp = fopen(unPath, "r")) == NULL) {
					printf("File was failed to open\n");
					sendto(servSock, "n", strlen("n"), 0, (SOCKADDR*)&clntAddr, clLen);
					continue;
				}

				sendto(servSock, "\n\t\tPlease input your password:", strlen("\n\t\tPlease input your password:"), 0, (SOCKADDR*)&clntAddr, clLen);

				/*检查密码*/
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);
				recvBuf[ret] = '\0';
				memset(temp, 0, BUF_SIZE);
				fgets(temp, ret + 1, ufp);
				if (strcmp(temp, recvBuf) == 0) {
					sendto(servSock, "y", strlen("y"), 0, (SOCKADDR*)&clntAddr, clLen);
					//break;
				}
				else {
					sendto(servSock, "n", strlen("n"), 0, (SOCKADDR*)&clntAddr, clLen);
					fclose(ufp);
					continue;
				}
				fclose(ufp);

				/*检查客户端登录数量*/
				linkNum++;
				ci++;
				//打印当前客户端连接数量
				printf("the amount of online client is %d\n", linkNum);
				printf("Its username is %s\n", g_Client[linkNum - 1].userName);

				//后台发送客户端连接数量，注意两个客户端都要发送，不然会出现一个客户端能聊天另一个客户端不能聊天的bug
				memset(recvBuf, 0, BUF_SIZE);
				sprintf(recvBuf, "%c", ci);
				recvBuf[2] = '\0';
				sendto(servSock, recvBuf, 2, 0, (SOCKADDR*)&g_Client[linkNum - 1].clntAddr, clLen);
				memset(temp, 0, BUF_SIZE);
				sprintf(temp, "The network have %d on-line, these are: ", linkNum);
				if (linkNum == 2) sprintf(temp, "%s%s and %s\n", temp, g_Client[linkNum - 2].userName, g_Client[linkNum - 1].userName);
				else sprintf(temp, "%s%s\n", temp, g_Client[linkNum - 1].userName);

				sendto(servSock, temp, strlen(temp), 0, (SOCKADDR*)&g_Client[linkNum - 1].clntAddr, clLen);

				//这里的代码是专用的，因为只考虑了连接两个客户端，如果是三个客户端这样写就不行了
				if (linkNum == 2) {//也就是说如果有两台客户端连接上来了，那么更新在线数量必须同时向两台客户端一起发送在线数量
					sendto(servSock, recvBuf, 2, 0, (SOCKADDR*)&g_Client[linkNum - 2].clntAddr, clLen);
					memset(temp, 0, BUF_SIZE);
					sprintf(temp, "The network have %d on-line, these are: ", linkNum);
					sprintf(temp, "%s%s and %s\n", temp, g_Client[linkNum - 2].userName, g_Client[linkNum - 1].userName);
					sendto(servSock, temp, strlen(temp), 0, (SOCKADDR*)&g_Client[linkNum - 2].clntAddr, clLen);
					break;
				}
			}
			break;
		}

		if (linkNum == 2);//i==2什么都不执行，空语句
		else continue;

		//打开文件用来保存客户端的聊天记录,wb+,包括读和写，文件不存在时新建
		sprintf(unPath, "%s", "E:\\SYSTEM\\Server\\Record\\");
		sprintf(unPath, "%s%s%s", unPath, g_Client[0].userName, ".txt");
		FILE* fpc0 = fopen(unPath, "ab+");

		sprintf(unPath, "%s", "E:\\SYSTEM\\Server\\Record\\");
		sprintf(unPath, "%s%s%s", unPath, g_Client[1].userName, ".txt");
		FILE* fpc1 = fopen(unPath, "ab+");

		//创建指针数组保存文件指针
		FILE* fparr[2] = { fpc0,fpc1 };
		long filelen = 0;

		while (1) {

			/*这个while()循环处理聊天室功能*/

			//接收客户端功能请求:0,1,2,3
			memset(recvBuf, 0, BUF_SIZE);
			memset(temp, 0, BUF_SIZE);
			recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);
			sendto(servSock, "y", strlen("y"), 0, (SOCKADDR*)&clntAddr, clLen);
			sprintf(temp, "%s", recvBuf);

			if (strcmp(temp, "1") == 0) {

				i = 0;
				while (1) {//需要两台客户端同时发起会话才能进入聊天
					memset(recvBuf, 0, BUF_SIZE);
					if (i == 2) {
						sendto(servSock, "\t\tEnjoy your chatting!\n", strlen("\t\tEnjoy your chatting!\n"), 0, (SOCKADDR*)&g_Client[0].clntAddr, clLen);
						sendto(servSock, "\t\tEnjoy your chatting!\n", strlen("\t\tEnjoy your chatting!\n"), 0, (SOCKADDR*)&g_Client[1].clntAddr, clLen);
						break;
					}
					recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);
					if (strcmp(recvBuf, "1") == 0) {
						sendto(servSock, "y", strlen("y"), 0, (SOCKADDR*)&clntAddr, clLen);
						continue;
					}
					else if (strcmp(recvBuf, " ") == 0) {
						i++;
						continue;
					}
				}

				//处理客户端的消息：主要是转发信息
				while (1) {

					/*
					注意的地方是，客户端会统一发送信息到服务器的缓冲区，需要copy到结构体客户的缓冲区中
					*/

					//把先发送消息的客户端称为C1，后来发送消息的称为C2
					/*流程是
					1、C1 sendto()一个消息到S 的recvBuf[]缓冲区
					2、S 判断C1 SOCKADDR结构体中的IP、PORT信息，然后将recvBuf[]内容copy到C1对应的结构体缓冲区buf[]，
						最后设置flag成员变量标记该客户端是否已发送信息
					3、S 进行消息转发，这里的关键是转发地址是结构体中flag标记为缺省状态的
					4、S 根据客户信息结构体缓冲区保存聊天记录为文件格式,也就是将客户端缓冲区内容拷贝到本地文件
					*/
					i = 0;
					ret = 0;
					memset(recvBuf, 0, BUF_SIZE);
					ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);//完成第1步
					if (ret == 0) continue;
					printf("Ret=%d\n", ret);
					recvBuf[ret] = '\0';
					if (strcmp(recvBuf, "quit") == 0) {
						sendto(servSock, "quit", strlen("quit"), 0, (SOCKADDR*)&g_Client[i].clntAddr, clLen);
						sendto(servSock, "quit", strlen("quit"), 0, (SOCKADDR*)&g_Client[!i].clntAddr, clLen);
						break;
					}
					if (g_Client[i].clntAddr.sin_addr.S_un.S_addr == clntAddr.sin_addr.S_un.S_addr && g_Client[i].clntAddr.sin_family == clntAddr.sin_family && g_Client[i].clntAddr.sin_port == clntAddr.sin_port) {
						//先用字符串函数"加工"需要转发的内容，主要是添加用户名
						strcpy_s(g_Client[i].buf, sizeof(g_Client[i].buf), g_Client[i].userName);
						strcat_s(g_Client[i].buf, sizeof(g_Client[i].buf), " saies: ");
						strcat_s(g_Client[i].buf, sizeof(g_Client[i].buf), recvBuf);
						g_Client[i].flag = 1;//flag的值为0时标记为未转发，为1时标记为已转发
						g_Client[!i].flag = 0;
					}
					else {
						strcpy_s(g_Client[!i].buf, sizeof(g_Client[!i].buf), g_Client[!i].userName);
						strcat_s(g_Client[!i].buf, sizeof(g_Client[!i].buf), " saies: ");
						strcat_s(g_Client[!i].buf, sizeof(g_Client[!i].buf), recvBuf);
						g_Client[!i].flag = 1;
						g_Client[i].flag = 0;
					}//第2步完
					for (i = 0; i < 2; i++) {
						if (g_Client[i].flag == 0) {//flag为0标记的未转发，所以哪个为0转发到哪个
							sendto(servSock, g_Client[!i].buf, ret + strlen(g_Client[!i].userName) + 8, 0, (SOCKADDR*)&g_Client[i].clntAddr, clLen);
							fwrite(recvBuf, 1, ret + 1, fparr[!i]);//将消息内容以二进制形式追加到文件中
							//memset(recvBuf, 0, BUF_SIZE);
							break;
						}
					}//第34步完
				}
			}
			else if (strcmp(temp, "2") == 0) {
				//刷新作用
				recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);

				sendto(servSock, "Whose record would you wanna save?\n", strlen("Whose record would you wanna save?\n"), 0, (SOCKADDR*)&clntAddr, clLen);
				memset(recvBuf, 0, BUF_SIZE);
				//接收客户机应答——用户名
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);
				if (strcmp(recvBuf, g_Client[0].userName) == 0) {
					memset(recvBuf, 0, BUF_SIZE);
					/*
					fclose(fpc0);
					sprintf(unPath, "%s", "E:\\SYSTEM\\Server\\Record\\");
					sprintf(unPath, "%s%s%s", unPath, g_Client[0].userName, ".txt");
					fpc0 = fopen(unPath, "wb+");
					*/
					//求文件长度,fseek()会把文件指针移动到末尾，rewind()再把指针复位到开头
					fseek(fpc0, 0L, SEEK_END);
					filelen = ftell(fpc0);
					rewind(fpc0);
					while ((ret = fread(recvBuf, 1, BUF_SIZE, fpc0)) >= 0) {
						sendto(servSock, recvBuf, ret, 0, (SOCKADDR*)&g_Client[0].clntAddr, clLen);
						if (ret == 0) break;
					}
				}
				else if (strcmp(recvBuf, g_Client[1].userName) == 0) {
					memset(recvBuf, 0, BUF_SIZE);
					/*
					fclose(fpc1);
					sprintf(unPath, "%s", "E:\\SYSTEM\\Server\\Record\\");
					sprintf(unPath, "%s%s%s", unPath, g_Client[1].userName, ".txt");
					fpc1 = fopen(unPath, "wb+");
					*/
					//求文件长度,fseek()会把文件指针移动到末尾，rewind()再把指针复位到开头
					fseek(fpc1, 0L, SEEK_END);
					filelen = ftell(fpc1);
					rewind(fpc1);
					ret = fread(recvBuf, 1, filelen, fpc1);
					sendto(servSock, recvBuf, ret, 0, (SOCKADDR*)&g_Client[1].clntAddr, clLen);
					/*
					while ((ret = fread(recvBuf, 1, filelen, fpc1)) >= 0) {
						sendto(servSock, recvBuf, ret, 0, (SOCKADDR *)&g_Client[1].clntAddr, clLen);
						if (ret == 0) break;
					}
					*/
				}
			}
			else if (strcmp(temp, "3") == 0) {

				ci = '0';
				i = 0;
				while (1) {//需要两台客户端同时发起会话才能进入聊天
					memset(recvBuf, 0, BUF_SIZE);
					if (ci == '2') {
						if (g_Client[0].flag == 0) {
							sendto(servSock, "\t\tAnother client is onlining! Please wait for his/her respond...\n", strlen("\t\tAnother client is onlining! Please wait for his/her respond...\n"), 0, (SOCKADDR*)&g_Client[0].clntAddr, clLen);
							sprintf(recvBuf, "%s%s%s", "\t\tYour friend ", g_Client[0].userName, "  want to send you some files, will you receive?[y/n]\n");
							sendto(servSock, recvBuf, strlen(recvBuf), 0, (SOCKADDR*)&g_Client[1].clntAddr, clLen);
						}
						else if (g_Client[1].flag == 0) {
							sendto(servSock, "\t\tAnother client is onlining! Please wait for his/her respond...\n", strlen("\t\tAnother client is onlining! Please wait for his/her respond...\n"), 0, (SOCKADDR*)&g_Client[1].clntAddr, clLen);
							sprintf(recvBuf, "%s%s%s", "\t\tYour friend ", g_Client[1].userName, "want to send you some files, will you receive?[y/n]\n");
							sendto(servSock, recvBuf, strlen(recvBuf), 0, (SOCKADDR*)&g_Client[0].clntAddr, clLen);
						}
						break;
					}
					recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);
					//记录客户端连接的先后顺序，先连接置flag=0；否则flag=1
					if (clntAddr.sin_addr.S_un.S_addr == g_Client[i].clntAddr.sin_addr.S_un.S_addr && ci == '0') {
						g_Client[i].flag = 0;
						g_Client[!i].flag = 1;
					}
					else if (clntAddr.sin_addr.S_un.S_addr == g_Client[!i].clntAddr.sin_addr.S_un.S_addr && ci == '0') {
						g_Client[!i].flag = 0;
						g_Client[i].flag = 1;
					}

					if (strcmp(recvBuf, "3") == 0) {
						sendto(servSock, "x", 2, 0, (SOCKADDR*)&clntAddr, clLen);
						continue;
					}
					else if (strcmp(recvBuf, " ") == 0) {
						ci++;
						continue;
					}
				}
				//接收来自接收端客户端(也就是后来连接上来的客户端)的响应
				memset(recvBuf, 0, BUF_SIZE);
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&g_Client[!i].clntAddr, &clLen);
				recvBuf[ret] = '\0';
				sendto(servSock, recvBuf, ret, 0, (SOCKADDR*)&g_Client[i].clntAddr, clLen);
				//接收发送客户端发来的文件名
				memset(recvBuf, 0, BUF_SIZE);
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&g_Client[i].clntAddr, &clLen);
				recvBuf[ret] = '\0';
				FileTrans_recv(recvBuf);
				//发送文件给接收客户端
				FileTrans_send(recvBuf);
			}
			else if (strcmp(temp, "4") == 0) {
				ci = '0';
				i = 0;
				while (1) {//需要两台客户端同时发起会话才能进入聊天
					memset(recvBuf, 0, BUF_SIZE);
					if (ci == '2') {
						if (g_Client[0].flag == 0) {
							sendto(servSock, "\t\tAnother client is onlining! Please wait for his/her respond...\n", strlen("\t\tAnother client is onlining! Please wait for his/her respond...\n"), 0, (SOCKADDR*)&g_Client[0].clntAddr, clLen);
							sprintf(recvBuf, "%s%s%s", "\t\tYour friend ", g_Client[0].userName, "  want to send you some files, will you receive?[y/n]\n");
							sendto(servSock, recvBuf, strlen(recvBuf), 0, (SOCKADDR*)&g_Client[1].clntAddr, clLen);
						}
						else if (g_Client[1].flag == 0) {
							sendto(servSock, "\t\tAnother client is onlining! Please wait for his/her respond...\n", strlen("\t\tAnother client is onlining! Please wait for his/her respond...\n"), 0, (SOCKADDR*)&g_Client[1].clntAddr, clLen);
							sprintf(recvBuf, "%s%s%s", "\t\tYour friend ", g_Client[1].userName, "want to send you some files, will you receive?[y/n]\n");
							sendto(servSock, recvBuf, strlen(recvBuf), 0, (SOCKADDR*)&g_Client[0].clntAddr, clLen);
						}
						break;
					}
					recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);
					//记录客户端连接的先后顺序，先连接置flag=0；否则flag=1
					if (clntAddr.sin_addr.S_un.S_addr == g_Client[i].clntAddr.sin_addr.S_un.S_addr && ci == '0') {
						g_Client[i].flag = 0;
						g_Client[!i].flag = 1;
					}
					else if (clntAddr.sin_addr.S_un.S_addr == g_Client[!i].clntAddr.sin_addr.S_un.S_addr && ci == '0') {
						g_Client[!i].flag = 0;
						g_Client[i].flag = 1;
					}

					if (strcmp(recvBuf, "4") == 0) {
						sendto(servSock, "x", 2, 0, (SOCKADDR*)&clntAddr, clLen);
						continue;
					}
					else if (strcmp(recvBuf, " ") == 0) {
						ci++;
						continue;
					}
				}
				//接收来自接收端客户端(也就是后来连接上来的客户端)的响应
				memset(recvBuf, 0, BUF_SIZE);
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&g_Client[!i].clntAddr, &clLen);
				recvBuf[ret] = '\0';
				sendto(servSock, recvBuf, ret, 0, (SOCKADDR*)&g_Client[i].clntAddr, clLen);
				//接收发送客户端发来的文件名
				memset(recvBuf, 0, BUF_SIZE);
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&g_Client[i].clntAddr, &clLen);
				recvBuf[ret] = '\0';
				FileTrans_recv(recvBuf);
				//发送文件给接收客户端
				FileTransIntnue_send(recvBuf);
			}
		}


		fclose(fpc0);
		fclose(fpc1);
	}

	closesocket(servSock);
	WSACleanup();

	printf("\n");
	system("pause");
	return 0;
}

void FileTrans_send(char* filename) {

	char filepath[100] = "E:\\SYSTEM\\Server\\FileTrans\\";
	sprintf(filepath, "%s%s", filepath, filename);
	FILE* fp = fopen(filepath, "rb");  //以二进制方式打开文件
	if (fp == NULL) {
		printf("Cannot open file to send, press any key to exit!\n");
		system("pause");
		exit(0);
	}

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET servSock = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockAddr.sin_port = htons(1234);
	bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
	listen(servSock, 20);

	SOCKADDR clntAddr;
	int nSize = sizeof(SOCKADDR);
	SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);

	//循环发送数据，直到文件结尾
	char buffer[BUF_SIZE] = { 0 };  //缓冲区
	int nCount;
	while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
		send(clntSock, buffer, nCount, 0);
	}

	shutdown(clntSock, SD_SEND);  //文件读取完毕，断开输出流，向客户端发送FIN包
	recv(clntSock, buffer, BUF_SIZE, 0);  //阻塞，等待客户端接收完毕

	fclose(fp);
	closesocket(clntSock);
	closesocket(servSock);
	WSACleanup();

	//system("pause");
}

void FileTrans_recv(char* filename) {

	char filepath[100] = "E:\\SYSTEM\\Server\\FileTrans\\";
	sprintf(filepath, "%s%s", filepath, filename);
	FILE* fp = fopen(filepath, "wb");  //以二进制方式打开/创建文件
	if (fp == NULL) {
		printf("Cannot open file to recv, press any key to exit!\n");
		system("pause");
		exit(0);
	}

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET servSock = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockAddr.sin_port = htons(1234);
	bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
	listen(servSock, 20);

	SOCKADDR clntAddr;
	int nSize = sizeof(SOCKADDR);
	SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);

	//循环接收数据，直到文件结尾
	char buffer[BUF_SIZE] = { 0 };  //文件缓冲区
	int nCount;
	while ((nCount = recv(clntSock, buffer, BUF_SIZE, 0)) > 0) {
		fwrite(buffer, nCount, 1, fp);
	}
	puts("File transfer success!\n");

	recv(clntSock, buffer, BUF_SIZE, 0);  //阻塞，等待客户端接收完毕
	fclose(fp);
	closesocket(clntSock);
	closesocket(servSock);
	WSACleanup();

	//system("pause");
}

void FileTransIntnue_send(char* filename) {
	/*
	*InterNue means Interrupt&contiNue
	*/
	char filepath[100] = "E:\\SYSTEM\\Server\\FileTrans\\";
	sprintf(filepath, "%s%s", filepath, filename);
	FILE* fp = fopen(filepath, "rb");  //以二进制方式打开文件
	if (fp == NULL) {
		printf("Cannot open file to send, press any key to exit!\n");
		system("pause");
		exit(0);
	}

	//S2，求得文件大小
	fseek(fp, 0L, SEEK_END);
	//size即文件大小
	long size = ftell(fp);
	rewind(fp);
	printf("\tget the size,the size is:%ld\t", size);

	//S3,申请一块能装下整个文件的空间
	char* ar = (char*)malloc(sizeof(char) * size);

	//S4,读文件，每次读一个，共读size次
	/*
	 *文件大小不确定，有可能比缓冲区大很多，调用一次 send() 函数不能完成文件内容的发送
	 *接收数据时 recv() 也会遇到同样的情况
	 *要解决这个问题，需要使用 while 循环
	*/
	int nCount;
	int i = 0;
	//这里fread()是不会出现不确定的情况，因为缓冲区ar是根据文件大小size创建的，所以总能一次read()完文件
	while ((nCount = fread(ar, 1, size, fp)) > 0) {
		printf("\tto read file of %d,and the nCount is %d\n", i + 1, nCount);
		i++;
	}
	/*
	 *注意读完后文件fp指针后移1*size个单位
	*/

	//S5,分片，分成PIECE份
	typedef struct
	{
		int grade;//当前下载标识
		char data[BUF_SIZE] = { 0 };
	}Slice;
	Slice sli[PIECE];

	//定义指针数组，指向结构体数组中的成员data[]
	//之所以需要这样一个数组，是因为send()/recv()参数buf的类型必须为const char*
	const char* dp[PIECE];
	for (i = 0; i < PIECE; i++)
	{
		dp[i] = sli[i].data;
	}

	//S6,ar数组缓存的数据赋值到堆栈内存
	//tsize记录每份数据大小,四舍五入。num记录ar指针指向的缓冲区的下一次读取到的数据
	int num = 0, j, tsize = (int)(size / PIECE) + 1;

	//余数表示额外的字节数
	int excP = size % tsize;
	//实际分片数应该等于，实际字节size除以每份分片大小tsize。而且无论有无余数，实际分片数应多1，因为数组计算从0而不是1开始
	int actP = size / tsize + 1;
	for (i = 0; i < PIECE; i++)
	{
		//初始化标号,ASCII码48就是整型的0
		sli[i].grade = i + 48;
		for (j = 0; j < tsize; j++)
		{
			//赋值之前先检查是否已经读取完了ar指向的动态数组元素
			//如果是做以下工作，把额外字节封装进结构体的成员数组以后便以'\0'结尾，避免访问到ar指向的未知区域
			if (i == actP - 1 && j == excP) {
				sli[actP - 1].data[excP] = '\0';
				break;
			}
			//上一步执行之后，可直接退出该循环
			if (i >= actP - 1 && sli[actP - 1].data[excP] == '\0') break;
			sli[i].data[j] = ar[j + num];
		}
		num += tsize;
	}

	//S7,准备套接字
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//服务器监听套接字为servSock
	SOCKET servSock = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockAddr.sin_port = htons(1234);
	bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
	listen(servSock, 20);

	SOCKADDR clntAddr;
	int nSize = sizeof(SOCKADDR);
	//服务器的连接套接字为sock
	SOCKET sock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);

	//S8,先检查下载标记，再从上次下载标识标记的地方开始下载
	FILE* tfp;
	//rb+表示文件必须先存在，读写方式为从内容的开始处覆盖原来内容
	if ((tfp = fopen("E:\\SYSTEM\\Server\\Piece\\Temp.txt", "rb+")) == NULL) {
		printf("Cannot open the file2, press any key to exit!\n");
		system("pause");
		exit(0);
	}
	//保存上次下载标识
	int id;
	//从tfp指向的文件即Temp.txt读取内容，默认为0，即从第一个片开始
	id = fgetc(tfp);
	int idsize = sizeof(id);

	//需要一个接收客户端应答的缓冲区
	char buffer[BUF_SIZE] = { 0 }, ibuf[2] = { 0 };
	char* bp = buffer;
	printf("Server will send files to the client\n\n\n\n");
	int recvLen = 0, sendLen = 0;
	for (i = 0; i < PIECE; i++)
	{
		//记录当前下载到哪一个片，保存到Temp.txt
		ibuf[0] = i + 48;
		ibuf[1] = '\0';
		fwrite(ibuf, 1, 1, tfp);
		rewind(tfp);
		printf("the %d of for circle\n", i);
		//上次下载到哪里，这一次便从那里开始
		if (sli[i].grade == id) {
			//告诉客户端现在发送哪一个分片
			send(sock, ibuf, 2, 0);
			recv(sock, buffer, 2, 0);
			memset(buffer, 0, BUF_SIZE);

			//每次发送一个分片
			printf("\tserver will send the %d piece of the file\n", i + 1);
			if ((sendLen = send(sock, dp[i], tsize, 0)) < 0)
			{
				printf("\tsend a mess to client had failed");
			}
			else
			{
				printf("\tsend a mess to client was succeeded, and the successful bytes is:%d, the mess is %s\n", sendLen, dp[i]);
			}
			/*
			 *服务器每发送一个分片，必须根据客户端的应答进行判断
			  *1、客户端应答已收到(用字符'f'表示)——继续发送剩下的分片，并且显示传输进度
			  *2、客户端应答未收到(用字符'n'表示)——保存当前下载标识，结束传输
			*/

			buffer[0] = 'n';
			//recv()的作用是阻塞，因为要接收客户端的应答
			printf("\t\twait to receive respond from client\n");
			if ((recvLen = recv(sock, buffer, 2, 0)) > 0) {
				printf("\tget a respond from client\n");
			}
			else
			{
				printf("\treceive respond failed!\n");
			}
			//应答超时设置为6s，表示经过了6s以后，若客户端还没有应答，就不再等待该应答
			//Sleep(6000);
			if (*bp == 'f')
			{
				//若客户端已收到应答
				//表示已发送完毕一个分片，下载标识+1，并且打印传输进度
				printf("\tthe rate of transported procession now is:%d%%\n", (i + 1) * 100 / PIECE);
				if (i % 10 == 0)
					printf("\n");
				id++;
				*bp = 'n';
			}
			else if (*bp == 'n')
			{
				//若客户端收不到应答
				//也就是当前分片也发送失败，即当前下载标识不需要自增1——记录当前下载标识
				printf("\tServer misses the respond from client\n");
				fwrite(ibuf, 1, 1, tfp);
				//printf("write in file %d\n", fwrite(ibuf, 1, 1, tfp));
				//rewind(tfp);
				fclose(tfp);
				break;
			}
			else
			{
				printf("nothing\n");
			}
			if (id - 49 == PIECE - 1)
				printf("\n传输完成！\n");
			printf("\n");
		}
	}

	shutdown(sock, SD_SEND);  //文件传输完毕，断开输出流，向客户端发送FIN包
	recv(sock, buffer, BUF_SIZE, 0);  //阻塞，等待客户端接收完毕

	fclose(fp);
	fclose(tfp);
	closesocket(sock);
	closesocket(servSock);
	WSACleanup();
}