#include "chip-8.h"

CChip8<>::~CChip8() {
	//	Dispose of allocated memory
	delete[] m_ram;
}