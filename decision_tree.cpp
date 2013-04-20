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

class Set;    //�����������
class Sample;		//�����������
class Attribute;		//�����������
class TreeNode;		//���ڵ��������

Sample * TrainingSet;	//ѵ��������
Attribute * Attrs;	//�洢�������Ե�����
set<string> TotalClass;	//��¼���п��ܵ������Ϊ���������ظ�������ʹ��STL��set
int NumOfAttrs,NumOfSamples;//    �޸Ķ�Ӧ�����Ĳ���������������������
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
	double start,end,value;	//start��end�ֱ�Ϊ��������Ҷ˵㣬valueΪ�û���������������ʱ��ȡֵ
	string str;		//�û��������ɢ�����ݵ�ȡֵ
public:
	friend class Attribute;	//����������Ϊ��Ԫ
	friend class Sample;	//����������Ϊ��Ԫ
	friend double SplitInfo(Set *,Attribute *);	//��������Ϣ���������Ϣ�����ʵĺ���Ϊ��Ԫ����
	friend double InfoGain(Set *, Attribute);
	friend TreeNode * buildTree(TreeNode *,Set &);
	friend double Accuracy(TreeNode*, Sample*, int);
	node(){}
};

class Sample
{
private:
	node * value;	  //�������������Ե�ȡֵ
	string Class;    //��¼�����������
	int NumOfAttrs;  //���Ը���
public:
	Sample(){};

	//�������ƣ�Init
	//����������������Sample ������г�ʼ��
	//����ֵ����
	//���������������������Ŀ 
	void Init(int _NumOfAttrs)
	{
		NumOfAttrs = _NumOfAttrs;
		value = new node[_NumOfAttrs];	//Ϊ����ȡֵ��������СΪ�������Ŀռ�
		int i,j;
		for (i = 0;i < NumOfAttrs;i++)
		{
			char temp[20];
			cin >> temp;	//�û�����һ��ȡֵ�����ڲ�ȷ��������ֵ������ɢֵ������������ʱ�����洢
			int len=strlen(temp);
			for(j=0;j<len;j++)
			{
				if(!(temp[j]>='0' && temp[j]<='9' || temp[j]=='.'))		//����ַ��������ֻ���С���㣬����ѭ��
					break;
			}
			if(j==len)	//���������ֵ
				value[i].value = atof(temp);		//���ַ���ת��double�����ִ洢
			else	//�������ɢֵ
			{
				value[i].value=INFINITE;	//��������ΪINFINITE
				value[i].str = temp;		//ͬʱ���ַ���ת��string�ʹ洢
			}
		}
		cout << "�����������������";
		cin >> Class;
		TotalClass.insert(Class);		//������������TotalClass
	}

	//�������ƣ�Value
	//������������������������ȡֵ����
	//����ֵ������ȡֵ������׵�ַ
	//�����������������
	node * Value()
	{
		return value;
	}

	//�������ƣ�GetClass
	//�������������������������������
	//����ֵ�������������
	//�����������������
	string GetClass()
	{
		return Class;
	}

	//�������ƣ�GetNumOfAttrs
	//������������������������Ŀ
	//����ֵ��������Ŀ
	//�����������������
	int GetNumOfAttrs()
	{
		return NumOfAttrs;
	}
};

class Set
{
private:
	int num;//��������������
public:
	vector<Sample> sam;//�����е�����
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
	int id;//���Եı��
	string name;//���Ե�����
	int num;//�������ж����ֲ�ͬ�Ŀ���ȡֵ
	node * value;//�����Ե�ȡֵ����
	bool discrete;//�����������ɢֵ��������ֵ
	bool valid;//�����Ƿ���Ч
public:
	friend TreeNode * buildTree(TreeNode *,Set &);
	Attribute(){}
	//�������ƣ�Init
	//����������������Attribute������г�ʼ��
	//����ֵ����
	//������������������Եı��
	void Init(int n)
	{
		cin >> name;
		valid = true;
		num = 0;
		id = n;
		value = NULL;
	}

	//�������ƣ�SetValue
	//���������������������Ե����п���ȡֵ
	//����ֵ����
	//������������������Եı��index����������Ŀn
	void SetValue(int index,int n)
	{
		if(TrainingSet[0].Value()[index].value == INFINITE)	//�����������ֵȡֵΪINFINITE��˵������ɢ����ֵ
		{
			discrete = true;	//���ñ��
			int i;
			set<string> temp;	//ʹ��STL��set���������ظ��ļ�¼���п���ȡֵ
			set<string>::iterator ite;
			for(i = 0;i < n;i++)
				temp.insert(TrainingSet[i].Value()[index].str);	//�Ƚ�����ȡֵ����temp
			value = new node[temp.size()];		//ȫ�������Ϊvalue���鿪�ٿռ�
			for(i = 0,ite = temp.begin();ite != temp.end();ite++)		//��temp�е�Ԫ�ش�������
			{
				value[i].str = *ite;
				i++,num++;	//�޸�ȡֵ��Ŀ
			}
		}
		else	//���������������
		{
			discrete = false;
			cout << "���� " << name << " �����������ԣ���������n,��ȡֵ����ȷֳ�n��С���䣺" << endl;
			int i,n;
			cin >> n;
			value = new node [n];
			num = n;
			double max = 0,min = INFINITE,delta;
			for(i = 0;i < n;i++)	//��������������ȡֵ�������Сֵ
			{
				double temp = TrainingSet[i].Value()[index].value;
				if(temp > max)
					max = temp;
				if(temp < min)
					min = temp;
			}
			delta = (max - min) / n;	//����ȷֶϵ�
			for(i = 0 ; i < n ; i++)	//�öϵ�ָ����䣬����n��������Ϊ��ɢ�����ȡֵ
			{
				value[i].start = min;
				value[i].end = min + delta;
				min = min + delta;
			}
			value[i - 1].end += 0.0001;	//�����һ��������Ҷ˵��һ��΢С�������Ա����ʱ������������ҿ�
		}
	}

	//�������ƣ�NumOfAttribute
	//������������������ȡֵ��Ŀ
	//����ֵ�����Ե�ȡֵ��Ŀ
	//�����������������
	int NumOfAttribute()
	{
		return num;
	}

	//�������ƣ�Value
	//���������������������Ե�ȡֵ����
	//����ֵ����������������׵�ַ
	//�����������������
	node * Value()
	{
		return value;
	}

	//�������ƣ�Value
	//���������������������Եı��
	//����ֵ�����Եı��
	//�����������������
	int Id()
	{
		return id;
	}

	//�������ƣ�Discrete
	//�����������������������Ƿ�Ϊ��ɢ����
	//����ֵ�������Ƿ�Ϊ��ɢ����
	//�����������������
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
	for(i=p.sam.begin();i<p.sam.end();i++)//�����������е������������Ƿ�����ͬһ���
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
		//����������
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
			//�ҵ����Ż��ֵ�����
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
			Attrs[maxNum].valid=false;//��������Ϊ����
			//����״̬ת��Ϊ�ڵ㣬����Ϊfather���ӽڵ�
			temp=new TreeNode(Attrs[maxNum].GetName(),false);
			father->son.push_back(temp);
			father=temp;

//����ΪѡȡDealing���ֵ����ԣ���Ϊ�ڵ������
//����Ϊ���ò����Ը����Ի��ֳɵ��Ӳ��ֳ�Ϊ�ڵ���ýڵ�ҹ������������ڵ���кͲ��ֶ���
			
			//���ֲ���Ϊ�Ӳ���
			//divide״̬,�������Ӳ��ֶ���		

			for(j=0;j<Attrs[maxNum].NumOfAttribute();j++)
			{
				Set part;
				int num=0;
				if(discrete)//��ɢ
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
				//������ȡֵ���ɽڵ������					
				father->son.push_back(temp);
				//�����ӽڵ�����ջ
				T.push(temp);
				//�����Ӳ����������
				Parts->push(part);
			}
		}
	}
	delete Parts;
	root=root->son[0];
	return root;
}

//�������ƣ�InfoGain
//���������������������ѵ����S������A����Ϣ����
//��������֮ǰ��Ԥ�������������ѳ�ʼ����
//���غ�Ĵ���
//����ֵ������еĻ�����
//���������������
//���������������
//�����ĳ����㷨��α�룩�� ���������������к����ĵ��úͱ����ù�ϵ��

double InfoGain(Set *S, Attribute A)
{
	int i, j, k;
	bool discrete = A.Discrete();
	int n = TotalClass.size();
	int m = A.NumOfAttribute();
	set<string>::iterator itr = TotalClass.begin();

	//����������е�����c
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

	//����infoA
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

	//����info
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

//�������ƣ�SplitInfo
//�����������������㰴������A����������S�Ĺ�Ⱥ;�����
//����ֵ��������Ϣ�Ĺ�Ⱥ;�����
//���������������������S������A
//�����ĳ����㷨�� �ɹ�ʽSplitInfo = ��(Si/S)*log(Si/S)/log2�ó�������Ϣ
//���������������к����ĵ��úͱ����ù�ϵ������Set������Attribute�����
double SplitInfo(Set * S,Attribute * A)
{
	int i,j;
	double splitinfo = 0;	//������Ϣ
	bool discrete = A->Discrete();
	int * num = new int [A->NumOfAttribute()];		//��¼��ͬ����ֵ��������Ŀ������
	memset(num,0,sizeof(num));	//��������
	if(discrete)	//�������������ɢ�͵�
	{
		string temp;	
		for(i = 0;i < S->NumOfSamples();i++)
		{
			temp = S->sam[i].Value()[A->Id()].str;	//���������е�����������ȡ��ÿ������������ֵ
			for(j = 0;j < A->NumOfAttribute();j++)	//�ٱ������Ե�����ȡֵ
			{
				if(temp == A->Value()[j].str)	//һ������ֵ��ȣ����������һ������ѭ��
				{
					num[j]++;
					break;
				}
			}
		}
	}
	else	//�������ʱ�����͵�
	{
		double temp;
		for(i = 0;i < S->NumOfSamples();i++)
		{
			temp = S->sam[i].Value()[A->Id()].value;	//���������е�����������ȡ��ÿ������������ֵ
			for(j = 0;j < A->NumOfAttribute();j++)	//�ٱ������Ե�����ȡֵ����
			{
				if(temp >= A->Value()[j].start && temp < A->Value()[j].end)		//���ȡֵ������ҿ�������
				{
					num[j]++;	//���������һ������ѭ��
					break;
				}
			}
		}
	}
	for(i=0;i<A->NumOfAttribute();i++)		//���չ�ʽSplitInfo = ��(Si/S)*log(Si/S)/log2���������Ϣ��ֵ
	{
		double ratio = num[i] / S->NumOfSamples();
		splitinfo -= ratio * (log(ratio) / log(double(2)));
	}
	return splitinfo;
}

//�������ƣ�GainInfo
//������������������������A���ּ���S����Ϣ������
//����ֵ����Ϣ������
//���������������������S������A
//�����ĳ����㷨�� ��Ϣ������=��Ϣ����/������Ϣ�Ĺ�Ⱥ;�����
//���������������к����ĵ��úͱ����ù�ϵ������Set������Attribute�����
double GainRatio(Set * S,Attribute * A)
{
	return InfoGain(S,*A) / SplitInfo(S,A);
}
  
double Accuracy(TreeNode* root, Sample* sam, int num)
{
	int i, j, k;
	int flag;     //�ж���ɢ������
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
		cout << "����ѡ�������������㷨���س���ȷ�ϣ�" << endl;
		cout << "��  ��ID3�㷨����������" << endl;
		cout << "    ��C4.5�㷨����������" <<endl;
	}
	else if(flag==1)
	{
		cout << "����ѡ�������������㷨���س���ȷ�ϣ�" << endl;
		cout << "    ��ID3�㷨����������" << endl;
		cout << "��  ��C4.5�㷨����������" << endl;
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
			cout << "����ѵ�����������������ԣ�������ֻ��ʹ��C4.5�㷨����������" <<endl;
			choice = false;
			break;
		}
	}
	if(i == NumOfAttrs)
	{
		Printmode(choice);
		int input;//�洢���������ֵ
		input = _getch();
		while(input!=13)//����Ϊ�س���ֹͣ
		{
			if(input == 224)//��Ϊ�����
			{
				int in=_getch();
				if(in==72||in==80)//����Ϊ�Ϸ����
				{
					system("cls");//����
					choice=!choice;
					Printmode(choice);//��ӡ
				}			
			}
			input = _getch();//�ڴ˶�ȡ����
		}
		choice = !choice;
	}
}
//Set S;
void InputData()
{
	system("cls");
	int i;
	cout << "������ѵ����������Ŀ��" << endl;
	cin >> NumOfSamples;
	TrainingSet = new Sample[NumOfSamples];
	cout << "������������Ŀ��" << endl;
	cin >> NumOfAttrs;
	Attrs = new Attribute[NumOfAttrs];
	cout << "������" << NumOfAttrs << "����������" << endl;//�����զ��
	for(i = 0;i < NumOfAttrs;i++)
		Attrs[i].Init(i);
	for (i = 0;i < NumOfSamples;i++)
	{
		cout << "����������" << i + 1 << "��" << NumOfAttrs << "������ȡֵ" << endl;//�����զ��
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
	cout << "������������";
	if(choice)
		cout << "ID3�㷨�����ľ�������" << endl;
	else
		cout << "C4.5�㷨�����ľ�������" << endl;
	CrossValidate(root);
}

void Welcome()
{
	cout << "������������������������������������������������������" << endl;
	cout << "��                                                  ��" << endl;
	cout << "��                    ��ӭʹ��                      ��" << endl;
	cout << "��                                                  ��" << endl;
	cout << "��                   ������ϵͳ                     ��" << endl;
	cout << "��                                                  ��" << endl;
	cout << "��             By  00948136 ������                  ��" << endl;
	cout << "��                 00948226 ��˹��                  ��" << endl;
	cout << "��                 00948256 �ȹ���                  ��" << endl;		
	cout << "��                                                  ��" << endl;
	cout << "��                 �밴���������                   ��" << endl;
	cout << "��                                                  ��" << endl;		
	cout << "������������������������������������������������������" << endl;
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