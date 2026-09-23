#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "VeC.h"

static inline void *vec_element_at(Vector *v, size_t index) {
    return (char *)v->data + index * v->element_size;
}

bool vec_get(Vector *v, size_t index, void *out) {
    if (index >= v->length)
        return false;

    memcpy(out, vec_element_at(v, index), v->element_size);
    return true;
}

bool vec_set(Vector *v, size_t index, const void *replacement) {
    if (index >= v->length)
        return false;

    memcpy(vec_element_at(v, index), replacement, v->element_size);
    return true;
}

bool vec_reserve(Vector *v, size_t new_capacity) {
    if (v->capacity >= new_capacity)
        return true;

    void *new_data = reallocarray(v->data, new_capacity, v->element_size);

    if (new_data == NULL)
        return false;

    v->data = new_data;
    v->capacity = new_capacity;

    return true;
}

bool vec_chop(Vector *v) {
    if (v->length == 0) {
        free(v->data);
        v->data = NULL;
        v->capacity = 0;
        return true;
    }

    void *chopped_data;
    if (v->capacity != v->length) {
        chopped_data = reallocarray(v->data, v->length, v->element_size);
        if (chopped_data == NULL)
            return false;

        v->data = chopped_data;
        v->capacity = v->length;
    }
    return true;
}

Vector *vec_new(size_t element_size) {
    if (element_size == 0)
        return NULL;

    Vector *v = malloc(sizeof(Vector));

    if (v == NULL)
        return NULL;

    v->data = malloc(element_size * 4);
    if (v->data == NULL) {
        free(v);
        return NULL;
    }

    v->length = 0;
    v->capacity = 4;
    v->element_size = element_size;

    return v;
}

void vec_free(Vector *v) {
    free(v->data);
    free(v);
}

bool vec_push(Vector *v, const void *data) {
    size_t new_length = v->length + 1;

    if (new_length > v->capacity) {
        size_t new_capacity = v->capacity == 0 ? 2 : v->capacity * 2;

        if (!vec_reserve(v, new_capacity))
            return false;
    }
    memcpy(vec_element_at(v, v->length), data, v->element_size);

    v->length = new_length;

    return true;
}

bool vec_pop(Vector *v, void *out) {
    if (v->length == 0)
        return false;

    memcpy(out, vec_element_at(v, v->length - 1), v->element_size);

    --v->length;

    return true;
}

bool vec_insert(Vector *v, size_t index, const void *data) {
    if (index > v->length)
        return false;

    size_t new_length = v->length + 1;

    if (new_length > v->capacity) {
        size_t new_capacity = v->capacity == 0 ? 2 : v->capacity * 2;

        if (!vec_reserve(v, new_capacity))
            return false;
    }

    if (index < v->length) {
        // Shift the data by 1 to right.
        memmove(vec_element_at(v, index + 1), vec_element_at(v, index),
                (v->length - index) * v->element_size);
    }

    memcpy(vec_element_at(v, index), data, v->element_size);

    v->length = new_length;

    return true;
}

bool vec_delete(Vector *v, size_t index, void *out) {
    if (v->length > 0 && index < v->length) {

        memcpy(out, vec_element_at(v, index), v->element_size);

        memmove(vec_element_at(v, index), vec_element_at(v, index + 1),
                (v->length - index - 1) * v->element_size);

        --v->length;

        return true;
    }
    return false;
}

void vec_clear(Vector *v) { v->length = 0; }
