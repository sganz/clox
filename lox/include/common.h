#ifndef clox_common_h
#define clox_common_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NAN_BOXING
// Print a disassembly of a programs code after compilation and before execution
#define DEBUG_PRINT_CODE
// Print the opcode and stack at the beginning of every loop through the VM
#define DEBUG_TRACE_EXECUTION

//#define DEBUG_STRESS_GC
//#define DEBUG_LOG_GC

// Integer MAX used throughout codebase
// NQQ Added a UINT16 type so also include that here
#define UINT8_COUNT (UINT8_MAX + 1)
//#define UINT16_COUNT (UINT16_MAX + 1)

#endif