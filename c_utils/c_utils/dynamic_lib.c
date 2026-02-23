#include "dynamic_lib.h"
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

dynamic_lib_t* dynamic_lib_open(const char *path, dynamic_lib_error_t *error) {
    if (!path) {
        if (error) *error = DYNAMIC_LIB_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    dynamic_lib_t *lib = calloc(1, sizeof(dynamic_lib_t));
    if (!lib) {
        if (error) *error = DYNAMIC_LIB_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    lib->handle = dlopen(path, RTLD_LAZY);
    if (!lib->handle) {
        free(lib);
        if (error) *error = DYNAMIC_LIB_ERROR_OPEN_FAILED;
        return NULL;
    }
    
    lib->path = strdup(path);
    lib->has_error = false;
    lib->error = DYNAMIC_LIB_OK;
    
    if (error) *error = DYNAMIC_LIB_OK;
    return lib;
}

bool dynamic_lib_close(dynamic_lib_t *lib, dynamic_lib_error_t *error) {
    if (!lib) {
        if (error) *error = DYNAMIC_LIB_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (lib->handle) {
        dlclose(lib->handle);
    }
    free(lib->path);
    free(lib);
    
    if (error) *error = DYNAMIC_LIB_OK;
    return true;
}

void* dynamic_lib_get_symbol(dynamic_lib_t *lib, const char *name, dynamic_lib_error_t *error) {
    if (!lib || !name) {
        if (error) *error = DYNAMIC_LIB_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    if (!lib->handle) {
        if (error) *error = DYNAMIC_LIB_ERROR_OPEN_FAILED;
        return NULL;
    }
    
    void *symbol = dlsym(lib->handle, name);
    if (!symbol) {
        if (error) *error = DYNAMIC_LIB_ERROR_SYMBOL_NOT_FOUND;
        return NULL;
    }
    
    if (error) *error = DYNAMIC_LIB_OK;
    return symbol;
}

bool dynamic_lib_is_valid(const dynamic_lib_t *lib) {
    return lib && lib->handle != NULL;
}

const char* dynamic_lib_get_path(const dynamic_lib_t *lib) {
    return lib ? lib->path : NULL;
}
