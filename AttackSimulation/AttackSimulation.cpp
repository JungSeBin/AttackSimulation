// AttackSimulation.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>
#include <process.h>

#define NUM_OF_PLAYER 3
struct Player{
	int mPlayerId;
	int mHP;
	CRITICAL_SECTION mLock;
};

Player GamerData[NUM_OF_PLAYER];

void Attack(Player* from, Player* to)
{
	_tprintf(_T("BEGIN: from #%d to #%d\n"), from->mPlayerId, to->mPlayerId);

	EnterCriticalSection(&from->mLock);
	_tprintf(_T("--from #%d enter critical section\n"), from->mPlayerId);

	from->mHP += 70;

	LeaveCriticalSection(&from->mLock);

	EnterCriticalSection(&to->mLock);
	_tprintf(_T("--to #%d enter critical section\n"), to->mPlayerId);

	to->mHP -= 100;

	LeaveCriticalSection(&to->mLock);

	Sleep(100);
}

unsigned int WINAPI ThreadProc(LPVOID lpparam)
{
	srand(GetCurrentThreadId());

	int from = (int)lpparam;

	for (int i = 0; i < 10; ++i)
	{
		int to = 0;

		while (true)
		{
			to = rand() % NUM_OF_PLAYER;
			if (from != to)
				break;
		}
		Attack(&GamerData[from], &GamerData[to]);
	}
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD dwThreadId[NUM_OF_PLAYER];
	HANDLE hThread[NUM_OF_PLAYER];

	for (int i = 0; i < NUM_OF_PLAYER; ++i)
	{
		GamerData[i].mPlayerId = i + 1001;
		GamerData[i].mHP = 2000;
		InitializeCriticalSection(&(GamerData[i].mLock));
	}

	for (DWORD i = 0; i < NUM_OF_PLAYER; ++i)
	{
		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, (LPVOID)i,
			CREATE_SUSPENDED, (unsigned int*)&dwThreadId[i]);
	}

	_tprintf(_T("==============BEGIN=============\n"));
	for (DWORD i = 0; i < NUM_OF_PLAYER; ++i)
	{
		ResumeThread(hThread[i]);
	}

	WaitForMultipleObjects(NUM_OF_PLAYER, hThread, TRUE, INFINITE);

	_tprintf(_T("===============RESULT============\n"));

	for (DWORD i = 0; i < NUM_OF_PLAYER; ++i)
	{
		_tprintf(_T("---Player# %d, HP: %d\n"), GamerData[i].mPlayerId, GamerData[i].mHP);
		DeleteCriticalSection(&(GamerData[i].mLock));
	}

	for (DWORD i = 0; i < NUM_OF_PLAYER; ++i)
	{
		CloseHandle(hThread[i]);
	}
	getchar();
	return 0;
}

