#include "../src/VeC.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    double value;
} Item;

/* ------------------------------------------------------------
 * Helpers
 * ------------------------------------------------------------ */

static void assert_int_vector(Vector *v, const int *expected, size_t length) {
    assert(v->length == length);

    for (size_t i = 0; i < length; ++i) {
        int out = 0;
        assert(vec_get(v, i, &out));
        assert(out == expected[i]);
    }
}

/* ------------------------------------------------------------
 * Construction and destruction
 * ------------------------------------------------------------ */

static void test_new(void) {
    printf("[TEST] vec_new\n");

    assert(vec_new(0) == NULL);

    Vector *v = vec_new(sizeof(int));

    assert(v != NULL);
    assert(v->data != NULL);
    assert(v->length == 0);
    assert(v->capacity == 4);
    assert(v->element_size == sizeof(int));

    vec_free(v);

    v = vec_new(sizeof(double));

    assert(v != NULL);
    assert(v->element_size == sizeof(double));

    vec_free(v);

    v = vec_new(sizeof(Item));

    assert(v != NULL);
    assert(v->element_size == sizeof(Item));

    vec_free(v);
}

/* ------------------------------------------------------------
 * Push
 * ------------------------------------------------------------ */

static void test_push(void) {
    printf("[TEST] vec_push\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    int values[] = {10, 20, 30, 40, 50};

    for (size_t i = 0; i < 5; ++i)
        assert(vec_push(v, &values[i]));

    assert(v->length == 5);
    assert(v->capacity == 8);

    assert_int_vector(v, values, 5);

    vec_free(v);
}

static void test_push_exact_capacity(void) {
    printf("[TEST] vec_push exact capacity boundary\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    for (int i = 0; i < 4; ++i)
        assert(vec_push(v, &i));

    assert(v->length == 4);
    assert(v->capacity == 4);

    int value = 99;
    assert(vec_push(v, &value));

    assert(v->length == 5);
    assert(v->capacity == 8);

    vec_free(v);
}

/* ------------------------------------------------------------
 * Pop
 * ------------------------------------------------------------ */

static void test_pop(void) {
    printf("[TEST] vec_pop\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    int values[] = {10, 20, 30};

    for (size_t i = 0; i < 3; ++i)
        assert(vec_push(v, &values[i]));

    int out = 0;

    assert(vec_pop(v, &out));
    assert(out == 30);
    assert(v->length == 2);

    assert(vec_pop(v, &out));
    assert(out == 20);
    assert(v->length == 1);

    assert(vec_pop(v, &out));
    assert(out == 10);
    assert(v->length == 0);

    vec_free(v);
}

/* ------------------------------------------------------------
 * Get / Set
 * ------------------------------------------------------------ */

static void test_get(void) {
    printf("[TEST] vec_get\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    int values[] = {100, 200, 300, 400};

    for (size_t i = 0; i < 4; ++i)
        assert(vec_push(v, &values[i]));

    assert_int_vector(v, values, 4);

    vec_free(v);
}

static void test_set(void) {
    printf("[TEST] vec_set\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    int values[] = {10, 20, 30};

    for (size_t i = 0; i < 3; ++i)
        assert(vec_push(v, &values[i]));

    int replacement = 999;
    int out = 0;

    assert(vec_set(v, 0, &replacement));
    assert(vec_get(v, 0, &out));
    assert(out == 999);

    assert(vec_set(v, 1, &replacement));
    assert(vec_get(v, 1, &out));
    assert(out == 999);

    assert(vec_set(v, 2, &replacement));
    assert(vec_get(v, 2, &out));
    assert(out == 999);

    /* Length must not change. */
    assert(v->length == 3);

    vec_free(v);
}

/* ------------------------------------------------------------
 * Insert
 * ------------------------------------------------------------ */

static void test_insert(void) {
    printf("[TEST] vec_insert\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    int a = 20;
    int b = 30;

    assert(vec_push(v, &a));
    assert(vec_push(v, &b));

    /* Insert at beginning. */
    int first = 10;
    assert(vec_insert(v, 0, &first));

    int expected1[] = {10, 20, 30};
    assert_int_vector(v, expected1, 3);

    /* Insert in middle. */
    int middle = 25;
    assert(vec_insert(v, 2, &middle));

    int expected2[] = {10, 20, 25, 30};
    assert_int_vector(v, expected2, 4);

    /* Insert at end. */
    int last = 40;
    assert(vec_insert(v, v->length, &last));

    int expected3[] = {10, 20, 25, 30, 40};
    assert_int_vector(v, expected3, 5);

    vec_free(v);
}

static void test_insert_growth(void) {
    printf("[TEST] vec_insert causing growth\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    for (int i = 0; i < 4; ++i)
        assert(vec_push(v, &i));

    assert(v->capacity == 4);

    int value = 999;
    assert(vec_insert(v, 2, &value));

    assert(v->length == 5);
    assert(v->capacity == 8);

    int expected[] = {0, 1, 999, 2, 3};
    assert_int_vector(v, expected, 5);

    vec_free(v);
}

/* ------------------------------------------------------------
 * Delete
 * ------------------------------------------------------------ */

static void test_delete(void) {
    printf("[TEST] vec_delete\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    int values[] = {10, 20, 30, 40, 50};

    for (size_t i = 0; i < 5; ++i)
        assert(vec_push(v, &values[i]));

    int out = 0;

    /* Delete first. */
    assert(vec_delete(v, 0, &out));
    assert(out == 10);

    int expected1[] = {20, 30, 40, 50};
    assert_int_vector(v, expected1, 4);

    /* Delete middle. */
    assert(vec_delete(v, 1, &out));
    assert(out == 30);

    int expected2[] = {20, 40, 50};
    assert_int_vector(v, expected2, 3);

    /* Delete last. */
    assert(vec_delete(v, 2, &out));
    assert(out == 50);

    int expected3[] = {20, 40};
    assert_int_vector(v, expected3, 2);

    vec_free(v);
}

/* ------------------------------------------------------------
 * Reserve
 * ------------------------------------------------------------ */

static void test_reserve(void) {
    printf("[TEST] vec_reserve\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    assert(v->capacity == 4);

    assert(vec_reserve(v, 100));
    assert(v->capacity == 100);
    assert(v->length == 0);

    /* Reserving less must succeed without changing capacity. */
    assert(vec_reserve(v, 50));
    assert(v->capacity == 100);

    /* Reserving the same capacity must also succeed without moving data. */
    void *old_data = v->data;
    assert(vec_reserve(v, 100));
    assert(v->capacity == 100);
    assert(v->data == old_data);

    /* Data must survive reallocation. */
    int value = 12345;
    assert(vec_push(v, &value));

    assert(vec_reserve(v, 200));

    int out = 0;
    assert(vec_get(v, 0, &out));
    assert(out == 12345);
    assert(v->capacity == 200);

    vec_free(v);
}

/* ------------------------------------------------------------
 * Chop
 * ------------------------------------------------------------ */

static void test_chop(void) {
    printf("[TEST] vec_chop\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    for (int i = 0; i < 8; ++i)
        assert(vec_push(v, &i));

    assert(v->length == 8);
    assert(v->capacity == 8);

    int out = 0;
    assert(vec_pop(v, &out));
    assert(vec_pop(v, &out));
    assert(vec_pop(v, &out));

    assert(v->length == 5);
    assert(v->capacity == 8);

    assert(vec_chop(v));

    assert(v->length == 5);
    assert(v->capacity == 5);

    int expected[] = {0, 1, 2, 3, 4};
    assert_int_vector(v, expected, 5);

    vec_free(v);
}

static void test_chop_when_already_compact(void) {
    printf("[TEST] vec_chop already compact\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    for (int i = 0; i < 4; ++i)
        assert(vec_push(v, &i));

    assert(v->length == v->capacity);

    void *old_data = v->data;

    assert(vec_chop(v));

    assert(v->length == 4);
    assert(v->capacity == 4);
    assert(v->data == old_data);

    vec_free(v);
}

static void test_chop_empty(void) {
    printf("[TEST] vec_chop empty vector\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    assert(v->length == 0);
    assert(v->capacity == 4);
    assert(v->data != NULL);

    assert(vec_chop(v));

    assert(v->length == 0);
    assert(v->capacity == 0);
    assert(v->data == NULL);

    vec_free(v);
}

/* ------------------------------------------------------------
 * Clear and zero-capacity growth
 * ------------------------------------------------------------ */

static void test_clear(void) {
    printf("[TEST] vec_clear\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    for (int i = 0; i < 10; ++i)
        assert(vec_push(v, &i));

    size_t old_capacity = v->capacity;

    vec_clear(v);

    assert(v->length == 0);
    assert(v->capacity == old_capacity);

    /* Vector must still be usable. */
    int value = 999;
    assert(vec_push(v, &value));

    assert(v->length == 1);

    int out = 0;
    assert(vec_get(v, 0, &out));
    assert(out == 999);

    vec_free(v);
}

static void test_zero_capacity_growth(void) {
    printf("[TEST] zero-capacity growth after empty chop\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    assert(vec_chop(v));

    assert(v->data == NULL);
    assert(v->length == 0);
    assert(v->capacity == 0);

    int value = 123;
    assert(vec_push(v, &value));

    assert(v->length == 1);
    assert(v->capacity == 2);

    int out = 0;
    assert(vec_get(v, 0, &out));
    assert(out == 123);

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
        assert(vec_push(v, &items[i]));

    assert(v->length == 3);

    Item out;

    assert(vec_get(v, 1, &out));
    assert(out.id == 2);
    assert(out.value == 2.5);

    Item replacement = {99, 99.99};

    assert(vec_set(v, 1, &replacement));
    assert(vec_get(v, 1, &out));

    assert(out.id == 99);
    assert(out.value == 99.99);

    Item inserted = {50, 50.5};

    assert(vec_insert(v, 1, &inserted));
    assert(vec_get(v, 1, &out));

    assert(out.id == 50);
    assert(out.value == 50.5);

    Item deleted;

    assert(vec_delete(v, 1, &deleted));
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
        assert(vec_push(v, &strings[i]));

    assert(v->length == 5);

    for (size_t i = 0; i < 5; ++i) {
        char *out;

        assert(vec_get(v, i, &out));
        assert(out == strings[i]);
        assert(strcmp(out, strings[i]) == 0);
    }

    /* Set. */
    char *replacement = "replacement";

    assert(vec_set(v, 2, &replacement));

    char *out;

    assert(vec_get(v, 2, &out));
    assert(out == replacement);
    assert(strcmp(out, "replacement") == 0);

    /* Insert. */
    char *inserted = "inserted";

    assert(vec_insert(v, 1, &inserted));
    assert(vec_get(v, 1, &out));

    assert(out == inserted);
    assert(strcmp(out, "inserted") == 0);

    /* Delete. */
    char *deleted;

    assert(vec_delete(v, 1, &deleted));
    assert(deleted == inserted);

    /* Pop. */
    char *popped;

    assert(vec_pop(v, &popped));
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

    assert(vec_push(v, &a));
    assert(vec_push(v, &b));
    assert(vec_push(v, &c));

    assert(v->length == 3);

    char *out;

    assert(vec_get(v, 1, &out));
    assert(strcmp(out, "beta") == 0);

    char *deleted;

    assert(vec_delete(v, 1, &deleted));
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
 * Invalid operation handling
 * ------------------------------------------------------------ */

static void test_invalid_operations(void) {
    printf("[TEST] invalid operation handling\n");

    Vector *v = vec_new(sizeof(int));
    assert(v != NULL);

    int value = 42;
    assert(vec_push(v, &value));

    int out = 999;

    /* Get rejects indexes outside the current length. */
    assert(!vec_get(v, 1, &out));
    assert(out == 999);

    /* Set and delete also reject indexes outside the current length. */
    assert(!vec_set(v, 1, &value));
    assert(!vec_delete(v, 1, &out));

    /* Pop rejects an empty vector. */
    assert(vec_pop(v, &out));
    assert(out == 42);
    assert(!vec_pop(v, &out));

    /* Insert allows index == length, but rejects gaps. */
    assert(v->length == 0);

    int first = 10;
    assert(vec_insert(v, 0, &first));
    assert(v->length == 1);

    int invalid = 20;
    assert(!vec_insert(v, 2, &invalid));
    assert(v->length == 1);

    assert(vec_get(v, 0, &out));
    assert(out == 10);

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
        assert(vec_push(v, &i));

    assert(v->length == 1000);

    for (int i = 0; i < 500; ++i) {
        int out;

        assert(vec_pop(v, &out));
        assert(out == 999 - i);
    }

    assert(v->length == 500);

    for (int i = 0; i < 100; ++i) {
        int value = -i;
        assert(vec_insert(v, 0, &value));
    }

    assert(v->length == 600);

    for (int i = 0; i < 100; ++i) {
        int out;

        assert(vec_delete(v, 0, &out));
        assert(out == -99 + i);
    }

    assert(v->length == 500);

    for (size_t i = 0; i < 500; ++i) {
        int out;

        assert(vec_get(v, i, &out));
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

    test_chop();
    test_chop_when_already_compact();
    test_chop_empty();

    test_clear();
    test_zero_capacity_growth();

    test_struct();

    test_string_pointers();
    test_owned_string_pointers();

    test_invalid_operations();

    test_mixed_operations();

    printf("\n========== ALL TESTS PASSED ==========\n");

    return 0;
}
