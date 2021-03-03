#include "chip-8.h"

CChip8<>::~CChip8() {
	//	Dispose of graphics
	m_graphics.shrink_to_fit();
	
	//	Dispose of stack
	m_stack.~vector();

	//	Dispose of ram
	m_ram.~vector();
}

void CChip8<>::Update() {
	static BYTE old_screen_size[2] =
	{
		m_screen_size[_Screen::WIDTH],
		m_screen_size[_Screen::HEIGHT]
	};

	if ((old_screen_size[_Screen::WIDTH] != m_screen_size[_Screen::WIDTH]) ||
		(old_screen_size[_Screen::HEIGHT] != m_screen_size[_Screen::HEIGHT])) {
		auto graphics_size = m_screen_size[_Screen::WIDTH] * m_screen_size[_Screen::HEIGHT];
		std::vector<BYTE> new_graphics;

		m_graphics = new_graphics;
		new_graphics.shrink_to_fit();

		BYTE new_screen_size[2] =
		{
			m_screen_size[_Screen::WIDTH],
			m_screen_size[_Screen::HEIGHT]
		};

		std::swap(old_screen_size, new_screen_size);
	}
}