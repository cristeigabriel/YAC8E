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
		switch (m_opcode) {
		case CLS:
			LOG("$0x%X: CLS\n", m_program_counter);

			//	Clear graphics buffer
			memset(m_graphics, 0, sizeof(m_graphics));
			break;

		case RET:
			LOG("$0x%X: RET\n", m_program_counter);

			if (m_stack[m_stack_pointer] > 0) {
				assert("Invalid return");
			}
			
			//	Return from subroutine
			m_program_counter = m_stack[m_stack_pointer];

			if (m_stack[m_stack_pointer] > 0) {
				//	Substract 1 from stack pointer
				--m_stack_pointer;
			}
			break;

		default:
			break;
		}
		break;

	case 0x1000:
		//	Jump to address
		LOG("$0x%X: JP, 0x%X\n", m_program_counter, GET_NNN(m_opcode));

		//	Check if we're in jump to avoid incrementing within a jump, thus 
		//	causing trouble within the program flow
		jump = true;

		//	Set PC to NNN
		m_program_counter = GET_NNN(m_opcode);
		break;

	case 0x2000:
		//	Call address
		LOG("$0x%X: CALL, 0x%X\n", m_program_counter, GET_NNN(m_opcode));

		if (m_stack[m_stack_pointer] != 0) {
			//	Increment the stack pointer
			++m_stack_pointer;
		}

		//	Place current PC on top of the stack
		m_stack[m_stack_pointer] = m_program_counter;

		//	Set PC to NNN
		m_program_counter = GET_NNN(m_opcode);

		//	Set jump flag so we don't progress accidentally
		jump = true;
		break;

	case 0x3000:
		//	Skip next instruction if Vx == kk
		LOG("$0x%X: SE V%X, 0x%X\n", m_program_counter, m_general_purpose_registers[x], GET_KK(m_opcode));

		//	Compare register Vx to kk, and if they are equal, increments the program counter by 2.
		if (m_general_purpose_registers[x] & GET_KK(m_opcode)) {
			m_program_counter += 2;
		}
		break;

	case 0x4000:
		//	Skip next instruction if Vx != kk
		LOG("$0x%X: SNE V%X, 0x%X\n", m_program_counter, m_general_purpose_registers[x], GET_KK(m_opcode));

		//	Compare register Vx to kk, and if they are not equal, increments the program counter by 2.
		if (!(m_general_purpose_registers[x] & GET_KK(m_opcode))) {
			m_program_counter += 2;
		}		
		break;
		
	case 0x5000:
		//	Skip next instruction if Vx == Vy
		LOG("$0x%X: SE V%X, V%X\n", m_program_counter, m_general_purpose_registers[x], m_general_purpose_registers[y]);

		//	Compare register Vx to kk, and if they are not equal, increments the program counter by 2.
		if (m_general_purpose_registers[x] & m_general_purpose_registers[y]) {
			m_program_counter += 2;
		}
		break;

	case 0x6000:
		//	Set Vx = kk
		LOG("$0x%X: LD V%X, %X\n", m_program_counter, m_general_purpose_registers[x], GET_KK(m_opcode));

		m_general_purpose_registers[x] = GET_KK(m_opcode);
		break;

	case 0x7000:
		//	Set Vx = Vx + kk
		LOG("$0x%X: ADD V%X, %X\n", m_program_counter, m_general_purpose_registers[x], GET_KK(m_opcode));

		m_general_purpose_registers[x] += GET_KK(m_opcode);
		break;

	case 0x8000:
		switch (GET_N(m_opcode)) {
		case 0x0:
			//	Set Vx = Vy
			LOG("$0x%X: LD V%X, V%X\n", m_program_counter, m_general_purpose_registers[x], m_general_purpose_registers[y]);

			m_general_purpose_registers[x] = m_general_purpose_registers[y];
			break;

		case 0x1:
			//	Set Vx = Vx OR Vy
			LOG("$0x%X: OR V%X, V%X\n", m_program_counter, m_general_purpose_registers[x], m_general_purpose_registers[y]);

			m_general_purpose_registers[x] |= m_general_purpose_registers[y];
			break;

		case 0x2:
			//	Set Vx = Vx AND Vy
			LOG("$0x%X: AND V%X, V%X\n", m_program_counter, m_general_purpose_registers[x], m_general_purpose_registers[y]);

			m_general_purpose_registers[x] &= m_general_purpose_registers[y];
			break;

		case 0x3:
			//	Set Vx = Vx XOR Vy
			LOG("$0x%X: XOR V%X, V%X\n", m_program_counter, m_general_purpose_registers[x], m_general_purpose_registers[y]);

			m_general_purpose_registers[x] ^= m_general_purpose_registers[y];
			break;

		case 0x4:
			//	Set Vx = Vx + Vy, set VF = carry
			LOG("$0x%X: ADD V%X, V%X\n", m_program_counter, m_general_purpose_registers[x], m_general_purpose_registers[y]);

			//	Vx = Vx + Vy
			m_general_purpose_registers[x] = m_general_purpose_registers[x] + m_general_purpose_registers[y];
			
			//	Carry
			//	If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0
			m_general_purpose_registers[0xF] = (int)((m_general_purpose_registers[x] + m_general_purpose_registers[y]) > 0xFF);
			break;

		case 0x5:
			//	Set Vx = Vx - Vy, set VF = NOT borrow.
			LOG("$0x%X: SUB V%X, V%X\n", m_program_counter, m_general_purpose_registers[x], m_general_purpose_registers[y]);

			//	If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
			m_general_purpose_registers[0xF] = (int)(m_general_purpose_registers[x] > m_general_purpose_registers[y]);

			m_general_purpose_registers[x] -= m_general_purpose_registers[y];
			break;

		case 0x6:
			//	Set Vx = Vx SHR 1
			LOG("$0x%X: SHR V%X{, V%X}\n", m_program_counter, m_general_purpose_registers[x], m_general_purpose_registers[y]);

			//	If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
			m_general_purpose_registers[0xF] = (int)(m_general_purpose_registers[x] % 2 == 1);

			m_general_purpose_registers[x] /= 2;
			break;

		case 0x7:
			//	Set Vx = Vy - Vx, set VF = NOT borrow
			LOG("$0x%X: SUBN V%X, V%X\n", m_program_counter, m_general_purpose_registers[x], m_general_purpose_registers[y]);

			//	 If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
			m_general_purpose_registers[0xF] = (int)(m_general_purpose_registers[y] > m_general_purpose_registers[x]);

			m_general_purpose_registers[x] = m_general_purpose_registers[y] - m_general_purpose_registers[x];
			break;

		case 0xE:
			//	Set Vx = Vx SHL 1
			LOG("$0x%X: SHL V%X{, V%X}\n", m_program_counter, m_general_purpose_registers[x], m_general_purpose_registers[y]);

			//	If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
			m_general_purpose_registers[0xF] = (int)((m_general_purpose_registers[y] > 128) == 1);

			m_general_purpose_registers[x] *= 2;
			break;

		default:
			break;
		}
		break;

	case 0x9000:
		//	Skip next instruction if Vx != Vy.
		LOG("$0x%X: SNE V%X, V%X\n", m_program_counter, m_general_purpose_registers[x], m_general_purpose_registers[y]);
		
		if (!(m_general_purpose_registers[x] & m_general_purpose_registers[y])) {
			m_program_counter += 2;
		}
		break;

	default:
		break;
	}

	if (!jump) {
		//	Advance in the program
		m_program_counter += 2;
	}
}
