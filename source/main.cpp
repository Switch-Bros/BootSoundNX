#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>

#include <switch.h>

#include "mp3.h"
#include "util.h"

#define ERPT_SAVE_ID 0x80000000000000D1
#define TITLE_ID 0x00FF0000000002AA
#define INNER_HEAP_SIZE 0x40000
size_t nx_inner_heap_size = INNER_HEAP_SIZE;
char   nx_inner_heap[INNER_HEAP_SIZE];

extern "C"
{
    void __libnx_initheap(void)
{
	void*  addr = nx_inner_heap;
	size_t size = nx_inner_heap_size;

	extern char* fake_heap_start;
	extern char* fake_heap_end;

	fake_heap_start = (char*)addr;
	fake_heap_end   = (char*)addr + size;
}

void __attribute__((weak)) __appInit(void)
{
    Result rc;

    rc = smInitialize();
    rc = hidInitialize();
    rc = fsInitialize();
    fsdevMountSdmc();
}

void __attribute__((weak)) userAppExit(void);

void __attribute__((weak)) __appExit(void)
{
    fsdevUnmountAll();
    fsExit();
    hidExit();
    smExit();
}
}

int main(int argc, char* argv[])
{
    FILE * Soundfile = fopen ("sdmc://config/BootSoundNX/sound/bootsound.mp3","r+");
    
    if (Soundfile != NULL)
    {
        playMp3("sdmc://config/BootSoundNX/sound/bootsound.mp3");
        fclose (Soundfile);
    }
    return 0;
}