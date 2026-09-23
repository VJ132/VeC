#ifndef VEC_H
#define VEC_H

#include <stddef.h>

typedef struct {
    void *data;
    size_t length;
    size_t capacity;
    size_t element_size;
} Vector;

Vector *vec_new(size_t element_size);
bool vec_reserve(Vector *v, size_t new_size);
bool vec_chop(Vector *v);
void vec_free(Vector *v);
bool vec_push(Vector *v, const void *data);
bool vec_pop(Vector *v, void *out);
bool vec_get(Vector *v, size_t index, void *out);
bool vec_set(Vector *v, size_t index, const void *replacement);
bool vec_insert(Vector *v, size_t index, const void *data);
bool vec_delete(Vector *v, size_t index, void *out);
void vec_clear(Vector *v);

#endif // VEC_H
