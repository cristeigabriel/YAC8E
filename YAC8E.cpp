#include "chip-8/chip-8.h"

int main() {
	srand({});

	CChip8 my_emulator;
	my_emulator.Initialize();

	for (;;) {
		my_emulator.ComputeByte();
		my_emulator.ComputeInstruction();
	}
}