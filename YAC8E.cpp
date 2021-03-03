#include "chip-8/chip-8.h"
#include <cstdio>

int main() {
	CChip8 my_emulator;
	
	my_emulator.Initialize();
	my_emulator.GetScreenSize();
	my_emulator.GetRam();
	my_emulator.GetStack();
}