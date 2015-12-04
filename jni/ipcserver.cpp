#include <stdio.h>
#include <ElfIPCServer.h>



int main()
{
	elfIPCServer * IPCServer = new elfIPCServer();
	IPCServer->StartIPCServer();
}
