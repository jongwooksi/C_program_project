#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"

#define KEY_ESC 27
#define KEY_ENTER '\r'
#define KEY_BS '\b'
#define KEY_TAP 9                                //(추가 문제) TAP키를 누르면 화면에 출력된 모든 단어가 사라지게 구현함.


#define MAX_WORD_LENGTH 24
#define MAX_SCREEN_WORD_COUNT 150

int g_word_count = 11;                          // 확장을 위해 변수로 선언
char *g_words[11] =
{
	"programming",
	"school",
	"student",
	"chair",
	"desk",
	"average",
	"screen",
	"mouse",
	"window",
	"door",
	"building"
};
int g_fail_count = 0;                           // 실패한 개수
int g_score = 0;                                // 점수
char g_input_word[MAX_WORD_LENGTH + 1];         // 입력 단어 저장
int g_input_word_length = 0;                    // 입력 단어 길이
int g_item_score = 3;                           // 시작 아이템 개수
int a = 0;                                      // stage에 대한 전역 변수
int g_stage_score = 0;                          // 함수 안에서의 stage 변수

struct ScreenWord
{
	int index;
	int x;
	int y;
};
typedef struct ScreenWord ScreenWord;
ScreenWord g_screen_word[MAX_SCREEN_WORD_COUNT];// 화면에 나타난 단어들(g_words의 원소 index 저장)
int g_screen_word_count = 0;                    // 화면에 나타난 단어 개수

clock_t g_move_start_time;                      // 단어 이동 기준 시각
double g_move_speed = 0.2;                      // 단어 이동 시간 간격(초 단위)
clock_t g_new_word_start_time;                  // 새 단어 출현 시각
double g_new_word_speed = 1.5;                  // 새 단어가 나타나는 시간 간격(초 단위)
int print = 0;                                  // 지우는 함수에서 끝날 때 까지 지우기위해 변수를 하나 더 선언한다.
int g_score_count=0;                            // 몇 개를 맞추었는지 알게하는 변수이다.
int g_stage_count = 0;                          // 함수안에서 stage 값을 올리는 변수이다.
int stop = 0;                                   // 일시정지에 활용하는 변수이며, 0일 때 움직인다는 것을 말한다.



void InitScreen(void);                          // 파일로부터 단어들 입력
void InitData(void);                            // 아직 내용 없음 (확장 가능)
void Run(void);                                 // 타자 게임 실행
void CompareWords(void);                        // 화면에 출력된 단어들 중에 입력 단어와 일치하는 단어가 있는지 검사하고 일치하는 단어가 있다면 삭제 및 점수 갱신
void ProcessInput(int key);                     // 키 입력이 있을 경우 현재 입력된 문자열에 합쳐 줌
void NormalMode(void);                          // 키 입력이 없을 경우 처리 내용
void DrawWord(int i);                           // 화면에 나타난 i번째 단어 출력
void EraseWord(int i);                          // 화면에 나타난 i번째 단어 지우기
void UpdateInputWord(void);                     // 입력된 단어를 화면에 출력 
void UpdateScore(void);                         // 점수 출력창에 점수 출력 
void UpdateFailCount(void);                     // 실패 횟수 출력
void UpdateWordScore(int i);                    // (추가함수) 단어 마다 다른 배점을 두는 함수
void UpdateItem(void);                          // (추가함수) 남은 아이템 개수 출력
void Stage(void);                               // (추가함수) Stage 출력



int main(void)
{
	srand(time(NULL));
	InitScreen();
	//InitData();
	Run();

	return 0;
}

void NormalMode(void)
{
	
	int len = strlen(g_words[g_screen_word[g_screen_word_count].index]);
	g_screen_word[g_screen_word_count].x = 60;                      // 출력되는 단어의 첫 등장 좌표를 63으로 고정시킨다.
	g_screen_word[g_screen_word_count].y = rand() % 18 + 2;         // 주어진 칸안에 랜덤으로 등장시키기 위해 사용한다.
	g_screen_word[g_screen_word_count].index = rand() % 11;   // 11개의 정의된 문자열배열에서 랜덤으로 등장시키기 위해 사용한다.

	if (g_stage_count == 10 * a) 
	{
		Stage();
		a++;
	} // 10개의 단어를 맞출 때 마다, stge가 올라가는 기능을 지닌 조건문이다.

	if (stop == 0)
	{
		if (GetDiffTime(g_move_start_time) > g_move_speed)      // 출력되는 문자가 g_move_speed 만큼의 시간이 지난다면
		{

			for (int i = print; i < g_screen_word_count; i++)
			{

				if (g_screen_word[i].index != 12)
				{
					DrawWord(i);
					EraseWord(i);
					g_screen_word[i].x--;
					DrawWord(i);

					// 문자를 출력후 왼쪽으로 이동시키기 위해 출력된 문자를 지운다음 x좌표를 하나 감소시켜 재출력시킴으로써
					// 문자가 움직이는 것 처럼 보이게 한다.


					if (g_screen_word[i].x == 3) // x좌표가 왼쪽에 가까워진 후 처리를 해주지않으면 오류가 생긴다. 이를위해 x좌표가 3이 될 때
					{                            // 아래와 같은 일 처리를 하게 정의하였다.
						g_fail_count++;          // g_fail_count 값을 1 증가시킨다.
						UpdateFailCount();       // 오른쪽 아래 fail 칸의 값에 1을 증가시켜 출력시킨다.
						print++;                 // 이 처리를 하지않으면 좌표가 3이 될 때 잠깐 깜빡거린 후 재출력되는데 이를 방지하기 위함이다.
						GotoXY(g_screen_word[i].x, g_screen_word[i].y); // 그 자리에 출력된 좌표로 이동시킨 다음
						EraseWord(i);// 그 배열을 지워버린다.
						g_screen_word[i].index = 12;
					}

				}
			}
			g_move_start_time = clock();

			// 다시 g_move_speed 만큼 이동시키기 위해 g_move_start_time을 이용하여 값을 초기화시킨다.
		}
		if (GetDiffTime(g_new_word_start_time) > g_new_word_speed) // 새로운 단어를 출력하기 위함이다.
		{

			if (g_move_speed <= 0.18)
				g_words[0] = "unfortunately";

			if (g_move_speed <= 0.16)
				g_words[1] = "discouragement";

			if (g_move_speed <= 0.14)
				g_words[2] = "congratulation";

			// 맞추는 개수가 많아져 g_new_word_speed가 줄어들 때 특정한 시간 이하가되면 조금 더 어려운 단어가 출력되는데,
			// 원래의 배열에있는 단어가 삭제되는 의미가 아니라 추가되는 의미이다.

			DrawWord(g_screen_word_count);
			g_screen_word_count++;
			g_new_word_start_time = clock();

			// DrawWord를 이용하여 새로운 단어를 출력한 후 g_screen_word_count값을 1 증가시킨 후
			// 다시 새로운 단어를 출력시키기 위하여 g_new_word_start_time을 이용해 초기화 시킨다. 
		}
	}
	

}
void EraseWord(int i)
{
	int len = strlen(g_words[g_screen_word[i].index]); // len 이라는 변수를 선언하여, 함수로 넘어온 문자열의 길이를 저장한다.

	GotoXY(g_screen_word[i].x, g_screen_word[i].y); // 그 해당하는 좌표로 이동한 다음

	for (int j = -1; j < len; j++)	
		printf(" ");	

	// 공백을 출력하여 지워진 것 처럼 보이게 만든다.
	
}

void Run(void)
{
	g_move_start_time = clock();            // 현재 시각(clock) 초기화
	g_new_word_start_time = clock();        // 현재 시각(clock) 초기화

	int key;
	int position = 30;



	while (g_fail_count < 5)                // 5회 실패 시 종료
	{
		
		GotoXY(30, 22);                    // 입력 칸으로 이동시키기 위함이다.


		if (_kbhit() != 0)                  // 키보드가 눌려진다면
		{
			key = _getch();                 // 그 값을 key에 저장한다.
			SetCursorVisible(1);

			if (stop == 0)
			{
				if (isalpha(key) != 0)          // key가 알파벳이라면
				{
					g_input_word[g_input_word_length] = key;     // g_input_word[g_input_word_length]에 key를 저장하고
					g_input_word[g_input_word_length + 1] = '\0'; // 그다음 배열에 널문자를 저장하고
					g_input_word_length++; // 입력된 문자열의 길이를 증가시킨다음
					position++; // 다음 좌표로 이동하여 다른 값을 받을 준비를 한다.
				}


				else if (key == '\r') // Enter 키가 눌려졌을 때에 대한 내용이다.
				{
					CompareWords(); // 입력 값과 화면에 출력되어있는 값을 비교하기 위해 compare함수를 이용한다.


					GotoXY(30, 22); //좌표를 이동한다.
					for (int i = 0; i < g_input_word_length; i++)
					{
						printf(" ");
						g_input_word[i] = NULL;
					}
					// Enter가 입력되었으면 입력칸에 입력한 단어들을 지워야 하는데, g_input_word_length만큼 for문을 사용하여
					// 공백을 출력하고 그 단어를 지워버리는 내용이다.

					g_input_word_length = 0; // 그 후 입력된 단어의 길이를 0으로 초기화 시킨후
					GotoXY(30, 22); //다시 좌표를 이동한다.
				}
				else if (key == '\b') // 벡스페이스가 눌러졌을 때에 대한 내용이다.
				{
					if (g_input_word_length > 0) // 이 조건문을 거는 이유는 입력된 단어가 없을 때 지워져 오류가 발생하는 걸 방지하기 위해서다.
					{
						g_input_word[g_input_word_length - 1] = '\0'; // 
						for (int i = 0; i < g_input_word_length; i++)
							printf(" ");
						g_input_word_length--;
						// 현재까지 입력한 단어의 앞자리에 널문자를 출력하여 지운 후 공백을 출력한 다음, g_input_word_length을 1 감소하여
						// 그 앞자리의 단어에 똑같은 처리를 하는 내용이다.
					}

					GotoXY(position, 22); // 다음 좌표로 이동하여 다른 값을 받을 준비를 한다.
				}

				else if (key == 27) // ESC 키가 눌려졌을 때에 대한 내용이다.
				{
					if (stop == 0) // 현재 움직이고 있다면
					{
						stop = 1;
						GotoXY(3, 22);
						printf("< 일시정지 > 입력불가능");

						// stop 값을 1로 바꾸어 움직이지 않다는 것을 알려주고, 좌표로 이동하여 일시정지라는 것을 알려준다.
					}
	
				}

				else if (key == 9) //(추가내용) Tap 키를 눌렀을 때에 대한 내용이다.
				{
					if (g_item_score > 0) // g_item_score가 1이상일 때만 사용가능하게 하는 조건문이다.(3개로 한정되어있다.)
					{

						for (int i = 0; i < g_screen_word_count; i++)
						{
							g_screen_word[i].index = 12;
							GotoXY(g_screen_word[i].x, g_screen_word[i].y);
							printf("              ");

							// 화면에 나타난 모든 단어를 일괄적으로 없애주는 기능이다. (점수는 없다)
							// 임의로 아래에서 지워야할 때 g_screen_word[i].index = 12으로 정의하였는데, 이것을 이용하여
							// 화면에 나타난 모든 단어를 지우기 위하여, 그 좌표로 찾아서 공백을 출력하여 지워버린다.

						}
						g_item_score--;	 // 이 기능을 사용했으므로 g_item_score을 1 감소시킨다.
						UpdateItem(); // 값을 감소시켰으므로 화면에도 1을 감소시킨 내용을 출력시킨다.

					}
				}
			}
			
			else 
			{
		        if (key == 27) // stop이 1일 때에 ESC를 누르는 것에 대한 내용이다.
				{
					stop = 0;
					GotoXY(3, 22);
					printf(" Programmed by 시종욱  ");
					// stop값을 다시 0으로 바꾸어 움직이게 하고 해당좌표로 이동하여 위에 해당하는 말을 출력한다.
				}

			}
			


			ProcessInput(key); // ProcessInput함수로 이동한다.
		}
		else// 키입력이 되어있지 않은 상황에 대한 내용이다.
		{
			NormalMode(); // NormalMode함수로 이동하는데, 이 함수는 출력된 단어를 오른쪽에서 왼쪽으로 이동시키는 내용이다.		
		}
			
	}
	

	GotoXY(27, 12);  
	printf("게임이 종료되었습니다. \n");
	GotoXY(27, 13);
	printf("최종점수 (score * stage) : %d                  ", g_score*a);
	// 해당하는 좌표로 이동하여 게임이 종료되었음을 알리는 출력을 한 후, 그 아래 좌표로 이동하여 최종점수를 출력하여 사용자에게 알린다.

	_getch(); 
}
void CompareWords(void)
{
	int compare; //compare라는 변수를 선언한다.
	
	for (int i = 0; i < g_screen_word_count; i++)
	{
		if (g_screen_word[i].index != 12) //12가 아닌 조건문을 건 이유는, g_screen_word[i].index는 0에서 11까지 존재하고, 
			                             // 12일 때 다른 조건을 정의하였기 때문이다.
		{
			compare = strcmp(g_input_word, g_words[g_screen_word[i].index]); // strcmp를 이용하여 두 값이 같으면 0을 compare에 저장한다.

			if (compare == 0) // 그 값이 동일하다면 
			{
				UpdateWordScore(i);				// 단어마다 점수가 다르므로, 이 함수로 이동해 해당하는 단어의 점수가 몇 점인지 확인한다.
				
				GotoXY(30, 22);                 // 입력하는 칸의 좌표를 이동한다.
				EraseWord(i);                   // 입력된 단어를 지운다.
				g_screen_word[i].index = 12;    // g_screen_word[i].index = 12로 변경하여 단어를 지운다.
				

				
			}	
			
		}
		
	}
	
}

void UpdateWordScore(int i) // (추가함수)
{
	if (g_words[g_screen_word[i].index] == "congratulation") // congratulation으로 맞추었을 때 50점을 획득한다.
		g_score += 50;
	

	else if (g_words[g_screen_word[i].index] == "discouragement") // discouragement로 맞추었을 때 45점을 획득한다.
		g_score += 45;
	
	else if (g_words[g_screen_word[i].index] == "unfortunately") // unfortunately로 맞추었을 때 40점을 획득한다.
		g_score += 40;
	
	else if (g_words[g_screen_word[i].index] == "programming") // unfortunately로 맞추었을 때 20점을 획득한다.
		g_score += 20;

	else if (g_words[g_screen_word[i].index] == "building") // building로 맞추었을 때 15점을 획득한다.
		g_score += 15;

	else // 위 단어를 제외한 다른 단어로 맞추었을 경우 10점을 획득한다.
		g_score += 10;
		
	g_stage_count++;
}
void Stage(void)
{

	for (int j = 0; j < g_screen_word_count; j++)
	{
		g_screen_word[j].index = 12;
		GotoXY(g_screen_word[j].x, g_screen_word[j].y);
		printf("              ");
     // 전체를 지우는 기능이다. (위에서 자세히 설명함)
	} 

	g_stage_score++; // 스테이지 값을 증가한다.

	
	GotoXY(36, 10); //해당 좌표로 이동한다.
	printf("Stage %d ", g_stage_score); // stage를 출력한다.

	if (g_stage_score == 1)
	{
		GotoXY(30, 12);
		printf(" TAP 키 : 전체지우기");
		GotoXY(30, 13);
		printf(" ESC 키 : 일시정지");
		// stage가 1일 때 사용자에게 키에대한 설명을 하기위해 만든 조건문이다.
	}

	Sleep(3000); //그 상태를 3초 유지한다.
	GotoXY(36, 10); // 다시 처음 좌표로 이동한다.
	printf("        "); // 공백을 출력한다.
	
	if (g_stage_score == 1)
	{
		GotoXY(30, 12);
		printf("                   ");
		GotoXY(30, 13);
		printf("                   ");
		// 위 처럼 출력을 했으니 다시 지운다.
	}

	if (g_move_speed > 0.02)
	    g_move_speed -= 0.02;        // 0.02초가 될 때 까지만 g_move_speed 값을 줄인다.

}

void InitScreen(void)
{
	int i;

	system("cls");

	printf("┌─────────────────────────────────────┐\n");
	for (i = 0; i < 19; i++)
		printf("│                                                                          │\n");
	printf("└─────────────────────────────────────┘\n");

	// Programmed by
	printf("┌────────────┐┌───input ──┐┌ Item ┐┌score ┐┌ fail ┐\n");
	printf("│  Programed by 시종욱   ││                ││      ││      ││      │\n");
	printf("└────────────┘└────────┘└───┘└───┘└───┘\n");


	// 입력칸
	UpdateInputWord();


	// 커서 이동
	GotoXY(30, 22);


	// 점수 초기화
	UpdateScore();


	// 실패 횟수 초기화
	UpdateFailCount();


	// 아이템 개수 초기화
	UpdateItem();

}

void ProcessInput(int key)
{
	UpdateInputWord(); 
	UpdateScore();
	// 입력된 단어를 화면에 출력하고 점수를 출력한다.
}

void UpdateInputWord(void)
{
	GotoXY(30, 22);                   
	printf("%s", g_input_word);
	// 해당 좌표로 이동하여 g_input_word을 출력한다.
}

void UpdateFailCount(void)
{
	GotoXY(72, 22);
	printf("%d", g_fail_count);
	// 해당 좌표로 이동하여 g_fail+count를 출력한다.
}

void DrawWord(int i)
{

	GotoXY(g_screen_word[i].x, g_screen_word[i].y);
	printf("%s", g_words[g_screen_word[i].index]);
	// 감소된 x좌표로 이동하여 그 단어를 출력한다.
}

void UpdateScore(void)
{
	GotoXY(62, 22);
	printf("%d", g_score);
	// 해당좌표로 이동하여 g_score을 출력한다.
}

void UpdateItem(void) // (추가함수)
{
	GotoXY(52, 22);
	printf("%d", g_item_score);
	// 해당좌표로 이동하여 g_item_score을 출력한다.
}

void InitData(void)
{
	// 내용 없음. 필요하다면 추가 가능
}
