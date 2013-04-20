#include<iostream>
#include<conio.h>
#include<memory>
#include<cmath>
#include<string>
#include<set>
#include<vector>
#include<queue>
#include<stack>
using namespace std;

#define INFINITE 99999

class Set;    //集合类的声明
class Sample;		//样本类的声明
class Attribute;		//属性类的声明
class TreeNode;		//树节点类的声明

Sample * TrainingSet;	//训练集对象
Attribute * Attrs;	//存储所有属性的数组
set<string> TotalClass;	//记录所有可能的类别，因为不允许有重复，所以使用STL的set
int NumOfAttrs,NumOfSamples;//    修改对应函数的参数！！！！！！！！！
bool choice = false;

bool checkFinish(Set &);
TreeNode * buildTree(TreeNode *,Set &);
double InfoGain(Set *, Attribute);
double SplitInfo(Set *,Attribute *);
double GainRatio(Set *,Attribute *);
double Accuracy(TreeNode *,Sample * ,int);
void InputData();
void Welcome();
void CrossValidate(TreeNode *&);
void Choose();
void OutputTree();

class node
{
private:
	double start,end,value;	//start，end分别为区间的左右端点，value为用户输入连续型数据时的取值
	string str;		//用户输入的离散型数据的取值
public:
	friend class Attribute;	//设置属性类为友元
	friend class Sample;	//设置样本类为友元
	friend double SplitInfo(Set *,Attribute *);	//设置求信息增益和求信息增益率的函数为友元函数
	friend double InfoGain(Set *, Attribute);
	friend TreeNode * buildTree(TreeNode *,Set &);
	friend double Accuracy(TreeNode*, Sample*, int);
	node(){}
};

class Sample
{
private:
	node * value;	  //样本对所有属性的取值
	string Class;    //记录该样本的类别
	int NumOfAttrs;  //属性个数
public:
	Sample(){};

	//函数名称：Init
	//函数功能描述：对Sample 对象进行初始化
	//返回值：无
	//函数的输入参数：属性数目 
	void Init(int _NumOfAttrs)
	{
		NumOfAttrs = _NumOfAttrs;
		value = new node[_NumOfAttrs];	//为属性取值数组分配大小为属性数的空间
		int i,j;
		for (i = 0;i < NumOfAttrs;i++)
		{
			char temp[20];
			cin >> temp;	//用户输入一个取值，由于不确定是连续值还是离散值，所以先用临时变量存储
			int len=strlen(temp);
			for(j=0;j<len;j++)
			{
				if(!(temp[j]>='0' && temp[j]<='9' || temp[j]=='.'))		//如果字符不是数字或者小数点，跳出循环
					break;
			}
			if(j==len)	//如果是连续值
				value[i].value = atof(temp);		//将字符串转成double型数字存储
			else	//如果是离散值
			{
				value[i].value=INFINITE;	//将数字设为INFINITE
				value[i].str = temp;		//同时将字符串转成string型存储
			}
		}
		cout << "请输入样本所属类别：";
		cin >> Class;
		TotalClass.insert(Class);		//将所属类别放入TotalClass
	}

	//函数名称：Value
	//函数功能描述：返回样本的取值数组
	//返回值：样本取值数组的首地址
	//函数的输入参数：无
	node * Value()
	{
		return value;
	}

	//函数名称：GetClass
	//函数功能描述：返回样本的所属类别
	//返回值：样本所属类别
	//函数的输入参数：无
	string GetClass()
	{
		return Class;
	}

	//函数名称：GetNumOfAttrs
	//函数功能描述：返回属性数目
	//返回值：属性数目
	//函数的输入参数：无
	int GetNumOfAttrs()
	{
		return NumOfAttrs;
	}
};

class Set
{
private:
	int num;//集合中样本个数
public:
	vector<Sample> sam;//集合中的样本
	Set(){}
	void setNum(int n)
	{
		num=n;
	}
		void InitSet(int n, Sample * _sam)
	{
		num = n;
		for(int i=0;i<n;i++)
			sam.push_back(_sam[i]);
	}
	void Insert(Sample &S)
	{
		sam.push_back(S);
	}
	int NumOfSamples()
	{
		return num;
	}
};

class Attribute
{
private:
	int id;//属性的编号
	string name;//属性的名称
	int num;//该属性有多少种不同的可能取值
	node * value;//该属性的取值集合
	bool discrete;//标记属性是离散值还是连续值
	bool valid;//属性是否有效
public:
	friend TreeNode * buildTree(TreeNode *,Set &);
	Attribute(){}
	//函数名称：Init
	//函数功能描述：对Attribute对象进行初始化
	//返回值：无
	//函数的输入参数：属性的编号
	void Init(int n)
	{
		cin >> name;
		valid = true;
		num = 0;
		id = n;
		value = NULL;
	}

	//函数名称：SetValue
	//函数功能描述：设置属性的所有可能取值
	//返回值：无
	//函数的输入参数：属性的编号index和样本的数目n
	void SetValue(int index,int n)
	{
		if(TrainingSet[0].Value()[index].value == INFINITE)	//如果连续型数值取值为INFINITE，说明是离散型数值
		{
			discrete = true;	//设置标记
			int i;
			set<string> temp;	//使用STL的set，用来不重复的记录所有可能取值
			set<string>::iterator ite;
			for(i = 0;i < n;i++)
				temp.insert(TrainingSet[i].Value()[index].str);	//先将所有取值存入temp
			value = new node[temp.size()];		//全部存完后为value数组开辟空间
			for(i = 0,ite = temp.begin();ite != temp.end();ite++)		//将temp中的元素存入数组
			{
				value[i].str = *ite;
				i++,num++;	//修改取值数目
			}
		}
		else	//如果是连续型属性
		{
			discrete = false;
			cout << "属性 " << name << " 是连续型属性，请您设置n,将取值区间等分成n个小区间：" << endl;
			int i,n;
			cin >> n;
			value = new node [n];
			num = n;
			double max = 0,min = INFINITE,delta;
			for(i = 0;i < n;i++)	//查找所有样本中取值的最大最小值
			{
				double temp = TrainingSet[i].Value()[index].value;
				if(temp > max)
					max = temp;
				if(temp < min)
					min = temp;
			}
			delta = (max - min) / n;	//计算等分断点
			for(i = 0 ; i < n ; i++)	//用断点分割区间，设置n个区间作为离散化后的取值
			{
				value[i].start = min;
				value[i].end = min + delta;
				min = min + delta;
			}
			value[i - 1].end += 0.0001;	//将最后一个区间的右端点加一个微小增量，以便决策时将区间变成左闭右开
		}
	}

	//函数名称：NumOfAttribute
	//函数功能描述：返回取值数目
	//返回值：属性的取值数目
	//函数的输入参数：无
	int NumOfAttribute()
	{
		return num;
	}

	//函数名称：Value
	//函数功能描述：返回属性的取值数组
	//返回值：样本属性数组的首地址
	//函数的输入参数：无
	node * Value()
	{
		return value;
	}

	//函数名称：Value
	//函数功能描述：返回属性的编号
	//返回值：属性的编号
	//函数的输入参数：无
	int Id()
	{
		return id;
	}

	//函数名称：Discrete
	//函数功能描述：返回属性是否为离散属性
	//返回值：属性是否为离散属性
	//函数的输入参数：无
	bool Discrete()
	{
		return discrete;
	}

	string GetName()
	{
		return name;
	}
};

class TreeNode
{
private:
	string value;
	double start,end;
	bool leaf;
	bool discrete;
	vector<TreeNode *> son;
public:
	friend TreeNode *buildTree(TreeNode *root,Set &S);
	friend double Accuracy(TreeNode* root, Sample* sam, int num);
	TreeNode(string V,bool Leaf)
	{
		value=V;
		discrete=true;
		leaf=Leaf;
	}
	TreeNode(double S,double E,bool Leaf)
	{
		start=S;
		end=E;
		discrete=false;
		leaf=Leaf;
	}
	TreeNode(){}
};

bool checkFinish(Set &p)
{
	vector<Sample>::iterator i=p.sam.begin();
	cout<<(*i).GetClass()<<endl;
	string Class=(*(p.sam.begin())).GetClass();	
	for(i=p.sam.begin();i<p.sam.end();i++)//检查这个集合中的样本，看看是否属于同一类别
		if((*i).GetClass()!=Class)
			return false;
	return true;
}

TreeNode * buildTree(TreeNode *root,Set &S)
{
	queue<Set > *Parts=new queue<Set >;	
	Parts->push(S);
	
	queue<TreeNode *> T;
	T.push(root);
	
	Set *Dealing;
	
	TreeNode * father;
	while(!(Parts->empty()))
	{
		int i,j;		
		string a;
		TreeNode *temp;
		vector<Sample>::iterator k;			
			
		Set x=Parts->front();
		Dealing=&x;
		Parts->pop();
		father=T.front();
		T.pop();
		//如果分类完毕
		if(checkFinish(*Dealing))
		{
			TreeNode *temp=new TreeNode(Dealing->sam.begin()->GetClass(),true);
			father->son.push_back(temp);
		}
		else
		{
			double maxGain=0;
			double v;
			int maxNum;
			//找到最优划分的属性
			for(i=0;i<NumOfAttrs;i++)
			{
				if(!Attrs[i].valid)
					continue;
				if(choice)
				{
					if((v=InfoGain(Dealing,Attrs[i]))>maxGain)
					{
						maxGain=v;
						maxNum=i;
					}
				}
				else
				{
					if((v=GainRatio(Dealing,&Attrs[i]))>maxGain)
					{
						maxGain=v;
						maxNum=i;
					}
				}
			}
			bool discrete=Attrs[maxNum].discrete;
			Attrs[maxNum].valid=false;//属性设置为已用
			//将该状态转化为节点，设置为father的子节点
			temp=new TreeNode(Attrs[maxNum].GetName(),false);
			father->son.push_back(temp);
			father=temp;

//以上为选取Dealing部分的属性，成为节点存入树
//以下为将该部分以该属性划分成的子部分成为节点与该节点挂钩，并存入树节点队列和部分队列
			
			//划分部分为子部分
			//divide状态,并进入子部分队列		

			for(j=0;j<Attrs[maxNum].NumOfAttribute();j++)
			{
				Set part;
				int num=0;
				if(discrete)//离散
				{
					a=(Attrs[maxNum].Value())[j].str;
					for(k=Dealing->sam.begin();k<Dealing->sam.end();k++)
					{
						if(((*k).Value())[maxNum].str==a)
						{						
							part.Insert((*k));
							num++;
						}
					}
					temp=new TreeNode(a,false);
				}
				else
				{
					double start=Attrs[maxNum].Value()[j].start;
					double end=Attrs[maxNum].Value()[j].end;
					for(k=Dealing->sam.begin();k<Dealing->sam.end();k++)
					{						
						if(((*k).Value()[maxNum]).start>=start&&((*k).Value()[maxNum]).end<=end)
						{

							part.Insert((*k));
							num++;
						}
					}
					temp=new TreeNode(start,end,false);
				}
				part.setNum(num);
				//将属性取值建成节点存入树					
				father->son.push_back(temp);
				//将该子节点推入栈
				T.push(temp);
				//将该子部分推入队列
				Parts->push(part);
			}
		}
	}
	delete Parts;
	root=root->son[0];
	return root;
}

//函数名称：InfoGain
//函数功能描述：计算相对训练集S，属性A的信息增益
//函数调用之前的预备条件：属性已初始化，
//返回后的处理：
//返回值（如果有的话）：
//函数的输入参数：
//函数的输出参数：
//函数的抽象算法（伪码）： 函数与其他对象中函数的调用和被调用关系：

double InfoGain(Set *S, Attribute A)
{
	int i, j, k;
	bool discrete = A.Discrete();
	int n = TotalClass.size();
	int m = A.NumOfAttribute();
	set<string>::iterator itr = TotalClass.begin();

	//计算出题意中的数组c
	int **c = new int*[n];
	for (i = 0;i < n;i++)
		c[i] = new int[m];
	for (i = 0;i < n;i++)
	{
		if (i > 0)
			itr++;
		for (j = 0;j < m;j++)
		{
			c[i][j] = 0;
			for (k = 0;k < S->NumOfSamples();k++)
			{
				if (S->sam[k].Value()[A.Id()].value == INFINITE)
				{
					if (S->sam[k].Value()[A.Id()].str == A.Value()[j].str && S->sam[k].GetClass() == *itr)
						c[i][j]++;
				}
				else
				{
					if (S->sam[k].Value()[A.Id()].value >= A.Value()[j].start && 
						S->sam[k].Value()[A.Id()].value <= A.Value()[j].end && S->sam[k].GetClass() == *itr)
						c[i][j]++;
				}
			}
		}
	}

	//计算infoA
	double infoA = 0;
	double tmpsumj = 0;
	for (j = 0;j < m;j++)
	{
		tmpsumj = 0;
		for (i = 0;i < n;i++)
			tmpsumj += c[i][j];
		for (i = 0;i < n;i++)
		{
			if (c[i][j] > 0)
				infoA += c[i][j]*log((double)(c[i][j]/tmpsumj))/log((double)2);
		}
	}
	infoA /= S->NumOfSamples();
	infoA *= -1;

	//计算info
	double info = 0;
	double *p = new double[n];
	itr = TotalClass.begin();
	for (i = 0;i < n;i++)
	{
		p[i] = 0;
		if (i > 0)
			itr++;
		for (j = 0;j < S->NumOfSamples();j++)
		{
			if (S->sam[j].GetClass() == *itr)
				p[i]++;
		}
		p[i] /= S->NumOfSamples();
	}
	for (i = 0;i < n;i++)
	{
		if (p[i] > 0)
			info -= p[i] * log(p[i])/log((double)2); 
	}
	return info - infoA;
}

//函数名称：SplitInfo
//函数功能描述：计算按照属性A分裂样本集S的广度和均匀性
//返回值：分裂信息的广度和均匀性
//函数的输入参数：样本集S，属性A
//函数的抽象算法： 由公式SplitInfo = ∑(Si/S)*log(Si/S)/log2得出分裂信息
//函数与其他对象中函数的调用和被调用关系：调用Set类对象和Attribute类对象
double SplitInfo(Set * S,Attribute * A)
{
	int i,j;
	double splitinfo = 0;	//分裂信息
	bool discrete = A->Discrete();
	int * num = new int [A->NumOfAttribute()];		//记录不同属性值的样本数目的数组
	memset(num,0,sizeof(num));	//数组清零
	if(discrete)	//如果该属性是离散型的
	{
		string temp;	
		for(i = 0;i < S->NumOfSamples();i++)
		{
			temp = S->sam[i].Value()[A->Id()].str;	//遍历集合中的所有样本，取出每个样本的属性值
			for(j = 0;j < A->NumOfAttribute();j++)	//再遍历属性的所有取值
			{
				if(temp == A->Value()[j].str)	//一旦属性值相等，计数数组加一，跳出循环
				{
					num[j]++;
					break;
				}
			}
		}
	}
	else	//如果属性时连续型的
	{
		double temp;
		for(i = 0;i < S->NumOfSamples();i++)
		{
			temp = S->sam[i].Value()[A->Id()].value;	//遍历集合中的所有样本，取出每个样本的属性值
			for(j = 0;j < A->NumOfAttribute();j++)	//再遍历属性的所有取值区间
			{
				if(temp >= A->Value()[j].start && temp < A->Value()[j].end)		//如果取值在左闭右开区间中
				{
					num[j]++;	//计数数组加一，跳出循环
					break;
				}
			}
		}
	}
	for(i=0;i<A->NumOfAttribute();i++)		//按照公式SplitInfo = ∑(Si/S)*log(Si/S)/log2计算分裂信息的值
	{
		double ratio = num[i] / S->NumOfSamples();
		splitinfo -= ratio * (log(ratio) / log(double(2)));
	}
	return splitinfo;
}

//函数名称：GainInfo
//函数功能描述：计算用属性A划分集合S的信息增益率
//返回值：信息增益率
//函数的输入参数：样本集S，属性A
//函数的抽象算法： 信息增益率=信息增益/分裂信息的广度和均匀性
//函数与其他对象中函数的调用和被调用关系：调用Set类对象和Attribute类对象
double GainRatio(Set * S,Attribute * A)
{
	return InfoGain(S,*A) / SplitInfo(S,A);
}
  
double Accuracy(TreeNode* root, Sample* sam, int num)
{
	int i, j, k;
	int flag;     //判断离散或连续
	int count = 0;
	TreeNode* ptr = root;
	for (i = 0;i < num;i++)
	{
		flag = (sam[i].Value()->value == INFINITE);
		while (1)
		{
			if (ptr -> son.empty())
			{
				if (ptr -> value == sam[i].GetClass())
					count++;
				break;
			}
			else
			{
				j = 0;
				k = 0;
				while(1)
				{
					if (ptr -> value == Attrs[k].GetName())
						break;
					k++;
				}
				while(1)
				{
					if (flag)
					{
						if (ptr->son[j]->value == sam[i].Value()[k].str)
						{
							ptr = ptr->son[j]->son[0];
							break;
						}
					}
					else
					{
						if (sam[i].Value()[k].value >= ptr->son[j]->start && sam[i].Value()[k].value <= ptr->son[j]->end)
						{
							ptr = ptr->son[j]->son[0];
							break;
						}
					}
					j++;
				}
			}
		}
	}
	return (double)(count / num);
}

void CrossValidate(TreeNode *& Root)
{
	int num,i,j,k,start,end,delta,maxroot;
	delta = NumOfSamples / 10;
	num = NumOfSamples - delta * 9;
	start = 0,end = start + num;
	TreeNode * root[10];
	double max = 0,accu[10];
	Sample * TestCase = new Sample[num];
	Set S[10];
	for(i = 0;i < 10;i++)
	{
		k = 0;
		S[i].setNum(delta * 9);		
		root[i] = new TreeNode;
		for(j = 0;j < NumOfSamples;j++)
		{
			if(j >= start && j < end)
				TestCase[k++] = TrainingSet[j];
			else
				S[i].sam.push_back(TrainingSet[j]);
		}
		root[i] = buildTree(root[i],S[i]);
		accu[i] = Accuracy(root[i],TestCase,num);
		start +=delta ,end += delta;
	}
	for(i = 0;i < 10;i++)
	{
		if(accu[i] > max)
		{
			max = accu[i];
			maxroot = i;
		}
	}
	Root = buildTree(Root,S[maxroot]);
}

void Printmode(bool flag)
{
	if(flag==0)
	{
		cout << "请您选择建立决策树的算法，回车键确认：" << endl;
		cout << "→  用ID3算法建立决策树" << endl;
		cout << "    用C4.5算法建立决策树" <<endl;
	}
	else if(flag==1)
	{
		cout << "请您选择建立决策树的算法，回车键确认：" << endl;
		cout << "    用ID3算法建立决策树" << endl;
		cout << "→  用C4.5算法建立决策树" << endl;
	}
}

void Choose()
{
	system("cls");
	int i;
	for(i = 0;i < NumOfAttrs;i++)
	{
		if(Attrs[i].Discrete() == false)
		{
			cout << "由于训练集含有连续型属性，所以您只能使用C4.5算法建立决策树" <<endl;
			choice = false;
			break;
		}
	}
	if(i == NumOfAttrs)
	{
		Printmode(choice);
		int input;//存储键盘输入的值
		input = _getch();
		while(input!=13)//输入为回车即停止
		{
			if(input == 224)//若为方向键
			{
				int in=_getch();
				if(in==72||in==80)//输入为上方向键
				{
					system("cls");//清屏
					choice=!choice;
					Printmode(choice);//打印
				}			
			}
			input = _getch();//在此读取输入
		}
		choice = !choice;
	}
}
//Set S;
void InputData()
{
	system("cls");
	int i;
	cout << "请输入训练集样本数目：" << endl;
	cin >> NumOfSamples;
	TrainingSet = new Sample[NumOfSamples];
	cout << "请输入属性数目：" << endl;
	cin >> NumOfAttrs;
	Attrs = new Attribute[NumOfAttrs];
	cout << "请输入" << NumOfAttrs << "个属性名称" << endl;//输多了咋办
	for(i = 0;i < NumOfAttrs;i++)
		Attrs[i].Init(i);
	for (i = 0;i < NumOfSamples;i++)
	{
		cout << "请输入样本" << i + 1 << "的" << NumOfAttrs << "个属性取值" << endl;//输多了咋办
		TrainingSet[i].Init(NumOfAttrs);
	}
	for(i = 0;i < NumOfAttrs;i++)
		Attrs[i].SetValue(i,NumOfSamples);
	//S.InitSet(NumOfSamples, TrainingSet);
	Choose();
}

void OutputTree()
{
	TreeNode * root = new TreeNode;
	cout << choice  << endl;
	cout << "以下是您根据";
	if(choice)
		cout << "ID3算法建立的决策树：" << endl;
	else
		cout << "C4.5算法建立的决策树：" << endl;
	CrossValidate(root);
}

void Welcome()
{
	cout << "┏━━━━━━━━━━━━━━━━━━━━━━━━━┓" << endl;
	cout << "┃                                                  ┃" << endl;
	cout << "┃                    欢迎使用                      ┃" << endl;
	cout << "┃                                                  ┃" << endl;
	cout << "┃                   决策树系统                     ┃" << endl;
	cout << "┃                                                  ┃" << endl;
	cout << "┃             By  00948136 何宇宁                  ┃" << endl;
	cout << "┃                 00948226 黎斯达                  ┃" << endl;
	cout << "┃                 00948256 谌国风                  ┃" << endl;		
	cout << "┃                                                  ┃" << endl;
	cout << "┃                 请按任意键继续                   ┃" << endl;
	cout << "┃                                                  ┃" << endl;		
	cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━┛" << endl;
	getch();
	InputData();
	OutputTree();
}

int main()
{
	Welcome();
	//for (int i = 0;i < 4;i++)
	//	cout << InfoGain(&S, Attrs[i]) << endl;
	return 0;
}