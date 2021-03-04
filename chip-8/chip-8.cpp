#include "chip-8.h"

CChip8<>::~CChip8() {
	assert(m_has_been_initialized);

	//	Dispose of graphics
	delete[] m_graphics;

	//	Dispose of ram
	delete[] m_ram;
}

void CChip8<>::CheckGraphicsUpdate() {
	assert(m_has_been_initialized);

	static decltype(m_screen_size) old_screen_size;
	memcpy_s(old_screen_size, sizeof(old_screen_size), m_screen_size, sizeof(m_screen_size));

	if (old_screen_size != m_screen_size) {
		auto graphics_size = m_screen_size[_Screen::WIDTH] * m_screen_size[_Screen::HEIGHT];
		BYTE* new_graphics = new BYTE[graphics_size];

		m_graphics = new_graphics;
		delete[] new_graphics;

		BYTE new_screen_size[2] =
		{
			m_screen_size[_Screen::WIDTH],
			m_screen_size[_Screen::HEIGHT]
		};

		std::swap(old_screen_size, new_screen_size);
	}
}

void CChip8<>::ComputeInstruction() {
	assert(m_has_been_initialized);

	//	NNN registers
	switch (INVERSE_NNN(m_opcode)) {
	case _Instructions::JP_ADDR:
		m_program_counter = GET_NNN(m_opcode);
		break;

	default:
		break;
	}
}
