// B_P1.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAX_MA 100
#define MAX_MMA 1000
static int len_sBC = 0;//sBC[]�ĳ��ȣ��ں���ִ���и�ֵ�����Զ���Ϊȫ�ֱ���
#pragma warning(disable:4996)


					   /*
					   ���������Ĵ洢�ṹ����Ҳ��һ�ֶ������ṹ��
					   ���ִ洢�ṹ���ʺϱ�ʾ����Ҳ�ʺϱ�ʾɭ�֡�
					   */
typedef struct Node
{
	int weight;                //Ȩֵ
	int parent;                //���ڵ����ţ�Ϊ-1���Ǹ��ڵ�
	int lchild, rchild;         //���Һ��ӽڵ����ţ�Ϊ-1����Ҷ�ӽڵ�
	char data;					//��¼Ҷ�ӽ�������
}HTNode, * HuffmanTree;          //�����洢���������е����нڵ�
typedef char** HuffmanCode;    //�����洢ÿ��Ҷ�ӽڵ�Ĺ���������


							   //******************************ѡ����СҶ�ӽ�㣬�����溯���Ļ���******************************//
							   /*
							   ��HT�����ǰk��Ԫ����ѡ��weight��С��parentΪ-1��Ԫ�أ�������Ԫ�ص���ŷ���
							   k��ʵ������type��ͬ����һЩҶ�ӽ�㹹���˶��������ö������ĸ�Ҳ�൱���µ�Ҷ�ӽ�㣬Ҳ���뵽select�н�����һ�ֵ�������Сֵ��ɸѡ
							   */
int minium(HuffmanTree HT, int k)
{
	int i = 0;
	int min;        //�������weight��С��parentΪ-1��Ԫ�ص����
	int min_weight; //�������weight��С��parentΪ-1��Ԫ�ص�weightֵ

					//�Ƚ���һ��parentΪ-1��Ԫ�ص�weightֵ����min_weight
	while (HT[i].parent != -1)
		i++;
	min_weight = HT[i].weight;
	min = i;

	//ѡ��weight��С��parentΪ-1��Ԫ�أ�����Ÿ�min
	for (; i < k; i++)
	{
		if (HT[i].weight < min_weight && HT[i].parent == -1)
		{
			min_weight = HT[i].weight;
			min = i;
		}
	}

	//ѡ��weight��С��Ԫ�غ󣬽�����˫��parent��״̬1��״̬1��ʾ��һ�αȽ�ʱ���ų������㡣
	HT[min].parent = 1;

	return min;
}

//******************************ѡ����С�ʹ�СҶ�ӽ��******************************//
void select(HuffmanTree HT, int k, int& min1, int& min2)
{
	//k��ֵ�������˼����һ������������ע��
	min1 = minium(HT, k);
	min2 = minium(HT, k);
}

//******************************������������******************************//
/*
�β�1(int*)str_times���Ǹ��Ʊ���Ȩֵ�����ָ�룬�β�2��������ַ������飬�β�3��ʵ�ʵ�����
*/
HuffmanTree create_HuffmanTree(int* str_times, char* str_what, int n)
{
	//һ����n��Ҷ�ӽڵ�Ĺ�����������2*n-1���ڵ�
	int total = 2 * n - 1;
	HuffmanTree HT = (HuffmanTree)malloc(total * sizeof(HTNode));

	int i;
	int min1, min2;//��������������min1������СҶ�ӽ�㣬min2�����СҶ�ӽ��
				   //HT[0],HT[1]...HT[n-1]�д����Ҫ�����n��Ҷ�ӽڵ�
	for (i = 0; i < n; i++)
	{
		HT[i].parent = -1;
		HT[i].lchild = -1;
		HT[i].rchild = -1;
		HT[i].weight = *str_times;
		HT[i].data = *str_what;
		str_times++;//ָ�����һ���ṹ�������Ԫ�ص�Ԫ
		str_what++;
	}

	//HT[n],HT[n+1]...HT[2n-2]�д�ŵ����м乹�����ÿ�ö������ĸ��ڵ�
	for (; i < total; i++)
	{
		HT[i].parent = -1;
		HT[i].lchild = -1;
		HT[i].rchild = -1;
		HT[i].weight = 0;
		HT[i].data = '\0';
	}

	/*
	ѡ����С�ʹ�С������Ҷ�ӽ��
	��������Ҷ�ӽ�㹹��һ�ö�����
	�Ѷ������ĸ��������Ҷ�ӽ��ѭ����һ������
	���չ���һ�ù�������
	*/
	for (i = n; i < total; i++)
	{
		select(HT, i, min1, min2);
		HT[min1].parent = i;
		HT[min2].parent = i;
		//�涨������С���Һ��Ӵ�С
		HT[i].lchild = min1;
		HT[i].rchild = min2;
		HT[i].weight = HT[min1].weight + HT[min2].weight;
	}
	return HT;
}


//******************************������������******************************//
void HuffmanCoding(HuffmanTree HT, HuffmanCode& HC, char* str, int n)
{
	//��������ָ��ÿ�����������봮��ָ��
	HC = (HuffmanCode)malloc(n * sizeof(char*));

	//��ʱ��������ÿ����õĹ��������봮
	char* code = (char*)malloc(n * sizeof(char));
	int cur = 2 * n - 2;    //��ǰ�������Ľ�����ţ���ʼʱΪ��������
	int count = 0;   //�������ĳ���

					 //�����ù��������󣬰�weight������������ÿ������״̬��־
					 //weight=0Ϊ���Һ��Ӷ���û�б�����
					 //weight=1Ϊ�����Ѿ������������Һ���û�б�����
					 //weight=2Ϊ���Һ��Ӷ��Ѿ���������
	int i;
	for (i = 0; i < cur + 1; i++)
	{
		HT[i].weight = 0;
	}

	//�Ӹ��ڵ㿪ʼ������������־�ǻص����ڵ�
	while (cur != -1)
	{
		//���Һ��Ӷ�û���������ȱ�������
		if (HT[cur].weight == 0)
		{
			HT[cur].weight = 1;    //ÿ�α����������Ȱ�״̬��Ϊ1
			if (HT[cur].lchild != -1)
			{   //�����ǰ�ڵ㲻��Ҷ�ӽڵ㣬�ͼ��±��룬�ټ���������������
				code[count++] = '0';
				cur = HT[cur].lchild;
			}
			else
			{   //�����ǰ�ڵ���Ҷ�ӽڵ㣬����ֹ���룬�����䱣������
				code[count] = '\0';
				HC[cur] = (char*)malloc((count + 1) * sizeof(char));
				strcpy(HC[cur], code);  //���Ʊ��봮

										//�����ʱ����һ�������¼���봮��Ӧ���ַ�
										//�����ʱ����Ҫ����
				str[cur] = HT[cur].data;
			}
		}

		//�����ѱ���������ʼ���ұ����Һ���
		else if (HT[cur].weight == 1)
		{
			HT[cur].weight = 2;   //���������Һ��Ӿ�����������
			if (HT[cur].rchild != -1)
			{   //�����ǰ�ڵ㲻��Ҷ�ӽڵ㣬����±��룬���������ұ���
				code[count++] = '1';
				cur = HT[cur].rchild;
			}
		}

		//���Һ��Ӿ��ѱ��������˻ص����ڵ㣬ͬʱ���볤�ȼ�1
		else
		{
			HT[cur].weight = 0;
			cur = HT[cur].parent;
			--count;
		}

	}
	free(code);
}

//**************************���ӹ���������******************************//
void LinkHuffmanCode(HuffmanCode& HC, char* tS, int n, char* str, char* sBC)
{
	int i = 0, j, k = 0, count = 0;//i,j,kΪѭ��������countΪ�����±�ļ�����
	int len;
	char hc[MAX_MA] = { 0 };//���ƶ����Ʊ��뵽�����ʱ������

							//��HC[i]�����ж����Ʊ��봮���ӵ�ͬһ������str_BinCoded[]
	while (tS[i] != '\0')
	{
		for (j = 0; j < n; j++)
		{
			//ȷ����ӡ˳�򣬱�֤����the_String[]������˳��
			//��Ϊ������ǰ��ֵ�˳�����еģ���������ַ��϶������ֵ�˳��
			//*����apple���ڱ��������aelp�������У���ô����˳��Ҳ�����*
			//*���Ҫ�õ���ȷ������˳��aple��������������һ��*
			if (tS[i] == str[j])
			{
				//�ַ������Ӻ����Ĳ���Ӧ����Ŀ���ַ�����Դ�ַ����ĳ��Ⱥ�
				len = strlen(HC[j]) + 1 + strlen(hc);
				strcat_s(hc, len, HC[j]);

				//��ӵ�str_BinCoded[]�е�ÿһ�ζ����Ʊ��볤�Ȳ�����HC[j]���鳤��
				while (k < len - 1)
				{
					//hc[]����ʱ���飬����������������
					//���������Ҫ��hc[]��ÿһ��Ԫ��ֵ��ֵ��str_BinCoded[]
					*(sBC + count) = hc[k];
					count++;
					k++;
				}
				break;
			}
		}
		i++;
	}
	len_sBC = count;
}

//******************************��������ƴ�******************************//
void Scanf_str(char* sfC)
{
	char ch;
	int i = 0;
	scanf("%c", &ch);
	while ((ch = getchar()) != '\n')
	{
		sfC[i++] = ch;
	}
	sfC[i] = '\0';
}


//******************************������������******************************//
void HuffmanDecoded(HuffmanTree HT, char* str, char* sfC, char* sD, int n)
{
	int i = 0, count = 0;//iΪѭ��������countΪ����str_Decoded[]�±�ļ�����

	int total = 2 * n - 2;//ָ���
	for (i = 0; sfC[i] != '\0'; i++)
	{
		if (sfC[i] == '0')
			total = HT[total].lchild;
		else if (sfC[i] == '1')
			total = HT[total].rchild;
		if (HT[total].lchild == -1 && HT[total].rchild == -1)
		{
			//ֻ�е����Һ��Ӷ���-1���ý��ΪҶ�ӽ�㣬�ŵõ�һ������
			sD[count++] = str[total];
			total = 2 * n - 2;//����ִӸ���ʼ����
		}
	}
	sD[count] = '\0';//��������ַ�����str_Decoded[]�����һλ��ֵΪ'\0'�Է�����

}

int main()
{
	//******************************�����ַ���******************************//
	/*
	��ȡ�����ַ�����gets_s()������scanf()
	��Ϊgets_s(s)���տո��Ʊ��tab�ͻس���scanf("%s",&s)������
	���Ƕ��ǻ����ַ������ս������Զ���������'\0'
	*/
	char the_String[MAX_MMA] = { 0 };//���������ַ�
	char* tS = the_String;
	int count_ts = 0;//��¼the_String[]����
	int str_ascii[256] = { 0 };//ASCII������256���ַ����㹻������һASCII����д��ڵ��ַ�
	int i, j;//ѭ������
	int type = 0;//��¼�����˶���������ַ�����������Щ�ظ��ģ���0��ʼ�����ʵ�������1
	int str_times[MAX_MA] = { 0 };//��¼ÿ�������ַ����ֵĴ���
	int* st = str_times;
	char str_what[MAX_MA] = { 0 };//��¼������ʲô�ַ�
	char* sw = str_what;
	char str_AsciiCoding[MAX_MA] = { 0 };//�������������¼�����ʱ��ÿ�����봮��Ӧ���ַ�
	char* sAC = str_AsciiCoding;//ָ��sָ������飬���ڴ��ε�������ʵ���ں����ж�����Ĵ���
	//char str_Coding[MAX_MMA];//����������ַ�
	//char*sC = str_Coding;
	char str_Decoded[MAX_MMA];//����������ַ�
	char* sD = str_Decoded;
	char str_BinCoded[MAX_MA];//�������������
	char* sBC = str_BinCoded;
	HuffmanTree HT = NULL;//��ʼ����Ϊ����
	HuffmanCode HC = NULL;//��ʼ�������Ϊ�ձ�

	//******************************��ӭ����������������ҵ******************************//
	printf("========================�������������˵�========================\n");
	printf("\t\t  ������һ������/�ַ���");
	//%[]���뼯�����޶����ַ��������һ���ַ���'^'���Ƕ�ȡ��'^'������ַ��ͻ�ֹͣ
	//�����ַ������Զ���һ�������ո���ַ���
	scanf("%[^\n]", the_String);

	//******************************ͳ��ÿ���ַ��ĳ��ִ���******************************//
	//�Ѷ����ַ���ascii����Ϊ�±괢��Ϊ��Ӧ��Ԫ��
	//����һ��Ҫ��ǰ��ʼ��str_ascii����Ϊȫ0
	for (i = 0; the_String[i] != 0; i++)
		str_ascii[the_String[i]]++;
	for (i = 0; i < 256; i++)
	{
		//����0���Ǵ������ַ�
		if (str_ascii[i] > 0)
		{
			//str_times[]��str_what[]��һһ��Ӧ��
			//����˵�������ǵ�0��Ԫ����a��1���Ǿ���a������1��
			str_times[type] = str_ascii[i];
			str_what[type] = (char)i;
			type++;//ÿ����һ�����͵��ַ�������ͼ�1
		}
	}
	printf("\n\n========================�����������Ҷ��========================\n");
	printf("���������������/�ַ��������Ƕ������г��ֵ�ÿ���ַ��Ĵ�����ͳ�ƣ�\n");
	printf("���ֵ��ַ�\t\t���ֵĴ���\n");
	for (i = 0; i < type; i++)
		printf("%c\t\t\t%d\n", str_what[i], str_times[i]);


	//******************************������������******************************//
	HT = create_HuffmanTree(str_times, str_what, type);
	printf("\n\n========================���������ı����========================\n");
	printf("�������������ɹ��������Ǹù��������Ĳ��������\n");
	printf("���i\t�ַ�\tȨֵ\t˫��\t����\t�Һ���\n");
	for (i = 0; i < type; i++)
		printf("%d\t%c\t%d\t%d\t%d\t%d\n", i, HT[i].data, HT[i].weight, HT[i].parent, HT[i].lchild, HT[i].rchild);

	//******************************������������******************************//
	HuffmanCoding(HT, HC, str_AsciiCoding, type);
	printf("\n\n========================�����������ڱ���========================\n");
	printf("����ɹ���\n");
	LinkHuffmanCode(HC, the_String, type, str_AsciiCoding, str_BinCoded);
	for (i = 0; i < len_sBC; i++)    //��ӡ���봮
	{
		printf("%c", sBC[i]);
	}

	//�����������ÿ���ַ��Ķ����Ʊ���
	printf("\n\n====================��ӡ��������Ҷ�ӵĶ����Ʊ���====================\n");
	printf("�����ǹ�������Ҷ�ӵĶ����Ʊ��룺\n");
	printf("�ַ�\t����������\n");
	for (i = 0; i < type; i++)
	{
		printf("%c\t", sAC[i]);//�ȴ�ӡ�ַ���������
		for (j = 0; j < strlen(HC[i]); j++)
		{
			printf("%c", *(*(HC + i) + j));//�ٽ�������ָ���ӡ����������
		}
		printf("\n");
	}

	//ͳ�Ʊ���Ч��
	/*���õ�ѹ��Ч��ͳ��ԭ���ǣ�һ����n���ַ����ַ���ռ��n*sizeof(char)�ֽڣ���������������ֻ����len_sBC������λ����
	��ôѹ��Ч�ʾ��ǣ�len_sBC/n*sizeof(char)*/
	printf("\n====================ͳ�ƹ���������Ч��====================\n");
	count_ts = strlen(tS);
	int total = count_ts * 8;
	printf("ԭ�����Ʊ�����ռ�ڴ��СΪ������λ����");
	printf("%d\n", total);
	printf("��������������ռ�ڴ��СΪ������λ����");
	printf("%d\n", len_sBC);
	printf("�������������ѹ��Ч��Ϊ��\n");
	//������λС�����ȳ���1000�ټ�0.5������1000.0
	float perc = int((total - len_sBC) * 1000 + 0.5) / 1000.0;
	//����ע�͵�����С����ʾ��ѹ��Ч�ʣ����ܲ����ٷֱ����ֱ������û����
	//printf("%.5f\n",float(total - len_sBC) / total );
	//����ٷ�������ʽ���������Ҫ��%%�������'%'�ķ���
	printf("%.2f%%", (perc / total) * 100.0);

	//******************************������������******************************//
	printf("\n\n========================����������������========================\n");
	printf("\n��������������봮��\n");

	char scanf_Coded[MAX_MA];    //�洢����ı��봮
	Scanf_str(scanf_Coded);

	HuffmanDecoded(HT, str_AsciiCoding, scanf_Coded, str_Decoded, type);
	printf("����ɹ���\n");

	//�����������������ַ������
	printf("\n\n======================��ӡ�����������봮======================\n");
	printf("���������������ַ���Ϊ��\n");

	for (i = 0; i < len_sBC; i++)
	{
		if (sBC[i] != scanf_Coded[i])
		{
			printf("����Ķ����ƴ���ԭ���봮����ͬ��");
			printf("����Ȼ��ԭ�����������\n");
			break;
		}
	}

	for (i = 0; sD[i] != '\0'; i++)
	{
		printf("%c", str_Decoded[i]);
	}

	printf("\n\n==================��������������ף���������==================\n");
	return 0;
	system("pause");
}