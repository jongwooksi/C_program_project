#ifndef JHHWANG_UTIL_H
#define JHHWANG_UTIL_H

#include <time.h>
#include <Windows.h>

void GotoXY(int x, int y);		            // Ŀ�� ��ġ �̵�
double GetDiffTime(clock_t start_time);     // �ð� ���� �˾Ƴ���(�� ����)
void SetCursorVisible(BOOL tf);             // Ŀ�� �Ⱥ��̰� �ϱ�

#endif