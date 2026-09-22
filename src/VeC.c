#include "VeC.h"

static inline void *vec_element_at(Vector *v, size_t index) {
    return (char *)v->data + index * v->element_size;
}

void vec_get(Vector *v, size_t index, void *out) {
    assert(index < v->length);
    memcpy(out, vec_element_at(v, index), v->element_size);
}

void vec_set(Vector *v, size_t index, const void *replacement) {
    assert(index < v->length);
    memcpy(vec_element_at(v, index), replacement, v->element_size);
}

void vec_reserve(Vector *v, size_t new_capacity) {
    if (v->capacity >= new_capacity)
        return;

    void *new_data = reallocarray(v->data, new_capacity, v->element_size);

    assert(new_data != NULL);

    v->data = new_data;
    v->capacity = new_capacity;
}

void vec_chop(Vector *v) {
    void *chopped_data;
    if (v->capacity != v->length) {
        chopped_data = reallocarray(v->data, v->length, v->element_size);
        assert(chopped_data != NULL);

        v->data = chopped_data;
        v->capacity = v->length;
    }
}

Vector *vec_new(size_t element_size) {
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

void vec_push(Vector *v, const void *data) {
    size_t new_length = v->length + 1;

    if (new_length > v->capacity) {
        vec_reserve(v, v->capacity * 2);
    }
    memcpy(vec_element_at(v, v->length), data, v->element_size);

    v->length = new_length;
}

void vec_pop(Vector *v, void *out) {
    assert(v->length != 0);

    memcpy(out, vec_element_at(v, v->length - 1), v->element_size);

    --v->length;
}

void vec_insert(Vector *v, size_t index, const void *data) {
    assert(index <= v->length);

    size_t new_length = v->length + 1;
    if (new_length > v->capacity) {
        vec_reserve(v, v->capacity * 2);
    }

    // Shift the data by 1 to right.
    memmove(vec_element_at(v, index + 1), vec_element_at(v, index),
            (v->length - index) * v->element_size);

    memcpy(vec_element_at(v, index), data, v->element_size);

    v->length = new_length;
}

void vec_delete(Vector *v, size_t index, void *out) {
    assert(v->length > 0 && index < v->length);

    memcpy(out, vec_element_at(v, index), v->element_size);

    memmove(vec_element_at(v, index), vec_element_at(v, index + 1),
            (v->length - index) * v->element_size);

    --v->length;
}

void vec_clear(Vector *v) { v->length = 0; }
