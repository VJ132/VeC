#include "../src/VeC.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------
 * Helpers
 * ------------------------------------------------------------ */

typedef struct {
    int id;
    double value;
} Item;

static void test_new(void) {
    printf("[TEST] vec_new\n");

    Vector *v = vec_new(sizeof(int));

    assert(v != NULL);
    assert(v->data != NULL);
    assert(v->length == 0);
    assert(v->capacity == 4);
    assert(v->element_size == sizeof(int));

    vec_free(v);

    /* Different element sizes */
    v = vec_new(sizeof(double));

    assert(v != NULL);
    assert(v->element_size == sizeof(double));

    vec_free(v);

    v = vec_new(sizeof(Item));

    assert(v != NULL);
    assert(v->element_size == sizeof(Item));

    vec_free(v);
}

static void test_push(void) {
    printf("[TEST] vec_push\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    int values[] = {10, 20, 30, 40, 50};

    for (size_t i = 0; i < 5; ++i)
        vec_push(v, &values[i]);

    assert(v->length == 5);
    assert(v->capacity == 8);

    for (size_t i = 0; i < 5; ++i) {
        int value;
        vec_get(v, i, &value);

        assert(value == values[i]);
    }

    vec_free(v);
}

static void test_push_exact_capacity(void) {
    printf("[TEST] vec_push exact capacity boundary\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    for (int i = 0; i < 4; ++i)
        vec_push(v, &i);

    assert(v->length == 4);
    assert(v->capacity == 4);

    int value = 99;
    vec_push(v, &value);

    assert(v->length == 5);
    assert(v->capacity == 8);

    vec_free(v);
}

static void test_pop(void) {
    printf("[TEST] vec_pop\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    int values[] = {10, 20, 30};

    for (size_t i = 0; i < 3; ++i)
        vec_push(v, &values[i]);

    int out;

    vec_pop(v, &out);
    assert(out == 30);
    assert(v->length == 2);

    vec_pop(v, &out);
    assert(out == 20);
    assert(v->length == 1);

    vec_pop(v, &out);
    assert(out == 10);
    assert(v->length == 0);

    vec_free(v);
}

static void test_get(void) {
    printf("[TEST] vec_get\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    int values[] = {100, 200, 300, 400};

    for (size_t i = 0; i < 4; ++i)
        vec_push(v, &values[i]);

    for (size_t i = 0; i < 4; ++i) {
        int out = 0;

        vec_get(v, i, &out);

        assert(out == values[i]);
    }

    vec_free(v);
}

static void test_set(void) {
    printf("[TEST] vec_set\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    int values[] = {10, 20, 30};

    for (size_t i = 0; i < 3; ++i)
        vec_push(v, &values[i]);

    int replacement = 999;

    vec_set(v, 0, &replacement);

    int out;
    vec_get(v, 0, &out);

    assert(out == 999);

    vec_set(v, 1, &replacement);
    vec_get(v, 1, &out);

    assert(out == 999);

    vec_set(v, 2, &replacement);
    vec_get(v, 2, &out);

    assert(out == 999);

    /* Length must not change */
    assert(v->length == 3);

    vec_free(v);
}

static void test_insert(void) {
    printf("[TEST] vec_insert\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    int a = 20;
    int b = 30;

    vec_push(v, &a);
    vec_push(v, &b);

    /* Insert at beginning */
    int first = 10;
    vec_insert(v, 0, &first);

    assert(v->length == 3);

    int expected[] = {10, 20, 30};

    for (size_t i = 0; i < 3; ++i) {
        int out;
        vec_get(v, i, &out);

        assert(out == expected[i]);
    }

    /* Insert in middle */
    int middle = 25;
    vec_insert(v, 2, &middle);

    int expected2[] = {10, 20, 25, 30};

    for (size_t i = 0; i < 4; ++i) {
        int out;
        vec_get(v, i, &out);

        assert(out == expected2[i]);
    }

    /* Insert at end */
    int last = 40;
    vec_insert(v, v->length, &last);

    int expected3[] = {10, 20, 25, 30, 40};

    for (size_t i = 0; i < 5; ++i) {
        int out;
        vec_get(v, i, &out);

        assert(out == expected3[i]);
    }

    vec_free(v);
}

static void test_insert_growth(void) {
    printf("[TEST] vec_insert causing growth\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    for (int i = 0; i < 4; ++i)
        vec_push(v, &i);

    assert(v->capacity == 4);

    int value = 999;

    vec_insert(v, 2, &value);

    assert(v->length == 5);
    assert(v->capacity == 8);

    int expected[] = {0, 1, 999, 2, 3};

    for (size_t i = 0; i < 5; ++i) {
        int out;

        vec_get(v, i, &out);

        assert(out == expected[i]);
    }

    vec_free(v);
}

static void test_delete(void) {
    printf("[TEST] vec_delete\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    int values[] = {10, 20, 30, 40, 50};

    for (size_t i = 0; i < 5; ++i)
        vec_push(v, &values[i]);

    int out;

    /* Delete first */
    vec_delete(v, 0, &out);

    assert(out == 10);
    assert(v->length == 4);

    int expected1[] = {20, 30, 40, 50};

    for (size_t i = 0; i < 4; ++i) {
        vec_get(v, i, &out);
        assert(out == expected1[i]);
    }

    /* Delete middle */
    vec_delete(v, 1, &out);

    assert(out == 30);
    assert(v->length == 3);

    int expected2[] = {20, 40, 50};

    for (size_t i = 0; i < 3; ++i) {
        vec_get(v, i, &out);
        assert(out == expected2[i]);
    }

    /* Delete last */
    vec_delete(v, 2, &out);

    assert(out == 50);
    assert(v->length == 2);

    int expected3[] = {20, 40};

    for (size_t i = 0; i < 2; ++i) {
        vec_get(v, i, &out);
        assert(out == expected3[i]);
    }

    vec_free(v);
}

static void test_reserve(void) {
    printf("[TEST] vec_reserve\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    assert(v->capacity == 4);

    vec_reserve(v, 100);

    assert(v->capacity == 100);
    assert(v->length == 0);

    /* Reserving less must do nothing */
    vec_reserve(v, 50);

    assert(v->capacity == 100);

    /* Data must survive reallocation */
    int value = 12345;
    vec_push(v, &value);

    vec_reserve(v, 200);

    int out;
    vec_get(v, 0, &out);

    assert(out == 12345);
    assert(v->capacity == 200);

    vec_free(v);
}

static void test_reserve_same_capacity(void) {
    printf("[TEST] vec_reserve same capacity\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    void *old_data = v->data;

    vec_reserve(v, v->capacity);

    assert(v->capacity == 4);
    assert(v->data == old_data);

    vec_free(v);
}

static void test_chop(void) {
    printf("[TEST] vec_chop\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    for (int i = 0; i < 8; ++i)
        vec_push(v, &i);

    assert(v->length == 8);
    assert(v->capacity == 8);

    vec_pop(v, &(int){0});
    vec_pop(v, &(int){0});
    vec_pop(v, &(int){0});

    assert(v->length == 5);
    assert(v->capacity == 8);

    vec_chop(v);

    assert(v->length == 5);
    assert(v->capacity == 5);

    for (size_t i = 0; i < 5; ++i) {
        int out;

        vec_get(v, i, &out);

        assert(out == (int)i);
    }

    vec_free(v);
}

static void test_chop_when_already_compact(void) {
    printf("[TEST] vec_chop already compact\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    for (int i = 0; i < 4; ++i)
        vec_push(v, &i);

    assert(v->length == v->capacity);

    void *old_data = v->data;

    vec_chop(v);

    assert(v->length == 4);
    assert(v->capacity == 4);
    assert(v->data == old_data);

    vec_free(v);
}

static void test_clear(void) {
    printf("[TEST] vec_clear\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    for (int i = 0; i < 10; ++i)
        vec_push(v, &i);

    size_t old_capacity = v->capacity;

    vec_clear(v);

    assert(v->length == 0);
    assert(v->capacity == old_capacity);

    /* Vector must still be usable */
    int value = 999;

    vec_push(v, &value);

    assert(v->length == 1);

    int out;
    vec_get(v, 0, &out);

    assert(out == 999);

    vec_free(v);
}

/* ------------------------------------------------------------
 * Generic struct test
 * ------------------------------------------------------------ */

static void test_struct(void) {
    printf("[TEST] generic struct\n");

    Vector *v = vec_new(sizeof(Item));
    assert(v != NULL);

    Item items[] = {{1, 1.5}, {2, 2.5}, {3, 3.5}};

    for (size_t i = 0; i < 3; ++i)
        vec_push(v, &items[i]);

    assert(v->length == 3);

    Item out;

    vec_get(v, 1, &out);

    assert(out.id == 2);
    assert(out.value == 2.5);

    Item replacement = {99, 99.99};

    vec_set(v, 1, &replacement);

    vec_get(v, 1, &out);

    assert(out.id == 99);
    assert(out.value == 99.99);

    Item inserted = {50, 50.5};

    vec_insert(v, 1, &inserted);

    vec_get(v, 1, &out);

    assert(out.id == 50);
    assert(out.value == 50.5);

    Item deleted;

    vec_delete(v, 1, &deleted);

    assert(deleted.id == 50);
    assert(deleted.value == 50.5);

    vec_free(v);
}

/* ------------------------------------------------------------
 * String pointer test
 * ------------------------------------------------------------ */

static void test_string_pointers(void) {
    printf("[TEST] char * string pointers\n");

    /*
     * The vector stores char * values, NOT the strings themselves.
     */
    Vector *v = vec_new(sizeof(char *));
    assert(v != NULL);

    char *strings[] = {"hello", "world", "VeC", "C programming",
                       "generic vector"};

    for (size_t i = 0; i < 5; ++i)
        vec_push(v, &strings[i]);

    assert(v->length == 5);

    for (size_t i = 0; i < 5; ++i) {
        char *out;

        vec_get(v, i, &out);

        assert(out == strings[i]);
        assert(strcmp(out, strings[i]) == 0);
    }

    /* Set */
    char *replacement = "replacement";

    vec_set(v, 2, &replacement);

    char *out;

    vec_get(v, 2, &out);

    assert(out == replacement);
    assert(strcmp(out, "replacement") == 0);

    /* Insert */
    char *inserted = "inserted";

    vec_insert(v, 1, &inserted);

    vec_get(v, 1, &out);

    assert(out == inserted);
    assert(strcmp(out, "inserted") == 0);

    /* Delete */
    char *deleted;

    vec_delete(v, 1, &deleted);

    assert(deleted == inserted);

    /* Pop */
    char *popped;

    vec_pop(v, &popped);

    assert(popped == strings[4]);
    assert(strcmp(popped, "generic vector") == 0);

    vec_free(v);
}

/* ------------------------------------------------------------
 * String pointers with dynamically allocated strings
 * ------------------------------------------------------------ */

static void test_owned_string_pointers(void) {
    printf("[TEST] dynamically allocated string pointers\n");

    Vector *v = vec_new(sizeof(char *));
    assert(v != NULL);

    char *a = strdup("alpha");
    char *b = strdup("beta");
    char *c = strdup("gamma");

    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);

    vec_push(v, &a);
    vec_push(v, &b);
    vec_push(v, &c);

    assert(v->length == 3);

    char *out;

    vec_get(v, 1, &out);

    assert(strcmp(out, "beta") == 0);

    char *deleted;

    vec_delete(v, 1, &deleted);

    assert(strcmp(deleted, "beta") == 0);

    /*
     * VeC does NOT own the strings.
     * We must free them ourselves.
     */
    free(deleted);
    free(a);
    free(c);

    vec_free(v);
}

/* ------------------------------------------------------------
 * Mixed operation stress test
 * ------------------------------------------------------------ */

static void test_mixed_operations(void) {
    printf("[TEST] mixed operations\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    for (int i = 0; i < 1000; ++i)
        vec_push(v, &i);

    assert(v->length == 1000);

    for (int i = 0; i < 500; ++i) {
        int out;

        vec_pop(v, &out);

        assert(out == 999 - i);
    }

    assert(v->length == 500);

    for (int i = 0; i < 100; ++i) {
        int value = -i;

        vec_insert(v, 0, &value);
    }

    assert(v->length == 600);

    for (int i = 0; i < 100; ++i) {
        int out;

        vec_delete(v, 0, &out);

        assert(out == -99 + i);
    }

    assert(v->length == 500);

    for (size_t i = 0; i < 500; ++i) {
        int out;

        vec_get(v, i, &out);

        assert(out == (int)i);
    }

    vec_free(v);
}

/* ------------------------------------------------------------
 * Main
 * ------------------------------------------------------------ */

int main(void) {
    printf("========== VeC TEST SUITE ==========\n\n");

    test_new();

    test_push();
    test_push_exact_capacity();

    test_pop();
    test_get();
    test_set();

    test_insert();
    test_insert_growth();

    test_delete();

    test_reserve();
    test_reserve_same_capacity();

    test_chop();
    test_chop_when_already_compact();

    test_clear();

    test_struct();

    test_string_pointers();
    test_owned_string_pointers();

    test_mixed_operations();

    printf("\n========== ALL TESTS PASSED ==========\n");

    return 0;
}
