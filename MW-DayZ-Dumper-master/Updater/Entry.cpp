#include "Framework.h"

Updater* g_Updater = new Updater;

int main() {

	if (!g_Updater->Init())
		return 0;

	if (!g_Updater->Scan()) {
#if _DEBUG
		printf("[UPDATER] Scanning Failed!\n");
#endif
		return 0;
	}

	if (!g_Updater->Release())
		return 0;

	return 1;
}
