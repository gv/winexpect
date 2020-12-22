#include "expect.h"
#include <stdio.h>
#include <windows.h>

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

	EnumChildWindows(w, ProcessWindow, l);
	
	return TRUE;
}

int CountStr(struct Search *s) {
	memset(&s->count, 0, sizeof(s->count));
	if (!EnumChildWindows(NULL, ProcessWindow, (LPARAM)s))
		// FALSE return interrupts the enumeration, but is
		// counted as failure too
		if (!s->count.found && !s->count.error)
			return -1;
	return s->count.found;
}

