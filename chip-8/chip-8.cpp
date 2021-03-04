#include "chip-8.h"

CChip8<>::~CChip8() {
	assert(m_has_been_initialized);

	//	Dispose of graphics
	delete[] m_graphics;

	//	Dispose of ram
	delete[] m_ram;
}

void CChip8<>::ComputeInstruction() {
	assert(m_has_been_initialized);
		
	//	NNN registers
	switch (INVERSE_NNN(m_opcode)) {
	case _Instructions::JP_ADDR:
		LOG("$0x%x: JP, 0x%x\n", m_program_counter, GET_NNN(m_opcode));
		m_program_counter = GET_NNN(m_opcode);
		break; 

	default:
		break;
	}

	LOG("Computed operand 0x%X\n", m_opcode);
}
