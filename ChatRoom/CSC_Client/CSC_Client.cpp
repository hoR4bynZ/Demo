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
	// ����socket��̬���ӿ�(dll)
	WORD wVersionRequested;
	WSADATA wsaData;	// ��ṹ�����ڽ���Windows Socket�Ľṹ��Ϣ��
	int err;

	wVersionRequested = MAKEWORD(2, 2);	// ����2.2�汾��WinSock��

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return -1;			// ����ֵΪ���ʱ���Ǳ�ʾ�ɹ�����WSAStartup
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		// ���������ֽ��ǲ���2�����ֽ��ǲ���2��ȷ���Ƿ������������2.2�汾
		// ����Ļ�������WSACleanup()�����Ϣ����������
		WSACleanup();
		return -1;
	}

	/*
	 *�������ɼ�Ϊ
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	*/

	// ����socket������������ʽ�׽��֡�����UDP���ӣ������׽��ֺ�clntSock
	// SOCKET socket(int af, int type, int protocol);
	// ��һ��������ָ����ַ��(TCP/IPֻ����AF_INET��Ҳ��д��PF_INET)
	// �ڶ�����ѡ���׽��ֵ�����(��ʽ�׽���)�����������ض���ַ�������Э�飨0Ϊ�Զ���
	SOCKET clntSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (clntSock == INVALID_SOCKET)
	{
		printf("Socket error!");
		return 0;
	}

	// ���׽���clntSock��Զ����������
	// int connect( SOCKET s,  const struct sockaddr* name,  int namelen);
	// ��һ����������Ҫ�������Ӳ������׽���
	// �ڶ����������趨����Ҫ���ӵĵ�ַ��Ϣ
	// ��������������ַ�ĳ���
	SOCKADDR_IN servAddr;
	//servAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");		// ���ػ�·��ַ��127.0.0.1; 
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//�ȼ�����һ�д���
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(1234);
	int seLen = sizeof(servAddr);

	char recvBuf[BUF_SIZE] = "", sendBuf[BUF_SIZE] = "";
	int i = 0, ret;
	char ci = '0';//��¼�ͻ�����������

	//��һ��sendto()����������������ӣ�������sendto()�����Ѿ������������������
	sendto(clntSock, "Receive a client request \n", strlen("Receive a client request \n"), 0, (SOCKADDR*)&servAddr, seLen);
	//���ܷ���������
	recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
	printf("%s\n", recvBuf);

	//���ܱ��
	int iden;
	while (1)
	{
		//system("pause");
		system("cls");

		printf("\n\n\t\t\tBIG PROJECT_CHATTING ROOM\n\n");

		printf("\t\t\t1:Regist\n");
		printf("\t\t\t2:Login\n");
		printf("\t\t\t0:Exit\n\n");

		//ѡ���ܱ��
		printf("\t\tPlease choose functional identify:");
		scanf("%d", &iden);

		if (iden == 0) exit(1);
		else if (iden == 1) {
			sendto(clntSock, "1", strlen("1"), 0, (SOCKADDR*)&servAddr, seLen);
			recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);

			//����
			system("cls");
			//��������
			char reg_name[32] = "", reg_pwd[32] = "", recvBuf[BUF_SIZE] = { 0 };//�ݴ�ע���û���������
			int ret;//sendto()&recvfrom()����ֵ
			char* cret;//gets_s()����ֵ

			printf("\n\n\t\t\tRegist Module\n\n");
			while (1)
			{
				//�����û���
				printf("\t\tPlease input your username[within 32 bytes]:");
				scanf("%s", reg_name);
				if (strlen(reg_name) > 32) printf("Beyond 32 bytes!\n");
				//gets_s(reg_name, 32);
				//if(cret==NULL) printf("Beyond 32 bytes!\n");

				//���͸��������Ա��û����Ƿ��Ѿ�ע��
				memset(recvBuf, 0, BUF_SIZE);
				ret = sendto(clntSock, reg_name, strlen(reg_name), 0, (SOCKADDR*)&servAddr, seLen);
				if ((ret) <= 0) printf("Regist failed, error: unknown, try again��%d\n", errno);
				ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
				if ((ret) <= 0) printf("Regist failed, error: receive responds failed, try again, %d\n", errno);

				//recvfrom()���ܷ�����������ʶ��yΪ��ע�᣻nΪδע��
				///�������y������ת��Login()������n��������ע��
				recvBuf[strlen("y")] = '\0';
				if (strcmp(recvBuf, "y") == 0) {
					printf("Your username had already existd,");
					printf(" skip to init interface while you press a key\n");
					system("pause");
					break;
				}

				//����ע��
				printf("\t\tPlease input your password[within 10 bytes]:");
				scanf("%s", reg_pwd);
				if (strlen(reg_pwd) <= 10)
				{
					//ע��ɹ���Ҫ�����û���Ϣ����������recvfrom()ˢ���׽��ֻ�����
					sendto(clntSock, reg_pwd, strlen(reg_pwd), 0, (SOCKADDR*)&servAddr, seLen);
					memset(recvBuf, 0, BUF_SIZE);
					ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);

					printf("\n\n\t\tRegist succeeded��your username is %s,pwd is %s\n\n", reg_name, reg_pwd);
					break;
				}
				else printf("\n\t\tThe length of your pwd is %d��try again\n", strlen(reg_pwd));
			}
			continue;
		}
		else if (iden == 2) {
			sendto(clntSock, "2", strlen("2"), 0, (SOCKADDR*)&servAddr, seLen);
			recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);

			system("cls");

			//��������
			char on_name[32] = "", on_pwd[10] = "";

			printf("\n\n\t\t\tLogin Module\n\n");

			printf("\t\tPlease input your username:");
			scanf("%s", on_name);

			/*��¼ҲҪsendto()�û������������Ա��������֤*/
			memset(recvBuf, 0, BUF_SIZE);
			sendto(clntSock, on_name, strlen(on_name), 0, (SOCKADDR*)&servAddr, seLen);

			//�յ��������ظ���Ҳ�����û����������ʾ��������
			recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
			if (strcmp(recvBuf, "n") == 0) {
				printf("The username you input is not incorrect, try again\n");
				system("pause");
				continue;
			}
			printf("%s", recvBuf);
			scanf("%s", on_pwd);

			/*sendto()������������Ա��������֤�����������������֤�Ժ���ܵ�¼�ɹ�*/
			memset(recvBuf, 0, BUF_SIZE);
			sendto(clntSock, on_pwd, strlen(on_pwd), 0, (SOCKADDR*)&servAddr, seLen);

			//�յ��������ظ���Ҳ�������������ͨ�������֤
			recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);

			//recvfrom()���ܷ�����������ʶ��yΪ�����֤ͨ����nΪ�����֤δͨ�������������
			///�������y������ת��Login()������n��������ע��
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

	while (1)//��⵱ǰ�Ƿ�����̨�ͻ��������Ϸ����������û�о�һֱͣ��recvfrom()����
	{
		//���շ���������ʾ����������
		memset(recvBuf, 0, BUF_SIZE);
		recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
		ci = recvBuf[0];
		memset(recvBuf, 0, BUF_SIZE);
		ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
		recvBuf[ret] = '\0';
		printf("%s", recvBuf);

		//���û����̨�ͻ��������Ϸ������Ļ�
		if (ci == '2') {
			printf("\n\n\t\tStart your chatting!\n");
			break;
		}
		else printf("\nFailed to initiate conversation while only you exist!\n");
	}

	memset(recvBuf, 0, BUF_SIZE);
	fd_set readfds;//����һ���׽��ּ��ϱ���
	ret = 0;//����ֵ
	char temp[BUF_SIZE] = { 0 };//��ʱ����
	int sret = 0, glen = 0;//select()����ֵ
	struct timeval timeout = { 0,0 };
	iden = 0;
	FILE* fp, * tfp;//fp�Ǳ��������¼���ļ�ָ�룻tfp��Ҫ������ļ���ָ��
	char filename[100] = { 0 };//���Ҫ�����ļ���·��
	char fpath[100] = "E:\\SYSTEM\\Client\\Record_Saved\\";

	while (1) {//�����ҹ���ģ��

		system("pause");
		//system("cls");

		printf("\n\n\t\t\tChatting room function panel\n\n");

		printf("\t\t\t1:Chatting\n");
		printf("\t\t\t2:Save chatting records\n");
		printf("\t\t\t3:File translation\n");
		printf("\t\t\t4:File trans through broken point\n");
		printf("\t\t\t0:Exit\n\n");

		//ѡ���ܱ��
		printf("\t\tPlease choose functional identify:");
		scanf("%d", &iden);

		if (iden == 0) exit(1);
		else if (iden == 1) {

			while (1) {//����Ự
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

			while (1)//�շ���Ϣ
			{
				/*
				 *�ͻ��˵��շ�Ҫ����Ϊ��֪���ĸ��ͻ����ȷ��ͣ����Բ���ȷ���ͻ������ȵ���sendto()����recvfrom()
				 *��recvfrom()�ǻ������ģ�����ֱ��ذ�recvfrom()�ȵ��ã������Կͻ��˶���������
				 *
				 *�����Ҫ����select()�ĳ�ʱ���ư��������"������"
				*/
				/*select()��������
				1����FD_ZERO������ʼ�����Ǹ���Ȥ��fd_set(fd_set���ļ����������ϣ���ʼ������select()�ĵ�234������)
				2����FD_SET�������׽��־���������Ӧ��fd_set��Ҳ���Ǽ�������Ҫ���м��ӱ仯���׽��־��
				3������select()
				4����FD_ISSET�������׽��־�����м�飬����׽��ַ����˱仯��ôfd_set�ᱣ�����׽���
				5������I/O����
				*/

				//�ڵ���select()ǰ����Ҫ�ȳ�ʼ��
				FD_ZERO(&readfds);
				//���׽�����ӵ�readfds
				FD_SET(clntSock, &readfds);
				//����select()�ȴ�������ɣ�Ҳ���ǵȴ����clntSock�Ƿ�������д��
				/*select()��struct timeval *timeout��ֵ����
				1��NULL����һֱ����ֱ���ļ��仯
				2��0���������������ޱ仯
				3������0���ȴ���ʱ
				*/
				if ((sret = select(0, &readfds, NULL, NULL, &timeout)) == SOCKET_ERROR) {
					//�������
					printf("Select error\n");
					return -1;
				}
				//����ֵ����0��˵���з����������׽���
				//����ISSET����׽����Ƿ��ڷ��ؼ����У��ھ�˵���б仯�����ھ�˵��û�б仯
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
							printf(recvBuf);		// ������Ϣ
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

			//ˢ������
			sendto(clntSock, "Record Saving", strlen("Record Saving"), 0, (SOCKADDR*)&servAddr, seLen);

			memset(recvBuf, 0, BUF_SIZE);
			//���շ���������
			recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
			printf("\n%s", recvBuf);
			scanf("%s", temp);

			//���Ʊ���·��
			sprintf(fpath, "%s%s%s", fpath, temp, ".txt");
			fp = fopen(fpath, "wb+");

			//�����û���������Ҫ�õ��ĸ��û��������¼
			sendto(clntSock, temp, strlen(temp), 0, (SOCKADDR*)&servAddr, seLen);

			//���������¼
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

			while (1) {//��Ҫ�����ͻ���һ�����Ӳ��ܴ��ļ�
				sendto(clntSock, "3", strlen("3"), 0, (SOCKADDR*)&servAddr, seLen);
				recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);

				sendto(clntSock, " ", strlen(" "), 0, (SOCKADDR*)&servAddr, seLen);
				if (strcmp(recvBuf, "y") == 0) {
					//��һ̨�ͻ������������������Է�������Ӧ����'y'
					printf("\n\t\tThere are no other client online, ");
					printf("you cannot send file to only yourself!\n");
					memset(recvBuf, 0, BUF_SIZE);
					ret = recvfrom(clntSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&servAddr, &seLen);
					recvBuf[ret] = '\0';
					printf("%s\n", recvBuf);
					memset(recvBuf, 0, BUF_SIZE);
					//���տͻ��˵Ļظ�
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

						//�����ļ�����תվ������
						printf("\n\t\tPlease input the filepath you wanna send[such as 'X:\Program File\']:");
						memset(filename, 0, BUF_SIZE);
						scanf("%s", filename);
						memset(recvBuf, 0, BUF_SIZE);
						sprintf(recvBuf, "%s", filename);
						printf("\n\t\tNow input filename to send[such as 'xxx.mp4']:");
						scanf("%s", filename);
						//�����ļ�����������
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
					//�ͻ��˻ظ�
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

						//�ӷ����������ļ�
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
				//���տͻ��˵Ļظ�
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

					//�����ļ�����תվ������
					printf("\n\t\tPlease input the filepath you wanna send[such as 'X:\Program File\']:");
					memset(filename, 0, BUF_SIZE);
					scanf("%s", filename);
					memset(recvBuf, 0, BUF_SIZE);
					sprintf(recvBuf, "%s", filename);
					printf("\n\t\tNow input filename to send[such as 'xxx.mp4']:");
					scanf("%s", filename);
					//�����ļ�����������
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
				//�ͻ��˻ظ�
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

					//�ӷ����������ļ�
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
	WSACleanup();	// ��ֹ���׽��ֿ��ʹ��

	printf("\n");
	system("pause");
	return 0;
}

void FileTrans_recv() {
	//�������ļ��������ļ��Ƿ��ܴ����ɹ�
	char filename[100] = { 0 };  //�ļ���
	printf("Input filename to save: ");
	scanf("%s", filename);
	FILE* fp = fopen(filename, "wb");  //�Զ����Ʒ�ʽ�򿪣��������ļ�
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

	//ѭ���������ݣ�ֱ���ļ��������
	char buffer[BUF_SIZE] = { 0 };  //�ļ�������
	int nCount;
	while ((nCount = recv(sock, buffer, BUF_SIZE, 0)) > 0) {
		fwrite(buffer, nCount, 1, fp);
	}
	puts("File received successfully!");

	//�ļ�������Ϻ�ֱ�ӹر��׽��֣��������shutdown()
	fclose(fp);
	closesocket(sock);
	WSACleanup();
	system("pause");
}

void FileTrans_send(char* filename) {

	FILE* fp = fopen(filename, "rb");  //�Զ����Ʒ�ʽ���ļ�
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

	//ѭ���������ݣ�ֱ���ļ��������
	char buffer[BUF_SIZE] = { 0 };  //�ļ�������
	int nCount;
	while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
		send(sock, buffer, nCount, 0);
	}

	//�����ļ�һ��Ҫ�����Ͽ�����,�������������FIN��
	shutdown(sock, SD_SEND);
	puts("File sent successfully!");

	//�ļ�������Ϻ�ͻ��˱������ر��׽���
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
	*  ->"�ϵ�����"
	*   ->fwrite(buffer->�ļ�fp)
	*/

	//�������ļ��������ļ��Ƿ��ܴ����ɹ�
	char filename[100] = { 0 };  //�ļ���
	printf("Input filename to save: ");
	scanf("%s", filename);
	//�Զ����Ʒ�ʽ�򿪣��������ļ�,���ļ�����ĩ��׷������
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

	//��ʱServer send()��Ƭ
	//Client��recv()���ٴ���ϵ����ݣ�����fwrite()�������ļ�filename

	/*
	 *���ջ������������������ݵ�
	 *  ÿ�ν���һ��Ƭ�ͷ���Ӧ��
	 *  ͬʱд���ļ�(buffer->rfp)
	*/

	char buffer[BUF_SIZE] = { 0 };
	//�ͻ�����ָ���ַ�������ָ������ʾӦ��
	char idNarr[2] = "n";
	char idFarr[2] = "f";
	const char* ip = idNarr;
	//��Ӧ���ʶ��ʵ�ʳ���
	//int len = sizeof(ip) / sizeof(const char *);
	int len = 2;
	//ʵ�ʽ����ֽ�
	int rLen = 0, i, nCount, fwLen;
	printf("\n\n\n\n");
	for (i = 0; i < PIECE; i++)
	{
		//���ܷ����������ı��
		recv(clntSock, buffer, 2, 0);
		i = buffer[0] - 48;
		send(clntSock, "ok", 2, 0);
		memset(buffer, 0, BUF_SIZE);

		printf("the %d of for circle\n", i);
		rLen = recv(clntSock, buffer, BUF_SIZE, 0);
		buffer[rLen] = '\0';
		//�����Ƿ���׽��ֵ����뻺�������յ�����

		//���յ����ݾͰ�Ӧ���ʶ��־Ϊf
		ip = idFarr;
		//����Ӧ���ʶ��������Ƿ��ͳɹ�
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
		//�����յ�������д�뱾���ļ���������Ƿ�д��ɹ�
		if ((fwLen = fwrite(buffer, 1, rLen, fp)) > 0) {
			printf("\tto write something into file of %d,and the nCount is %d, _tfp is %d\n", i + 1, rLen, ftell(fp));
		}
		else
		{
			printf("fwrite failed!,_tfp is %d\n", ftell(fp));
		}
		Sleep(1000);
	}

	//�ļ�������Ϻ�ֱ�ӹر��׽��֣��������shutdown()
	fclose(fp);
	closesocket(clntSock);
	WSACleanup();
}