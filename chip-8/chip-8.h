#pragma once

//	Includes
#include <memory.h> //	memset
#include <utility> //	pair, swap
#include <assert.h>	//	assert
#include <vector> //	vector

//	Defines
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
	WORD program_safe_memory_start = PROGRAM_SAFE_MEMORY_START,
	WORD stack_size = STACK_SIZE>
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

		 LIST_SIZE //	16 general purpose 8-bit registers
	};

	enum _Screen {
		WIDTH,
		HEIGHT
	};
	
	//	Handlers
	template <BYTE base_w = 64, BYTE base_h = 32>
	bool Initialize() {
		m_has_been_initialized = false;

		m_program_counter = PROGRAM_SAFE_MEMORY_START;

		//	RAM
		m_ram.reserve(memory_cap);
		
		//	Stack
		m_stack.reserve(stack_size);

		//	Graphics
		m_graphics.reserve(base_w * base_h);

		//	The original implementation of the Chip-8 language used a 64x32-pixel 
		//	monochrome display
		m_screen_size[_Screen::WIDTH] = base_w;
		m_screen_size[_Screen::HEIGHT] = base_h;

		//	Null out register
		memset(m_general_purpose_registers, 0, _V::LIST_SIZE);

		return (m_has_been_initialized = true);
	};
	
	void Update();
	
	~CChip8();

	//	Get handle to ram
	inline const std::vector<BYTE>& GetRam() const {
		assert(m_has_been_initialized);
		return m_ram;
	}
	
	//	Get handle to stack
	inline const std::vector<WORD>& GetStack() const {
		assert(m_has_been_initialized);
		return m_stack;
	}

	//	Get graphics handle
	inline const std::vector<BYTE>& GetGraphics() {
		assert(m_has_been_initialized);
		return m_graphics;
	}

	//	Get screen size
	inline BYTE* GetScreenSize() {
		assert(m_has_been_initialized);
		return m_screen_size;
	}

	//	Change screen size
	inline void ChangeScreenSize(BYTE w, BYTE h) {
		assert(m_has_been_initialized);
		m_screen_size[_Screen::WIDTH] = w;
		m_screen_size[_Screen::HEIGHT] = h;
	}

private:
	//	Emulator status flag
	bool m_has_been_initialized;

	//	Emulator data
	BYTE m_screen_size[2];
	std::vector<BYTE> m_graphics;
	BYTE m_general_purpose_registers[_V::LIST_SIZE];
	std::vector<BYTE> m_ram;
	std::vector<WORD> m_stack;
	WORD m_opcode;

	
	//	 The program counter (PC) should be 16-bit, and is used to store the currently executing address.
	WORD m_program_counter;
};