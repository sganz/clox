#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "chunk.h"
#include "table.h"
#include "value.h"

#define OBJ_TYPE(value)         (AS_OBJ(value)->type)

// sjg - IS_BOUND AND IS_CLASS not in NQQ
#define IS_BOUND_METHOD(value)  isObjType(value, OBJ_BOUND_METHOD)
#define IS_CLASS(value)         isObjType(value, OBJ_CLASS)
#define IS_CLOSURE(value)       isObjType(value, OBJ_CLOSURE)
#define IS_FUNCTION(value)      isObjType(value, OBJ_FUNCTION)
// sjg - IS_INSTANCE is not in NQQ
#define IS_INSTANCE(value)      isObjType(value, OBJ_INSTANCE)
#define IS_NATIVE(value)        isObjType(value, OBJ_NATIVE)
#define IS_STRING(value)        isObjType(value, OBJ_STRING)

// sjg - AS_BOUND and AS_CLASS not in NQQ
#define AS_BOUND_METHOD(value)  ((ObjBoundMethod*)AS_OBJ(value))
#define AS_CLASS(value)         ((ObjClass*)AS_OBJ(value))
#define AS_CLOSURE(value)      ((ObjClosure*)AS_OBJ(value))

#define AS_FUNCTION(value)     ((ObjFunction*)AS_OBJ(value))
#define AS_NATIVE(value) (((ObjNative*)AS_OBJ(value))->function)
#define AS_LIST(value)          ((ObjList*)AS_OBJ(value))
#define AS_MAP(value)           ((ObjMap*)AS_OBJ(value))
// sjg - AS_INSTANCE not in NQQ
#define AS_INSTANCE(value)     ((ObjInstance*)AS_OBJ(value))
#define AS_STRING(value)       ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)      (((ObjString*)AS_OBJ(value))->chars)

typedef enum {
    OBJ_BOUND_METHOD,
    OBJ_CLASS,
    OBJ_CLOSURE,
    OBJ_FUNCTION,
    OBJ_LIST,
    OBJ_MAP,
    OBJ_INSTANCE, // sjg - Not in NQQ
    OBJ_NATIVE,
    OBJ_STRING,
    OBJ_UPVALUE
} ObjType;

// sjg - NQQ had a subset, added back in the object stuff from
// the ObjType enum. Might be for debugging??

static const char* OBJ_TYPE_STRINGS[] = {
    "OBJ_BOUND_METHOD",
    "OBJ_CLASS",
    "OBJ_CLOSURE",
    "OBJ_FUNCTION",
    "OBJ_LIST",
    "OBJ_MAP",
    "OBJ_INSTANCE",
    "OBJ_NATIVE",
    "OBJ_STRING",
    "OBJ_UPVALUE"
};

// sjg - NQQ has this as sOBJ, may be a good change but for now leave CLOX
struct Obj {
    ObjType type;
    bool isMarked;
    struct Obj* next;
};

typedef struct {
    Obj obj;
    int arity;
    int upvalueCount;
    Chunk chunk;
    ObjString* name;
} ObjFunction;

// sjg - NQQ uses this for native functions, this might be better, but
// not sure of the return type, bool will be faster and always handleing
// the params of result and errMsg a better way. HOLD on this change for the 
// moment
// typedef bool (*NativeFn) (int argCount, Value* args, Value* result, char errMsg[]);

// CLOX default way for a native function. Might first expand this to the new
// params then change the return value 
typedef Value (*NativeFn) (int argcount, Value* args);

typedef struct {
    Obj obj;
    NativeFn function;
} ObjNative;


// sjg - NQQ Note: hash is cached for strings because a) strings are immutable so the hash
// will never change b) string lookups are a very frequent action in Clox and
// we want it to be as quick as possible.
// MOST of the OBJ functions are prefixed as sObjStrin in NQQ
struct ObjString {
    Obj obj;
    int length;
    char* chars;
    uint32_t hash;
};

typedef struct ObjUpvalue {
    Obj obj;
    Value* location;
    Value closed;
    struct ObjUpvalue* next;
} ObjUpvalue;

typedef struct {
    Obj obj;
    ObjFunction* function;
    ObjUpvalue** upvalues;
    int upvalueCount;
} ObjClosure;

typedef struct {
    Obj obj;
    int count;
    int capacity;
    Value* items;
} ObjList;

typedef struct {
    Obj obj;
    Table items;
} ObjMap;

// sjg - not in NQQ
typedef struct {
    Obj obj;
    ObjString* name;
    Table methods;
} ObjClass;

// sjg - not in NQQ
typedef struct {
    Obj obj;
    ObjClass* klass;
    Table fields;
} ObjInstance;

// sjg - not in NQQ
typedef struct {
    Obj obj;
    Value receiver;
    ObjClosure* method;
} ObjBoundMethod;

ObjBoundMethod* newBoundMethod(Value receiver, 
    ObjClosure* method);
ObjClass* newClass(ObjString* name);
ObjClosure* newClosure(ObjFunction* function);
ObjFunction* newFunction();
ObjInstance* newInstance(ObjClass* klass);
ObjNative* newNative(NativeFn function);
ObjString* takeString(char* chars, int length);
ObjUpvalue* newUpvalue(Value* slot);
ObjString* copyString(const char* chars, int length);

// sjg - New from NQQ
Value indexFromString(ObjString* string, int index);
bool isValidStringIndex(ObjString* list, int index);
ObjUpvalue* newUpvalue(Value* slot);
ObjList* newList();
void appendToList(ObjList* list, Value value);
void storeToList(ObjList* list, int index, Value value);
Value indexFromList(ObjList* list, int index);
void deleteFromList(ObjList* list, int index);
bool isValidListIndex(ObjList* list, int index);
ObjMap* newMap();
void printObject(Value value);

static inline bool isObjType(Value value, ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

// sjg - New from NQQ
static inline const char* stringFromObjType(ObjType type) {
    return OBJ_TYPE_STRINGS[type];
}
#endif