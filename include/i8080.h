#ifndef CPU_H
#define CPU_H

#include<stdint.h>
#include<stdbool.h>

// -- Register names --
enum Register {
    A,  // accumulator
    B,
    C,
    D,
    E,
    H,
    L,
    SP, // stack pointer
    M,  // memory reference
    PSW,
};

// -- System state --
typedef struct {
    bool z;
    bool s;
    bool p;
    bool cy;
    bool ac;
    //bool pad;
} Condition_codes;

typedef struct {
    uint8_t regs[7];  // registers
    uint16_t sp;      // stack pointer
    uint16_t pc;      //program counter
    uint8_t memory[0x4000]; // The system has 8K of ROM and 8K of RAM
    Condition_codes cc;
    uint8_t int_enable;
} Cpu_state;

// CPU API
uint8_t read_memory(Cpu_state *state, uint16_t address);
void write_memory(Cpu_state *state, uint16_t address, uint8_t value);
int interrupt(Cpu_state *state, uint16_t offset);
int exec_opcode(Cpu_state *state);

#endif