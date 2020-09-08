//
// ���α׷� ����: �л�DB�� ���� ���ڵ� ���α׷� => ����ڰ� �л� DB�� ����ϱ� ���� �� �� �ֵ��� ����
// �й� : 2018111237
// �̸� : �ǿ���
// �й� : ����� 3-4����
// ��¥ : 2019.11.21.�����

#include <string.h>
#include <stdlib.h> 
#include <time.h>
#include <windows.h>
#include <assert.h>
#include <locale.h>

#include <iomanip>   // setw()
#include <iostream>
#include <fstream>   // open(), close(), eof()
using namespace std;

const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // �ڵ鰪

COORD  Pos;                                              // ��ġ

//-----------------------------------------------------
//  ȭ����� (x, y) ��ǥ ��ġ�� Ŀ�� �̵�
//
void gotoXY(int x, int y)
{
	Pos.X = x - 1;
	Pos.Y = y - 1;
	SetConsoleCursorPosition(hConsole, Pos);
}

//-----------------------------------------------------
// ���ڻ� ����
//
void textColor(int color_number)
{
	SetConsoleTextAttribute(hConsole, color_number);
}

//-----------------------------------------------------
// ȭ���� �����
//
void clrScr()
{
	system("cls");
}

//--------------------< sleep >----------------------
// (�־��� �ð�)/1000 �� ���� ������ �����.
//
void sleep(clock_t wait)
{
	clock_t goal;
	goal = wait + clock();
	while (goal > clock())
		;
}

// studentRec �� ����
#define FieldWidth 20
typedef char char20[FieldWidth];
typedef struct
{
	int      deleted;
	char20  studentID, name, major1, major2, advisor;
} studentRec;

// bstreeData�� bstreeKey �� ����
#define bstreeKey char20
typedef struct
{
	bstreeKey key;
	int       recNum;
} bstreeData;

// queueData �� ����
#define queueData bstreeData

#include "mp1bstree.h"
#include "mp1queue.h"

#define recordsPerScreen 15

fstream studentDB;            // Global variable, DB
queue Q;                  // Global variable, queue
bstree index;               // Global variable, ����

#include "mp1bstree.cpp"
#include "mp1queue.cpp"

// ǥ �׸��⸦ ���� Ư������ 
// �� �� �� �� �� ���� �� �� �� ��   

char LT[] = "��", LB[] = "��", RT[] = "��", RB[] = "��", V[] = "��", H[] = "��";
char TU[] = "��", TD[] = "��", TL[] = "��", TR[] = "��", CR[] = "��";

//ǥ �׸��� ���� �Լ�
void BarField();
void TopLine();
void TitleLine();
void DataLine();
void BottomLine();

void displayHead();
void displayRecord(studentRec, int);
void displayBox(int left, int top, int right, int bottom);

// MP1 �Լ�
void dumpDB();
studentRec inputRecord(int);
bool matchRecord(studentRec student, int recNo);
void compress();

typedef char Message[80];

//--------------------< displayMessage>-------------------------

void displayMessage(const Message msg)
{
	gotoXY(1, 23);
	cout << msg << endl;
	sleep((clock_t)2 * CLOCKS_PER_SEC); // delay 2 seconds
	system("cls");
} 

//---------------------------------------------------------------
// ���� ���
//
void BarField()
{
	for (int i = 0; i < 15; i++)
		cout << '-';
}

//---------------------------------------------------------------
// �������������������������������� ���
//
void TopLine() {
	cout << "\n\n";
	cout << '+'; BarField();
	for (int i = 0; i < 5; i++)
	{
		cout << '+';
		BarField();
	}
	cout << '+' << endl;
}

//---------------------------------------------------------------
// �������������������������������� ���
//
void TitleLine()
{
	cout << '|'; BarField();
	for (int i = 0; i < 5; i++)
	{
		cout << '+';
		BarField();
	}
	cout << '|' << endl;
}

//---------------------------------------------------------------
//
// �������������������������������� ���
//
void BottomLine()
{
	cout << LB; BarField();
	for (int i = 0; i < 5; i++)
	{
		cout << TU; BarField();
	}
	cout << RB << endl;
}

//---------------------------------------------------------------
void displayRecord(studentRec student, int recNo)
{
	int width = 15;
	cout << '|' << setw(width) << recNo << '|' << setw(width) << student.studentID << '|' << setw(width) << student.name
		<< '|' << setw(width) << student.major1 << '|' << setw(width) << student.major2 << '|' << setw(width) << student.advisor
		<< '|' << endl;
}

//---------------------------------------------------------------
void displayHead()
{
	system("cls");
	TopLine();
	cout << '|' << "   ���ڵ��ȣ  " << '|' << "     �� ��     " << '|' << "     �� ��     " <<
		'|' << "     �� ��     " << '|' << "    �� �� ��   " << '|' << "    ��������   " << '|' << endl;
	TitleLine();
}

//--------------------------------------------------------------------
// ���� �� ��ǥ�� ������ �Ʒ� ��ǥ�� �̿��Ͽ� BOX ǥ��
//
void displayBox(int left, int top, int right, int bottom)
{
	int col, row;   // ��, �� loop counters
	int width = 2;

	// ��
	gotoXY(left, top); cout << '+';
	for (col = left + 1; col < right; col++)
		cout << '-';
	cout << '+';

	// ��, ��
	for (row = top + 1; row < bottom; row++)
	{
		gotoXY(left, row);
		cout << '|';
		gotoXY(right, row);
		cout << '|';
	}

	// �Ʒ�
	gotoXY(left, bottom);
	cout << '+';
	for (col = left + 1; col < right; col++)
		cout << '-';
	cout << '+';
}

void bstree::inOrder()
{
	inOrderSub(root);
}//���ο� �ִ� ��� �����͸� ������������ ť�� �����ϴ� �Լ�

//----------------------------------------------------------------
void bstree::inOrderSub(bnodePtr p)
{
	if (p)//p�� �����Ѵٸ�
	{
		inOrderSub(p->left); //L
		Q.enqueue(p->data);// ���̳ʸ� ��ġƮ���� ť�� ����Ÿ Ÿ���� ��ġ�ؾ��� => ť�� ���̳ʸ� ��ġƮ�� D
		inOrderSub(p->right);//R
	}
}//���ο� �ִ� ��� �����͸� ������������ ť�� �����ϴ� �Լ�

void bstree::partialInOrder(bstreeKey SrchKey)
{
	partialInOrderSub(root, SrchKey);
} //���ڵ带 ã�� ���� ����ϴ� �Լ�

//----------------------------------------------------------------
void bstree::partialInOrderSub(bnodePtr p, bstreeKey SrchKey)
{
	/*strstr�ؼ� ���ǿ� �´°͸� enqueue*/
	if (p)
	{//LDR
		partialInOrderSub(p->left, SrchKey);//L
		// <���� �������� ���� > Q.enqueue(p->data);
		if (strstr(p->data.key, SrchKey)) //���ڿ��ӿ��� ���ڿ� ã�� (p�� �����;ȿ� srchkey�� �ִٸ�)
			Q.enqueue(p->data); //D
		partialInOrderSub(p->right, SrchKey);//
	}
}//partinorder�� ���ϴ� ���ڵ常 ������������ Ʈ���� �ֱ�(s��ɾ�� ���Ǵµ� SrchKey�� �����ϴ� �����͵��� Q�� ������������ �ִ� �Լ�)
void bstree::buildIndex()
{
	studentRec student; 
	int recNo, numRec;
	bstreeData indexEntry;

	studentDB.seekg(0, ios::beg); //������ ��ġ ����Ŵ
	studentDB.read((char*)(&numRec), sizeof(numRec));//numRec�о����

	for (recNo = 0; recNo < numRec; recNo++)  //recNo�� for������ ��� ���鼭
	{
		studentDB.seekg(recNo * sizeof(student) + sizeof(int), ios::beg);   //ã��
		studentDB.read((char*)(&student), sizeof(student));   //�������Ͽ��� ���� �о�´�
		strcpy_s(indexEntry.key, student.studentID);          //student��ü�� id�κ��� indexEntry�� key���� �������ش�(strcpy_s�Լ��̿�)
		indexEntry.recNum = recNo;                            //indexEntry�� recNum�κп� recNo�� �������ش�
		insert(indexEntry);  //insert�Լ��� �̿��ؼ� bstree�� �ϳ��� �־��ش�
	}
}//�������Ͽ��� �о bstree����� �Լ�

void dumpDB()
{
	studentRec student;
	int        recNo = 0, numRec, lineNo = 1; //ó������ recNo�� �о�;��ؼ� recNo�� �ʱ�ȭ���� 0�̿����Ѵ�.

	studentDB.seekg(0, ios::beg); //������ ��ġ
	studentDB.read((char*)(&numRec), sizeof(numRec)); //numRec�� �о�´�.

	for (recNo; recNo < numRec; lineNo++) //for������ �ѹ������� lineNo�߰��Ǿ���Ѵ�. (�ڿ��� recNo�� ++�������)
	{
		studentDB.seekg(recNo * sizeof(student) + sizeof(int), ios::beg); // ã��
		studentDB.read((char*)(&student), sizeof(student));   //�������� �о��
		if (lineNo % 17 == 1) //�������� 1�ΰ�� (�׷����� �������������� ���ʹ����� �� displayHead�� ��µ� �� ����)
			displayHead(); //���ڵ��ȣ|�й�|���� ���� ����ϴ� �Լ�
		else if (lineNo % 17 == 0) //�������� 0�ΰ�� => ���ڵ� ��ȣ�� ������ ���� (���ʹ����� ����)
		{
			BottomLine(); //������ ���� �׷��ش�
			cout << "������ ���ڵ带 ������ ����Ű�� ��������.";
			getchar(); //����ڷκ��� ����Ű�� �޴´�.
		}
		else  
		{
			displayRecord(student, recNo); //displayRecord�� ���ؼ� ���ڵ� �����ֵ��� �� 
			recNo++; 
		}
	}
	BottomLine(); //�� �о �������� �������� bottomLIne�� �׷��ֱ�


}//�������Ͽ� �ִ� ������ ���ڵ��������� �����ְ���

void bstree::findKey(bstreeKey SID)
{
	studentRec student;
	bstreeData indexEntry;

	if (find(SID) == 0) //������ �й��� ã�� ���� ���
		cout << "ID�� ��ġ�ϴ� ���ڵ尡 �����ϴ�." << endl;
	else //ã�����
	{
		indexEntry = getData(); //����Ʈ���� getData�� �̿��ؼ� indexEntry�� ������ �������� 
		studentDB.seekg(indexEntry.recNum * sizeof(student) + sizeof(int), ios::beg); // ã��
		studentDB.read((char*)(&student), sizeof(student)); //�������Ͽ��� ã�� ������ �о��
		//ã�� ���� ������ 
		displayHead(); 
		displayRecord(student, indexEntry.recNum);  
		BottomLine();
	}
}//�й��� ã�� �Լ�

void bstree::displayAll()
{
	inOrder();  // ���ο� �ִ� ��絥��Ÿ�� ������������ ť�� ����)
	Q.displayQueue(); 
}//�й������� ������������ �����ִ� �Լ�

void bstree::search(bstreeKey SID)
{
	partialInOrder(SID); //s��ɾ�� ���Ǵµ� SrchKey�� �����ϴ� �����͵��� Q�� ������������ �ִ� �Լ�
	Q.displayQueue();
} //s2019��� ġ�� �й��� 2019�����Ե� �ֵ��� �̾Ƴ�

void queue::displayQueue()
{
	studentRec student;
	int lineNo = 1, recNo, numRec;
	bstreeData indexEntry;

	studentDB.seekg(0, ios::beg); 
	studentDB.read((char*)(&numRec), sizeof(numRec)); //numRec�� �о�´�.

	while (!isEmpty()) //Q�� ������� ���������� �ݺ�
	{

		indexEntry = dequeue(); //indexEntey�� queue�� �ִ� �����͸� ������
		studentDB.seekg(indexEntry.recNum * sizeof(student) + sizeof(int), ios::beg); //ã��
		studentDB.read((char*)(&student), sizeof(student)); //���������� �о��

		recNo = indexEntry.recNum; //�ڿ� displayRecord���� �Ű������� recNo�� �޾ƾ��ϱ� ���� 

		if (lineNo == 1) //lineNo�� 1�ΰ�쿡��
		{
			displayHead(); //displayHead����ϰ�
			lineNo++;  //���γѹ� ++������Ѵ�
		}
		else if (lineNo % 17 == 0) //++�Ǵ� lineNo�� 17�� �������� �� �������� 0�̵Ǵ°�� 
		{ 
			BottomLine(); //bottomline���
			cout << "������ ���ڵ带 ������ ����Ű�� ��������.";
			getchar(); //����ڷκ��� ����Ű �޾ƿ´�
			displayHead(); //���� ģ�� ���� ȭ�鿡 display record��� ��
			lineNo++;  
		}
		//���ڵ� ���
		displayRecord(student, recNo); 
		lineNo++;
	}
	BottomLine(); //�� ������ �������� bottomLine�׷�����Ѵ�.
} //ť���ִ� ���� ���ڵ�� �����ְ� �ϴ� �Լ�

bool matchRecord(studentRec studentImage, studentRec student)
{
	
	//�������񺰷� �̸����� ������������ ���� ���ؾ��� 
	if ((strcmp(studentImage.major1, student.major1) == 0 ||
		strcmp(studentImage.major1, "") == 0) && //����ڰ� ���͸� ġ�� �Ѿ�� ���
		(strcmp(studentImage.major2, student.major2) == 0 || //����ڰ� �Է������� ���� ���ؾ��Ѵ�.
			strcmp(studentImage.major2, "") == 0) && 
			(strcmp(studentImage.name, student.name) == 0 ||
				strcmp(studentImage.name, "") == 0) &&
				(strcmp(studentImage.advisor, student.advisor) == 0 ||
					strcmp(studentImage.advisor, "") == 0))
		return true; //�����ϸ� Ʈ�� ��ȯ
	else
		return false; //�ƴϸ� false

	//������(����2, ��, �������� ��� �� ����)

}//����ڰ� �Է��ѰͰ� ���� ���ڵ忡 �ִ°� ���ϴ� �Լ�

void bstree::match()
{
	studentRec student, studentImage;
	int recNo;
	bstreeData indexEntry;
	queue matchQ;

	inOrder(); //���ο� �ִ� ��� �����͸� ������������ ť�� ����

	studentImage = inputRecord(0);// ����ڷ� ���� �����޾Ƴֱ�

	while (!Q.isEmpty()) //Q�� empty�� �ƴ϶��
	{
		indexEntry = Q.dequeue();//queue�κ��� dequeue�ؼ� indexEntry�� ����
		studentDB.seekg(indexEntry.recNum * sizeof(student) + sizeof(int), ios::beg);//ã��
		studentDB.read((char*)(&student), sizeof(student)); //�о�´�

		if (matchRecord(studentImage, student)) //matchrecord�� �����Ѵٸ�
		{
			matchQ.enqueue(indexEntry); //matchQ�� �־��ش�
		}
	}
	if (matchQ.isEmpty()) { //��ġ�ϴ� ���ڵ尡 �������
		displayMessage("��ġ�ϴ� ���ڵ尡 �������� �ʽ��ϴ�.\n");
	}
	else
	{
		matchQ.displayQueue(); //��ġ�� ���� ���ڵ�� ������!
	}

}//����ڰ� �Է��� �Ͱ� �������ڵ忡 ���� ���� ������ �й������� ������������ �����

//---------------------------------------------------------------
// ���ڵ� ���� �ܼ� �Է�
// InsertRecord() ȣ��
//---------------------------------------------------------------
studentRec inputRecord(int IncludeKey)
{
	// includeKey : Input ��� 1, Match ��� 0
	studentRec student;
	student.deleted = 0;
	system("cls");
	displayBox(20, 8, 60, 16);
	if (IncludeKey == 1)
	{
		gotoXY(25, 10); cout << " �й�    :";
		cin.getline(student.studentID, FieldWidth - 2, '\n');
	}
	gotoXY(25, 11); cout << " ����    :";
	cin.getline(student.name, FieldWidth, '\n');
	gotoXY(25, 12); cout << " ����    :";
	cin.getline(student.major1, FieldWidth, '\n');
	gotoXY(25, 13); cout << " ������  :";
	cin.getline(student.major2, FieldWidth, '\n');
	gotoXY(25, 14); cout << " ��������:";
	cin.getline(student.advisor, FieldWidth, '\n');

	return student;
}


void bstree::insertRecord()
{
	studentRec student;
	bstreeData indexEntry;
	int numRec;


	student = inputRecord(1); //����ڷκ��� �Է¹ޱ�
	if (find(student.studentID) == 1) //�̹� �����ϴ� ID���
		displayMessage("ID�� �̹� �����մϴ�.\n");
	else //�����ϴ� ID�� �ƴ϶��
	{
		student.deleted = 0;  //deleted������ 0���� ���� �������ͷ� ������ش�

		studentDB.seekg(0, ios::beg); 
		studentDB.read((char*)(&numRec), sizeof(numRec)); //numRec�� �о�´�

		studentDB.seekp((numRec) * sizeof(student) + sizeof(int), ios::beg); //����� ��ġ ã��
		studentDB.write((char*)(&student), sizeof(student)); //���ڵ忡 ������ �߰����ش�

		//Ʈ���� �־�����ϱ⶧���� �̷��� �ؾ���
		strcpy_s(indexEntry.key, student.studentID);//strcpy_s�Լ��� �̿��ؼ� studentID�� inDexEntry.StudentID�� �������ش�.
		indexEntry.recNum = numRec; //numRec���� �о�� ���� indexrecNUm�� �־���

		insert(indexEntry); //Ʈ���� IndexEntry�� �־���
		numRec++; //�߰��� ���ڵ� ���� ������Ʈ
		studentDB.seekp(0, ios::beg); //DB�� ���� ������ �̵���
		studentDB.write((char*)(&numRec), sizeof(numRec)); //���ڵ� ������ ������Ʈ ���ش�
		displayMessage("�߰��Ǿ����ϴ�.\n");
	}
} //����ڷκ��� ���ο� ���� �޾ƿͼ� �߰����ִ� �Լ� (���ڵ� �Ӹ� �ƴ϶� bstree�� �ε����� �߰�������Ѵ�)

void bstree::deleteRecord(bstreeKey SID)
{

	studentRec student;
	int recNo;
	bstreeData indexEntry;
	char ch;

	if (find(SID) == 0)//�����ϰ��� �ϴ� ���ڵ带 ã�����ϴ� ���
	{
		displayMessage("ID�� ��ġ�ϴ� ���ڵ尡 �������� �ʽ��ϴ�.\n");
	}
	else //�����ϰ� �ϰ��� �ϴ� ���ڵ尡 �����ϴ� ���
	{
		indexEntry = getData(); //getData�� �̿��ؼ� ã�� ���� indexEntry�� ������

		studentDB.seekg(indexEntry.recNum * sizeof(student) + sizeof(int), ios::beg);  //ã��
		studentDB.read((char*)(&student), sizeof(student)); //������ ������ ã�Ƽ� �о�� 
		//�����ϰ��� �ϴ� ���ڵ带 ������
		displayHead(); 
		displayRecord(student, indexEntry.recNum);
		BottomLine();
		cout << "������ ���� �ϰڽ��ϱ� (Y/N)" << endl;
		cin >> ch; //����ڷκ��� �Է¹ޱ�
		if (ch == 'Y' || ch == 'y') 
		{
			displayMessage("�����Ǿ����ϴ�.\n"); 
			student.deleted = 1; //����ڰ� y�� ���� ��� -> deleted�� 1�� �ٲ���!
			studentDB.seekp(indexEntry.recNum * sizeof(student) + sizeof(int), ios::beg);
			studentDB.write((char*)(&student), sizeof(student));
			remove(SID);//bstree������ remove�̿��ؼ� ���� 
		}
	}


} //�����ϱ� ���ϴ� �������� deleted���� 1�� �ٲ��ִ� �Լ� (���� ���ڵ� ������ �ƴ� => ex .-2018111237 �ϰ� y������ d������ ���ڵ忡 ���� ����!) 

void compress()
{

	int current, nextFree, numRec, eofMark = EOF;
	studentRec student;
	current = 0;  //current �ʱ�ȭ����
	nextFree = 0; //nextFree �ʱ�ȭ ����
	studentDB.seekg(0, ios::beg);
	studentDB.read((char*)(&numRec), sizeof(numRec)); //numRec�о�� 
	
	while (current != numRec) {  //delete�� y���
		studentDB.seekg(current * sizeof(student) + sizeof(int), ios::beg); //ã��
		studentDB.read((char*)(&student), sizeof(student)); //�о�´� 


		if (student.deleted == 0) {  //������ ���ڵ尡 �ƴѰ��
			studentDB.seekp(nextFree * sizeof(student) + sizeof(int), ios::beg);
			studentDB.write((char*)(&student), sizeof(student));
			nextFree++;         //current�� ���� ���Ƽ� �������� numRec�� �ֱ� ����
		}
		else
			nextFree = current;//i�� ���� ���Ƽ� �������� numRec�� �ֱ� ����

		current++; //studentDB�� recNo���� �ϳ��� ������Ų��.
	}




	// �������� EOF �߰�
	studentDB.seekp((nextFree + 1) * sizeof(student) + sizeof(int), ios::beg);   
	studentDB.write((char*)(&eofMark), sizeof(int));        



	// ���� �տ� ���ڵ� ���� ����
	studentDB.seekp(0, ios::beg);             
	studentDB.write((char*)(&nextFree), sizeof(nextFree));


} //deleted�� 0�ξֵ鸸 ���ڵ�� �����ִ� �Լ�

//---------------------------------------------------------------
void menu()
{
	system("cls");
	cout << "\n\n";
	cout << '+'; BarField(); BarField(); BarField(); cout << '+' << endl;
	cout << '|' << "   ��ɾ�:                                   " << '|' << endl;
	cout << '|' << "                                             " << '|' << endl;
	cout << '|' << "     F ID        : id�� �˻�                 " << '|' << endl;
	cout << '|' << "     S PartialID : �κ� id�� �˻�            " << '|' << endl;
	cout << '|' << "     - ID        : ����                      " << '|' << endl;
	cout << '|' << "     +           : �߰�                      " << '|' << endl;
	cout << '|' << "     A           : id������ ��ü ���̱�      " << '|' << endl;
	cout << '|' << "     M           : Match(QBE)                " << '|' << endl;
	cout << '|' << "     Q           : ���� & ����               " << '|' << endl;
	cout << '|' << "     D           : DumpDB                    " << '|' << endl;
	cout << '+'; BarField(); BarField(); BarField(); cout << '+' << endl;
}


//---------------------------------------------------------------
int main()
{
	char cmd;
	bstreeKey SID;

	system("color 1f");                      // �Ķ� ����, ��۾�
	system("cls");

	studentDB.open("studentDB.mp1", ios::in | ios::out | ios::binary); // DB file, binary
	index.buildIndex();

	while (1)
	{
		gotoXY(1, 22);
		cout << "��ɾ�: ";
		cin >> cmd;
		if ((toupper(cmd) == 'F') || (cmd == '-') || (toupper(cmd) == 'S'))
			cin >> SID;
		cin.ignore(80, '\n');

		if (toupper(cmd) == 'A')
			index.displayAll();
		else if (toupper(cmd) == 'F')
			index.findKey(SID);
		else if (toupper(cmd) == '+')
			index.insertRecord();
		else if (toupper(cmd) == '-')
			index.deleteRecord(SID);
		else if (toupper(cmd) == 'S')
			index.search(SID);
		else if (toupper(cmd) == 'M')
			index.match();
		else if (toupper(cmd) == 'Q')
		{
			compress();
			exit(1);
		}
		else if (toupper(cmd) == 'D')
			dumpDB();
		else if (cmd == '?')
			menu();
		else
			displayMessage("�߸��� ��ɾ��Դϴ�.");
	} // while
	return 0;
}