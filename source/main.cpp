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

#define BOOT_SOUND "boot_sound.mp3"
#define SOUND_FOLDER "sdmc:/config/BootSoundNX/sounds/"

char* read_line(FILE* file)
{
    char* line = NULL;
    size_t len = 0;
    ssize_t read = getline(&line, &len, file);
    if (read != -1) {
        // Remove newline character
        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        return line;
    } else {
        free(line);
        return NULL;
    }
}

// Initialize/exit services, update as needed.
void __attribute__((weak)) __appInit(void)
{
    Result rc;

    // Initialize default services.
    rc = smInitialize();
    //if (R_FAILED(rc))
        //fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));

    // Enable this if you want to use HID.
    rc = hidInitialize();
    //if (R_FAILED(rc))
        //fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));

    //Enable this if you want to use time.
    /*rc = timeInitialize();
    if (R_FAILED(rc))
        fatalLater(MAKERESULT(Module_Libnx, LibnxError_InitFail_Time));
    __libnx_init_time();*/

    rc = fsInitialize();
    //if (R_FAILED(rc))
        //fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

    fsdevMountSdmc();
}

void __attribute__((weak)) userAppExit(void);

void __attribute__((weak)) __appExit(void)
{
    // Cleanup default services.
    fsdevUnmountAll();
    fsExit();
    //timeExit();//Enable this if you want to use time.
    hidExit();// Enable this if you want to use HID.
    smExit();
}

// Main program entrypoint
int main(int argc, char* argv[])
{
    FILE *soundfile, *soundlist;
    char *soundfilename = NULL, *line;

    soundlist = fopen("sdmc:/config/BootSoundNX/soundlist.txt", "r");
    if (soundlist == NULL) {
        // File doesn't exist, play default boot sound
        playMp3(SOUND_FOLDER BOOT_SOUND);
        return 0;
    }

    // Read first line of soundlist file
    line = read_line(soundlist);
    if (line != NULL) {
        // Construct path to sound file
        soundfilename = (char*)malloc(strlen(line) + strlen(SOUND_FOLDER) + 1);
        sprintf(soundfilename, "%s%s", SOUND_FOLDER, line);

        // Try to open the sound file
        soundfile = fopen(soundfilename, "r");
        if (soundfile != NULL) {
            // Play the sound file
            playMp3(soundfilename);
            fclose(soundfile);
        } else {
            // Sound file not found, play default boot sound
            playMp3(SOUND_FOLDER BOOT_SOUND);
        }
    } else {
        // Empty soundlist file,
