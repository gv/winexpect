#include "expect.h"
#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <limits.h>

struct WinexpectUi {
	unsigned long long start;
	HWND window;
	struct Search search;
};

const unsigned TIMER_MAIN_ID = 1;
static struct WinexpectUi globalUi;

static struct WinexpectUi *GetUi() {
	return &globalUi; 
}

static void WuUpdate(struct WinexpectUi *ui) {
	CountStr(&ui->search);
	InvalidateRect(ui->window, NULL, TRUE);
	SetTimer(ui->window, TIMER_MAIN_ID, 1000, NULL); 
}

static LRESULT CALLBACK HandleMessage(
	HWND w, UINT m, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paints;
	HDC c;
	WCHAR buf[128];
	struct WinexpectUi *ui = GetUi();
	int durationSec;

	switch(m) {
	case WM_PAINT:
		durationSec = GetDurationSec(ui->start);
		c = BeginPaint(w, &paints);
		TextOut(c,
			10, /*x*/
			1,  /*y*/
			buf, _snwprintf(buf, sizeof buf/sizeof(buf[0]),
				L"Waiting for '%s' (%d windows) %d:%02d", ui->search.str,
				ui->search.count.total, durationSec/60, durationSec%60)
		);
		EndPaint(w, &paints);
		return 0;
	case WM_TIMER:
		WuUpdate(GetUi());
		return 0;
	}
	return DefWindowProc(w, m, wParam, lParam);
}

static void ShowError(const WCHAR* what) {
	static WCHAR buf[128];
	_snwprintf(buf, sizeof buf/sizeof(buf[0]), L"%s: error %x", what,
		GetLastError());
	MessageBox(NULL, buf, NULL, MB_ICONERROR|MB_SETFOREGROUND);
}

static int WuCreate(struct WinexpectUi *ui) {
	WNDCLASS cl = {0};
	cl.lpfnWndProc = HandleMessage;
	cl.hCursor = LoadCursor(NULL, IDC_ARROW);
	cl.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	cl.lpszClassName = L"ui";
	if(!RegisterClass(&cl)) {
		ShowError(L"RegisterClass");
		return 0;
	}
	ui->window = CreateWindow(L"ui", L"Winexpect",
		WS_VISIBLE | WS_POPUP, 10, 10, 400, 50,
		NULL /*parent*/, NULL, GetModuleHandle(0), NULL);
	if(!ui->window) {
		ShowError(L"CreateWindow");
		return 0;
	}
    return 1;
}

static int GetDurationSec(unsigned long long start) {
	unsigned long long now, f, r;
	QueryPerformanceCounter((LARGE_INTEGER*)&now);
	QueryPerformanceFrequency((LARGE_INTEGER*)&f);
	r = (now - start)/f;
	if(r > INT_MAX)
		r = INT_MAX;
	return (int)r;
}

static void usage() {
	MessageBox(NULL, L"Usage: winexpect.exe STRING", NULL,
		MB_ICONERROR|MB_SETFOREGROUND);
}

int WINAPI wWinMain(
	HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR cmdline, int cmdshow)
{
  MSG msg;
  struct WinexpectUi *ui = GetUi();
  int argc;
  LPWSTR *argv = CommandLineToArgvW(cmdline, &argc);
  if (NULL == argv) {
    MessageBox(NULL, L"winexpect.exe out of memory", NULL,
      MB_ICONERROR|MB_SETFOREGROUND);
    return 1;
  }
  if(1 != argc) {
    usage();
    return 1;
  }
  QueryPerformanceCounter((LARGE_INTEGER*)&ui->start);
  ui->search.str = argv[1];
  if(!WuCreate(ui))
    return 1;
  WuUpdate(ui);
  while (GetMessage(&msg,  NULL, 0, 0)){
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return 0;
}
