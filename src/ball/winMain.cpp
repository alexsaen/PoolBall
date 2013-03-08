#include <windows.h>

extern int main(int argc, char* argv[]);

HINSTANCE hInstance;
char modulename[MAX_PATH];

int runGame() {
	char *argv[] = { modulename };
	return main(1, argv);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	hInstance=hInst;
	GetModuleFileNameA(hInstance,modulename,sizeof(modulename));

	HANDLE handle=CreateMutexA(0,true,"PoolBall.Mutex");
	bool alreadyRun = GetLastError()==ERROR_ALREADY_EXISTS;

	switch(*(short*)lpCmdLine) {
	case '2-':
		runGame();
		break;               
	default:            
		if(!alreadyRun) 
			runGame();
		break;
	}

	CloseHandle(handle);
	return 0;
}
