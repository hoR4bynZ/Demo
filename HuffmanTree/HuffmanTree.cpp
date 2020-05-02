// B_P1.cpp : 定义控制台应用程序的入口点。
//
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAX_MA 100
#define MAX_MMA 1000
static int len_sBC = 0;//sBC[]的长度，在函数执行中赋值，所以定义为全局变量
#pragma warning(disable:4996)


					   /*
					   哈夫曼树的存储结构，它也是一种二叉树结构，
					   这种存储结构既适合表示树，也适合表示森林。
					   */
typedef struct Node
{
	int weight;                //权值
	int parent;                //父节点的序号，为-1的是根节点
	int lchild, rchild;         //左右孩子节点的序号，为-1的是叶子节点
	char data;					//记录叶子结点的数据
}HTNode, * HuffmanTree;          //用来存储哈夫曼树中的所有节点
typedef char** HuffmanCode;    //用来存储每个叶子节点的哈夫曼编码


							   //******************************选出最小叶子结点，是下面函数的基础******************************//
							   /*
							   从HT数组的前k个元素中选出weight最小且parent为-1的元素，并将该元素的序号返回
							   k与实际种类type不同，有一些叶子结点构成了二叉树，该二叉树的根也相当于新的叶子结点，也加入到select中进行新一轮的两个最小值的筛选
							   */
int minium(HuffmanTree HT, int k)
{
	int i = 0;
	int min;        //用来存放weight最小且parent为-1的元素的序号
	int min_weight; //用来存放weight最小且parent为-1的元素的weight值

					//先将第一个parent为-1的元素的weight值赋给min_weight
	while (HT[i].parent != -1)
		i++;
	min_weight = HT[i].weight;
	min = i;

	//选出weight最小且parent为-1的元素，赋序号给min
	for (; i < k; i++)
	{
		if (HT[i].weight < min_weight && HT[i].parent == -1)
		{
			min_weight = HT[i].weight;
			min = i;
		}
	}

	//选出weight最小的元素后，将它的双亲parent置状态1，状态1表示下一次比较时将排除这个结点。
	HT[min].parent = 1;

	return min;
}

//******************************选出最小和次小叶子结点******************************//
void select(HuffmanTree HT, int k, int& min1, int& min2)
{
	//k的值代表的意思在下一个函数里面有注明
	min1 = minium(HT, k);
	min2 = minium(HT, k);
}

//******************************创建哈夫曼树******************************//
/*
形参1(int*)str_times就是复制保存权值数组的指针，形参2是输入的字符的数组，形参3是实际的种类
*/
HuffmanTree create_HuffmanTree(int* str_times, char* str_what, int n)
{
	//一棵有n个叶子节点的哈夫曼树共有2*n-1个节点
	int total = 2 * n - 1;
	HuffmanTree HT = (HuffmanTree)malloc(total * sizeof(HTNode));

	int i;
	int min1, min2;//构建哈夫曼树用min1保存最小叶子结点，min2保存次小叶子结点
				   //HT[0],HT[1]...HT[n-1]中存放需要编码的n个叶子节点
	for (i = 0; i < n; i++)
	{
		HT[i].parent = -1;
		HT[i].lchild = -1;
		HT[i].rchild = -1;
		HT[i].weight = *str_times;
		HT[i].data = *str_what;
		str_times++;//指针后移一个结构体数组的元素单元
		str_what++;
	}

	//HT[n],HT[n+1]...HT[2n-2]中存放的是中间构造出的每棵二叉树的根节点
	for (; i < total; i++)
	{
		HT[i].parent = -1;
		HT[i].lchild = -1;
		HT[i].rchild = -1;
		HT[i].weight = 0;
		HT[i].data = '\0';
	}

	/*
	选出最小和次小的两个叶子结点
	用这两个叶子结点构成一棵二叉树
	把二叉树的根和其余的叶子结点循环第一步操作
	最终构成一棵哈夫曼树
	*/
	for (i = n; i < total; i++)
	{
		select(HT, i, min1, min2);
		HT[min1].parent = i;
		HT[min2].parent = i;
		//规定左孩子最小，右孩子次小
		HT[i].lchild = min1;
		HT[i].rchild = min2;
		HT[i].weight = HT[min1].weight + HT[min2].weight;
	}
	return HT;
}


//******************************哈夫曼树编码******************************//
void HuffmanCoding(HuffmanTree HT, HuffmanCode& HC, char* str, int n)
{
	//用来保存指向每个哈夫曼编码串的指针
	HC = (HuffmanCode)malloc(n * sizeof(char*));

	//临时用来保存每次求得的哈夫曼编码串
	char* code = (char*)malloc(n * sizeof(char));
	int cur = 2 * n - 2;    //当前遍历到的结点的序号，初始时为根结点序号
	int count = 0;   //定义编码的长度

					 //构建好哈夫曼树后，把weight用来当做遍历每个结点的状态标志
					 //weight=0为左右孩子都还没有被遍历
					 //weight=1为左孩子已经被遍历过，右孩子没有被遍历
					 //weight=2为左右孩子都已经被遍历过
	int i;
	for (i = 0; i < cur + 1; i++)
	{
		HT[i].weight = 0;
	}

	//从根节点开始遍历，结束标志是回到根节点
	while (cur != -1)
	{
		//左右孩子都没被遍历，先遍历左孩子
		if (HT[cur].weight == 0)
		{
			HT[cur].weight = 1;    //每次遍历左孩子首先把状态置为1
			if (HT[cur].lchild != -1)
			{   //如果当前节点不是叶子节点，就记下编码，再继续遍历它的左孩子
				code[count++] = '0';
				cur = HT[cur].lchild;
			}
			else
			{   //如果当前节点是叶子节点，就终止编码，并将其保存起来
				code[count] = '\0';
				HC[cur] = (char*)malloc((count + 1) * sizeof(char));
				strcpy(HC[cur], code);  //复制编码串

										//编码的时候用一个数组记录编码串对应的字符
										//译码的时候需要用上
				str[cur] = HT[cur].data;
			}
		}

		//左孩子已被遍历，开始向右遍历右孩子
		else if (HT[cur].weight == 1)
		{
			HT[cur].weight = 2;   //表明其左右孩子均被遍历过了
			if (HT[cur].rchild != -1)
			{   //如果当前节点不是叶子节点，则记下编码，并继续向右遍历
				code[count++] = '1';
				cur = HT[cur].rchild;
			}
		}

		//左右孩子均已被遍历，退回到父节点，同时编码长度减1
		else
		{
			HT[cur].weight = 0;
			cur = HT[cur].parent;
			--count;
		}

	}
	free(code);
}

//**************************连接哈夫曼编码******************************//
void LinkHuffmanCode(HuffmanCode& HC, char* tS, int n, char* str, char* sBC)
{
	int i = 0, j, k = 0, count = 0;//i,j,k为循环变量，count为数组下标的计数器
	int len;
	char hc[MAX_MA] = { 0 };//复制二进制编码到这个临时数组中

							//把HC[i]的所有二进制编码串连接到同一个数组str_BinCoded[]
	while (tS[i] != '\0')
	{
		for (j = 0; j < n; j++)
		{
			//确定打印顺序，保证遵守the_String[]的输入顺序
			//因为编码表是按字典顺序排列的，而输入的字符肯定不是字典顺序
			//*比如apple，在编码表中是aelp这样排列，那么编码顺序也是如此*
			//*如果要得到正确的输入顺序aple，必须重新排列一次*
			if (tS[i] == str[j])
			{
				//字符串连接函数的步长应该是目标字符串和源字符串的长度和
				len = strlen(HC[j]) + 1 + strlen(hc);
				strcat_s(hc, len, HC[j]);

				//添加到str_BinCoded[]中的每一次二进制编码长度不超过HC[j]数组长度
				while (k < len - 1)
				{
					//hc[]是临时数组，函数结束不复存在
					//因此在这里要把hc[]的每一个元素值赋值给str_BinCoded[]
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

//******************************输入二进制串******************************//
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


//******************************哈夫曼树译码******************************//
void HuffmanDecoded(HuffmanTree HT, char* str, char* sfC, char* sD, int n)
{
	int i = 0, count = 0;//i为循环变量，count为数组str_Decoded[]下标的计数器

	int total = 2 * n - 2;//指向根
	for (i = 0; sfC[i] != '\0'; i++)
	{
		if (sfC[i] == '0')
			total = HT[total].lchild;
		else if (sfC[i] == '1')
			total = HT[total].rchild;
		if (HT[total].lchild == -1 && HT[total].rchild == -1)
		{
			//只有当左右孩子都是-1即该结点为叶子结点，才得到一个译码
			sD[count++] = str[total];
			total = 2 * n - 2;//最后又从根开始检索
		}
	}
	sD[count] = '\0';//最后把译出字符数组str_Decoded[]的最后一位赋值为'\0'以防错误

}

int main()
{
	//******************************输入字符串******************************//
	/*
	获取输入字符串用gets_s()而不用scanf()
	因为gets_s(s)接收空格、制表符tab和回车；scanf("%s",&s)不接收
	他们都是会在字符串接收结束后自动最后面加上'\0'
	*/
	char the_String[MAX_MMA] = { 0 };//保存读入的字符
	char* tS = the_String;
	int count_ts = 0;//记录the_String[]长度
	int str_ascii[256] = { 0 };//ASCII码表包含256个字符，足够处理任一ASCII码表中存在的字符
	int i, j;//循环变量
	int type = 0;//记录出现了多少种类的字符，即忽略那些重复的，从0开始，因此实际种类加1
	int str_times[MAX_MA] = { 0 };//记录每个读入字符出现的次数
	int* st = str_times;
	char str_what[MAX_MA] = { 0 };//记录读入了什么字符
	char* sw = str_what;
	char str_AsciiCoding[MAX_MA] = { 0 };//这个数组用来记录编码的时候每个编码串对应的字符
	char* sAC = str_AsciiCoding;//指针s指向该数组，便于传参到函数，实现在函数中对数组的处理
	//char str_Coding[MAX_MMA];//储存待编码字符
	//char*sC = str_Coding;
	char str_Decoded[MAX_MMA];//储存译出的字符
	char* sD = str_Decoded;
	char str_BinCoded[MAX_MA];//储存二进制译码
	char* sBC = str_BinCoded;
	HuffmanTree HT = NULL;//初始化树为空数
	HuffmanCode HC = NULL;//初始化编码表为空表

	//******************************欢迎操作哈夫曼树大作业******************************//
	printf("========================哈夫曼树操作菜单========================\n");
	printf("\t\t  请输入一段文字/字符：");
	//%[]读入集合所限定的字符，如果第一个字符是'^'，那读取到'^'后面的字符就会停止
	//用这种方法可以读入一串包含空格的字符串
	scanf("%[^\n]", the_String);

	//******************************统计每个字符的出现次数******************************//
	//把读入字符的ascii码作为下标储存为对应的元素
	//但是一定要提前初始化str_ascii数组为全0
	for (i = 0; the_String[i] != 0; i++)
		str_ascii[the_String[i]]++;
	for (i = 0; i < 256; i++)
	{
		//大于0就是存在有字符
		if (str_ascii[i] > 0)
		{
			//str_times[]和str_what[]是一一对应的
			//就是说比如他们的0号元素是a和1，那就是a出现了1次
			str_times[type] = str_ascii[i];
			str_what[type] = (char)i;
			type++;//每出现一种类型的字符，种类就加1
		}
	}
	printf("\n\n========================输出哈夫曼树叶子========================\n");
	printf("根据你输入的文字/字符，以下是对于其中出现的每个字符的次数的统计！\n");
	printf("出现的字符\t\t出现的次数\n");
	for (i = 0; i < type; i++)
		printf("%c\t\t\t%d\n", str_what[i], str_times[i]);


	//******************************构建哈夫曼树******************************//
	HT = create_HuffmanTree(str_times, str_what, type);
	printf("\n\n========================哈夫曼树的编码表========================\n");
	printf("创建哈夫曼树成功！下面是该哈夫曼树的参数输出：\n");
	printf("结点i\t字符\t权值\t双亲\t左孩子\t右孩子\n");
	for (i = 0; i < type; i++)
		printf("%d\t%c\t%d\t%d\t%d\t%d\n", i, HT[i].data, HT[i].weight, HT[i].parent, HT[i].lchild, HT[i].rchild);

	//******************************哈夫曼树编码******************************//
	HuffmanCoding(HT, HC, str_AsciiCoding, type);
	printf("\n\n========================哈夫曼树正在编码========================\n");
	printf("编码成功！\n");
	LinkHuffmanCode(HC, the_String, type, str_AsciiCoding, str_BinCoded);
	for (i = 0; i < len_sBC; i++)    //打印编码串
	{
		printf("%c", sBC[i]);
	}

	//输出哈夫曼树每个字符的二进制编码
	printf("\n\n====================打印哈夫曼树叶子的二进制编码====================\n");
	printf("以下是哈夫曼树叶子的二进制编码：\n");
	printf("字符\t哈夫曼编码\n");
	for (i = 0; i < type; i++)
	{
		printf("%c\t", sAC[i]);//先打印字符并不换行
		for (j = 0; j < strlen(HC[i]); j++)
		{
			printf("%c", *(*(HC + i) + j));//再解引二级指针打印哈夫曼编码
		}
		printf("\n");
	}

	//统计编码效率
	/*我用的压缩效率统计原理是：一串有n个字符的字符串占用n*sizeof(char)字节，而哈夫曼编码则只用上len_sBC个比特位储存
	那么压缩效率就是：len_sBC/n*sizeof(char)*/
	printf("\n====================统计哈夫曼编码效率====================\n");
	count_ts = strlen(tS);
	int total = count_ts * 8;
	printf("原二进制编码所占内存大小为（比特位）：");
	printf("%d\n", total);
	printf("哈夫曼树编码所占内存大小为（比特位）：");
	printf("%d\n", len_sBC);
	printf("哈夫曼树编码的压缩效率为：\n");
	//保留三位小数：先乘以1000再加0.5最后除以1000.0
	float perc = int((total - len_sBC) * 1000 + 0.5) / 1000.0;
	//下面注释的是用小数表示的压缩效率，可能不够百分比输出直观所以没用上
	//printf("%.5f\n",float(total - len_sBC) / total );
	//输出百分数：格式化输出里面要用%%才能输出'%'的符号
	printf("%.2f%%", (perc / total) * 100.0);

	//******************************哈夫曼树译码******************************//
	printf("\n\n========================哈夫曼树正在译码========================\n");
	printf("\n请输入哈夫曼编码串：\n");

	char scanf_Coded[MAX_MA];    //存储输入的编码串
	Scanf_str(scanf_Coded);

	HuffmanDecoded(HT, str_AsciiCoding, scanf_Coded, str_Decoded, type);
	printf("译码成功！\n");

	//将哈夫曼树译码后的字符串输出
	printf("\n\n======================打印哈夫曼树译码串======================\n");
	printf("哈夫曼树译码后的字符串为：\n");

	for (i = 0; i < len_sBC; i++)
	{
		if (sBC[i] != scanf_Coded[i])
		{
			printf("输入的二进制串与原编码串不相同，");
			printf("但依然按原编码进行译码\n");
			break;
		}
	}

	for (i = 0; sD[i] != '\0'; i++)
	{
		printf("%c", str_Decoded[i]);
	}

	printf("\n\n==================哈夫曼树结束，祝你生活愉快==================\n");
	return 0;
	system("pause");
}