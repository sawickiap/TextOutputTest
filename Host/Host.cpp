#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cctype>
#include <thread>

int wmain()
{
	wprintf(L"[ Hello World from the host! ]\n");

	SECURITY_ATTRIBUTES pipeSecurityAttributes = {
		.nLength = sizeof(SECURITY_ATTRIBUTES),
		.bInheritHandle = TRUE };
	HANDLE readPipe, writePipe;
	BOOL success = CreatePipe(
		&readPipe, // hReadPipe
		&writePipe, // hWritePipe
		&pipeSecurityAttributes, // lpPipeAttributes
		0); // nSize (0 means default)
	assert(success);

	auto readingThreadFunc = [readPipe]()
	{
		char buf[256];
		for(;;)
		{
			DWORD dw;
			BOOL ok = ReadFile(readPipe, buf, sizeof(buf), &dw, NULL);
			if(ok)
			{
				if(dw > 0)
					printf("%.*s", (int)dw, buf);
			}
			else
				return;
		}
	};
	std::thread readingThread{readingThreadFunc};

	wchar_t path[MAX_PATH];
	wcscpy_s(path, L"Test.exe");
	STARTUPINFO startupInfo = {
		.cb = sizeof(STARTUPINFO),
		.dwFlags = STARTF_USESTDHANDLES,
		.hStdInput = INVALID_HANDLE_VALUE,
		.hStdOutput = writePipe,
		.hStdError = writePipe
	};
	PROCESS_INFORMATION processInfo = {};
	success = CreateProcess(
		path, // lpApplicationName
		NULL, // lpCommandLine
		NULL, // lpProcessAttributes
		NULL, // lpThreadAttributes
		TRUE, // bInheritHandles
		DEBUG_PROCESS, // dwCreationFlags
		NULL, // lpEnvironment
		NULL, // lpCurrentDirectory
		&startupInfo,
		&processInfo);
	assert(success);

	DEBUG_EVENT e;
	bool running = true;
	while(running)
	{
		ZeroMemory(&e, sizeof(e));
		BOOL ok = WaitForDebugEventEx(&e, INFINITE); // Must be on the same thread as CreateProcess!!!
		assert(ok);
		DWORD continueStatus = DBG_CONTINUE;
		switch(e.dwDebugEventCode)
		{
		case EXCEPTION_DEBUG_EVENT:
			if(e.u.Exception.ExceptionRecord.ExceptionCode != EXCEPTION_BREAKPOINT)
				continueStatus = DBG_EXCEPTION_NOT_HANDLED;
			break;
		case CREATE_PROCESS_DEBUG_EVENT:
			CloseHandle(e.u.CreateProcessInfo.hFile);
			break;
		case EXIT_PROCESS_DEBUG_EVENT:
			if(e.dwProcessId == processInfo.dwProcessId)
				running = false;
			break;
		case OUTPUT_DEBUG_STRING_EVENT:
			if(e.u.DebugString.nDebugStringLength > 0)
			{
				char buf[1024];
				WORD len = e.u.DebugString.nDebugStringLength;
				assert(len < _countof(buf));
				size_t bytesRead = 0;
				ok = ReadProcessMemory(processInfo.hProcess, e.u.DebugString.lpDebugStringData, buf, len, &bytesRead);
				assert(ok && bytesRead == len);
				if(e.u.DebugString.fUnicode)
					wprintf(L"%.*s", (int)(len / sizeof(wchar_t)), (const wchar_t*)buf);
				else
					printf("%.*s", (int)len, buf);
			}
			break;
		}
		ok = ContinueDebugEvent(e.dwProcessId, e.dwThreadId, continueStatus);
		assert(ok);
	}

	WaitForSingleObject(processInfo.hProcess, INFINITE);
	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);

	CloseHandle(writePipe);
	CloseHandle(readPipe);

	readingThread.join();

	wprintf(L"[ Host end. ]\n");
}
