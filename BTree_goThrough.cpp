// BTree_goThrough.cpp : 定义控制台应用程序的入口点。
//

/*
知识储备：
1.内存分配方式
内存分配方式有三种：
[1]从静态存储区域分配。内存在程序编译的时候就已经分配好，这块内存在程序的整个运行期间都存在。例如全局变量，static变量。
[2]在栈上创建。在执行函数时，函数内局部变量的存储单元都可以在栈上创建，函数执行结束时这些存储单元自动被释放。栈内存分配运算内置于处理器的指令集中，效率很高，但是分配的内存容量有限。
[3]从堆上分配，亦称动态内存分配。程序在运行的时候用malloc或new申请任意多少的内存，程序员自己负责在何时用free或delete释放内存。动态内存的生存期由程序员决定，使用非常灵活，但如果在堆上分配了空间，就有责任回收它，否则运行的程序会出现内存泄漏，频繁地分配和释放不同大小的堆空间将会产生堆内碎块。
2.程序的内存空间
一个程序将操作系统分配给其运行的内存块分为4个区域，如下图所示。
代码区(code area)
程序内存空间
全局数据区(data area)
堆区(heap area)
栈区(stack area)
一个由C/C++编译的程序占用的内存分为以下几个部分,
1、栈区（stack）   由编译器自动分配释放 ，存放为运行函数而分配的局部变量、函数参数、返回数据、返回地址等。其操作方式类似于数据结构中的栈。
2、堆区（heap）    一般由程序员分配释放， 若程序员不释放，程序结束时可能由OS回收 。分配方式类似于链表。
3、全局区（静态区）（static）存放全局变量、静态数据、常量。程序结束后有系统释放
4、文字常量区 常量字符串就是放在这里的。 程序结束后由系统释放。
5、程序代码区存放函数体（类成员函数和全局函数）的二进制代码。
下面给出例子程序，
int a = 0; //全局初始化区
char *p1; //全局未初始化区
int main() {
int b; //栈
char s[] = /"abc/"; //栈
char *p2; //栈
char *p3 = /"123456/"; //123456//0在常量区，p3在栈上。
static int c =0;//全局（静态）初始化区
p1 = new char[10];
p2 = new char[20];
//分配得来得和字节的区域就在堆区。
strcpy(p1, /"123456/"); //123456//0放在常量区，编译器可能会将它与p3所指向的/"123456/"优化成一个地方。
}
3．堆与栈的比较
3.1申请方式
stack: 由系统自动分配。 例如，声明在函数中一个局部变量 int b; 系统自动在栈中为b开辟空间。
heap: 需要程序员自己申请，并指明大小，在C中malloc函数，C++中是new运算符。
如p1 = (char *)malloc(10); p1 = new char[10];
如p2 = (char *)malloc(10); p2 = new char[20];
但是注意p1、p2本身是在栈中的。
3.2申请后系统的响应
栈：只要栈的剩余空间大于所申请空间，系统将为程序提供内存，否则将报异常提示栈溢出。
堆：首先应该知道操作系统有一个记录空闲内存地址的链表，当系统收到程序的申请时，会遍历该链表，寻找第一个空间大于所申请空间的堆结点，然后将该结点从空闲结点链表中删除，并将该结点的空间分配给程序。
对于大多数系统，会在这块内存空间中的首地址处记录本次分配的大小，这样，代码中的delete语句才能正确的释放本内存空间。
由于找到的堆结点的大小不一定正好等于申请的大小，系统会自动的将多余的那部分重新放入空闲链表中。
3.3申请大小的限制
栈：在Windows下,栈是向低地址扩展的数据结构，是一块连续的内存的区域。这句话的意思是栈顶的地址和栈的最大容量是系统预先规定好的，在 WINDOWS下，栈的大小是2M（也有的说是1M，总之是一个编译时就确定的常数），如果申请的空间超过栈的剩余空间时，将提示overflow。因 此，能从栈获得的空间较小。
堆：堆是向高地址扩展的数据结构，是不连续的内存区域。这是由于系统是用链表来存储的空闲内存地址的，自然是不连续的，而链表的遍历方向是由低地址向高地址。堆的大小受限于计算机系统中有效的虚拟内存。由此可见，堆获得的空间比较灵活，也比较大。
3.4申请效率的比较
栈由系统自动分配，速度较快。但程序员是无法控制的。
堆是由new分配的内存，一般速度比较慢，而且容易产生内存碎片,不过用起来最方便。
另外，在WINDOWS下，最好的方式是用VirtualAlloc分配内存，他不是在堆，也不是栈，而是直接在进程的地址空间中保留一快内存，虽然用起来最不方便。但是速度快，也最灵活。
3.5堆和栈中的存储内容
栈：在函数调用时，第一个进栈的是主函数中后的下一条指令（函数调用语句的下一条可执行语句）的地址，然后是函数的各个参数，在大多数的C编译器中，参数是由右往左入栈的，然后是函数中的局部变量。注意静态变量是不入栈的。
当本次函数调用结束后，局部变量先出栈，然后是参数，最后栈顶指针指向最开始存的地址，也就是主函数中的下一条指令，程序由该点继续运行。
堆：一般是在堆的头部用一个字节存放堆的大小。堆中的具体内容有程序员安排。
3.6存取效率的比较
char s1[] = /"a/";
char *s2 = /"b/";
a是在运行时刻赋值的；而b是在编译时就确定的；但是，在以后的存取中，在栈上的数组比指针所指向的字符串(例如堆)快。 比如：
int main(){
char a = 1;
char c[] = /"1234567890/";
char *p =/"1234567890/";
a = c[1];
a = p[1];
return 0;
}
对应的汇编代码
10: a = c[1];
00401067 8A 4D F1 mov cl,byte ptr [ebp-0Fh]
0040106A 88 4D FC mov byte ptr [ebp-4],cl
11: a = p[1];
0040106D 8B 55 EC mov edx,dword ptr [ebp-14h]
00401070 8A 42 01 mov al,byte ptr [edx+1]
00401073 88 45 FC mov byte ptr [ebp-4],al
第一种在读取时直接就把字符串中的元素读到寄存器cl中，而第二种则要先把指针值读到edx中，再根据edx读取字符，显然慢了。
3.7小结
堆和栈的主要区别由以下几点：
1、管理方式不同；
2、空间大小不同；
3、能否产生碎片不同；
4、生长方向不同；
5、分配方式不同；
6、分配效率不同；
管理方式：对于栈来讲，是由编译器自动管理，无需我们手工控制；对于堆来说，释放工作由程序员控制，容易产生memory leak。
空间大小：一般来讲在32位系统下，堆内存可以达到4G的空间，从这个角度来看堆内存几乎是没有什么限制的。但是对于栈来讲，一般都是有一定的空间大小的，例如，在VC6下面，默认的栈空间大小是1M。当然，这个值可以修改。
碎片问题：对于堆来讲，频繁的new/delete势必会造成内存空间的不连续，从而造成大量的碎片，使程序效率降低。对于栈来讲，则不会存在这个问题，因为栈是先进后出的队列，他们是如此的一一对应，以至于永远都不可能有一个内存块从栈中间弹出，在他弹出之前，在他上面的后进的栈内容已经被弹出，详细的可以参考数据结构。
生长方向：对于堆来讲，生长方向是向上的，也就是向着内存地址增加的方向；对于栈来讲，它的生长方向是向下的，是向着内存地址减小的方向增长。
分配方式：堆都是动态分配的，没有静态分配的堆。栈有2种分配方式：静态分配和动态分配。静态分配是编译器完成的，比如局部变量的分配。动态分配由malloca函数进行分配，但是栈的动态分配和堆是不同的，他的动态分配是由编译器进行释放，无需我们手工实现。
分配效率：栈是机器系统提供的数据结构，计算机会在底层对栈提供支持：分配专门的寄存器存放栈的地址，压栈出栈都有专门的指令执行，这就决定了栈的效率比较高。堆则是C/C++函数库提供的，它的机制是很复杂的，例如为了分配一块内存，库函数会按照一定的算法（具体的算法可以参考数据结构/操作系统）在堆内存中搜索可用的足够大小的空间，如果没有足够大小的空间（可能是由于内存碎片太多），就有可能调用系统功能去增加程序数据段的内存空间，这样就有机会分 到足够大小的内存，然后进行返回。显然，堆的效率比栈要低得多。
从这里我们可以看到，堆和栈相比，由于大量new/delete的使用，容易造成大量的内存碎片；由于没有专门的系统支持，效率很低；由于可能引发用户态和核心态的切换，内存的申请，代价变得更加昂贵。所以栈在程序中是应用最广泛的，就算是函数的调用也利用栈去完成，函数调用过程中的参数，返回地址， EBP和局部变量都采用栈的方式存放。所以，我们推荐大家尽量用栈，而不是用堆。
虽然栈有如此众多的好处，但是由于和堆相比不是那么灵活，有时候分配大量的内存空间，还是用堆好一些。
无论是堆还是栈，都要防止越界现象的发生（除非你是故意使其越界），因为越界的结果要么是程序崩溃，要么是摧毁程序的堆、栈结构，产生以想不到的结果。
4.new/delete与malloc/free比较
从C++角度上说，使用new分配堆空间可以调用类的构造函数，而malloc()函数仅仅是一个函数调用，它不会调用构造函数，它所接受的参数是一个unsigned long类型。同样，delete在释放堆空间之前会调用析构函数，而free函数则不会。
class Time{
public:
Time(int,int,int,string);
~Time(){
cout<</"call Time/'s destructor by:/"<<name<<endl;
}
private:
int hour;
int min;
int sec;
string name;
};
Time::Time(int h,int m,int s,string n){
hour=h;
min=m;
sec=s;
name=n;
cout<</"call Time/'s constructor by:/"<<name<<endl;
}
int main(){
Time *t1;
t1=(Time*)malloc(sizeof(Time));
free(t1);
Time *t2;
t2=new Time(0,0,0,/"t2/");
delete t2;
system(/"PAUSE/");
return EXIT_SUCCESS;
}
结果：
call Time/'s constructor by:t2
call Time/'s destructor by:t2
从结果可以看出，使用new/delete可以调用对象的构造函数与析构函数，并且示例中调用的是一个非默认构造函数。但在堆上分配对象数组时，只能调用默认构造函数，不能调用其他任何构造函数。
*/

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

#define LCOUNTS 1000
typedef struct BTreeNode//叶子结点结构体
{
	int data;//数据
	int number;//位置编号 在数组中的索引
	BTreeNode * lchild;//左节点
	BTreeNode * rchild;//右节点
}BTreeNode, * BTreeNodeP;

typedef struct BTree//二叉树结构体
{
	BTreeNodeP bTreeNodes[LCOUNTS] ;
	int nCount;
}BTree,* BTreeP;

int * LRM(BTreeP bTreeP)//左子树 右子树 中节点 
{
	printf("\n");
	int * go_through_array = (int *)malloc(sizeof(int)*bTreeP->nCount);
	BTreeNodeP root = bTreeP->bTreeNodes[0];
	if (root == NULL)
	{
		printf("The BTree hasn't created!");

	}
	int * SS = (int *)malloc(sizeof(int)*bTreeP->nCount*2);
	//实质为一个二维数组 0---bTreeP->nCount-1存的是节点编号  bTreeP->nCount----- bTreeP->nCount*2-1
	for (int i = 0; i < bTreeP->nCount*2; i++)//初始化SS栈
	{
		SS[i] = 0;
	}
	BTreeNodeP p = root;
    int top = 0;
	while (!(p == NULL&&top == 0))//p指向NULL 且栈为空就退出 
	{
		while (true)//这个while时入栈的过程
		{
			if (p == NULL)
			{
				break;
			}
			if (SS[bTreeP->nCount+p->number] > 0)//检验节点是否被遍历过
			{
				break;
			}
			SS[top++] = p->number;
			SS[bTreeP->nCount + p->number]++;
			p = p->lchild;
		}
		
		if (top == 0)//入栈过程结束 还是top==0，说明 p指向NULL 且栈为空
		{
			printf("LRM遍历完成");
			break;
		}
		p = bTreeP->bTreeNodes[SS[--top]];
		if (SS[bTreeP->nCount + p->number] == 2)
			//等于2说明这个节点是遍历右子树时入栈的，到这里此节点的右子树已经遍历完，该此节点输出了
		{
			printf("%d	", p->data);
		}
		else
		{
			if (p->rchild == NULL)
			{
				printf("%d	", p->data);
				//go_through_array[]
			}
			else
			{
				SS[top++] = p->number;
				SS[bTreeP->nCount + p->number]++;
				p = p->rchild;
			}
		}

	}
	return 0;

}
int * MLR(BTreeP bTreeP)// 中节点 左子树 右子树
{
	printf("\n");
	int * go_through_array = (int *)malloc(sizeof(int)*bTreeP->nCount);
	BTreeNodeP root = bTreeP->bTreeNodes[0];
	if (root == NULL)
	{
		printf("The BTree hasn't created!");

	}
	int * SS = (int *)malloc(sizeof(int)*bTreeP->nCount * 2);
	//实质为一个二维数组 0---bTreeP->nCount-1存的是节点编号  bTreeP->nCount----- bTreeP->nCount*2-1
	for (int i = 0; i < bTreeP->nCount * 2; i++)//初始化SS栈
	{
		SS[i] = 0;
	}
	BTreeNodeP p = root;
	int top = 0;
	while (!(p == NULL&&top == 0))//p指向NULL 且栈为空就退出 
	{
		while (true)//这个while时入栈的过程
		{
			if (p == NULL)
			{
				break;
			}
			if (SS[bTreeP->nCount + p->number] > 0)//检验节点是否被遍历过
			{
				break;
			}
			printf("%d	", p->data);//入栈时输出
			SS[top++] = p->number;
			SS[bTreeP->nCount + p->number]++;
			p = p->lchild;
		}

		if (top == 0)//入栈过程结束 还是top==0，说明 p指向NULL 且栈为空
		{
			printf("MLR遍历完成");
			break;
		}
		p = bTreeP->bTreeNodes[SS[--top]];
		/*if (SS[bTreeP->nCount + p->number] == 2)
		{
			printf("%d	", p->data);
		}  
	   else
		{*/
			if (p->rchild == NULL)
			{
				//printf("%d	", p->data);
				//go_through_array[]
				continue;
			}
			else
			{
				//SS[top++] = p->number;
				//SS[bTreeP->nCount + p->number]++;
				p = p->rchild;
			}
		/*}*/

	}
	return 0;
}
int * LMR(BTreeP bTreeP)//左子树 中节点 右子树
{
	printf("\n");
	int * go_through_array = (int *)malloc(sizeof(int)*bTreeP->nCount);
	BTreeNodeP root = bTreeP->bTreeNodes[0];
	if (root == NULL)
	{
		printf("The BTree hasn't created!");

	}
	int * SS = (int *)malloc(sizeof(int)*bTreeP->nCount * 2);
	//实质为一个二维数组 0---bTreeP->nCount-1存的是节点编号  bTreeP->nCount----- bTreeP->nCount*2-1
	for (int i = 0; i < bTreeP->nCount * 2; i++)//初始化SS栈
	{
		SS[i] = 0;
	}
	BTreeNodeP p = root;
	int top = 0;
	while (!(p == NULL&&top == 0))//p指向NULL 且栈为空就退出 
	{
		while (true)//这个while时入栈的过程
		{
			if (p == NULL)
			{
				break;
			}
			if (SS[bTreeP->nCount + p->number] > 0)//检验节点是否被遍历过
			{
				break;
			}
			SS[top++] = p->number;
			SS[bTreeP->nCount + p->number]++;
			p = p->lchild;
		}

		if (top == 0)//入栈过程结束 还是top==0，说明 p指向NULL 且栈为空
		{
			printf("LMR遍历完成");
			break;
		}
		p = bTreeP->bTreeNodes[SS[--top]];
		printf("%d	", p->data);//出栈时输出
		/*if (SS[bTreeP->nCount + p->number] == 2)
		{
		printf("%d	", p->data);
		}
		else
		{*/
		if (p->rchild == NULL)
		{
			//printf("%d	", p->data);
			//go_through_array[]
			continue;
		}
		else
		{
			//SS[top++] = p->number;
			//SS[bTreeP->nCount + p->number]++;
			p = p->rchild;
		}
		/*}*/

	}
	return 0;
}

int addBtreeNode(int data,BTreeP bTreeP)//返回当前的叶子数
{
	
	
	/*
	BTreeNode newBTreeNode;
	BTreeNodeP newBTreeNodeP = &newBTreeNode;
	这种是分配了栈内存，等待函数返回时这个结构体在栈空间中回收，栈顶esp 栈底edp返回上层函数位置
	即这块BTreeNode 内存 失效 ，放在BTree 数组中的指针所指向的内存就会失效，导致下一次在栈空间调用下一个函数时这块栈内存会被覆盖
	所以得用malloc分配一块堆内存
	*/
	BTreeNodeP newBTreeNodeP = (BTreeNodeP)malloc(sizeof(BTreeNode));
	newBTreeNodeP->data = data;
	newBTreeNodeP->number = bTreeP->nCount;//编号
	newBTreeNodeP->lchild = NULL;
	newBTreeNodeP->rchild = NULL;
	bTreeP->bTreeNodes[bTreeP->nCount] = newBTreeNodeP;//添加进Nodes数组

	//确定父节点
	if (bTreeP->nCount != 0)//不是根节点，就要确定父节点
	{
		int fatherNum = (bTreeP->nCount + 1) / 2 - 1;
		if ((bTreeP->nCount + 1) % 2 == 0)
		{
			bTreeP->bTreeNodes[fatherNum]->lchild = newBTreeNodeP;

		}
		else
		{
			bTreeP->bTreeNodes[fatherNum]->rchild = newBTreeNodeP;
		}
		
	}
	bTreeP->nCount++;

	return bTreeP->nCount;

}
int BTreeConstructor(int * datas,int n, BTreeP bTreeP)
{
	for (int i = 0; i < n; i++)
	{
		addBtreeNode(datas[i], bTreeP);
	}
	return bTreeP->nCount;
}

int main()
{
	int datas[10] = {1,3,4,5,6,10,2,9,4,1};
	BTreeP bTreeP= (BTreeP)malloc(sizeof(BTree));
	bTreeP->nCount = 0;
	bTreeP->bTreeNodes[0] = NULL;
    int nCount=BTreeConstructor(datas, 10, bTreeP);
	for (int i = 0; i < bTreeP->nCount; i++)
	{
		printf("%d	", bTreeP->bTreeNodes[i]->data);
	}
	LRM(bTreeP);
	MLR(bTreeP);
	LMR(bTreeP);
	system("pause");
	
    return 0;
}

