#include <stdlib.h>

#include "compiler.h"
#include "common.h"
#include "memory.h"
#include "vm.h"

#ifdef DEBUG_LOG_GC
#include <stdio.h>
#include "debug.h"
#endif

#define GC_HEAP_GROW_FACTOR 2

// sjg - NQQ uses a different name for pointer parameter
void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    vm.bytesAllocated += newSize - oldSize;
    if (newSize > oldSize) {
#ifdef DEBUG_STRESS_GC
    collectGarbage();
#endif
        if (vm.bytesAllocated > vm.nextGC) {
            collectGarbage();
        }
    }
    
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }

    // This return in the clox will exit the interpreter if
    // can't realloc(). In NQQ it may be handled later. NOTE
    // might need to remove the check and pass back the result of
    // the realloc directly depending how the error is handled.
    // Ususally this is a super fatal error.
    void* result = realloc(pointer, newSize);
    
    if(result == NULL) 
        exit(1);

    return result;
}

void markObject(Obj* object) {
    if (object == NULL) return;
    if (object->isMarked) return;
#ifdef DEBUG_LOG_GC
    printf("%p mark ", (void*)object);
    printValue(OBJ_VAL(object));
    printf("\n");
#endif

    object->isMarked = true;
    
    // Add object to working list
    if (vm.grayCapacity < vm.grayCount + 1) {
        vm.grayCapacity = GROW_CAPACITY(vm.grayCapacity);
        
        // Use system realloc instead of our custom version
        // We don't want to recursively garbage collect
        // sjg - NQQ does not type the return from realloc()

        vm.grayStack = (Obj**)realloc(vm.grayStack, sizeof(Obj*) * vm.grayCapacity);
        // sjg - NQQ doesn't seem to check return of realloc()

        if (vm.grayStack == NULL) exit(1);
    }

    vm.grayStack[vm.grayCount++] = object;
}

void markValue(Value value) {
    // sjg - NQQ Checks tp see if an object , if not do nothing
    if (!IS_OBJ(value))
        return;
    markObject(AS_OBJ(value));
}

static void markArray(ValueArray* array) {
    for (int i = 0; i < array->count; i++) {
        markValue(array->values[i]);
    }
}

static void blackenObject(Obj* object) {
#ifdef DEBUG_LOG_GC
    printf("%p blacken ", (void*)object);
    printValue(OBJ_VAL(object));
    printf("\n");
#endif

    switch (object->type) {
        case OBJ_BOUND_METHOD: {    // sjg - not in NQQ
            ObjBoundMethod* bound = (ObjBoundMethod*)object;
            markValue(bound->receiver);
            markObject((Obj*)bound->method);
            break;
        }
        case OBJ_CLASS: {    // sjg - not in NQQ
            ObjClass* klass = (ObjClass*)object;
            markObject((Obj*)klass->name);
            markTable(&klass->methods);
            break;
        }
        case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*)object;
            markObject((Obj*)closure->function);
            for (int i = 0; i < closure->upvalueCount; i++) {
                markObject((Obj*)closure->upvalues[i]);
            }
            break;
        }
        case OBJ_FUNCTION: {
            ObjFunction* function = (ObjFunction*)object;
            markObject((Obj*)function->name);
            markArray(&function->chunk.constants);
            break;
        }
        case OBJ_UPVALUE: {
            markValue(((ObjUpvalue*)object)->closed);
            break;
        }
        case OBJ_LIST: {
            ObjList* list = (ObjList*)object;
            for (int i = 0; i < list->count; i++) {
                markValue(list->items[i]);
            }
            break;
        }
        case OBJ_MAP: {
            ObjMap* map = (ObjMap*)object;
            markTable(&map->items);
            break;
        }
        case OBJ_INSTANCE: {
            ObjInstance* instance = (ObjInstance*)object;
            markObject((Obj*)instance->klass);
            markTable(&instance->fields);
            break;
        }
        case OBJ_NATIVE:
        case OBJ_STRING:
            break;
    }
}



static void freeObject(Obj* object) {
#ifdef DEBUG_LOG_GC
    // sjg - NQQ Stringify the object->type, seem helpful, but unknown object
    // can crash the stringFromQbjType() call
    printf("%p free type %s\n", (void*)object, stringFromObjType(object->type));
    // printf("%p free type %d\n", (void*)object, object->type);
#endif

    switch (object->type) {
        case OBJ_BOUND_METHOD:
            FREE(ObjBoundMethod, object);
            break;        
        case OBJ_CLASS: {
            ObjClass* klass = (ObjClass*)object;
            freeTable(&klass->methods);
            FREE(ObjClass, object);
            break;
        }
        case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*)object;
            FREE_ARRAY(ObjUpvalue*, closure->upvalues, closure->upvalueCount);
            FREE(ObjClosure, object);
            break;
        }
        case OBJ_FUNCTION: {
            ObjFunction* function = (ObjFunction*)object;
            freeChunk(&function->chunk);
            FREE(ObjFunction, object);
            break;
        }
        case OBJ_INSTANCE: {
            ObjInstance* instance = (ObjInstance*)object;
            freeTable(&instance->fields);
            FREE(ObjInstance, object);
            break;
        }
        case OBJ_NATIVE: 
            FREE(ObjNative, object);
            break;
        case OBJ_STRING: {
            ObjString* string = (ObjString*)object;
            FREE_ARRAY(char, string->chars, string->length + 1);
            FREE(ObjString, object);
            break;
        }
        case OBJ_UPVALUE: {
            FREE(ObjUpvalue, object);
            break;
        }
        // sjg - next 2 are new from NQQ
        case OBJ_LIST: {
            ObjList* list = (ObjList*)object;
            FREE_ARRAY(Value*, list->items, list->count);
            FREE(ObjList, object);
            break;
        }
        case OBJ_MAP: {
            ObjMap* map = (ObjMap*)object;
            freeTable(&map->items);
            FREE(ObjMap, object);
            break;
        }
    }
}

static void markRoots() {
    // Stack (locals & temporaries)    
    for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
        markValue(*slot);
    }

    // Closures
    for (int i = 0; i < vm.frameCount; i++) {
        markObject((Obj*)vm.frames[i].closure);
    }

    // Open Upvalues
    for (ObjUpvalue* upvalue = vm.openUpvalues; upvalue != NULL; upvalue = upvalue->next) {
        markObject((Obj*)upvalue);
    }

    // Globals
    markTable(&vm.globals);
    // Compiler 
    markCompilerRoots();
    
    // sjg - Not in NQQ, looks to be removed from the 
    // vm object.
    markObject((Obj*)vm.initString);
}

static void traceReferences() {
    while (vm.grayCount > 0) {
        Obj* object = vm.grayStack[--vm.grayCount];
        blackenObject(object);
    }
}

static void sweep() {
    Obj* previous = NULL;
    Obj* object = vm.objects;
    while (object != NULL) {
        if (object->isMarked) {
            // Do not reclaim marked objects
            object->isMarked = false;
            previous = object;
            object = object->next;
        } else {
            Obj* unreached = object;

            object = object->next;
        if (previous != NULL) {
            previous->next = object;
        } else {
            vm.objects = object;
        }

        freeObject(unreached);
        }
    }
}

void collectGarbage() {
#ifdef DEBUG_LOG_GC
    printf("-- gc begin\n");
    size_t before = vm.bytesAllocated;
#endif

    markRoots();
    traceReferences();
    tableRemoveWhite(&vm.strings);
    sweep();

    vm.nextGC = vm.bytesAllocated * GC_HEAP_GROW_FACTOR;

#ifdef DEBUG_LOG_GC
    printf("-- gc end\n");
    // sjg - NQQ has %ld for the formatters, not sure if correct
    // TODO : check formatter
    printf("   collected %zu bytes (from %zu to %zu) next at %zu\n",
            before - vm.bytesAllocated, before, vm.bytesAllocated,
            vm.nextGC);   
#endif
}

void freeObjects() {
    Obj* object = vm.objects;
    while (object != NULL) {
        Obj* next = object->next;
        freeObject(object);
        object = next;
    }

    free(vm.grayStack);
}