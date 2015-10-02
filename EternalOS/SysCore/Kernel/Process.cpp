#include "Process.h"

Process::Process()
{
	TaskID = -1;
}


Process::~Process()
{
}

extern void install_pagedirectory(void* pPageDirectory);

void Process::SetPDBR()
{
	install_pagedirectory(pPageDirectory);
}
