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

//	Resources
//	[Section 2.4 (Display) - Fonts](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#font)
constexpr static BYTE g_font_set[] = {
	//	0-9
	0xF0, 0x90, 0x90, 0x90, 0xF0,
	0x20, 0x60, 0x20, 0x20, 0x70,
	0xF0, 0x10, 0xF0, 0x80, 0xF0,
	0xF0, 0x10, 0xF0, 0x10, 0xF0,
	0x90, 0x90, 0xF0, 0x10, 0x10,
	0xF0, 0x80, 0xF0, 0x10, 0xF0,
	0xF0, 0x80, 0xF0, 0x90, 0xF0,
	0xF0, 0x10, 0x20, 0x40, 0x40,
	0xF0, 0x90, 0xF0, 0x90, 0xF0,
	0xF0, 0x90, 0xF0, 0x10, 0xF0,

	//	A-F
	0xF0, 0x90, 0xF0, 0x90, 0x90,
	0xE0, 0x90, 0xE0, 0x90, 0xE0,
	0xF0, 0x80, 0x80, 0x80, 0xF0,
	0xE0, 0x90, 0x90, 0x90, 0xE0,
	0xF0, 0x80, 0xF0, 0x80, 0xF0,
	0xF0, 0x80, 0xF0, 0x80, 0x80
};

//	Relevant information
//		nnn or addr - A 12 - bit value, the lowest 12 bits of the instruction
//		n or nibble - A 4 - bit value, the lowest 4 bits of the instruction
//		x - A 4 - bit value, the lower 4 bits of the high byte of the instruction
//		y - A 4 - bit value, the upper 4 bits of the low byte of the instruction
//		kk or byte - An 8 - bit value, the lowest 8 bits of the instruction

//	0xABCD & 0x0FFF = 0x(0)BCD
//	If you want to understand this very case, 
//	think of & as an alpha mask.

#define INVERSE_NNN(byte) byte & 0xF000
#define GET_NNN(byte) byte & 0x0FFF

#define INVERSE_N(byte) byte & 0xFFF0
#define INVERSE_NIBBLE(byte) INVERSE_N(byte)

#define GET_N(byte) byte & 0x000F
#define GET_NIBBLE(byte) GET_N(byte)

#define GET_X(byte) (byte & 0x0F00) >> 8;
#define GET_Y(byte) (byte & 0x00F0) >> 4;

#define INVERSE_KK(byte) byte & 0xFF00
#define INVERSE_BYTE(byte) INVERSE_KK(byte)

#define GET_KK(byte) byte & 0x00FF
#define GET_BYTE(byte) GET_KK(byte)

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
	
	constexpr struct _Instructions {
		enum NNN : WORD {
			SYS = 0x0000,
			JP_ADDR = 0x1000,
			CALL = 0x2000,
			LD = 0xA000,
			JP_VX_ADDR = 0xB000
		};
	};
	
	enum _Screen {
		WIDTH,
		HEIGHT
	};
	
	//	Handlers
	template <BYTE base_w = 64, BYTE base_h = 32>
	bool Initialize() {
		//	Ensure that it's in BYTE range and therefore
		//	that it'll also not overflow in program_safe_memory_start
		//
		//	This is not a predicted occurence since Chip-8 have their own font, 
		//	but who knows what our user might want to do?
		static_assert(sizeof(g_font_set) < 0xFF);

		m_has_been_initialized = false;

		//	PC and I
		m_program_counter = program_safe_memory_start;
		m_indice = 0;

		//	RAM
		m_ram = new BYTE[memory_cap];

		//	Graphics
		m_graphics = new BYTE[base_w * base_h];

		//	The original implementation of the Chip-8 language used a 64x32-pixel 
		//	monochrome display
		m_screen_size[_Screen::WIDTH] = base_w;
		m_screen_size[_Screen::HEIGHT] = base_h;

		//	Null out register
		for (BYTE i = 0; i < _V::LIST_SIZE; ++i) {
			m_general_purpose_registers[i] = 0;
		}

		//	Add font-set
		for (BYTE i = 0; i < sizeof(g_font_set); ++i) {
			m_ram[i] = g_font_set[i];
		}

		return (m_has_been_initialized = true);
	};
	
	void CheckGraphicsUpdate();
	
	~CChip8();

	//	Compute byte by getting operand
	inline void ComputeByte() {
		//	Fetch Byte
		m_opcode = m_ram[m_program_counter] << 8 | m_ram[m_program_counter + 1];
	}

	//	Fetch computed byte
	inline BYTE FetchByte() {
		return m_opcode;
	}

	//	Actual logic
	void ComputeInstruction();

	//	Get handle to ram
	inline BYTE* GetRam() {
		assert(m_has_been_initialized);
		return m_ram;
	}
	
	//	Get handle to stack
	inline WORD* GetStack() {
		assert(m_has_been_initialized);
		return m_stack;
	}

	//	Get graphics handle
	inline BYTE* GetGraphics() {
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
	BYTE* m_graphics;
	BYTE m_general_purpose_registers[_V::LIST_SIZE];
	BYTE* m_ram;
	WORD m_stack[stack_size];
	WORD m_opcode;
	
	//	 The program counter (PC) should be 16-bit, and is used to store the currently executing address.
	WORD m_program_counter;

	//	Instruction relevant reg
	WORD m_indice;
};