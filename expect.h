#define UNICODE
#include <wchar.h>

struct Search {
	wchar_t *str;
	struct {
		int found, total, numThreads;
		int error87, error18, error;
	} count;
};

int CountStr(struct Search*);

