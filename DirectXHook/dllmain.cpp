			/* @'   @+          @;         @@@@@         @@@@@,                  @
			@@  ;@+          @;         @,  @         @'  @,                  @           @
			@@, @@+ +@@@: @@@@; @@@@    @,  @ @; '@   @'  @, @@@@  @@@@ :@@@' @@@@, @@@@:@@@#
			@,@.@#+ ,. ## @  @; @  @    @@@@@ ,@ @:   @'  @, @` @` @  @ +@ :; @, @; @  @' @.
			@ @@:#+ '@@@# @  @; @@@@    @,  @  @ @    @'  @, @  @` @@@@  @@@  @  @; @  @' @.
			@ '@ #+ @. @# @  @; @  .    @,  @  '@     @;  @, @  @` @  . `` +@ @  @; @  @' @.
			@    #+ @@@@# @@@@; @@@@    @@@@@ :@:     @@@@@. @  @. @@@@ '@@@@ @` @; @@@@' @@+ */
#include <Windows.h>
#include "detours.h"
#include "Patternscaning.h"
#include <d3d9.h>
#include <iostream>




typedef HRESULT(__stdcall * f_EndScene)(IDirect3DDevice9 * pDevice); // our function prototype 
f_EndScene oEndScene; // original endscene



HRESULT __stdcall Hooked_EndScene(IDirect3DDevice9 * pDevice) // our hooked endscene
{
	D3DRECT BarRect = { 100, 100, 200, 200 };
	pDevice->Clear(1, &BarRect, D3DCLEAR_TARGET, D3DCOLOR_ARGB(1, 1, 1, 1), 0.0f, 0); // was black because its upp to 255 and not 0 to 1 :')

	return oEndScene(pDevice); // call original ensdcene so the game can draw
}


DWORD WINAPI MainThread(LPVOID param) // our main thread
{
	static DWORD DirectXDevice = NULL; // our device address. made it static but you can make it just standard DWORD doesent mather.

	while (!DirectXDevice) // while loop so it tries to get the device untill it gets it.
		DirectXDevice = **(DWORD**)(FindPattern("shaderapidx9.dll", "A1 ?? ?? ?? ?? 50 8B 08 FF 51 0C") + 0x1); // geting the device by patternscaning


	void ** pVTable = *reinterpret_cast<void***>(DirectXDevice); // getting the vtable array
	oEndScene = (f_EndScene)DetourFunction((PBYTE)pVTable[42], (PBYTE)Hooked_EndScene); // hooking the endscene and putting a jump to our function and then storing original in oEndscene
	std::cout << oEndScene; // just writeing it to console to see if it hooked.


	return false; 
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: // gets runned when injected
		AllocConsole(); // enables the console
		freopen("CONIN$", "r", stdin); // makes it possible to output to output to console with cout.
		freopen("CONOUT$", "w", stdout);
		CreateThread(0, 0, MainThread, hModule, 0, 0); // creates our thread 
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

