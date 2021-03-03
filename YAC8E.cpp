#include "chip-8/chip-8.h"

#include <cstdio> //	printf

int main() {
	CChip8 my_emulator;

	my_emulator.Initialize();
	my_emulator.GetScreenSize();
	my_emulator.GetRam();
	my_emulator.GetStack();
	for (int i = 1;; ++i) {
		my_emulator.Update();
		if (i == 18) {
			my_emulator.ChangeScreenSize( 12, 24 );
		}
	}
}