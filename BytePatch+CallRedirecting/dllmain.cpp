// dllmain.cpp : Defines the entry point for the DLL application.
// created by Ege Demirbas - Egerion
// selected game to demonstrate hooking and patching is Black & White 2. 

#include "pch.h"
#include "patch.h"
#include "windows.h"

/*
* CALL REDIRECTING 
* 
.text:00588B00 F3 0F 10 44 24 04            movss   xmm0, [esp + arg_0]
.text:00588B06 F3 0F 10 0D E4 88 C1 00      movss   xmm1, ds:flt_C188E4
.text : 00588B0E 0F 2F C8                   comiss  xmm1, xmm0
.text:00588B11 F3 0F 11 81 F0 01 00 00      movss   dword ptr[ecx + 1F0h], xmm0
.text:00588B19 77 0D                        ja      short loc_588B28
.text:00588B1B F3 0F 10 0D 28 33 C1 00      movss   xmm1, ds:flt_C13328
.text : 00588B23 0F 2F C1                   comiss  xmm0, xmm1
.text:00588B26 76 08                        jbe     short locret_588B30
*/

//there is a function in the game called GAlignment::CrudeSet, which sets the player's initial 
//alignment value. And originally it's set to zero, which means neutral alignment. 
//we can redirect this initial call and set our alignment to say, super evil. Alignment values are 
//fluctioating in the range of -1.0 and 1.0.Here, we're making ourselves super evil. Now when you 
//start to new game, your initial alignment will be super evil.
//super cool part is, we can directly define our float value and use that in asm!

float SuperEvil = -1.0 ,superGood = 1.0, neutral = 0.0;

__declspec(naked) void __cdecl GAlignmentCrudeSet()
{
    __asm
    {
        movss   xmm1, SuperEvil
        movss   dword ptr[ecx + 0x1F0], xmm1

        ret 4
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        //do stuff here 

        uintptr_t moduleBase;
        moduleBase  = (uintptr_t)GetModuleHandle(NULL); 

        //some examples 
        //unlock tiger creature
        patch::Patch((BYTE*)(moduleBase + 0x333B1C), (BYTE*)"\x6A\x01", 2);

        /*
        .text:  00789EAE 8B 16                      mov     edx, [esi]
        .text : 00789EB0 8B CE                      mov     ecx, esi
        .text : 00789EB2 FF 52 34                   call    dword ptr[edx + 34h]
        .text : 00789EB5 8B 4E 40                   mov     ecx, [esi + 40h]
        .text : 00789EB8 83 B9 1C 02 00 00 03       cmp     dword ptr[ecx + 21Ch], 3; Get Tribe Type
        .text:00789EBF 75 1C                        jnz     short loc_789EDD
        */
        //we're trying to avoid "cmp dword ptr[ecx + 21Ch], 3" (which means in c, if my tribe type is == 3) parameter to apply functional features to all tribes town centers.
        //check the enum list and you'll see that 3 represents Greek tribe. we need to remove this jump operand to avoid tribe comparison.
        
        //makes every town center functional 
        patch::Patch((BYTE*)(moduleBase + 0x389EBF), (BYTE*)"\x90\x90", 2);

        //call redirecting example 
        //will only work if you call while launching the game.
        call::RedirectCall(0x1BAA81, NULL, &GAlignmentCrudeSet);

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

