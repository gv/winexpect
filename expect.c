#include "expect.h"
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

static int usage(wchar_t **argv) {
	fwprintf(stderr, L"Usage: %s STRING\n", argv[0]);
	return 1;
}

static BOOL CALLBACK ProcessWindow(HWND w, LPARAM l) {
	struct Search *p = (struct Search*)l;
	TCHAR c[256];
	p->count.total++;

	if(!GetWindowText(w, c, 256)) {
		int r = GetLastError();
		if(r == 87)
			p->count.error87++;
		p->count.error++;
	} else {
		if(wcsstr(c, p->str)) {
			p->count.found++;
			return FALSE;
		}
	}

	// EnumChildWindows(w, cb, l);
	
	return TRUE;
}

int CountStr(struct Search *s) {
	HDESK desktop = GetThreadDesktop(GetCurrentThreadId());
	if (NULL == desktop)
		return -1;
	memset(&s->count, 0, sizeof(s->count));
	if (!EnumChildWindows(NULL, ProcessWindow, (LPARAM)s))
		return -1;
	return s->count.found;
}

#if 0
int CountStr_old(struct Search *s) {
	THREADENTRY32 te;
	HANDLE ts = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 

	s->total = 0;
	s->numThreads = 0;
	s->errors87Count = 0;
	if(ts == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "CreateToolhelp32Snapshot: error %d\n", GetLastError());
		return 1;
	}

	te.dwSize = sizeof te;

	if(!Thread32First(ts, &te)) {
		fprintf(stderr, "Thread32First: error %d\n", GetLastError());
		return 1;
	}

	do {
		/*
		  s->total = s->errors87Count = 0;
		  fprintf(stderr, "Thread %d in %d: ",
		  te.th32ThreadID, te.th32OwnerProcessID);
		*/
		EnumThreadWindows(te.th32ThreadID, ProcessWindow, (LPARAM)s);
		s->numThreads++;
		if(s->count)
			break;
		/*
		  fwprintf(stderr,
		  L"%d windows found (in %d, %d threads, %d invalid parameters)\n",
		  s->count, s->total, numThreads, s->errors87Count);
		*/
	} while(Thread32Next(ts, &te));
	return s->count;
}
#endif
