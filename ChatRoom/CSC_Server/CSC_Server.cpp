/*Server.c
 *���ܣ�CSC Mod UDP_Chatting
*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")  //���� ws2_32.dll
#pragma warning (disable:4996)

#define BUF_SIZE 1024
#define PIECE 10

//�ͻ�����Ϣ�ṹ��
typedef struct _Client
{
	SOCKADDR_IN clntAddr;      //�ͻ����׽���
	char buf[BUF_SIZE];       //���ݻ���������Ϊ����ת�����ã�������һ���������͹���
	char userName[32];   //�ͻ����û���
	int flag = 0;       //��ǿͻ��ˣ��������ֲ�ͬ�Ŀͻ���
}Client;

Client g_Client[2] = { 0 };                  //����һ���ͻ��˽ṹ��
Client* cp = g_Client;

int linkNum = 0;//ȫ�ֱ�������ʾ��������

void FileTrans_send(char*);
void FileTrans_recv(char*);
void FileTransIntnue_send(char*);

int main() {
	//����socket��̬���ӿ�(dll)
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);    // ����2.2�汾��WinSock��

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
	char ci = '0';//��¼��������

	while (1) {
		/*���ӽ׶Σ����Թ���Ҫ��while()ѭ��һֱ�����������������ע���¼�������������ʧ��*/
		int ret = 0;//����ֵ
		//���տͻ��˵��������󣬳ɹ���������ʹ�ӡ�ÿͻ��˵�ip,���Ұ������ַ��Ϣ�����ͻ�����Ϣ�ṹ������
		ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);
		recvBuf[ret] = '\0';
		printf("%s\n", recvBuf);
		//���߿ͻ�����ɹ���������
		sendto(servSock, "You enter chatting room successfully!\n", strlen("You enter chatting room successfully!\n"), 0, (SOCKADDR*)&clntAddr, clLen);

		/*ע���¼�׶�*/
		//��������
		char unPath[BUF_SIZE];//�û���·��
		char temp[BUF_SIZE] = "";//��ʱ�����ݴ��������Ա���
		FILE* ufp;
		int i;//i����ֵ
		while (1) {

			/*���while()ѭ�������¼ע��*/

			//���տͻ��˹�������:0,1,2
			memset(recvBuf, 0, BUF_SIZE);
			recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);
			sendto(servSock, "y", strlen("y"), 0, (SOCKADDR*)&clntAddr, clLen);
			sprintf(temp, "%s", recvBuf);

			if (strcmp(temp, "1") == 0) {
				/*ע��ģ��*/

				/*����û����Ƿ����*/
				//�ͻ��˻ᷢ���û������������÷���������Ƿ���ע��
				memset(recvBuf, 0, BUF_SIZE);
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);
				recvBuf[ret] = '\0';
				//�����û���·��
				sprintf(unPath, "%s%s", "E:\\SYSTEM\\Server\\UserData\\", recvBuf);
				//r��һ���Ѵ����ļ����ļ������ڳ���ֻ��
				ufp = fopen(unPath, "r");
				//�ж��û����Ƿ����
				//fp!=NULL������ɹ������ļ�
				if (ufp != NULL) {
					sendto(servSock, "y", strlen("y"), 0, (SOCKADDR*)&clntAddr, clLen);
					continue;
				}
				else sendto(servSock, "n", strlen("n"), 0, (SOCKADDR*)&clntAddr, clLen);
				//fclose(ufp);

				/*��¼����*/
				memset(recvBuf, 0, BUF_SIZE);
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);

				//w��/�½�һ���ļ���ֻд
				ufp = fopen(unPath, "w");
				//���û����������ļ���������Ϊ�ļ��ı�����
				if ((fputs(recvBuf, ufp)) != 0) printf("Password was failed to save\n");

				sendto(servSock, "password saved", strlen("password saved"), 0, (SOCKADDR*)&clntAddr, clLen);
				fclose(ufp);
			}
			else if (strcmp(temp, "2") == 0) {
				/*��¼ģ��*/

				/*����û���*/
				memset(recvBuf, 0, BUF_SIZE);
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);

				//�ѿͻ���ip��port��Ϣ���浽�����Ա
				g_Client[linkNum].clntAddr = clntAddr;
				//�����û���
				recvBuf[ret] = '\0';
				sprintf(g_Client[linkNum].userName, "%s", recvBuf);

				//�����û���·��
				sprintf(unPath, "%s", "E:\\SYSTEM\\Server\\UserData\\");
				sprintf(unPath, "%s%s", unPath, recvBuf);
				//r��һ���Ѵ����ļ����ļ������ڳ���ֻ��
				if ((ufp = fopen(unPath, "r")) == NULL) {
					printf("File was failed to open\n");
					sendto(servSock, "n", strlen("n"), 0, (SOCKADDR*)&clntAddr, clLen);
					continue;
				}

				sendto(servSock, "\n\t\tPlease input your password:", strlen("\n\t\tPlease input your password:"), 0, (SOCKADDR*)&clntAddr, clLen);

				/*�������*/
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

				/*���ͻ��˵�¼����*/
				linkNum++;
				ci++;
				//��ӡ��ǰ�ͻ�����������
				printf("the amount of online client is %d\n", linkNum);
				printf("Its username is %s\n", g_Client[linkNum - 1].userName);

				//��̨���Ϳͻ�������������ע�������ͻ��˶�Ҫ���ͣ���Ȼ�����һ���ͻ�����������һ���ͻ��˲��������bug
				memset(recvBuf, 0, BUF_SIZE);
				sprintf(recvBuf, "%c", ci);
				recvBuf[2] = '\0';
				sendto(servSock, recvBuf, 2, 0, (SOCKADDR*)&g_Client[linkNum - 1].clntAddr, clLen);
				memset(temp, 0, BUF_SIZE);
				sprintf(temp, "The network have %d on-line, these are: ", linkNum);
				if (linkNum == 2) sprintf(temp, "%s%s and %s\n", temp, g_Client[linkNum - 2].userName, g_Client[linkNum - 1].userName);
				else sprintf(temp, "%s%s\n", temp, g_Client[linkNum - 1].userName);

				sendto(servSock, temp, strlen(temp), 0, (SOCKADDR*)&g_Client[linkNum - 1].clntAddr, clLen);

				//����Ĵ�����ר�õģ���Ϊֻ���������������ͻ��ˣ�����������ͻ�������д�Ͳ�����
				if (linkNum == 2) {//Ҳ����˵�������̨�ͻ������������ˣ���ô����������������ͬʱ����̨�ͻ���һ������������
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

		if (linkNum == 2);//i==2ʲô����ִ�У������
		else continue;

		//���ļ���������ͻ��˵������¼,wb+,��������д���ļ�������ʱ�½�
		sprintf(unPath, "%s", "E:\\SYSTEM\\Server\\Record\\");
		sprintf(unPath, "%s%s%s", unPath, g_Client[0].userName, ".txt");
		FILE* fpc0 = fopen(unPath, "ab+");

		sprintf(unPath, "%s", "E:\\SYSTEM\\Server\\Record\\");
		sprintf(unPath, "%s%s%s", unPath, g_Client[1].userName, ".txt");
		FILE* fpc1 = fopen(unPath, "ab+");

		//����ָ�����鱣���ļ�ָ��
		FILE* fparr[2] = { fpc0,fpc1 };
		long filelen = 0;

		while (1) {

			/*���while()ѭ�����������ҹ���*/

			//���տͻ��˹�������:0,1,2,3
			memset(recvBuf, 0, BUF_SIZE);
			memset(temp, 0, BUF_SIZE);
			recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);
			sendto(servSock, "y", strlen("y"), 0, (SOCKADDR*)&clntAddr, clLen);
			sprintf(temp, "%s", recvBuf);

			if (strcmp(temp, "1") == 0) {

				i = 0;
				while (1) {//��Ҫ��̨�ͻ���ͬʱ����Ự���ܽ�������
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

				//����ͻ��˵���Ϣ����Ҫ��ת����Ϣ
				while (1) {

					/*
					ע��ĵط��ǣ��ͻ��˻�ͳһ������Ϣ���������Ļ���������Ҫcopy���ṹ��ͻ��Ļ�������
					*/

					//���ȷ�����Ϣ�Ŀͻ��˳�ΪC1������������Ϣ�ĳ�ΪC2
					/*������
					1��C1 sendto()һ����Ϣ��S ��recvBuf[]������
					2��S �ж�C1 SOCKADDR�ṹ���е�IP��PORT��Ϣ��Ȼ��recvBuf[]����copy��C1��Ӧ�Ľṹ�建����buf[]��
						�������flag��Ա������Ǹÿͻ����Ƿ��ѷ�����Ϣ
					3��S ������Ϣת��������Ĺؼ���ת����ַ�ǽṹ����flag���Ϊȱʡ״̬��
					4��S ���ݿͻ���Ϣ�ṹ�建�������������¼Ϊ�ļ���ʽ,Ҳ���ǽ��ͻ��˻��������ݿ����������ļ�
					*/
					i = 0;
					ret = 0;
					memset(recvBuf, 0, BUF_SIZE);
					ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);//��ɵ�1��
					if (ret == 0) continue;
					printf("Ret=%d\n", ret);
					recvBuf[ret] = '\0';
					if (strcmp(recvBuf, "quit") == 0) {
						sendto(servSock, "quit", strlen("quit"), 0, (SOCKADDR*)&g_Client[i].clntAddr, clLen);
						sendto(servSock, "quit", strlen("quit"), 0, (SOCKADDR*)&g_Client[!i].clntAddr, clLen);
						break;
					}
					if (g_Client[i].clntAddr.sin_addr.S_un.S_addr == clntAddr.sin_addr.S_un.S_addr && g_Client[i].clntAddr.sin_family == clntAddr.sin_family && g_Client[i].clntAddr.sin_port == clntAddr.sin_port) {
						//�����ַ�������"�ӹ�"��Ҫת�������ݣ���Ҫ������û���
						strcpy_s(g_Client[i].buf, sizeof(g_Client[i].buf), g_Client[i].userName);
						strcat_s(g_Client[i].buf, sizeof(g_Client[i].buf), " saies: ");
						strcat_s(g_Client[i].buf, sizeof(g_Client[i].buf), recvBuf);
						g_Client[i].flag = 1;//flag��ֵΪ0ʱ���Ϊδת����Ϊ1ʱ���Ϊ��ת��
						g_Client[!i].flag = 0;
					}
					else {
						strcpy_s(g_Client[!i].buf, sizeof(g_Client[!i].buf), g_Client[!i].userName);
						strcat_s(g_Client[!i].buf, sizeof(g_Client[!i].buf), " saies: ");
						strcat_s(g_Client[!i].buf, sizeof(g_Client[!i].buf), recvBuf);
						g_Client[!i].flag = 1;
						g_Client[i].flag = 0;
					}//��2����
					for (i = 0; i < 2; i++) {
						if (g_Client[i].flag == 0) {//flagΪ0��ǵ�δת���������ĸ�Ϊ0ת�����ĸ�
							sendto(servSock, g_Client[!i].buf, ret + strlen(g_Client[!i].userName) + 8, 0, (SOCKADDR*)&g_Client[i].clntAddr, clLen);
							fwrite(recvBuf, 1, ret + 1, fparr[!i]);//����Ϣ�����Զ�������ʽ׷�ӵ��ļ���
							//memset(recvBuf, 0, BUF_SIZE);
							break;
						}
					}//��34����
				}
			}
			else if (strcmp(temp, "2") == 0) {
				//ˢ������
				recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);

				sendto(servSock, "Whose record would you wanna save?\n", strlen("Whose record would you wanna save?\n"), 0, (SOCKADDR*)&clntAddr, clLen);
				memset(recvBuf, 0, BUF_SIZE);
				//���տͻ���Ӧ�𡪡��û���
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&clntAddr, &clLen);
				if (strcmp(recvBuf, g_Client[0].userName) == 0) {
					memset(recvBuf, 0, BUF_SIZE);
					/*
					fclose(fpc0);
					sprintf(unPath, "%s", "E:\\SYSTEM\\Server\\Record\\");
					sprintf(unPath, "%s%s%s", unPath, g_Client[0].userName, ".txt");
					fpc0 = fopen(unPath, "wb+");
					*/
					//���ļ�����,fseek()����ļ�ָ���ƶ���ĩβ��rewind()�ٰ�ָ�븴λ����ͷ
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
					//���ļ�����,fseek()����ļ�ָ���ƶ���ĩβ��rewind()�ٰ�ָ�븴λ����ͷ
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
				while (1) {//��Ҫ��̨�ͻ���ͬʱ����Ự���ܽ�������
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
					//��¼�ͻ������ӵ��Ⱥ�˳����������flag=0������flag=1
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
				//�������Խ��ն˿ͻ���(Ҳ���Ǻ������������Ŀͻ���)����Ӧ
				memset(recvBuf, 0, BUF_SIZE);
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&g_Client[!i].clntAddr, &clLen);
				recvBuf[ret] = '\0';
				sendto(servSock, recvBuf, ret, 0, (SOCKADDR*)&g_Client[i].clntAddr, clLen);
				//���շ��Ϳͻ��˷������ļ���
				memset(recvBuf, 0, BUF_SIZE);
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&g_Client[i].clntAddr, &clLen);
				recvBuf[ret] = '\0';
				FileTrans_recv(recvBuf);
				//�����ļ������տͻ���
				FileTrans_send(recvBuf);
			}
			else if (strcmp(temp, "4") == 0) {
				ci = '0';
				i = 0;
				while (1) {//��Ҫ��̨�ͻ���ͬʱ����Ự���ܽ�������
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
					//��¼�ͻ������ӵ��Ⱥ�˳����������flag=0������flag=1
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
				//�������Խ��ն˿ͻ���(Ҳ���Ǻ������������Ŀͻ���)����Ӧ
				memset(recvBuf, 0, BUF_SIZE);
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&g_Client[!i].clntAddr, &clLen);
				recvBuf[ret] = '\0';
				sendto(servSock, recvBuf, ret, 0, (SOCKADDR*)&g_Client[i].clntAddr, clLen);
				//���շ��Ϳͻ��˷������ļ���
				memset(recvBuf, 0, BUF_SIZE);
				ret = recvfrom(servSock, recvBuf, BUF_SIZE, 0, (SOCKADDR*)&g_Client[i].clntAddr, &clLen);
				recvBuf[ret] = '\0';
				FileTrans_recv(recvBuf);
				//�����ļ������տͻ���
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
	FILE* fp = fopen(filepath, "rb");  //�Զ����Ʒ�ʽ���ļ�
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

	//ѭ���������ݣ�ֱ���ļ���β
	char buffer[BUF_SIZE] = { 0 };  //������
	int nCount;
	while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
		send(clntSock, buffer, nCount, 0);
	}

	shutdown(clntSock, SD_SEND);  //�ļ���ȡ��ϣ��Ͽ����������ͻ��˷���FIN��
	recv(clntSock, buffer, BUF_SIZE, 0);  //�������ȴ��ͻ��˽������

	fclose(fp);
	closesocket(clntSock);
	closesocket(servSock);
	WSACleanup();

	//system("pause");
}

void FileTrans_recv(char* filename) {

	char filepath[100] = "E:\\SYSTEM\\Server\\FileTrans\\";
	sprintf(filepath, "%s%s", filepath, filename);
	FILE* fp = fopen(filepath, "wb");  //�Զ����Ʒ�ʽ��/�����ļ�
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

	//ѭ���������ݣ�ֱ���ļ���β
	char buffer[BUF_SIZE] = { 0 };  //�ļ�������
	int nCount;
	while ((nCount = recv(clntSock, buffer, BUF_SIZE, 0)) > 0) {
		fwrite(buffer, nCount, 1, fp);
	}
	puts("File transfer success!\n");

	recv(clntSock, buffer, BUF_SIZE, 0);  //�������ȴ��ͻ��˽������
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
	FILE* fp = fopen(filepath, "rb");  //�Զ����Ʒ�ʽ���ļ�
	if (fp == NULL) {
		printf("Cannot open file to send, press any key to exit!\n");
		system("pause");
		exit(0);
	}

	//S2������ļ���С
	fseek(fp, 0L, SEEK_END);
	//size���ļ���С
	long size = ftell(fp);
	rewind(fp);
	printf("\tget the size,the size is:%ld\t", size);

	//S3,����һ����װ�������ļ��Ŀռ�
	char* ar = (char*)malloc(sizeof(char) * size);

	//S4,���ļ���ÿ�ζ�һ��������size��
	/*
	 *�ļ���С��ȷ�����п��ܱȻ�������ܶ࣬����һ�� send() ������������ļ����ݵķ���
	 *��������ʱ recv() Ҳ������ͬ�������
	 *Ҫ���������⣬��Ҫʹ�� while ѭ��
	*/
	int nCount;
	int i = 0;
	//����fread()�ǲ�����ֲ�ȷ�����������Ϊ������ar�Ǹ����ļ���Сsize�����ģ���������һ��read()���ļ�
	while ((nCount = fread(ar, 1, size, fp)) > 0) {
		printf("\tto read file of %d,and the nCount is %d\n", i + 1, nCount);
		i++;
	}
	/*
	 *ע�������ļ�fpָ�����1*size����λ
	*/

	//S5,��Ƭ���ֳ�PIECE��
	typedef struct
	{
		int grade;//��ǰ���ر�ʶ
		char data[BUF_SIZE] = { 0 };
	}Slice;
	Slice sli[PIECE];

	//����ָ�����飬ָ��ṹ�������еĳ�Աdata[]
	//֮������Ҫ����һ�����飬����Ϊsend()/recv()����buf�����ͱ���Ϊconst char*
	const char* dp[PIECE];
	for (i = 0; i < PIECE; i++)
	{
		dp[i] = sli[i].data;
	}

	//S6,ar���黺������ݸ�ֵ����ջ�ڴ�
	//tsize��¼ÿ�����ݴ�С,�������롣num��¼arָ��ָ��Ļ���������һ�ζ�ȡ��������
	int num = 0, j, tsize = (int)(size / PIECE) + 1;

	//������ʾ������ֽ���
	int excP = size % tsize;
	//ʵ�ʷ�Ƭ��Ӧ�õ��ڣ�ʵ���ֽ�size����ÿ�ݷ�Ƭ��Сtsize��������������������ʵ�ʷ�Ƭ��Ӧ��1����Ϊ��������0������1��ʼ
	int actP = size / tsize + 1;
	for (i = 0; i < PIECE; i++)
	{
		//��ʼ�����,ASCII��48�������͵�0
		sli[i].grade = i + 48;
		for (j = 0; j < tsize; j++)
		{
			//��ֵ֮ǰ�ȼ���Ƿ��Ѿ���ȡ����arָ��Ķ�̬����Ԫ��
			//����������¹������Ѷ����ֽڷ�װ���ṹ��ĳ�Ա�����Ժ����'\0'��β��������ʵ�arָ���δ֪����
			if (i == actP - 1 && j == excP) {
				sli[actP - 1].data[excP] = '\0';
				break;
			}
			//��һ��ִ��֮�󣬿�ֱ���˳���ѭ��
			if (i >= actP - 1 && sli[actP - 1].data[excP] == '\0') break;
			sli[i].data[j] = ar[j + num];
		}
		num += tsize;
	}

	//S7,׼���׽���
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//�����������׽���ΪservSock
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
	//�������������׽���Ϊsock
	SOCKET sock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);

	//S8,�ȼ�����ر�ǣ��ٴ��ϴ����ر�ʶ��ǵĵط���ʼ����
	FILE* tfp;
	//rb+��ʾ�ļ������ȴ��ڣ���д��ʽΪ�����ݵĿ�ʼ������ԭ������
	if ((tfp = fopen("E:\\SYSTEM\\Server\\Piece\\Temp.txt", "rb+")) == NULL) {
		printf("Cannot open the file2, press any key to exit!\n");
		system("pause");
		exit(0);
	}
	//�����ϴ����ر�ʶ
	int id;
	//��tfpָ����ļ���Temp.txt��ȡ���ݣ�Ĭ��Ϊ0�����ӵ�һ��Ƭ��ʼ
	id = fgetc(tfp);
	int idsize = sizeof(id);

	//��Ҫһ�����տͻ���Ӧ��Ļ�����
	char buffer[BUF_SIZE] = { 0 }, ibuf[2] = { 0 };
	char* bp = buffer;
	printf("Server will send files to the client\n\n\n\n");
	int recvLen = 0, sendLen = 0;
	for (i = 0; i < PIECE; i++)
	{
		//��¼��ǰ���ص���һ��Ƭ�����浽Temp.txt
		ibuf[0] = i + 48;
		ibuf[1] = '\0';
		fwrite(ibuf, 1, 1, tfp);
		rewind(tfp);
		printf("the %d of for circle\n", i);
		//�ϴ����ص������һ�α�����￪ʼ
		if (sli[i].grade == id) {
			//���߿ͻ������ڷ�����һ����Ƭ
			send(sock, ibuf, 2, 0);
			recv(sock, buffer, 2, 0);
			memset(buffer, 0, BUF_SIZE);

			//ÿ�η���һ����Ƭ
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
			 *������ÿ����һ����Ƭ��������ݿͻ��˵�Ӧ������ж�
			  *1���ͻ���Ӧ�����յ�(���ַ�'f'��ʾ)������������ʣ�µķ�Ƭ��������ʾ�������
			  *2���ͻ���Ӧ��δ�յ�(���ַ�'n'��ʾ)�������浱ǰ���ر�ʶ����������
			*/

			buffer[0] = 'n';
			//recv()����������������ΪҪ���տͻ��˵�Ӧ��
			printf("\t\twait to receive respond from client\n");
			if ((recvLen = recv(sock, buffer, 2, 0)) > 0) {
				printf("\tget a respond from client\n");
			}
			else
			{
				printf("\treceive respond failed!\n");
			}
			//Ӧ��ʱ����Ϊ6s����ʾ������6s�Ժ����ͻ��˻�û��Ӧ�𣬾Ͳ��ٵȴ���Ӧ��
			//Sleep(6000);
			if (*bp == 'f')
			{
				//���ͻ������յ�Ӧ��
				//��ʾ�ѷ������һ����Ƭ�����ر�ʶ+1�����Ҵ�ӡ�������
				printf("\tthe rate of transported procession now is:%d%%\n", (i + 1) * 100 / PIECE);
				if (i % 10 == 0)
					printf("\n");
				id++;
				*bp = 'n';
			}
			else if (*bp == 'n')
			{
				//���ͻ����ղ���Ӧ��
				//Ҳ���ǵ�ǰ��ƬҲ����ʧ�ܣ�����ǰ���ر�ʶ����Ҫ����1������¼��ǰ���ر�ʶ
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
				printf("\n������ɣ�\n");
			printf("\n");
		}
	}

	shutdown(sock, SD_SEND);  //�ļ�������ϣ��Ͽ����������ͻ��˷���FIN��
	recv(sock, buffer, BUF_SIZE, 0);  //�������ȴ��ͻ��˽������

	fclose(fp);
	fclose(tfp);
	closesocket(sock);
	closesocket(servSock);
	WSACleanup();
}