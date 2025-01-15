#include "main.h"

PSP_MODULE_INFO("mme", 0, 1, 1);
PSP_HEAP_SIZE_KB(-1024);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU | PSP_THREAD_ATTR_USER);

static volatile u32* mem = nullptr;
#define meCounter         (mem[0])
#define meStart           (mem[1])
#define meExit            (mem[2])

__attribute__((noinline, aligned(4)))
static void meLoop() {
  do {
    meDCacheWritebackInvalidAll();
  } while(!mem || !meStart);
  do {
    meCounter++;
  } while(!meExit);
}

extern char __start__me_section;
extern char __stop__me_section;
__attribute__((section("_me_section"), noinline, aligned(4)))
void meHandler() {
  vrg(0xbc100040) = 0x02; // mem access (64mb)
  vrg(0xbc100050) = 0x0f; // enable clocks: ME, AW bus RegA, RegB & Edram
  
  asm volatile(
    // clean status, disable interrupt
    "mtc0     $0, $12\n"
    "sync\n"
    // save context
    "addi     $sp, $sp, -16\n"
    "sw       $k0, 0($sp)\n"
    // setup meLoop
    "la       $k0, %0\n"
    "mtc0     $k0, $14\n"
    "sync\n"
    // restore context
    "lw       $k0, 0($sp)\n"
    "addi     $sp, $sp, 16\n"
    // exit to meLoop
    "eret\n"
    "nop\n"
    "nop\n"
    "nop\n"
    "nop\n"
    "nop\n"
    "nop\n"
    "nop\n"
    :
    : "i" (meLoop)
    : "k0", "memory"
  );
}


static int meInit() {
  #define me_section_size (&__stop__me_section - &__start__me_section)
  memcpy((void *)0xbfc00000, (void*)&__start__me_section, me_section_size);
  meDCacheWritebackInvalidAll();
  vrg(0xbc10004c) = 0b100;
  vrg(0xbc10004c) = 0x0;
  asm volatile("sync");
  return 0;
}

void quit(const char* const str) {
  pspDebugScreenClear();
  pspDebugScreenPrintf(str);
  sceKernelDelayThread(500000);
  sceKernelExitGame();
}

int main() {
  scePowerSetClockFrequency(333, 333, 166);

  pspDebugScreenInit();
  if (pspSdkLoadStartModule("ms0:/PSP/GAME/me/kcall.prx", PSP_MEMORY_PARTITION_KERNEL) < 0){
    quit("Can't load the PRX, exiting...");
    return 0;
  }

  mem = meGetUncached32(10);
  kcall(&meInit);
  meStart = true;
  
  SceCtrlData ctl;
  do {
    sceCtrlPeekBufferPositive(&ctl, 1);
    pspDebugScreenSetXY(0, 1);
    pspDebugScreenPrintf("%u   ", meCounter);
    sceDisplayWaitVblankStart();
  } while(!(ctl.Buttons & PSP_CTRL_HOME));
  
  meExit = true;
  meGetUncached32(0);
  quit("Exiting...");
  return 0;
}
