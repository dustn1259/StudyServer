//
// 프로그램 설명: 학생DB에 대한 레코드 프로그램 => 사용자가 학생 DB를 사용하기 쉽게 할 수 있도록 만듦
// 학번 : 2018111237
// 이름 : 권연수
// 분반 : 목요일 3-4교시
// 날짜 : 2019.11.21.목요일

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

const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // 핸들값

COORD  Pos;                                              // 위치

//-----------------------------------------------------
//  화면상의 (x, y) 좌표 위치로 커서 이동
//
void gotoXY(int x, int y)
{
	Pos.X = x - 1;
	Pos.Y = y - 1;
	SetConsoleCursorPosition(hConsole, Pos);
}

//-----------------------------------------------------
// 글자색 변경
//
void textColor(int color_number)
{
	SetConsoleTextAttribute(hConsole, color_number);
}

//-----------------------------------------------------
// 화면을 지운다
//
void clrScr()
{
	system("cls");
}

//--------------------< sleep >----------------------
// (주어진 시간)/1000 초 동안 실행을 멈춘다.
//
void sleep(clock_t wait)
{
	clock_t goal;
	goal = wait + clock();
	while (goal > clock())
		;
}

// studentRec 형 선언
#define FieldWidth 20
typedef char char20[FieldWidth];
typedef struct
{
	int      deleted;
	char20  studentID, name, major1, major2, advisor;
} studentRec;

// bstreeData와 bstreeKey 형 선언
#define bstreeKey char20
typedef struct
{
	bstreeKey key;
	int       recNum;
} bstreeData;

// queueData 형 선언
#define queueData bstreeData

#include "mp1bstree.h"
#include "mp1queue.h"

#define recordsPerScreen 15

fstream studentDB;            // Global variable, DB
queue Q;                  // Global variable, queue
bstree index;               // Global variable, 색인

#include "mp1bstree.cpp"
#include "mp1queue.cpp"

// 표 그리기를 위한 특수문자 
// ━ ┃ ┏ ┓ ┗ ┛┣ ┫ ┻ ┳ ╋   

char LT[] = "┏", LB[] = "┗", RT[] = "┓", RB[] = "┛", V[] = "┃", H[] = "━";
char TU[] = "┻", TD[] = "┳", TL[] = "┫", TR[] = "┣", CR[] = "╋";

//표 그리기 위한 함수
void BarField();
void TopLine();
void TitleLine();
void DataLine();
void BottomLine();

void displayHead();
void displayRecord(studentRec, int);
void displayBox(int left, int top, int right, int bottom);

// MP1 함수
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
// ── 출력
//
void BarField()
{
	for (int i = 0; i < 15; i++)
		cout << '-';
}

//---------------------------------------------------------------
// ┌──┬──┬──┬──┬──┐ 출력
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
// ├──┼──┼──┼──┼──┤ 출력
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
// └──┴──┴──┴──┴──┘ 출력
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
	cout << '|' << "   레코드번호  " << '|' << "     학 번     " << '|' << "     성 명     " <<
		'|' << "     전 공     " << '|' << "    부 전 공   " << '|' << "    지도교수   " << '|' << endl;
	TitleLine();
}

//--------------------------------------------------------------------
// 왼쪽 위 좌표와 오른쪽 아래 좌표를 이용하여 BOX 표시
//
void displayBox(int left, int top, int right, int bottom)
{
	int col, row;   // 행, 열 loop counters
	int width = 2;

	// 위
	gotoXY(left, top); cout << '+';
	for (col = left + 1; col < right; col++)
		cout << '-';
	cout << '+';

	// 좌, 우
	for (row = top + 1; row < bottom; row++)
	{
		gotoXY(left, row);
		cout << '|';
		gotoXY(right, row);
		cout << '|';
	}

	// 아래
	gotoXY(left, bottom);
	cout << '+';
	for (col = left + 1; col < right; col++)
		cout << '-';
	cout << '+';
}

void bstree::inOrder()
{
	inOrderSub(root);
}//색인에 있는 모든 데이터를 오름차순으로 큐에 저장하는 함수

//----------------------------------------------------------------
void bstree::inOrderSub(bnodePtr p)
{
	if (p)//p가 존재한다면
	{
		inOrderSub(p->left); //L
		Q.enqueue(p->data);// 바이너리 서치트리와 큐의 데이타 타입이 일치해야함 => 큐도 바이너리 서치트리 D
		inOrderSub(p->right);//R
	}
}//색인에 있는 모든 데이터를 오름차순으로 큐에 저장하는 함수

void bstree::partialInOrder(bstreeKey SrchKey)
{
	partialInOrderSub(root, SrchKey);
} //레코드를 찾기 위해 사용하는 함수

//----------------------------------------------------------------
void bstree::partialInOrderSub(bnodePtr p, bstreeKey SrchKey)
{
	/*strstr해서 조건에 맞는것만 enqueue*/
	if (p)
	{//LDR
		partialInOrderSub(p->left, SrchKey);//L
		// <위의 조건으로 수정 > Q.enqueue(p->data);
		if (strstr(p->data.key, SrchKey)) //문자열속에서 문자열 찾기 (p의 데이터안에 srchkey가 있다면)
			Q.enqueue(p->data); //D
		partialInOrderSub(p->right, SrchKey);//
	}
}//partinorder은 원하는 레코드만 오름차순으로 트리에 넣기(s명령어에서 사용되는데 SrchKey로 시작하는 데이터들을 Q에 오름차순으로 넣는 함수)
void bstree::buildIndex()
{
	studentRec student; 
	int recNo, numRec;
	bstreeData indexEntry;

	studentDB.seekg(0, ios::beg); //가져올 위치 가리킴
	studentDB.read((char*)(&numRec), sizeof(numRec));//numRec읽어오기

	for (recNo = 0; recNo < numRec; recNo++)  //recNo가 for루프로 계속 돌면서
	{
		studentDB.seekg(recNo * sizeof(student) + sizeof(int), ios::beg);   //찾기
		studentDB.read((char*)(&student), sizeof(student));   //이진파일에서 한줄 읽어온다
		strcpy_s(indexEntry.key, student.studentID);          //student객체의 id부분을 indexEntry의 key값에 복사해준다(strcpy_s함수이용)
		indexEntry.recNum = recNo;                            //indexEntry의 recNum부분에 recNo를 복사해준다
		insert(indexEntry);  //insert함수를 이용해서 bstree에 하나씩 넣어준다
	}
}//이진파일에서 읽어서 bstree만드는 함수

void dumpDB()
{
	studentRec student;
	int        recNo = 0, numRec, lineNo = 1; //처음부터 recNo가 읽어와야해서 recNo의 초기화값은 0이여야한다.

	studentDB.seekg(0, ios::beg); //가져올 위치
	studentDB.read((char*)(&numRec), sizeof(numRec)); //numRec을 읽어온다.

	for (recNo; recNo < numRec; lineNo++) //for루프가 한바퀴돌면 lineNo추가되어야한다. (뒤에서 recNo도 ++해줘야함)
	{
		studentDB.seekg(recNo * sizeof(student) + sizeof(int), ios::beg); // 찾기
		studentDB.read((char*)(&student), sizeof(student));   //이진파일 읽어옴
		if (lineNo % 17 == 1) //나머지가 1인경우 (그래야지 다음페이지에서 엔터누르면 또 displayHead가 출력될 수 있음)
			displayHead(); //레코드번호|학번|성명 등을 출력하는 함수
		else if (lineNo % 17 == 0) //나머지가 0인경우 => 레코드 번호가 닫히는 지점 (엔터누르기 직전)
		{
			BottomLine(); //마지막 줄을 그려준다
			cout << "나머지 레코드를 보려면 엔터키를 누르세요.";
			getchar(); //사용자로부터 엔터키를 받는다.
		}
		else  
		{
			displayRecord(student, recNo); //displayRecord를 통해서 레코드 보여주도록 함 
			recNo++; 
		}
	}
	BottomLine(); //다 읽어서 가져오면 마지막에 bottomLIne또 그려주기


}//이진파일에 있는 내용을 레코드형식으로 보여주게함

void bstree::findKey(bstreeKey SID)
{
	studentRec student;
	bstreeData indexEntry;

	if (find(SID) == 0) //동일한 학번을 찾지 못한 경우
		cout << "ID와 일치하는 레코드가 없습니다." << endl;
	else //찾은경우
	{
		indexEntry = getData(); //이진트리의 getData를 이용해서 indexEntry에 데이터 가져오기 
		studentDB.seekg(indexEntry.recNum * sizeof(student) + sizeof(int), ios::beg); // 찾기
		studentDB.read((char*)(&student), sizeof(student)); //이진파일에서 찾은 데이터 읽어옴
		//찾은 것을 보여줌 
		displayHead(); 
		displayRecord(student, indexEntry.recNum);  
		BottomLine();
	}
}//학번을 찾는 함수

void bstree::displayAll()
{
	inOrder();  // 색인에 있는 모든데이타를 오름차순으로 큐에 저장)
	Q.displayQueue(); 
}//학번에따라 오름차순으로 보여주는 함수

void bstree::search(bstreeKey SID)
{
	partialInOrder(SID); //s명령어에서 사용되는데 SrchKey로 시작하는 데이터들을 Q에 오름차순으로 넣는 함수
	Q.displayQueue();
} //s2019라고 치면 학번에 2019가포함된 애들을 뽑아냄

void queue::displayQueue()
{
	studentRec student;
	int lineNo = 1, recNo, numRec;
	bstreeData indexEntry;

	studentDB.seekg(0, ios::beg); 
	studentDB.read((char*)(&numRec), sizeof(numRec)); //numRec을 읽어온다.

	while (!isEmpty()) //Q가 비어있지 않을때까지 반복
	{

		indexEntry = dequeue(); //indexEntey에 queue에 있는 데이터를 가져옴
		studentDB.seekg(indexEntry.recNum * sizeof(student) + sizeof(int), ios::beg); //찾기
		studentDB.read((char*)(&student), sizeof(student)); //이진데이터 읽어옴

		recNo = indexEntry.recNum; //뒤에 displayRecord에서 매개변수인 recNo를 받아야하기 때문 

		if (lineNo == 1) //lineNo가 1인경우에는
		{
			displayHead(); //displayHead출력하고
			lineNo++;  //라인넘버 ++해줘야한다
		}
		else if (lineNo % 17 == 0) //++되던 lineNo를 17로 나누었을 때 나머지가 0이되는경우 
		{ 
			BottomLine(); //bottomline출력
			cout << "나머지 레코드를 보려면 엔터키를 누르세요.";
			getchar(); //사용자로부터 엔터키 받아온다
			displayHead(); //엔터 친후 다음 화면에 display record찍게 함
			lineNo++;  
		}
		//레코드 찍기
		displayRecord(student, recNo); 
		lineNo++;
	}
	BottomLine(); //다 끝나면 마지막에 bottomLine그려줘야한다.
} //큐에있는 것을 레코드로 보여주게 하는 함수

bool matchRecord(studentRec studentImage, studentRec student)
{
	
	//전공과목별로 이름별로 전공교수별로 각각 비교해야함 
	if ((strcmp(studentImage.major1, student.major1) == 0 ||
		strcmp(studentImage.major1, "") == 0) && //사용자가 엔터만 치고 넘어가는 경우
		(strcmp(studentImage.major2, student.major2) == 0 || //사용자가 입력했을때 서로 비교해야한다.
			strcmp(studentImage.major2, "") == 0) && 
			(strcmp(studentImage.name, student.name) == 0 ||
				strcmp(studentImage.name, "") == 0) &&
				(strcmp(studentImage.advisor, student.advisor) == 0 ||
					strcmp(studentImage.advisor, "") == 0))
		return true; //성립하면 트루 반환
	else
		return false; //아니면 false

	//전공비교(전공2, 과, 어드바이저 등등 비교 복붙)

}//사용자가 입력한것과 기존 레코드에 있는것 비교하는 함수

void bstree::match()
{
	studentRec student, studentImage;
	int recNo;
	bstreeData indexEntry;
	queue matchQ;

	inOrder(); //색인에 있는 모든 데이터를 오름차순으로 큐에 저장

	studentImage = inputRecord(0);// 사용자로 부터 정보받아넣기

	while (!Q.isEmpty()) //Q가 empty가 아니라면
	{
		indexEntry = Q.dequeue();//queue로부터 dequeue해서 indexEntry에 저장
		studentDB.seekg(indexEntry.recNum * sizeof(student) + sizeof(int), ios::beg);//찾기
		studentDB.read((char*)(&student), sizeof(student)); //읽어온다

		if (matchRecord(studentImage, student)) //matchrecord가 성립한다면
		{
			matchQ.enqueue(indexEntry); //matchQ에 넣어준다
		}
	}
	if (matchQ.isEmpty()) { //일치하는 레코드가 없을경우
		displayMessage("일치하는 레코드가 존재하지 않습니다.\n");
	}
	else
	{
		matchQ.displayQueue(); //일치한 내용 레코드로 보여줌!
	}

}//사용자가 입력한 것과 기존레코드에 같은 것이 있으면 학번순으로 오름차순으로 출력함

//---------------------------------------------------------------
// 레코드 정보 콘솔 입력
// InsertRecord() 호출
//---------------------------------------------------------------
studentRec inputRecord(int IncludeKey)
{
	// includeKey : Input 경우 1, Match 경우 0
	studentRec student;
	student.deleted = 0;
	system("cls");
	displayBox(20, 8, 60, 16);
	if (IncludeKey == 1)
	{
		gotoXY(25, 10); cout << " 학번    :";
		cin.getline(student.studentID, FieldWidth - 2, '\n');
	}
	gotoXY(25, 11); cout << " 성명    :";
	cin.getline(student.name, FieldWidth, '\n');
	gotoXY(25, 12); cout << " 전공    :";
	cin.getline(student.major1, FieldWidth, '\n');
	gotoXY(25, 13); cout << " 부전공  :";
	cin.getline(student.major2, FieldWidth, '\n');
	gotoXY(25, 14); cout << " 지도교수:";
	cin.getline(student.advisor, FieldWidth, '\n');

	return student;
}


void bstree::insertRecord()
{
	studentRec student;
	bstreeData indexEntry;
	int numRec;


	student = inputRecord(1); //사용자로부터 입력받기
	if (find(student.studentID) == 1) //이미 존재하는 ID라면
		displayMessage("ID가 이미 존재합니다.\n");
	else //존재하는 ID가 아니라면
	{
		student.deleted = 0;  //deleted변수를 0으로 만들어서 정상데이터로 만들어준다

		studentDB.seekg(0, ios::beg); 
		studentDB.read((char*)(&numRec), sizeof(numRec)); //numRec을 읽어온다

		studentDB.seekp((numRec) * sizeof(student) + sizeof(int), ios::beg); //써야할 위치 찾기
		studentDB.write((char*)(&student), sizeof(student)); //레코드에 정보를 추가해준다

		//트리에 넣어줘야하기때문에 이렇게 해야함
		strcpy_s(indexEntry.key, student.studentID);//strcpy_s함수를 이용해서 studentID를 inDexEntry.StudentID에 복사해준다.
		indexEntry.recNum = numRec; //numRec으로 읽어온 수를 indexrecNUm에 넣어줌

		insert(indexEntry); //트리에 IndexEntry를 넣어줌
		numRec++; //추가된 레코드 개수 업데이트
		studentDB.seekp(0, ios::beg); //DB의 제일 앞으로 이동함
		studentDB.write((char*)(&numRec), sizeof(numRec)); //레코드 개수를 업데이트 해준다
		displayMessage("추가되었습니다.\n");
	}
} //사용자로부터 새로운 정보 받아와서 추가해주는 함수 (레코드 뿐만 아니라 bstree에 인덱스도 추가해줘야한다)

void bstree::deleteRecord(bstreeKey SID)
{

	studentRec student;
	int recNo;
	bstreeData indexEntry;
	char ch;

	if (find(SID) == 0)//삭제하고자 하는 레코드를 찾지못하는 경우
	{
		displayMessage("ID와 일치하는 레코드가 존재하지 않습니다.\n");
	}
	else //삭제하고 하고자 하는 레코드가 존재하는 경우
	{
		indexEntry = getData(); //getData를 이용해서 찾은 정보 indexEntry로 가져옴

		studentDB.seekg(indexEntry.recNum * sizeof(student) + sizeof(int), ios::beg);  //찾기
		studentDB.read((char*)(&student), sizeof(student)); //삭제할 데이터 찾아서 읽어옴 
		//삭제하고자 하는 레코드를 보여줌
		displayHead(); 
		displayRecord(student, indexEntry.recNum);
		BottomLine();
		cout << "정말로 삭제 하겠습니까 (Y/N)" << endl;
		cin >> ch; //사용자로부터 입력받기
		if (ch == 'Y' || ch == 'y') 
		{
			displayMessage("삭제되었습니다.\n"); 
			student.deleted = 1; //사용자가 y를 누를 경우 -> deleted를 1로 바꿔줌!
			studentDB.seekp(indexEntry.recNum * sizeof(student) + sizeof(int), ios::beg);
			studentDB.write((char*)(&student), sizeof(student));
			remove(SID);//bstree에서만 remove이용해서 삭제 
		}
	}


} //삭제하길 원하는 데이터의 deleted값을 1로 바꿔주는 함수 (실제 레코드 삭제는 아님 => ex .-2018111237 하고 y누른뒤 d눌러도 레코드에 정보 나옴!) 

void compress()
{

	int current, nextFree, numRec, eofMark = EOF;
	studentRec student;
	current = 0;  //current 초기화해줌
	nextFree = 0; //nextFree 초기화 해줌
	studentDB.seekg(0, ios::beg);
	studentDB.read((char*)(&numRec), sizeof(numRec)); //numRec읽어옴 
	
	while (current != numRec) {  //delete가 y라면
		studentDB.seekg(current * sizeof(student) + sizeof(int), ios::beg); //찾기
		studentDB.read((char*)(&student), sizeof(student)); //읽어온다 


		if (student.deleted == 0) {  //삭제할 레코드가 아닌경우
			studentDB.seekp(nextFree * sizeof(student) + sizeof(int), ios::beg);
			studentDB.write((char*)(&student), sizeof(student));
			nextFree++;         //current랑 같이 돌아서 마지막에 numRec에 넣기 위함
		}
		else
			nextFree = current;//i랑 같이 돌아서 마지막에 numRec에 넣기 위함

		current++; //studentDB의 recNo값을 하나씩 증가시킨다.
	}




	// 마지막에 EOF 추가
	studentDB.seekp((nextFree + 1) * sizeof(student) + sizeof(int), ios::beg);   
	studentDB.write((char*)(&eofMark), sizeof(int));        



	// 제일 앞에 레코드 개수 쓰기
	studentDB.seekp(0, ios::beg);             
	studentDB.write((char*)(&nextFree), sizeof(nextFree));


} //deleted가 0인애들만 레코드로 보여주는 함수

//---------------------------------------------------------------
void menu()
{
	system("cls");
	cout << "\n\n";
	cout << '+'; BarField(); BarField(); BarField(); cout << '+' << endl;
	cout << '|' << "   명령어:                                   " << '|' << endl;
	cout << '|' << "                                             " << '|' << endl;
	cout << '|' << "     F ID        : id로 검색                 " << '|' << endl;
	cout << '|' << "     S PartialID : 부분 id로 검색            " << '|' << endl;
	cout << '|' << "     - ID        : 삭제                      " << '|' << endl;
	cout << '|' << "     +           : 추가                      " << '|' << endl;
	cout << '|' << "     A           : id순으로 전체 보이기      " << '|' << endl;
	cout << '|' << "     M           : Match(QBE)                " << '|' << endl;
	cout << '|' << "     Q           : 압축 & 종료               " << '|' << endl;
	cout << '|' << "     D           : DumpDB                    " << '|' << endl;
	cout << '+'; BarField(); BarField(); BarField(); cout << '+' << endl;
}


//---------------------------------------------------------------
int main()
{
	char cmd;
	bstreeKey SID;

	system("color 1f");                      // 파란 바탕, 흰글씨
	system("cls");

	studentDB.open("studentDB.mp1", ios::in | ios::out | ios::binary); // DB file, binary
	index.buildIndex();

	while (1)
	{
		gotoXY(1, 22);
		cout << "명령어: ";
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
			displayMessage("잘못된 명령어입니다.");
	} // while
	return 0;
}