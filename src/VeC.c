#include "VeC.h"

void vec_display_metadata(Vector *v) {
    printf("Length: %zu\n", v->length);
    printf("Capacity: %zu\n", v->capacity);
    printf("Element Size (In Bytes): %zu\n", v->element_size);
}

void vec_display_elements_as_int(Vector *v) {
    int value;
    printf("IntVec: [");
    for (size_t i = 0; i < v->length; ++i) {
        memcpy(&value, (char *)v->data + i * sizeof(int), sizeof(int));

        if (i == v->length - 1)
            printf("%d]\n", value);
        else
            printf("%d, ", value);
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

void vec_push(Vector *v, void *data) {
    size_t new_length = v->length + 1;

    if (new_length > v->capacity) {
        size_t new_capacity = v->capacity * 2;

        void *new_data = reallocarray(v->data, new_capacity, v->element_size);

        assert(new_data != NULL);

        v->data = new_data;
        v->capacity = new_capacity;
    }
    memcpy((char *)v->data + v->length * v->element_size, data,
           v->element_size);

    v->length = new_length;
}
