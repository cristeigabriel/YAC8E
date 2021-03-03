#pragma once

//	Defines
#define EMULATOR_API volatile

//	Type definitions
//	0x00 - 0xFF
using BYTE = unsigned char;

//	0x00 - 0xFFFF
using WORD = unsigned short;

//	=======================================================================================
//	TODO:
//		When implementing instructions, remember to have checks for range
//		of 0x000 to 0x1FF, for this, throw a warning that you're modifying
//		within interpreter range, but from 0x1FF to 0x200, say you're within
//		program non-safe range
//	=======================================================================================

//	These computers typically were designed to use a television as a display,
//	had between 1 and 4K of RAM (4096 bytes)
template <WORD memory_cap = 4096,
//	Most Chip-8 programs start at location 0x200 (512)
WORD program_safe_memory_start = 512>
class CChip8 {
public:
	//	Prototype
	CChip8() = default;
	
	//	Chip-8 has 16 general purpose 8-bit registers, usually referred to as Vx, where x is a hexadecimal digit (0 through F). There is also a 16-bit register called I. This register is generally used to store memory addresses, 
	//	so only the lowest (rightmost) 12 bits are usually used.
	enum _V : BYTE {
		 _0,
		 _1,
		 _2,
		 _3,
		 _4,
		 _5,
		 _6,
		 _7,
		 _8,
		 _9,
		 _A,
		 _B,
		 _C,
		 _D,
		 _E,

		 //	The VF register should not be used by any program,
		 //	as it is used as a flag by some instructions. See section 3.0, [Instructions](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.0) for details.

		 //	Deprecated for general use!
		 _F, 

		 LIST_SIZE, //	16 general purpose 8-bit registers
	};

	enum _Screen {
		WIDTH,
		HEIGHT
	};

	//	Handlers
	template <BYTE base_w = 64, BYTE base_h = 32>
	bool Initialize() {
		//	Memory
		//	The first 512 bytes, from 0x000 to 0x1FF, are where the original interpreter was located,
		//	and should not be used by programs.
		m_ram = new BYTE[memory_cap];

		//	The original implementation of the Chip-8 language used a 64x32-pixel 
		//	monochrome display
		m_screen_size[_Screen::WIDTH] = base_w;
		m_screen_size[_Screen::HEIGHT] = base_h;

		//	Null out registers
		for (BYTE i = 0; i < _V::LIST_SIZE; ++i) {
			m_general_purpose_registers[i] = 0;
		}

		return true;
	};

	~CChip8();

private:
	BYTE m_screen_size[2];
	EMULATOR_API BYTE m_general_purpose_registers[_V::LIST_SIZE];
	EMULATOR_API BYTE* m_ram;

	//	The stack is an array of 16 16-bit values, used to store the address that the interpreter shoud return to when finished with a subroutine.
	//	Chip-8 allows for up to 16 levels of nested subroutines.
	EMULATOR_API WORD m_stack[16];
};