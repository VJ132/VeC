#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    void *data;
    size_t length;
    size_t capacity;
    size_t element_size;
} Vector;

Vector *vec_new(size_t element_size);
void vec_reserve(Vector *v, size_t new_size);
void vec_chop(Vector *v);
void vec_free(Vector *v);
void vec_push(Vector *v, const void *data);
void vec_pop(Vector *v, void *out);
void vec_get(Vector *v, size_t index, void *out);
void vec_set(Vector *v, size_t index, const void *replacement);
void vec_insert(Vector *v, size_t index, const void *data);
void vec_delete(Vector *v, size_t index, void *out);
void vec_clear(Vector *v);
