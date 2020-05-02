/*Client Peer
 *Func:CSC_MOD Chatting
*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

#define BUF_SIZE 1024
#define PIECE 10

void FileTrans_send(char*);
void FileTrans_recv();
void FileTransIntnue_recv();

int main()
{
	// 加载socket动态链接库(dll)
	WORD wVersionRequested;
	WSADATA wsaData;	// 这结构是用于接收Windows Socket的结构信息的
	int err;

	wVersionRequested = MAKEWORD(2, 2);	// 请求2.2版本的WinSock库

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

	// 创建socket操作，建立流式套接字——即UDP连接，返回套接字号clntSock
	// SOCKET socket(int af, int type, int protocol);
	// 第一个参数，指定地址簇(TCP/IP只能是AF_INET，也可写成PF_INET)
	// 第二个，选择套接字的类型(流式套接字)，第三个，特定地址家族相关协议（0为自动）
	SOCKET clntSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (clntSock == INVALID_SOCKET)
	{
		printf("Socket error!");
		return 0;
	}

	// 将套接字clntSock与远程主机相连
	// int connect( SOCKET s,  const struct sockaddr* name,  int namelen);
	// 第一个参数：需要进行连接操作的套接字
	// 第二个参数：设定所需要连接的地址信息
	// 第三个参数：地址的长度
	SOCKADDR_IN servAddr;
	//servAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");		// 本地回路地址是127.0.0.1; 
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//等价于上一行代码
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(1234);
	int seLen = sizeof(servAddr);

	char recvBuf[BUF_SIZE] = "", sendBuf[BUF_SIZE] = "";
	int i = 0, ret;
	char ci = '0';//记录客户机连接数量

	//第一个sendto()是与服务器建立连接，发送完sendto()就是已经与服务器建立连接了
	sendto(clntSock, "Receive a client request \n", strlen("Receive a client request \n"), 0, (SOCKADDR*)&servAddr, seLen);
	//接受服务器反馈
	recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
	printf("%s\n", recvBuf);

	//功能编号
	int iden;
	while (1)
	{
		//system("pause");
		system("cls");

		printf("\n\n\t\t\tBIG PROJECT_CHATTING ROOM\n\n");

		printf("\t\t\t1:Regist\n");
		printf("\t\t\t2:Login\n");
		printf("\t\t\t0:Exit\n\n");

		//选择功能编号
		printf("\t\tPlease choose functional identify:");
		scanf("%d", &iden);

		if (iden == 0) exit(1);
		else if (iden == 1) {
			sendto(clntSock, "1", strlen("1"), 0, (SOCKADDR*)&servAddr, seLen);
			recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);

			//清屏
			system("cls");
			//声明变量
			char reg_name[32] = "", reg_pwd[32] = "", recvBuf[BUF_SIZE] = { 0 };//暂存注册用户名和密码
			int ret;//sendto()&recvfrom()返回值
			char* cret;//gets_s()返回值

			printf("\n\n\t\t\tRegist Module\n\n");
			while (1)
			{
				//输入用户名
				printf("\t\tPlease input your username[within 32 bytes]:");
				scanf("%s", reg_name);
				if (strlen(reg_name) > 32) printf("Beyond 32 bytes!\n");
				//gets_s(reg_name, 32);
				//if(cret==NULL) printf("Beyond 32 bytes!\n");

				//发送给服务器对比用户名是否已经注册
				memset(recvBuf, 0, BUF_SIZE);
				ret = sendto(clntSock, reg_name, strlen(reg_name), 0, (SOCKADDR*)&servAddr, seLen);
				if ((ret) <= 0) printf("Regist failed, error: unknown, try again，%d\n", errno);
				ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
				if ((ret) <= 0) printf("Regist failed, error: receive responds failed, try again, %d\n", errno);

				//recvfrom()接受服务器反馈标识：y为已注册；n为未注册
				///如果反馈y，就跳转到Login()；反馈n，就重新注册
				recvBuf[strlen("y")] = '\0';
				if (strcmp(recvBuf, "y") == 0) {
					printf("Your username had already existd,");
					printf(" skip to init interface while you press a key\n");
					system("pause");
					break;
				}

				//继续注册
				printf("\t\tPlease input your password[within 10 bytes]:");
				scanf("%s", reg_pwd);
				if (strlen(reg_pwd) <= 10)
				{
					//注册成功需要发送用户信息给服务器，recvfrom()刷新套接字缓冲区
					sendto(clntSock, reg_pwd, strlen(reg_pwd), 0, (SOCKADDR*)&servAddr, seLen);
					memset(recvBuf, 0, BUF_SIZE);
					ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);

					printf("\n\n\t\tRegist succeeded，your username is %s,pwd is %s\n\n", reg_name, reg_pwd);
					break;
				}
				else printf("\n\t\tThe length of your pwd is %d，try again\n", strlen(reg_pwd));
			}
			continue;
		}
		else if (iden == 2) {
			sendto(clntSock, "2", strlen("2"), 0, (SOCKADDR*)&servAddr, seLen);
			recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);

			system("cls");

			//声明变量
			char on_name[32] = "", on_pwd[10] = "";

			printf("\n\n\t\t\tLogin Module\n\n");

			printf("\t\tPlease input your username:");
			scanf("%s", on_name);

			/*登录也要sendto()用户名给服务器对比作身份认证*/
			memset(recvBuf, 0, BUF_SIZE);
			sendto(clntSock, on_name, strlen(on_name), 0, (SOCKADDR*)&servAddr, seLen);

			//收到服务器回复，也就是用户名无误才提示输入密码
			recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
			if (strcmp(recvBuf, "n") == 0) {
				printf("The username you input is not incorrect, try again\n");
				system("pause");
				continue;
			}
			printf("%s", recvBuf);
			scanf("%s", on_pwd);

			/*sendto()密码给服务器对比作身份认证，服务器反馈身份认证以后才能登录成功*/
			memset(recvBuf, 0, BUF_SIZE);
			sendto(clntSock, on_pwd, strlen(on_pwd), 0, (SOCKADDR*)&servAddr, seLen);

			//收到服务器回复，也就是密码无误才通过身份认证
			recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);

			//recvfrom()接受服务器反馈标识：y为身份认证通过；n为身份认证未通过——密码错误
			///如果反馈y，就跳转到Login()；反馈n，就重新注册
			recvBuf[strlen("y")] = '\0';
			if (strcmp(recvBuf, "y") == 0) {
				printf("You are login successfully\n");
				system("pause");
				system("cls");
				break;
			}
			else if (strcmp(recvBuf, "n") == 0) {
				printf("\n\n\t\tYour password is incorrect, try again\n\n");
				system("pause");
				continue;
			}
		}
		else {
			printf("\n\t\tFunctional iden you inputed was incorrect, try again!\n");
			continue;
		}
	}

	while (1)//检测当前是否有两台客户端连接上服务器，如果没有就一直停留recvfrom()阻塞
	{
		//接收服务器上显示的连接数量
		memset(recvBuf, 0, BUF_SIZE);
		recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
		ci = recvBuf[0];
		memset(recvBuf, 0, BUF_SIZE);
		ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
		recvBuf[ret] = '\0';
		printf("%s", recvBuf);

		//如果没有两台客户端连接上服务器的话
		if (ci == '2') {
			printf("\n\n\t\tStart your chatting!\n");
			break;
		}
		else printf("\nFailed to initiate conversation while only you exist!\n");
	}

	memset(recvBuf, 0, BUF_SIZE);
	fd_set readfds;//定义一个套接字集合变量
	ret = 0;//返回值
	char temp[BUF_SIZE] = { 0 };//临时数据
	int sret = 0, glen = 0;//select()返回值
	struct timeval timeout = { 0,0 };
	iden = 0;
	FILE* fp, * tfp;//fp是保存聊天记录的文件指针；tfp是要传输的文件的指针
	char filename[100] = { 0 };//存放要传输文件的路径
	char fpath[100] = "E:\\SYSTEM\\Client\\Record_Saved\\";

	while (1) {//聊天室功能模块

		system("pause");
		//system("cls");

		printf("\n\n\t\t\tChatting room function panel\n\n");

		printf("\t\t\t1:Chatting\n");
		printf("\t\t\t2:Save chatting records\n");
		printf("\t\t\t3:File translation\n");
		printf("\t\t\t4:File trans through broken point\n");
		printf("\t\t\t0:Exit\n\n");

		//选择功能编号
		printf("\t\tPlease choose functional identify:");
		scanf("%d", &iden);

		if (iden == 0) exit(1);
		else if (iden == 1) {

			while (1) {//发起会话
				sendto(clntSock, "1", strlen("1"), 0, (SOCKADDR*)&servAddr, seLen);
				recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);

				memset(recvBuf, 0, BUF_SIZE);
				sendto(clntSock, " ", strlen(" "), 0, (SOCKADDR*)&servAddr, seLen);
				printf("\n\t\tWait to initiate a conversation...\n");
				ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
				recvBuf[ret] = '\0';
				if (recvBuf) {
					printf("%s\n", recvBuf);
					break;
				}
			}
			printf("\t\tIf you wanna stop chatting input 'quit'!\n");
			system("pause");
			system("cls");

			while (1)//收发信息
			{
				/*
				 *客户端的收发要求：因为不知道哪个客户端先发送，所以不能确定客户端是先调用sendto()还是recvfrom()
				 *而recvfrom()是会阻塞的，如果粗暴地把recvfrom()先调用，那所以客户端都被阻塞了
				 *
				 *因此需要利用select()的超时机制把阻塞变成"非阻塞"
				*/
				/*select()工作流程
				1、用FD_ZERO宏来初始化我们感兴趣的fd_set(fd_set是文件描述符集合，初始化的是select()的第234个参数)
				2、用FD_SET宏来将套接字句柄分配给相应的fd_set，也就是加入我们要进行监视变化的套接字句柄
				3、调用select()
				4、用FD_ISSET宏来对套接字句柄进行检查，如果套接字发生了变化那么fd_set会保留给套接字
				5、进行I/O操作
				*/

				//在调用select()前，总要先初始化
				FD_ZERO(&readfds);
				//将套接字添加到readfds
				FD_SET(clntSock, &readfds);
				//调用select()等待它的完成，也就是等待检查clntSock是否有数据写入
				/*select()对struct timeval *timeout的值设置
				1、NULL：会一直阻塞直到文件变化
				2、0：非阻塞无论有无变化
				3、大于0：等待超时
				*/
				if ((sret = select(0, &readfds, NULL, NULL, &timeout)) == SOCKET_ERROR) {
					//处理错误
					printf("Select error\n");
					return -1;
				}
				//返回值大于0，说明有符合条件的套接字
				//并用ISSET检查套接字是否在返回集合中，在就说明有变化，不在就说明没有变化
				if (sret > 0) {
					if (FD_ISSET(clntSock, &readfds)) {
						memset(recvBuf, 0, BUF_SIZE);
						ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
						recvBuf[ret] = '\0';
						if (strcmp(recvBuf, "quit") == 0) break;
						if (ret < 0) {
							printf("Recv error");
							return -1;
						}
						else if (ret == 0) {
							printf("The other point had closed the connection\n");
							return -1;
						}
						else {
							printf(recvBuf);		// 接收信息
						}
					}
				}
				else
				{
					memset(sendBuf, 0, BUF_SIZE);
					//printf("\nPlease enter what you want to say next(within 32 bytes):");
					gets_s(sendBuf, 32);
					glen = strlen(sendBuf);
					sendBuf[glen] = '\0';
					if (strcmp(sendBuf, "quit") == 0) {
						sendto(clntSock, "quit", strlen("quit"), 0, (SOCKADDR*)&servAddr, seLen);
						recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
						break;
					}

					sendto(clntSock, sendBuf, strlen(sendBuf), 0, (SOCKADDR*)&servAddr, seLen);
				}
			}
		}
		else if (iden == 2) {
			memset(recvBuf, 0, BUF_SIZE);
			sendto(clntSock, "2", strlen("2"), 0, (SOCKADDR*)&servAddr, seLen);
			recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);

			system("cls");

			//刷新作用
			sendto(clntSock, "Record Saving", strlen("Record Saving"), 0, (SOCKADDR*)&servAddr, seLen);

			memset(recvBuf, 0, BUF_SIZE);
			//接收服务器提问
			recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
			printf("\n%s", recvBuf);
			scanf("%s", temp);

			//完善保存路径
			sprintf(fpath, "%s%s%s", fpath, temp, ".txt");
			fp = fopen(fpath, "wb+");

			//发送用户名——想要得到的该用户的聊天记录
			sendto(clntSock, temp, strlen(temp), 0, (SOCKADDR*)&servAddr, seLen);

			//接收聊天记录
			memset(recvBuf, 0, BUF_SIZE);
			ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
			fwrite(recvBuf, 1, ret, fp);
			/*
			while ((ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR *)&servAddr, &seLen)) > 0) {
				fwrite(recvBuf, 1, ret, fp);
				if (ret == 0) break;
			}
			*/
			printf("Record had saved!\n");
		}
		else if (iden == 3) {

			while (1) {//需要两个客户端一起连接才能传文件
				sendto(clntSock, "3", strlen("3"), 0, (SOCKADDR*)&servAddr, seLen);
				recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);

				sendto(clntSock, " ", strlen(" "), 0, (SOCKADDR*)&servAddr, seLen);
				if (strcmp(recvBuf, "y") == 0) {
					//第一台客户端连接上来接收来自服务器响应的是'y'
					printf("\n\t\tThere are no other client online, ");
					printf("you cannot send file to only yourself!\n");
					memset(recvBuf, 0, BUF_SIZE);
					ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
					recvBuf[ret] = '\0';
					printf("%s\n", recvBuf);
					memset(recvBuf, 0, BUF_SIZE);
					//接收客户端的回复
					ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
					recvBuf[ret] = '\0';
					if (strcmp(recvBuf, "n") == 0) {
						printf("\t\tYour friend rejusts the file transport\n");
						break;
					}
					else if (strcmp(recvBuf, "y") == 0) {
						system("pause");
						system("cls");
						printf("\t\tPlease remain the moniter as bright as possible!\n");

						//发送文件给中转站服务器
						printf("\n\t\tPlease input the filepath you wanna send[such as 'X:\Program File\']:");
						memset(filename, 0, BUF_SIZE);
						scanf("%s", filename);
						memset(recvBuf, 0, BUF_SIZE);
						sprintf(recvBuf, "%s", filename);
						printf("\n\t\tNow input filename to send[such as 'xxx.mp4']:");
						scanf("%s", filename);
						//发送文件名给服务器
						sendto(clntSock, filename, strlen(filename), 0, (SOCKADDR*)&servAddr, seLen);
						sprintf(recvBuf, "%s%s", recvBuf, filename);
						FileTrans_send(recvBuf);
						break;
					}
				}
				else if (strcmp(recvBuf, "x") == 0) {
					memset(recvBuf, 0, BUF_SIZE);
					ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
					recvBuf[ret] = '\0';
					printf("%s\n", recvBuf);
					memset(recvBuf, 0, BUF_SIZE);
					//客户端回复
					scanf("%s", recvBuf);
					sendto(clntSock, recvBuf, strlen(recvBuf), 0, (SOCKADDR*)&servAddr, seLen);
					if (strcmp(recvBuf, "n") == 0) {
						printf("\t\tYou rejust the file transport\n");
						break;
					}
					else if (strcmp(recvBuf, "y") == 0) {
						system("pause");
						system("cls");
						printf("\t\tPlease remain the moniter as bright as possible!\n");

						printf("\t\tPlease wait a minutes...\n");

						//从服务器接收文件
						//Sleep(10000);
						FileTrans_recv();
						break;
					}
				}
			}
		}
		else if (iden == 4) {
			sendto(clntSock, "4", strlen("4"), 0, (SOCKADDR*)&servAddr, seLen);
			recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);//x

			memset(recvBuf, 0, BUF_SIZE);
			printf("\t\t- - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
			printf("\t\t\t\tA test demo, just file-txt supposed!\n");
			printf("\t\t- - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
			printf("\n\t\tWhich choice do you want, send or receive?[send/recv]\n");
			scanf("%s", recvBuf);

			if (strcmp(recvBuf, "send") == 0) {
				printf("\n\t\tThere are no other client online, ");
				printf("you cannot send file to only yourself!\n");
				sendto(clntSock, " ", strlen(" "), 0, (SOCKADDR*)&servAddr, seLen);
				memset(recvBuf, 0, BUF_SIZE);
				ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
				recvBuf[ret] = '\0';
				printf("%s\n", recvBuf);
				memset(recvBuf, 0, BUF_SIZE);
				//接收客户端的回复
				ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
				recvBuf[ret] = '\0';
				if (strcmp(recvBuf, "n") == 0) {
					printf("\t\tYour friend rejusts the file transport\n");
					break;
				}
				else if (strcmp(recvBuf, "y") == 0) {
					system("pause");
					system("cls");
					printf("\t\tPlease remain the moniter as bright as possible!\n");

					//发送文件给中转站服务器
					printf("\n\t\tPlease input the filepath you wanna send[such as 'X:\Program File\']:");
					memset(filename, 0, BUF_SIZE);
					scanf("%s", filename);
					memset(recvBuf, 0, BUF_SIZE);
					sprintf(recvBuf, "%s", filename);
					printf("\n\t\tNow input filename to send[such as 'xxx.mp4']:");
					scanf("%s", filename);
					//发送文件名给服务器
					sendto(clntSock, filename, strlen(filename), 0, (SOCKADDR*)&servAddr, seLen);
					sprintf(recvBuf, "%s%s", recvBuf, filename);
					FileTrans_send(recvBuf);
					break;
				}
			}
			else if (strcmp(recvBuf, "recv") == 0) {
				sendto(clntSock, " ", strlen(" "), 0, (SOCKADDR*)&servAddr, seLen);
				ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
				recvBuf[ret] = '\0';
				printf("%s\n", recvBuf);
				//客户端回复
				scanf("%s", recvBuf);
				sendto(clntSock, recvBuf, strlen(recvBuf), 0, (SOCKADDR*)&servAddr, seLen);
				if (strcmp(recvBuf, "n") == 0) {
					printf("\t\tYou rejust the file transport\n");
					break;
				}
				else if (strcmp(recvBuf, "y") == 0) {
					system("pause");
					system("cls");
					printf("\t\tPlease remain the moniter as bright as possible!\n");

					printf("\t\tPlease wait a minutes...\n");

					//从服务器接收文件
					FileTransIntnue_recv();
					break;
				}

				memset(recvBuf, 0, BUF_SIZE); ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
				recvBuf[ret] = '\0';
				printf("\n\t\t%s\n", recvBuf);
			}
		}
	}

	closesocket(clntSock);
	WSACleanup();	// 终止对套接字库的使用

	printf("\n");
	system("pause");
	return 0;
}

void FileTrans_recv() {
	//先输入文件名，看文件是否能创建成功
	char filename[100] = { 0 };  //文件名
	printf("Input filename to save: ");
	scanf("%s", filename);
	FILE* fp = fopen(filename, "wb");  //以二进制方式打开（创建）文件
	if (fp == NULL) {
		printf("Cannot open file, press any key to exit!\n");
		system("pause");
		exit(0);
	}

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockAddr.sin_port = htons(1234);
	connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

	//循环接收数据，直到文件传输完毕
	char buffer[BUF_SIZE] = { 0 };  //文件缓冲区
	int nCount;
	while ((nCount = recv(sock, buffer, BUF_SIZE, 0)) > 0) {
		fwrite(buffer, nCount, 1, fp);
	}
	puts("File received successfully!");

	//文件接收完毕后直接关闭套接字，无需调用shutdown()
	fclose(fp);
	closesocket(sock);
	WSACleanup();
	system("pause");
}

void FileTrans_send(char* filename) {

	FILE* fp = fopen(filename, "rb");  //以二进制方式打开文件
	if (fp == NULL) {
		printf("Cannot open file, press any key to exit!\n");
		system("pause");
		exit(0);
	}

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockAddr.sin_port = htons(1234);
	connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

	//循环发送数据，直到文件传输完毕
	char buffer[BUF_SIZE] = { 0 };  //文件缓冲区
	int nCount;
	while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
		send(sock, buffer, nCount, 0);
	}

	//发送文件一方要主动断开连接,并向服务器发送FIN包
	shutdown(sock, SD_SEND);
	puts("File sent successfully!");

	//文件发送完毕后客户端便主动关闭套接字
	fclose(fp);
	closesocket(sock);
	WSACleanup();
	system("pause");
}

void FileTransIntnue_recv() {
	/*
	*InterNue means Interrupt&contiNue
	*
	*Client's socket names clntSock
	*Server's socket used to connect names sock
	*
	* recv(sock->buffer)
	*  ->"断点续传"
	*   ->fwrite(buffer->文件fp)
	*/

	//先输入文件名，看文件是否能创建成功
	char filename[100] = { 0 };  //文件名
	printf("Input filename to save: ");
	scanf("%s", filename);
	//以二进制方式打开（创建）文件,从文件数据末部追加数据
	FILE* fp = fopen(filename, "ab+");
	if (fp == NULL) {
		printf("Cannot open file, press any key to exit!\n");
		system("pause");
		exit(0);
	}

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET clntSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockAddr.sin_port = htons(1234);
	connect(clntSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
	printf("client was connected!\n");

	//此时Server send()分片
	//Client先recv()，再处理断点数据，最后才fwrite()到本地文件filename

	/*
	 *接收缓冲区是连续接受数据的
	 *  每次接收一个片就返回应答
	 *  同时写进文件(buffer->rfp)
	*/

	char buffer[BUF_SIZE] = { 0 };
	//客户端用指向字符常量的指针来表示应答
	char idNarr[2] = "n";
	char idFarr[2] = "f";
	const char* ip = idNarr;
	//该应答标识的实际长度
	//int len = sizeof(ip) / sizeof(const char *);
	int len = 2;
	//实际接收字节
	int rLen = 0, i, nCount, fwLen;
	printf("\n\n\n\n");
	for (i = 0; i < PIECE; i++)
	{
		//接受服务器发来的标记
		recv(clntSock, buffer, 2, 0);
		i = buffer[0] - 48;
		send(clntSock, "ok", 2, 0);
		memset(buffer, 0, BUF_SIZE);

		printf("the %d of for circle\n", i);
		rLen = recv(clntSock, buffer, BUF_SIZE, 0);
		buffer[rLen] = '\0';
		//检验是否从套接字的输入缓冲区接收到数据

		//接收到数据就把应答标识标志为f
		ip = idFarr;
		//发送应答标识，并检查是否发送成功
		if ((nCount = send(clntSock, ip, len, 0)) < 0)
		{
			printf("send a mess to server had failed, and the successful bytes is:%d\n", nCount);
		}
		else
		{
			printf("send a mess to server was succeeded, and the successful bytes is:%d\n", nCount);
		}
		ip = idNarr;
		printf("!!!_tfp is %d", ftell(fp));
		//将接收到的数据写入本地文件，并检查是否写入成功
		if ((fwLen = fwrite(buffer, 1, rLen, fp)) > 0) {
			printf("\tto write something into file of %d,and the nCount is %d, _tfp is %d\n", i + 1, rLen, ftell(fp));
		}
		else
		{
			printf("fwrite failed!,_tfp is %d\n", ftell(fp));
		}
		Sleep(1000);
	}

	//文件接收完毕后直接关闭套接字，无需调用shutdown()
	fclose(fp);
	closesocket(clntSock);
	WSACleanup();
}