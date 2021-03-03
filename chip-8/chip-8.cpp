#include "chip-8.h"

CChip8<>::~CChip8() {
	//	Dispose of graphics
	delete[] m_graphics;

	//	Dispose of ram
	delete[] m_ram;
}

void CChip8<>::CheckGraphicsUpdate() {
	static decltype(m_screen_size) old_screen_size;
	memcpy(old_screen_size, m_screen_size, sizeof(m_screen_size));

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