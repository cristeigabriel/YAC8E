#include "chip-8/chip-8.h"

#include <cstdio> //	printf

int main() {
	CChip8 my_emulator;
	my_emulator.Initialize();

	for (;;) {
		my_emulator.ComputeByte();
		my_emulator.CheckGraphicsUpdate();
	}
}