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

	bool jump = false;

	BYTE x = GET_X(m_opcode);
	BYTE y = GET_Y(m_opcode);

	switch (INVERSE_NNN(m_opcode)) {
	case 0x0000:
		if (m_opcode == CLS) {
			LOG("$0x%x: CLS\n", m_program_counter);

			//	Clear graphics buffer
			memset(m_graphics, 0, sizeof(m_graphics));
		}

		if (m_opcode == RET) {
			LOG("$0x%x: RET\n", m_program_counter);

			if (m_stack[m_stack_pointer] > 0) {
				assert("Invalid return");
			}

			//	Return from subroutine
			m_program_counter = m_stack[m_stack_pointer];

			if (m_stack[m_stack_pointer] > 0) {
				//	Substract 1 from stack pointer
				--m_stack_pointer;
			}
		}
		break;

	case 0x1000:
		//	Jump to address
		LOG("$0x%x: JP, 0x%x\n", m_program_counter, GET_NNN(m_opcode));

		//	Check if we're in jump to avoid incrementing within a jump, thus 
		//	causing trouble within the program flow
		jump = true;

		//	Set PC to NNN
		m_program_counter = GET_NNN(m_opcode);
		break;

	case 0x2000:
		//	Call address
		LOG("$0x%x: CALL, 0x%x\n", m_program_counter, GET_NNN(m_opcode));

		if (m_stack[m_stack_pointer] != 0) {
			//	Increment the stack pointer
			++m_stack_pointer;
		}

		//	Place current PC on top of the stack
		m_stack[m_stack_pointer] = m_program_counter;

		//	Set PC to NNN
		m_program_counter = GET_NNN(m_opcode);

		break;

	case 0x3000:
		//	Skip next instruction if Vx == kk
		LOG("$0x%x: SE V%x, 0x%x\n", m_program_counter, m_general_purpose_registers[x], GET_KK(m_opcode));

		//	Compare register Vx to kk, and if they are equal, increments the program counter by 2.
		if (m_general_purpose_registers[x] & GET_KK(m_opcode)) {
			m_program_counter += 2;
		}

		break;

	case 0x4000:
		//	Skip next instruction if Vx != kk
		LOG("$0x%x: SNE V%x, 0x%x\n", m_program_counter, m_general_purpose_registers[x], GET_KK(m_opcode));

		//	Compare register Vx to kk, and if they are not equal, increments the program counter by 2.
		if (!(m_general_purpose_registers[x] & GET_KK(m_opcode))) {
			m_program_counter += 2;
		}
		
		break;

	default:
		break;
	}

	LOG("Computed operand 0x%X\n", m_opcode);

	if (!jump) {
		//	Advance in the program
		m_program_counter += 2;
	}
}
