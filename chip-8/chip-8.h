#pragma once

//	Includes
#include <utility> //	pair
#include <assert.h>	//	assert

//	Defines
#define EMULATOR_API volatile
//	The stack is an array of 16 16-bit values, used to store the address that the interpreter shoud return to when finished with a subroutine.
//	Chip-8 allows for up to 16 levels of nested subroutines.
#define STACK_SIZE	16

//	These computers typically were designed to use a television as a display,
//	had between 1 and 4K of RAM (4096 bytes)
#define MEMORY_CAP	4096

//	Most Chip-8 programs start at location 0x200 (512)
#define PROGRAM_SAFE_MEMORY_START	512

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

template <WORD memory_cap = MEMORY_CAP,
	WORD program_safe_memory_start = PROGRAM_SAFE_MEMORY_START>
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
		m_has_been_initialized = false;

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

		return (m_has_been_initialized = true);
	};

	~CChip8();

	//	Inlined methods
	//	Get handle to ram
	inline EMULATOR_API BYTE* GetRam() const {
		assert(m_has_been_initialized);
		return m_ram;
	}
	
	//	Get handle to stack
	inline EMULATOR_API WORD* GetStack() {
		assert(m_has_been_initialized);
		return m_stack;
	}

	//	Get screen size
	inline std::pair<BYTE, BYTE> GetScreenSize() const {
		assert(m_has_been_initialized);
		return std::make_pair(m_screen_size[_Screen::WIDTH],
			m_screen_size[_Screen::HEIGHT]);
	}

private:
	//	Emulator status flag
	bool m_has_been_initialized;

	//	Emulator data
	BYTE m_screen_size[2];
	EMULATOR_API BYTE m_general_purpose_registers[_V::LIST_SIZE];
	EMULATOR_API BYTE* m_ram;
	EMULATOR_API WORD m_stack[STACK_SIZE];
};