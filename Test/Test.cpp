#include <Windows.h>
#include <iostream>
#include <cstdio>
#include <cassert>

int wmain()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	for(int i = 0; i < 5; ++i)
	{
		wprintf(L"wprintf: Hello World!\n");

		std::wcout << L"std::wcout: Hello World!\n";

		fwprintf(stderr, L"fwprintf to stderr: Hello World!\n");

		const wchar_t* writeConsoleMessage = L"WriteConsole: Hello World!\n";
		DWORD charsWritten = 0;
		WriteConsole(consoleHandle, writeConsoleMessage, (DWORD)wcslen(writeConsoleMessage),
			&charsWritten, NULL);

		OutputDebugString(L"OutputDebugString: Hello World!\n");

		Sleep(500);
	}
}
