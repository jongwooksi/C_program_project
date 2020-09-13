#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"

#define KEY_ESC 27
#define KEY_ENTER '\r'
#define KEY_BS '\b'
#define KEY_TAP 9                                //(�߰� ����) TAPŰ�� ������ ȭ�鿡 ��µ� ��� �ܾ ������� ������.


#define MAX_WORD_LENGTH 24
#define MAX_SCREEN_WORD_COUNT 150

int g_word_count = 11;                          // Ȯ���� ���� ������ ����
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
int g_fail_count = 0;                           // ������ ����
int g_score = 0;                                // ����
char g_input_word[MAX_WORD_LENGTH + 1];         // �Է� �ܾ� ����
int g_input_word_length = 0;                    // �Է� �ܾ� ����
int g_item_score = 3;                           // ���� ������ ����
int a = 0;                                      // stage�� ���� ���� ����
int g_stage_score = 0;                          // �Լ� �ȿ����� stage ����

struct ScreenWord
{
	int index;
	int x;
	int y;
};
typedef struct ScreenWord ScreenWord;
ScreenWord g_screen_word[MAX_SCREEN_WORD_COUNT];// ȭ�鿡 ��Ÿ�� �ܾ��(g_words�� ���� index ����)
int g_screen_word_count = 0;                    // ȭ�鿡 ��Ÿ�� �ܾ� ����

clock_t g_move_start_time;                      // �ܾ� �̵� ���� �ð�
double g_move_speed = 0.2;                      // �ܾ� �̵� �ð� ����(�� ����)
clock_t g_new_word_start_time;                  // �� �ܾ� ���� �ð�
double g_new_word_speed = 1.5;                  // �� �ܾ ��Ÿ���� �ð� ����(�� ����)
int print = 0;                                  // ����� �Լ����� ���� �� ���� ��������� ������ �ϳ� �� �����Ѵ�.
int g_score_count=0;                            // �� ���� ���߾����� �˰��ϴ� �����̴�.
int g_stage_count = 0;                          // �Լ��ȿ��� stage ���� �ø��� �����̴�.
int stop = 0;                                   // �Ͻ������� Ȱ���ϴ� �����̸�, 0�� �� �����δٴ� ���� ���Ѵ�.



void InitScreen(void);                          // ���Ϸκ��� �ܾ�� �Է�
void InitData(void);                            // ���� ���� ���� (Ȯ�� ����)
void Run(void);                                 // Ÿ�� ���� ����
void CompareWords(void);                        // ȭ�鿡 ��µ� �ܾ�� �߿� �Է� �ܾ�� ��ġ�ϴ� �ܾ �ִ��� �˻��ϰ� ��ġ�ϴ� �ܾ �ִٸ� ���� �� ���� ����
void ProcessInput(int key);                     // Ű �Է��� ���� ��� ���� �Էµ� ���ڿ��� ���� ��
void NormalMode(void);                          // Ű �Է��� ���� ��� ó�� ����
void DrawWord(int i);                           // ȭ�鿡 ��Ÿ�� i��° �ܾ� ���
void EraseWord(int i);                          // ȭ�鿡 ��Ÿ�� i��° �ܾ� �����
void UpdateInputWord(void);                     // �Էµ� �ܾ ȭ�鿡 ��� 
void UpdateScore(void);                         // ���� ���â�� ���� ��� 
void UpdateFailCount(void);                     // ���� Ƚ�� ���
void UpdateWordScore(int i);                    // (�߰��Լ�) �ܾ� ���� �ٸ� ������ �δ� �Լ�
void UpdateItem(void);                          // (�߰��Լ�) ���� ������ ���� ���
void Stage(void);                               // (�߰��Լ�) Stage ���



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
	g_screen_word[g_screen_word_count].x = 60;                      // ��µǴ� �ܾ��� ù ���� ��ǥ�� 63���� ������Ų��.
	g_screen_word[g_screen_word_count].y = rand() % 18 + 2;         // �־��� ĭ�ȿ� �������� �����Ű�� ���� ����Ѵ�.
	g_screen_word[g_screen_word_count].index = rand() % 11;   // 11���� ���ǵ� ���ڿ��迭���� �������� �����Ű�� ���� ����Ѵ�.

	if (g_stage_count == 10 * a) 
	{
		Stage();
		a++;
	} // 10���� �ܾ ���� �� ����, stge�� �ö󰡴� ����� ���� ���ǹ��̴�.

	if (stop == 0)
	{
		if (GetDiffTime(g_move_start_time) > g_move_speed)      // ��µǴ� ���ڰ� g_move_speed ��ŭ�� �ð��� �����ٸ�
		{

			for (int i = print; i < g_screen_word_count; i++)
			{

				if (g_screen_word[i].index != 12)
				{
					DrawWord(i);
					EraseWord(i);
					g_screen_word[i].x--;
					DrawWord(i);

					// ���ڸ� ����� �������� �̵���Ű�� ���� ��µ� ���ڸ� ������� x��ǥ�� �ϳ� ���ҽ��� ����½�Ŵ���ν�
					// ���ڰ� �����̴� �� ó�� ���̰� �Ѵ�.


					if (g_screen_word[i].x == 3) // x��ǥ�� ���ʿ� ������� �� ó���� ������������ ������ �����. �̸����� x��ǥ�� 3�� �� ��
					{                            // �Ʒ��� ���� �� ó���� �ϰ� �����Ͽ���.
						g_fail_count++;          // g_fail_count ���� 1 ������Ų��.
						UpdateFailCount();       // ������ �Ʒ� fail ĭ�� ���� 1�� �������� ��½�Ų��.
						print++;                 // �� ó���� ���������� ��ǥ�� 3�� �� �� ��� �����Ÿ� �� ����µǴµ� �̸� �����ϱ� �����̴�.
						GotoXY(g_screen_word[i].x, g_screen_word[i].y); // �� �ڸ��� ��µ� ��ǥ�� �̵���Ų ����
						EraseWord(i);// �� �迭�� ����������.
						g_screen_word[i].index = 12;
					}

				}
			}
			g_move_start_time = clock();

			// �ٽ� g_move_speed ��ŭ �̵���Ű�� ���� g_move_start_time�� �̿��Ͽ� ���� �ʱ�ȭ��Ų��.
		}
		if (GetDiffTime(g_new_word_start_time) > g_new_word_speed) // ���ο� �ܾ ����ϱ� �����̴�.
		{

			if (g_move_speed <= 0.18)
				g_words[0] = "unfortunately";

			if (g_move_speed <= 0.16)
				g_words[1] = "discouragement";

			if (g_move_speed <= 0.14)
				g_words[2] = "congratulation";

			// ���ߴ� ������ ������ g_new_word_speed�� �پ�� �� Ư���� �ð� ���ϰ��Ǹ� ���� �� ����� �ܾ ��µǴµ�,
			// ������ �迭���ִ� �ܾ �����Ǵ� �ǹ̰� �ƴ϶� �߰��Ǵ� �ǹ��̴�.

			DrawWord(g_screen_word_count);
			g_screen_word_count++;
			g_new_word_start_time = clock();

			// DrawWord�� �̿��Ͽ� ���ο� �ܾ ����� �� g_screen_word_count���� 1 ������Ų ��
			// �ٽ� ���ο� �ܾ ��½�Ű�� ���Ͽ� g_new_word_start_time�� �̿��� �ʱ�ȭ ��Ų��. 
		}
	}
	

}
void EraseWord(int i)
{
	int len = strlen(g_words[g_screen_word[i].index]); // len �̶�� ������ �����Ͽ�, �Լ��� �Ѿ�� ���ڿ��� ���̸� �����Ѵ�.

	GotoXY(g_screen_word[i].x, g_screen_word[i].y); // �� �ش��ϴ� ��ǥ�� �̵��� ����

	for (int j = -1; j < len; j++)	
		printf(" ");	

	// ������ ����Ͽ� ������ �� ó�� ���̰� �����.
	
}

void Run(void)
{
	g_move_start_time = clock();            // ���� �ð�(clock) �ʱ�ȭ
	g_new_word_start_time = clock();        // ���� �ð�(clock) �ʱ�ȭ

	int key;
	int position = 30;



	while (g_fail_count < 5)                // 5ȸ ���� �� ����
	{
		
		GotoXY(30, 22);                    // �Է� ĭ���� �̵���Ű�� �����̴�.


		if (_kbhit() != 0)                  // Ű���尡 �������ٸ�
		{
			key = _getch();                 // �� ���� key�� �����Ѵ�.
			SetCursorVisible(1);

			if (stop == 0)
			{
				if (isalpha(key) != 0)          // key�� ���ĺ��̶��
				{
					g_input_word[g_input_word_length] = key;     // g_input_word[g_input_word_length]�� key�� �����ϰ�
					g_input_word[g_input_word_length + 1] = '\0'; // �״��� �迭�� �ι��ڸ� �����ϰ�
					g_input_word_length++; // �Էµ� ���ڿ��� ���̸� ������Ų����
					position++; // ���� ��ǥ�� �̵��Ͽ� �ٸ� ���� ���� �غ� �Ѵ�.
				}


				else if (key == '\r') // Enter Ű�� �������� ���� ���� �����̴�.
				{
					CompareWords(); // �Է� ���� ȭ�鿡 ��µǾ��ִ� ���� ���ϱ� ���� compare�Լ��� �̿��Ѵ�.


					GotoXY(30, 22); //��ǥ�� �̵��Ѵ�.
					for (int i = 0; i < g_input_word_length; i++)
					{
						printf(" ");
						g_input_word[i] = NULL;
					}
					// Enter�� �ԷµǾ����� �Է�ĭ�� �Է��� �ܾ���� ������ �ϴµ�, g_input_word_length��ŭ for���� ����Ͽ�
					// ������ ����ϰ� �� �ܾ ���������� �����̴�.

					g_input_word_length = 0; // �� �� �Էµ� �ܾ��� ���̸� 0���� �ʱ�ȭ ��Ų��
					GotoXY(30, 22); //�ٽ� ��ǥ�� �̵��Ѵ�.
				}
				else if (key == '\b') // �������̽��� �������� ���� ���� �����̴�.
				{
					if (g_input_word_length > 0) // �� ���ǹ��� �Ŵ� ������ �Էµ� �ܾ ���� �� ������ ������ �߻��ϴ� �� �����ϱ� ���ؼ���.
					{
						g_input_word[g_input_word_length - 1] = '\0'; // 
						for (int i = 0; i < g_input_word_length; i++)
							printf(" ");
						g_input_word_length--;
						// ������� �Է��� �ܾ��� ���ڸ��� �ι��ڸ� ����Ͽ� ���� �� ������ ����� ����, g_input_word_length�� 1 �����Ͽ�
						// �� ���ڸ��� �ܾ �Ȱ��� ó���� �ϴ� �����̴�.
					}

					GotoXY(position, 22); // ���� ��ǥ�� �̵��Ͽ� �ٸ� ���� ���� �غ� �Ѵ�.
				}

				else if (key == 27) // ESC Ű�� �������� ���� ���� �����̴�.
				{
					if (stop == 0) // ���� �����̰� �ִٸ�
					{
						stop = 1;
						GotoXY(3, 22);
						printf("< �Ͻ����� > �ԷºҰ���");

						// stop ���� 1�� �ٲپ� �������� �ʴٴ� ���� �˷��ְ�, ��ǥ�� �̵��Ͽ� �Ͻ�������� ���� �˷��ش�.
					}
	
				}

				else if (key == 9) //(�߰�����) Tap Ű�� ������ ���� ���� �����̴�.
				{
					if (g_item_score > 0) // g_item_score�� 1�̻��� ���� ��밡���ϰ� �ϴ� ���ǹ��̴�.(3���� �����Ǿ��ִ�.)
					{

						for (int i = 0; i < g_screen_word_count; i++)
						{
							g_screen_word[i].index = 12;
							GotoXY(g_screen_word[i].x, g_screen_word[i].y);
							printf("              ");

							// ȭ�鿡 ��Ÿ�� ��� �ܾ �ϰ������� �����ִ� ����̴�. (������ ����)
							// ���Ƿ� �Ʒ����� �������� �� g_screen_word[i].index = 12���� �����Ͽ��µ�, �̰��� �̿��Ͽ�
							// ȭ�鿡 ��Ÿ�� ��� �ܾ ����� ���Ͽ�, �� ��ǥ�� ã�Ƽ� ������ ����Ͽ� ����������.

						}
						g_item_score--;	 // �� ����� ��������Ƿ� g_item_score�� 1 ���ҽ�Ų��.
						UpdateItem(); // ���� ���ҽ������Ƿ� ȭ�鿡�� 1�� ���ҽ�Ų ������ ��½�Ų��.

					}
				}
			}
			
			else 
			{
		        if (key == 27) // stop�� 1�� ���� ESC�� ������ �Ϳ� ���� �����̴�.
				{
					stop = 0;
					GotoXY(3, 22);
					printf(" Programmed by ������  ");
					// stop���� �ٽ� 0���� �ٲپ� �����̰� �ϰ� �ش���ǥ�� �̵��Ͽ� ���� �ش��ϴ� ���� ����Ѵ�.
				}

			}
			


			ProcessInput(key); // ProcessInput�Լ��� �̵��Ѵ�.
		}
		else// Ű�Է��� �Ǿ����� ���� ��Ȳ�� ���� �����̴�.
		{
			NormalMode(); // NormalMode�Լ��� �̵��ϴµ�, �� �Լ��� ��µ� �ܾ �����ʿ��� �������� �̵���Ű�� �����̴�.		
		}
			
	}
	

	GotoXY(27, 12);  
	printf("������ ����Ǿ����ϴ�. \n");
	GotoXY(27, 13);
	printf("�������� (score * stage) : %d                  ", g_score*a);
	// �ش��ϴ� ��ǥ�� �̵��Ͽ� ������ ����Ǿ����� �˸��� ����� �� ��, �� �Ʒ� ��ǥ�� �̵��Ͽ� ���������� ����Ͽ� ����ڿ��� �˸���.

	_getch(); 
}
void CompareWords(void)
{
	int compare; //compare��� ������ �����Ѵ�.
	
	for (int i = 0; i < g_screen_word_count; i++)
	{
		if (g_screen_word[i].index != 12) //12�� �ƴ� ���ǹ��� �� ������, g_screen_word[i].index�� 0���� 11���� �����ϰ�, 
			                             // 12�� �� �ٸ� ������ �����Ͽ��� �����̴�.
		{
			compare = strcmp(g_input_word, g_words[g_screen_word[i].index]); // strcmp�� �̿��Ͽ� �� ���� ������ 0�� compare�� �����Ѵ�.

			if (compare == 0) // �� ���� �����ϴٸ� 
			{
				UpdateWordScore(i);				// �ܾ�� ������ �ٸ��Ƿ�, �� �Լ��� �̵��� �ش��ϴ� �ܾ��� ������ �� ������ Ȯ���Ѵ�.
				
				GotoXY(30, 22);                 // �Է��ϴ� ĭ�� ��ǥ�� �̵��Ѵ�.
				EraseWord(i);                   // �Էµ� �ܾ �����.
				g_screen_word[i].index = 12;    // g_screen_word[i].index = 12�� �����Ͽ� �ܾ �����.
				

				
			}	
			
		}
		
	}
	
}

void UpdateWordScore(int i) // (�߰��Լ�)
{
	if (g_words[g_screen_word[i].index] == "congratulation") // congratulation���� ���߾��� �� 50���� ȹ���Ѵ�.
		g_score += 50;
	

	else if (g_words[g_screen_word[i].index] == "discouragement") // discouragement�� ���߾��� �� 45���� ȹ���Ѵ�.
		g_score += 45;
	
	else if (g_words[g_screen_word[i].index] == "unfortunately") // unfortunately�� ���߾��� �� 40���� ȹ���Ѵ�.
		g_score += 40;
	
	else if (g_words[g_screen_word[i].index] == "programming") // unfortunately�� ���߾��� �� 20���� ȹ���Ѵ�.
		g_score += 20;

	else if (g_words[g_screen_word[i].index] == "building") // building�� ���߾��� �� 15���� ȹ���Ѵ�.
		g_score += 15;

	else // �� �ܾ ������ �ٸ� �ܾ�� ���߾��� ��� 10���� ȹ���Ѵ�.
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
     // ��ü�� ����� ����̴�. (������ �ڼ��� ������)
	} 

	g_stage_score++; // �������� ���� �����Ѵ�.

	
	GotoXY(36, 10); //�ش� ��ǥ�� �̵��Ѵ�.
	printf("Stage %d ", g_stage_score); // stage�� ����Ѵ�.

	if (g_stage_score == 1)
	{
		GotoXY(30, 12);
		printf(" TAP Ű : ��ü�����");
		GotoXY(30, 13);
		printf(" ESC Ű : �Ͻ�����");
		// stage�� 1�� �� ����ڿ��� Ű������ ������ �ϱ����� ���� ���ǹ��̴�.
	}

	Sleep(3000); //�� ���¸� 3�� �����Ѵ�.
	GotoXY(36, 10); // �ٽ� ó�� ��ǥ�� �̵��Ѵ�.
	printf("        "); // ������ ����Ѵ�.
	
	if (g_stage_score == 1)
	{
		GotoXY(30, 12);
		printf("                   ");
		GotoXY(30, 13);
		printf("                   ");
		// �� ó�� ����� ������ �ٽ� �����.
	}

	if (g_move_speed > 0.02)
	    g_move_speed -= 0.02;        // 0.02�ʰ� �� �� ������ g_move_speed ���� ���δ�.

}

void InitScreen(void)
{
	int i;

	system("cls");

	printf("������������������������������������������������������������������������������\n");
	for (i = 0; i < 19; i++)
		printf("��                                                                          ��\n");
	printf("������������������������������������������������������������������������������\n");

	// Programmed by
	printf("������������������������������������input �������� Item ����score ���� fail ��\n");
	printf("��  Programed by ������   ����                ����      ����      ����      ��\n");
	printf("������������������������������������������������������������������������������\n");


	// �Է�ĭ
	UpdateInputWord();


	// Ŀ�� �̵�
	GotoXY(30, 22);


	// ���� �ʱ�ȭ
	UpdateScore();


	// ���� Ƚ�� �ʱ�ȭ
	UpdateFailCount();


	// ������ ���� �ʱ�ȭ
	UpdateItem();

}

void ProcessInput(int key)
{
	UpdateInputWord(); 
	UpdateScore();
	// �Էµ� �ܾ ȭ�鿡 ����ϰ� ������ ����Ѵ�.
}

void UpdateInputWord(void)
{
	GotoXY(30, 22);                   
	printf("%s", g_input_word);
	// �ش� ��ǥ�� �̵��Ͽ� g_input_word�� ����Ѵ�.
}

void UpdateFailCount(void)
{
	GotoXY(72, 22);
	printf("%d", g_fail_count);
	// �ش� ��ǥ�� �̵��Ͽ� g_fail+count�� ����Ѵ�.
}

void DrawWord(int i)
{

	GotoXY(g_screen_word[i].x, g_screen_word[i].y);
	printf("%s", g_words[g_screen_word[i].index]);
	// ���ҵ� x��ǥ�� �̵��Ͽ� �� �ܾ ����Ѵ�.
}

void UpdateScore(void)
{
	GotoXY(62, 22);
	printf("%d", g_score);
	// �ش���ǥ�� �̵��Ͽ� g_score�� ����Ѵ�.
}

void UpdateItem(void) // (�߰��Լ�)
{
	GotoXY(52, 22);
	printf("%d", g_item_score);
	// �ش���ǥ�� �̵��Ͽ� g_item_score�� ����Ѵ�.
}

void InitData(void)
{
	// ���� ����. �ʿ��ϴٸ� �߰� ����
}
