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
void vec_free(Vector *v);
void vec_push(Vector *v, void *data);
void vec_display_elements_as_int(Vector *v);
void vec_display_metadata(Vector *v);
