#ifndef clox_common_h
#define clox_common_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// sjg - NAN Boxing allows all simple data types to fit in a 64 bit work. This may not be helpful down the road
// and likely why their is a distinct STRING type in clox vs, a string in NQQ as an object. NQQ does NOT use
// NAN boxing, likely we wont either so expect some changes with STRINGS
// 
// #define NAN_BOXING

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