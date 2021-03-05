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
	BYTE n = GET_N(m_opcode);

	LOG("C PC: 0x%X: C SP: 0x%X C S[SP]: 0x%X C OP: 0x%X | <<", m_program_counter,
		m_stack_pointer, m_stack[m_stack_pointer], m_opcode);

	switch (INVERSE_NNN(m_opcode)) {
	case 0x0000:
		switch (m_opcode) {
		case CLS:
			LOG_NO_FMT("CLS");

			//	Clear graphics buffer
			memset(m_graphics, 0, sizeof(m_graphics));
			break;

		case RET:
			LOG_NO_FMT("RET");

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
		LOG("JP, 0x%X", GET_NNN(m_opcode));

		//	Check if we're in jump to avoid incrementing within a jump, thus 
		//	causing trouble within the program flow
		jump = true;

		//	Set PC to NNN
		m_program_counter = GET_NNN(m_opcode);
		break;

	case 0x2000:
		//	Call address
		LOG("CALL, 0x%X", GET_NNN(m_opcode));

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
		LOG("SE V%X, 0x%X", x, GET_KK(m_opcode));

		//	Compare register Vx to kk, and if they are equal, increments the program counter by 2.
		if (m_general_purpose_registers[x] & GET_KK(m_opcode)) {
			m_program_counter += 2;
		}
		break;

	case 0x4000:
		//	Skip next instruction if Vx != kk
		LOG("SNE V%X, 0x%X", x, GET_KK(m_opcode));

		//	Compare register Vx to kk, and if they are not equal, increments the program counter by 2.
		if (!(m_general_purpose_registers[x] & GET_KK(m_opcode))) {
			m_program_counter += 2;
		}		
		break;
		
	case 0x5000:
		//	Skip next instruction if Vx == Vy
		LOG("SE V%X, V%X", x, y);

		//	Compare register Vx to kk, and if they are not equal, increments the program counter by 2.
		if (m_general_purpose_registers[x] & m_general_purpose_registers[y]) {
			m_program_counter += 2;
		}
		break;

	case 0x6000:
		//	Set Vx = kk
		LOG("LD V%X, %X", x, GET_KK(m_opcode));

		m_general_purpose_registers[x] = GET_KK(m_opcode);
		break;

	case 0x7000:
		//	Set Vx = Vx + kk
		LOG("ADD V%X, %X", x, GET_KK(m_opcode));

		m_general_purpose_registers[x] += GET_KK(m_opcode);
		break;

	case 0x8000:
		switch (GET_N(m_opcode)) {
		case 0x0:
			//	Set Vx = Vy
			LOG("LD V%X, V%X", x, y);

			m_general_purpose_registers[x] = m_general_purpose_registers[y];
			break;

		case 0x1:
			//	Set Vx = Vx OR Vy
			LOG("OR V%X, V%X", x, y);

			m_general_purpose_registers[x] |= m_general_purpose_registers[y];
			break;

		case 0x2:
			//	Set Vx = Vx AND Vy
			LOG("AND V%X, V%X", x, y);

			m_general_purpose_registers[x] &= m_general_purpose_registers[y];
			break;

		case 0x3:
			//	Set Vx = Vx XOR Vy
			LOG("XOR V%X, V%X", x, y);

			m_general_purpose_registers[x] ^= m_general_purpose_registers[y];
			break;

		case 0x4:
			//	Set Vx = Vx + Vy, set VF = carry
			LOG("ADD V%X, V%X", x, y);

			//	Vx = Vx + Vy
			m_general_purpose_registers[x] = m_general_purpose_registers[x] + m_general_purpose_registers[y];
			
			//	Carry
			//	If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0
			m_general_purpose_registers[0xF] = (int)((m_general_purpose_registers[x] + m_general_purpose_registers[y]) > 0xFF);
			break;

		case 0x5:
			//	Set Vx = Vx - Vy, set VF = NOT borrow.
			LOG("SUB V%X, V%X", x, y);

			//	If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
			m_general_purpose_registers[0xF] = (int)(m_general_purpose_registers[x] > m_general_purpose_registers[y]);

			m_general_purpose_registers[x] -= m_general_purpose_registers[y];
			break;

		case 0x6:
			//	Set Vx = Vx SHR 1
			LOG("SHR V%X{, V%X}", x, y);

			//	If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
			m_general_purpose_registers[0xF] = (int)(m_general_purpose_registers[x] % 2 == 1);

			m_general_purpose_registers[x] /= 2;
			break;

		case 0x7:
			//	Set Vx = Vy - Vx, set VF = NOT borrow
			LOG("SUBN V%X, V%X", x, y);

			//	 If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
			m_general_purpose_registers[0xF] = (int)(m_general_purpose_registers[y] > m_general_purpose_registers[x]);

			m_general_purpose_registers[x] = m_general_purpose_registers[y] - m_general_purpose_registers[x];
			break;

		case 0xE:
			//	Set Vx = Vx SHL 1
			LOG("SHL V%X{, V%X}", x, y);

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
		LOG("SNE V%X, V%X", x, y);
		
		if (!(m_general_purpose_registers[x] & m_general_purpose_registers[y])) {
			m_program_counter += 2;
		}
		break;

	case 0xA000:
		//	Set I = nnn
		LOG("LD I, %X", GET_NNN(m_opcode));

		m_indice = GET_NNN(m_opcode);
		break;

	case 0xB000:
		//	Jump to location nnn + V0
		LOG("JP V0, %X", GET_NNN(m_opcode));

		jump = true;

		m_program_counter = GET_NNN(m_opcode) + m_general_purpose_registers[0];
		break;

	case 0xC000:
		//	Set Vx = random byte AND kk
		LOG("RND V%X, %d", x, GET_NNN(m_opcode));

		m_general_purpose_registers[x] = rand() % 256 & GET_KK(m_opcode);
		break;

	case 0xD000:
		//	Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
		LOG("DRW %d, %d, %d", x, y, n);

		m_general_purpose_registers[0xF] = 0;
		for (WORD start = m_indice; start < m_indice + n; ++start) {
			LOG("[ RENDER AT %d %d ]", m_general_purpose_registers[x], m_general_purpose_registers[y]);
		}
		break;

	case 0xF000:
		switch (GET_KK(m_opcode)) {
		case 0x0007:
			//	Set Vx = delay timer value
			LOG("LD V%X, DT", x);

			m_delay_timer = m_general_purpose_registers[x];
			break;

		case 0x0015:
			//	Set delay timer = Vx
			LOG("LD DT, V%X", x);

			m_general_purpose_registers[x] = m_delay_timer;
			break;

		default:
			break;
		}

		break;
	default:
		break;
	}

	LOG(">> | N PC: 0x%X: N SP: 0x%X N S[SP]: 0x%X N OP: 0x%X\n", m_program_counter,
		m_stack_pointer, m_stack[m_stack_pointer], m_opcode);

	if (!jump) {
		//	Advance in the program
		m_program_counter += 2;
	}
}
