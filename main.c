#include "src/VeC.h"

#include <assert.h>
#include <complex.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
 * Demo-only types
 * ============================================================ */

/*
 * A normal user-defined structure.
 *
 * VeC can store the entire structure as one element.
 */
typedef struct {
    int id;
    char *name;
    double score;
} Student;

/*
 * Another structure used to demonstrate ordinary structs.
 */
typedef struct {
    int x;
    int y;
} Point;

/*
 * Enum values are just integer-like objects to VeC.
 */
typedef enum { STATUS_IDLE, STATUS_RUNNING, STATUS_DONE } Status;

/*
 * Unions are also ordinary C objects with a known size.
 *
 * The caller decides which union member is currently valid.
 */
typedef union {
    int integer;
    float floating;
    char character;
} Value;

/* ============================================================
 * Demo helper functions
 * ============================================================ */

/*
 * Create a dynamically allocated copy of a C string.
 *
 * This is a demo helper, not part of VeC.
 */
static char *duplicate_string(const char *string) {
    /*
     * strlen() does not count '\0', so add one byte for it.
     */
    size_t length = strlen(string) + 1;

    /*
     * Allocate enough memory for the complete string.
     */
    char *copy = malloc(length);

    /*
     * Return NULL if the allocation failed.
     */
    if (copy == NULL)
        return NULL;

    /*
     * Copy the complete string, including '\0'.
     */
    memcpy(copy, string, length);

    return copy;
}

/*
 * Print a Vector containing int values.
 *
 * This belongs to the demo because VeC itself does not know
 * how the caller wants arbitrary bytes to be displayed.
 */
static void print_int_vector(const char *name, Vector *v) {
    printf("%s = [", name);

    /*
     * Visit every logical element currently stored.
     */
    for (size_t i = 0; i < v->length; ++i) {
        int value;

        /*
         * Copy one element from VeC into a caller-owned int.
         */
        vec_get(v, i, &value);

        printf("%d", value);

        /*
         * Don't print a comma after the final element.
         */
        if (i + 1 < v->length)
            printf(", ");
    }

    printf("]\n");
}

/*
 * Print a Vector whose elements are char *.
 *
 * The vector stores pointers, not the characters themselves.
 */
static void print_string_vector(const char *name, Vector *v) {
    printf("%s = [", name);

    for (size_t i = 0; i < v->length; ++i) {
        char *string;

        /*
         * Get the stored char * pointer.
         */
        vec_get(v, i, &string);

        printf("\"%s\"", string);

        if (i + 1 < v->length)
            printf(", ");
    }

    printf("]\n");
}

/*
 * Print a Vector containing Point structures.
 */
static void print_point_vector(const char *name, Vector *v) {
    printf("%s = [", name);

    for (size_t i = 0; i < v->length; ++i) {
        Point point;

        /*
         * Retrieve the complete structure.
         */
        vec_get(v, i, &point);

        printf("(%d, %d)", point.x, point.y);

        if (i + 1 < v->length)
            printf(", ");
    }

    printf("]\n");
}

/*
 * Print a byte vector as hexadecimal values.
 *
 * This is useful for binary data, packets, files, hashes,
 * encoded data, and other byte-oriented applications.
 */
static void print_bytes(const char *name, Vector *v) {
    printf("%s = [", name);

    for (size_t i = 0; i < v->length; ++i) {
        unsigned char byte;

        vec_get(v, i, &byte);

        printf("%02X", byte);

        if (i + 1 < v->length)
            printf(" ");
    }

    printf("]\n");
}

/*
 * Append ordinary text to a Vector of char.
 *
 * The vector itself acts as a dynamic string buffer here.
 */
static void string_append(Vector *string, const char *text) {
    /*
     * Copy characters without copying the terminating '\0'.
     *
     * The terminator is added separately when needed.
     */
    for (size_t i = 0; text[i] != '\0'; ++i)
        vec_push(string, &text[i]);
}

/*
 * Make sure a char vector contains a terminating '\0'.
 *
 * This allows its data buffer to be treated as a normal C string.
 */
static void string_terminate(Vector *string) {
    /*
     * Only append '\0' when the vector is not already terminated.
     */
    if (string->length == 0) {
        char nullByte = '\0';
        vec_push(string, &nullByte);

        return;
    }

    /*
     * Read the final character.
     */
    char last;

    vec_get(string, string->length - 1, &last);

    /*
     * Add the terminator when necessary.
     */
    if (last != '\0') {
        char nullByte = '\0';

        vec_push(string, &nullByte);
    }
}

/*
 * Print a dynamic string represented by a char vector.
 */
static void print_dynamic_string(const char *name, Vector *string) {
    /*
     * This assumes the vector is currently terminated.
     */
    printf("%s = \"%s\"\n", name, (char *)string->data);
}

/* ============================================================
 * 1. Built-in scalar types
 * ============================================================ */

static void demo_builtin_types(void) {
    printf("\n");
    printf("============================================================\n");
    printf("1. BUILT-IN DATA TYPES\n");
    printf("============================================================\n");

    /*
     * VeC does not need to know the C type itself.
     *
     * It only needs the size of one element.
     */

    char character = 'V';

    signed char signedCharacter = -42;
    unsigned char unsignedCharacter = 200;

    short shortValue = -1234;
    unsigned short unsignedShortValue = 54321;

    int intValue = -100000;
    unsigned int unsignedIntValue = 100000;

    long longValue = -123456789L;
    unsigned long unsignedLongValue = 123456789UL;

    long long longLongValue = -1234567890123LL;
    unsigned long long unsignedLongLongValue = 1234567890123ULL;

    float floatValue = 3.14159f;
    double doubleValue = 2.718281828;
    long double longDoubleValue = 1.618033988749894L;

    _Bool booleanValue = true;

    /*
     * C also provides complex floating-point types.
     */
    double _Complex complexValue = 3.0 + 4.0 * I;

    /*
     * Create one vector for each element type.
     */
    Vector *charVector = vec_new(sizeof(char));
    Vector *signedCharVector = vec_new(sizeof(signed char));
    Vector *unsignedCharVector = vec_new(sizeof(unsigned char));

    Vector *shortVector = vec_new(sizeof(short));
    Vector *unsignedShortVector = vec_new(sizeof(unsigned short));

    Vector *intVector = vec_new(sizeof(int));
    Vector *unsignedIntVector = vec_new(sizeof(unsigned int));

    Vector *longVector = vec_new(sizeof(long));
    Vector *unsignedLongVector = vec_new(sizeof(unsigned long));

    Vector *longLongVector = vec_new(sizeof(long long));
    Vector *unsignedLongLongVector = vec_new(sizeof(unsigned long long));

    Vector *floatVector = vec_new(sizeof(float));
    Vector *doubleVector = vec_new(sizeof(double));
    Vector *longDoubleVector = vec_new(sizeof(long double));

    Vector *boolVector = vec_new(sizeof(_Bool));

    Vector *complexVector = vec_new(sizeof(double _Complex));

    /*
     * Verify that every vector was allocated successfully.
     */
    assert(charVector != NULL);
    assert(signedCharVector != NULL);
    assert(unsignedCharVector != NULL);

    assert(shortVector != NULL);
    assert(unsignedShortVector != NULL);

    assert(intVector != NULL);
    assert(unsignedIntVector != NULL);

    assert(longVector != NULL);
    assert(unsignedLongVector != NULL);

    assert(longLongVector != NULL);
    assert(unsignedLongLongVector != NULL);

    assert(floatVector != NULL);
    assert(doubleVector != NULL);
    assert(longDoubleVector != NULL);

    assert(boolVector != NULL);
    assert(complexVector != NULL);

    /*
     * Add one value to every vector.
     *
     * vec_push() copies exactly element_size bytes.
     */
    vec_push(charVector, &character);

    vec_push(signedCharVector, &signedCharacter);
    vec_push(unsignedCharVector, &unsignedCharacter);

    vec_push(shortVector, &shortValue);
    vec_push(unsignedShortVector, &unsignedShortValue);

    vec_push(intVector, &intValue);
    vec_push(unsignedIntVector, &unsignedIntValue);

    vec_push(longVector, &longValue);
    vec_push(unsignedLongVector, &unsignedLongValue);

    vec_push(longLongVector, &longLongValue);
    vec_push(unsignedLongLongVector, &unsignedLongLongValue);

    vec_push(floatVector, &floatValue);
    vec_push(doubleVector, &doubleValue);
    vec_push(longDoubleVector, &longDoubleValue);

    vec_push(boolVector, &booleanValue);

    vec_push(complexVector, &complexValue);

    /*
     * Retrieve the values.
     *
     * The caller decides which type to use for the destination.
     */

    {
        char value;

        vec_get(charVector, 0, &value);

        printf("char                : %c\n", value);
    }

    {
        signed char value;

        vec_get(signedCharVector, 0, &value);

        printf("signed char         : %d\n", value);
    }

    {
        unsigned char value;

        vec_get(unsignedCharVector, 0, &value);

        printf("unsigned char       : %u\n", value);
    }

    {
        short value;

        vec_get(shortVector, 0, &value);

        printf("short               : %hd\n", value);
    }

    {
        unsigned short value;

        vec_get(unsignedShortVector, 0, &value);

        printf("unsigned short      : %hu\n", value);
    }

    {
        int value;

        vec_get(intVector, 0, &value);

        printf("int                 : %d\n", value);
    }

    {
        unsigned int value;

        vec_get(unsignedIntVector, 0, &value);

        printf("unsigned int        : %u\n", value);
    }

    {
        long value;

        vec_get(longVector, 0, &value);

        printf("long                : %ld\n", value);
    }

    {
        unsigned long value;

        vec_get(unsignedLongVector, 0, &value);

        printf("unsigned long       : %lu\n", value);
    }

    {
        long long value;

        vec_get(longLongVector, 0, &value);

        printf("long long           : %lld\n", value);
    }

    {
        unsigned long long value;

        vec_get(unsignedLongLongVector, 0, &value);

        printf("unsigned long long  : %llu\n", value);
    }

    {
        float value;

        vec_get(floatVector, 0, &value);

        printf("float               : %.5f\n", value);
    }

    {
        double value;

        vec_get(doubleVector, 0, &value);

        printf("double              : %.9f\n", value);
    }

    {
        long double value;

        vec_get(longDoubleVector, 0, &value);

        printf("long double         : %.12Lf\n", value);
    }

    {
        _Bool value;

        vec_get(boolVector, 0, &value);

        printf("_Bool               : %s\n", value ? "true" : "false");
    }

    {
        double _Complex value;

        vec_get(complexVector, 0, &value);

        printf("double _Complex     : %.2f + %.2fi\n", creal(value),
               cimag(value));
    }

    /*
     * Release every vector.
     */
    vec_free(charVector);
    vec_free(signedCharVector);
    vec_free(unsignedCharVector);

    vec_free(shortVector);
    vec_free(unsignedShortVector);

    vec_free(intVector);
    vec_free(unsignedIntVector);

    vec_free(longVector);
    vec_free(unsignedLongVector);

    vec_free(longLongVector);
    vec_free(unsignedLongLongVector);

    vec_free(floatVector);
    vec_free(doubleVector);
    vec_free(longDoubleVector);

    vec_free(boolVector);
    vec_free(complexVector);
}

/* ============================================================
 * 2. Normal dynamic array
 * ============================================================ */

static void demo_dynamic_array(void) {
    printf("\n");
    printf("============================================================\n");
    printf("2. DYNAMIC ARRAY\n");
    printf("============================================================\n");

    /*
     * This is the standard vector use case:
     *
     *     dynamic array of int
     *
     * The final number of elements does not need to be known
     * before the program starts.
     */
    Vector *numbers = vec_new(sizeof(int));

    assert(numbers != NULL);

    printf("Initial capacity: %zu\n", numbers->capacity);

    /*
     * Push twenty integers.
     *
     * VeC automatically reallocates when capacity is exhausted.
     */
    for (int i = 0; i < 20; ++i)
        vec_push(numbers, &i);

    printf("Length after 20 pushes: %zu\n", numbers->length);
    printf("Capacity after growth : %zu\n", numbers->capacity);

    print_int_vector("numbers", numbers);

    /*
     * Explicitly reserve space for at least 100 elements.
     *
     * reserve() changes capacity, not logical length.
     */
    vec_reserve(numbers, 100);

    printf("Capacity after reserve: %zu\n", numbers->capacity);

    /*
     * Remove elements from the end.
     */
    for (int i = 0; i < 15; ++i) {
        int ignored;

        vec_pop(numbers, &ignored);
    }

    printf("After popping 15 elements:\n");
    printf("Length   : %zu\n", numbers->length);
    printf("Capacity : %zu\n", numbers->capacity);

    /*
     * Remove unused allocation.
     *
     * chop() reduces capacity to the current length.
     */
    vec_chop(numbers);

    printf("After chop:\n");
    printf("Length   : %zu\n", numbers->length);
    printf("Capacity : %zu\n", numbers->capacity);

    print_int_vector("numbers", numbers);

    vec_free(numbers);
}

/* ============================================================
 * 3. Static/fixed arrays as elements
 * ============================================================ */

static void demo_static_arrays(void) {
    printf("\n");
    printf("============================================================\n");
    printf("3. STATIC ARRAYS AS ELEMENTS\n");
    printf("============================================================\n");

    /*
     * The element type is int[3].
     *
     * Therefore every vector element contains three integers.
     */
    Vector *triples = vec_new(sizeof(int[3]));

    assert(triples != NULL);

    /*
     * These are ordinary fixed-size arrays.
     */
    int first[3] = {1, 2, 3};
    int second[3] = {10, 20, 30};
    int third[3] = {100, 200, 300};

    /*
     * VeC copies all three integers from each array.
     */
    vec_push(triples, &first);
    vec_push(triples, &second);
    vec_push(triples, &third);

    /*
     * Fetch one complete array at a time.
     */
    for (size_t i = 0; i < triples->length; ++i) {
        int triple[3];

        vec_get(triples, i, &triple);

        printf("[%d, %d, %d]\n", triple[0], triple[1], triple[2]);
    }

    vec_free(triples);
}

/* ============================================================
 * 4. Dynamic string
 * ============================================================ */

static void demo_dynamic_string(void) {
    printf("\n");
    printf("============================================================\n");
    printf("4. DYNAMIC STRING\n");
    printf("============================================================\n");

    /*
     * A dynamic string is simply:
     *
     *     Vector<char>
     *
     * In C terms:
     *
     *     Vector *string = vec_new(sizeof(char));
     *
     * VeC grows the character buffer automatically.
     */
    Vector *string = vec_new(sizeof(char));

    assert(string != NULL);

    /*
     * Reserve some space up front.
     *
     * This is optional, but useful when the approximate final
     * size is known.
     */
    vec_reserve(string, 64);

    /*
     * Build the string dynamically.
     */
    string_append(string, "Hello ");
    string_append(string, "from ");
    string_append(string, "VeC!");

    /*
     * Add the C string terminator.
     */
    string_terminate(string);

    print_dynamic_string("string", string);

    printf("String length including '\\0': %zu\n", string->length);

    printf("String capacity            : %zu\n", string->capacity);

    /*
     * Because the elements are chars, the vector can be modified
     * character-by-character using normal VeC operations.
     *
     * Change the first character:
     *
     *     H -> h
     */
    char replacement = 'h';

    vec_set(string, 0, &replacement);

    print_dynamic_string("after set", string);

    /*
     * Remove the terminating '\0' temporarily.
     *
     * This lets us use ordinary vector operations on the text.
     */
    int popped;

    vec_pop(string, (char *)&popped);

    /*
     * Insert characters into the string.
     *
     * Insert '>' near the beginning.
     */
    char inserted = '>';

    vec_insert(string, 6, &inserted);

    /*
     * Put the null terminator back.
     */
    string_terminate(string);

    print_dynamic_string("after insert", string);

    /*
     * The same allocation can also be viewed as bytes.
     *
     * There is no conversion here.
     *
     * A char occupies one byte, so the underlying memory can be
     * interpreted as unsigned char data.
     */
    unsigned char *bytes = string->data;

    printf("Same string as bytes: [");

    for (size_t i = 0; i < string->length; ++i) {
        printf("%02X", bytes[i]);

        if (i + 1 < string->length)
            printf(" ");
    }

    printf("]\n");

    vec_free(string);
}

/* ============================================================
 * 5. Byte array / binary buffer
 * ============================================================ */

static void demo_byte_array(void) {
    printf("\n");
    printf("============================================================\n");
    printf("5. BYTE ARRAY / BINARY BUFFER\n");
    printf("============================================================\n");

    /*
     * A byte array is simply:
     *
     *     Vector<unsigned char>
     *
     * Unlike a string, bytes do not have to end in '\0'.
     *
     * They can contain ANY 8-bit value.
     */
    Vector *bytes = vec_new(sizeof(unsigned char));

    assert(bytes != NULL);

    /*
     * Reserve room for a small binary packet.
     */
    vec_reserve(bytes, 32);

    /*
     * Example binary data.
     *
     * Notice that 0x00 is perfectly valid here.
     */
    unsigned char packet[] = {0x7E, 0x01, 0x00, 0xFF, 0x42, 0x10, 0x80, 0xAA};

    /*
     * Copy every byte into VeC.
     */
    for (size_t i = 0; i < sizeof(packet); ++i)
        vec_push(bytes, &packet[i]);

    print_bytes("packet", bytes);

    /*
     * Access and modify a byte like any other vector element.
     */
    unsigned char byte;

    vec_get(bytes, 3, &byte);

    printf("Byte at index 3: 0x%02X\n", byte);

    /*
     * Change 0xFF into 0x55.
     */
    unsigned char replacement = 0x55;

    vec_set(bytes, 3, &replacement);

    print_bytes("after set", bytes);

    /*
     * Insert another byte.
     */
    unsigned char inserted = 0xDE;

    vec_insert(bytes, 2, &inserted);

    print_bytes("after insert", bytes);

    /*
     * Remove a byte and retrieve it.
     */
    unsigned char deleted;

    vec_delete(bytes, 2, &deleted);

    printf("Deleted byte: 0x%02X\n", deleted);

    print_bytes("after delete", bytes);

    /*
     * Pop a byte from the end.
     */
    unsigned char popped;

    vec_pop(bytes, &popped);

    printf("Popped byte: 0x%02X\n", popped);

    print_bytes("after pop", bytes);

    /*
     * Shrink away unused capacity.
     */
    vec_chop(bytes);

    printf("Length after chop   : %zu\n", bytes->length);
    printf("Capacity after chop : %zu\n", bytes->capacity);

    vec_free(bytes);
}

/* ============================================================
 * 6. String pointers
 * ============================================================ */

static void demo_string_pointers(void) {
    printf("\n");
    printf("============================================================\n");
    printf("6. VECTOR OF STRING POINTERS\n");
    printf("============================================================\n");

    /*
     * This vector stores char * pointers.
     *
     * It does NOT store the strings themselves.
     */
    Vector *strings = vec_new(sizeof(char *));

    assert(strings != NULL);

    /*
     * String literals generally have static storage duration.
     *
     * We only store their addresses.
     */
    char *hello = "hello";
    char *world = "world";
    char *library = "VeC";

    vec_push(strings, &hello);
    vec_push(strings, &world);
    vec_push(strings, &library);

    /*
     * These strings are dynamically allocated.
     *
     * Their ownership stays with this demo.
     */
    char *dynamicOne = duplicate_string("dynamic string");
    char *dynamicTwo = duplicate_string("heap string");

    assert(dynamicOne != NULL);
    assert(dynamicTwo != NULL);

    /*
     * Again, VeC stores the POINTERS, not the strings.
     */
    vec_push(strings, &dynamicOne);
    vec_push(strings, &dynamicTwo);

    print_string_vector("strings", strings);

    /*
     * Retrieve and modify one pointer.
     */
    char *replacement = "replacement";

    vec_set(strings, 1, &replacement);

    print_string_vector("after set", strings);

    /*
     * Remove one pointer.
     */
    char *deleted;

    vec_delete(strings, 3, &deleted);

    printf("Deleted string: \"%s\"\n", deleted);

    /*
     * The removed pointer was pointing at dynamicOne.
     *
     * The vector does not free it.
     */
    free(deleted);

    /*
     * Clean up the remaining dynamically allocated string.
     */
    free(dynamicTwo);

    vec_free(strings);
}

/* ============================================================
 * 7. Structs
 * ============================================================ */

static void demo_structs(void) {
    printf("\n");
    printf("============================================================\n");
    printf("7. STRUCTS\n");
    printf("============================================================\n");

    /*
     * Every element is now a complete Point structure.
     */
    Vector *points = vec_new(sizeof(Point));

    assert(points != NULL);

    Point first = {10, 20};
    Point second = {30, 40};
    Point third = {-5, 15};

    /*
     * VeC copies the entire structure.
     */
    vec_push(points, &first);
    vec_push(points, &second);
    vec_push(points, &third);

    print_point_vector("points", points);

    /*
     * Replace one structure.
     */
    Point replacement = {100, 200};

    vec_set(points, 1, &replacement);

    print_point_vector("after set", points);

    /*
     * Insert a complete structure.
     */
    Point inserted = {50, 50};

    vec_insert(points, 1, &inserted);

    print_point_vector("after insert", points);

    /*
     * Delete a complete structure.
     */
    Point deleted;

    vec_delete(points, 0, &deleted);

    printf("Deleted point: (%d, %d)\n", deleted.x, deleted.y);

    print_point_vector("after delete", points);

    vec_free(points);
}

/* ============================================================
 * 8. Structs containing pointers
 * ============================================================ */

static void demo_complex_struct(void) {
    printf("\n");
    printf("============================================================\n");
    printf("8. STRUCTS CONTAINING POINTERS\n");
    printf("============================================================\n");

    /*
     * Student contains:
     *
     *     int
     *     char *
     *     double
     *
     * VeC copies the complete Student object.
     *
     * It does NOT deep-copy student.name.
     */
    Vector *students = vec_new(sizeof(Student));

    assert(students != NULL);

    /*
     * String literals are used for names here.
     */
    Student alice = {.id = 1, .name = "Alice", .score = 91.5};

    Student bob = {.id = 2, .name = "Bob", .score = 87.25};

    Student charlie = {.id = 3, .name = "Charlie", .score = 95.75};

    /*
     * Store the complete structures.
     */
    vec_push(students, &alice);
    vec_push(students, &bob);
    vec_push(students, &charlie);

    /*
     * Retrieve complete Student objects.
     */
    for (size_t i = 0; i < students->length; ++i) {
        Student student;

        vec_get(students, i, &student);

        printf("ID=%d Name=%s Score=%.2f\n", student.id, student.name,
               student.score);
    }

    vec_free(students);
}

/* ============================================================
 * 9. Enum and union
 * ============================================================ */

static void demo_enum_and_union(void) {
    printf("\n");
    printf("============================================================\n");
    printf("9. ENUMS AND UNIONS\n");
    printf("============================================================\n");

    /*
     * Enum values can be stored exactly like integer values.
     */
    Vector *statuses = vec_new(sizeof(Status));

    assert(statuses != NULL);

    Status idle = STATUS_IDLE;
    Status running = STATUS_RUNNING;
    Status done = STATUS_DONE;

    vec_push(statuses, &idle);
    vec_push(statuses, &running);
    vec_push(statuses, &done);

    for (size_t i = 0; i < statuses->length; ++i) {
        Status status;

        vec_get(statuses, i, &status);

        printf("Status %zu: %d\n", i, status);
    }

    vec_free(statuses);

    /*
     * A union is another ordinary fixed-size C object.
     */
    Vector *values = vec_new(sizeof(Value));

    assert(values != NULL);

    Value integerValue = {.integer = 123};

    Value floatingValue = {.floating = 3.14f};

    Value characterValue = {.character = 'X'};

    /*
     * Store the complete union objects.
     */
    vec_push(values, &integerValue);
    vec_push(values, &floatingValue);
    vec_push(values, &characterValue);

    /*
     * Read the corresponding active member.
     *
     * The caller must know which union member is valid.
     */
    Value out;

    vec_get(values, 0, &out);
    printf("Union integer   : %d\n", out.integer);

    vec_get(values, 1, &out);
    printf("Union float     : %.2f\n", out.floating);

    vec_get(values, 2, &out);
    printf("Union character : %c\n", out.character);

    vec_free(values);
}

/* ============================================================
 * 10. Nested vectors
 * ============================================================ */

static void demo_nested_vectors(void) {
    printf("\n");
    printf("============================================================\n");
    printf("10. NESTED VECTORS\n");
    printf("============================================================\n");

    /*
     * The outer vector stores Vector * pointers.
     *
     * Each pointer points to another independent VeC.
     */
    Vector *matrix = vec_new(sizeof(Vector *));

    assert(matrix != NULL);

    /*
     * Create three inner vectors.
     */
    Vector *rowOne = vec_new(sizeof(int));
    Vector *rowTwo = vec_new(sizeof(int));
    Vector *rowThree = vec_new(sizeof(int));

    assert(rowOne != NULL);
    assert(rowTwo != NULL);
    assert(rowThree != NULL);

    /*
     * Fill the first row.
     */
    for (int i = 1; i <= 3; ++i)
        vec_push(rowOne, &i);

    /*
     * Fill the second row.
     */
    for (int i = 4; i <= 6; ++i)
        vec_push(rowTwo, &i);

    /*
     * Fill the third row.
     */
    for (int i = 7; i <= 9; ++i)
        vec_push(rowThree, &i);

    /*
     * Store the child-vector pointers in the outer vector.
     */
    vec_push(matrix, &rowOne);
    vec_push(matrix, &rowTwo);
    vec_push(matrix, &rowThree);

    /*
     * Traverse the outer vector.
     */
    printf("3 x 3 matrix:\n");

    for (size_t row = 0; row < matrix->length; ++row) {
        Vector *currentRow;

        /*
         * Fetch one Vector * from the outer vector.
         */
        vec_get(matrix, row, &currentRow);

        printf("[ ");

        /*
         * Traverse the inner vector.
         */
        for (size_t column = 0; column < currentRow->length; ++column) {

            int value;

            vec_get(currentRow, column, &value);

            printf("%d ", value);
        }

        printf("]\n");
    }

    /*
     * The outer vector owns the pointer slots only.
     *
     * It does NOT recursively free rowOne, rowTwo, or rowThree.
     */
    vec_free(rowOne);
    vec_free(rowTwo);
    vec_free(rowThree);

    vec_free(matrix);
}

/* ============================================================
 * 11. Dynamic arrays stored through pointers
 * ============================================================ */

static void demo_dynamic_arrays_inside_vector(void) {
    printf("\n");
    printf("============================================================\n");
    printf("11. DYNAMIC ARRAYS STORED THROUGH POINTERS\n");
    printf("============================================================\n");

    /*
     * Every vector element is an int *.
     *
     * Each pointer can therefore refer to a dynamically allocated
     * array of a completely different size.
     */
    Vector *arrays = vec_new(sizeof(int *));

    assert(arrays != NULL);

    /*
     * Length of each separately allocated array.
     */
    size_t lengths[] = {3, 5, 2};

    /*
     * Allocate three independent arrays.
     */
    int *arrayOne = malloc(lengths[0] * sizeof(int));
    int *arrayTwo = malloc(lengths[1] * sizeof(int));
    int *arrayThree = malloc(lengths[2] * sizeof(int));

    /*
     * Check all allocations before using them.
     */
    if (arrayOne == NULL || arrayTwo == NULL || arrayThree == NULL) {

        free(arrayOne);
        free(arrayTwo);
        free(arrayThree);

        vec_free(arrays);

        return;
    }

    /*
     * Fill the first array.
     */
    for (size_t i = 0; i < lengths[0]; ++i)
        arrayOne[i] = (int)(i + 1);

    /*
     * Fill the second array.
     */
    for (size_t i = 0; i < lengths[1]; ++i)
        arrayTwo[i] = (int)((i + 1) * 10);

    /*
     * Fill the third array.
     */
    for (size_t i = 0; i < lengths[2]; ++i)
        arrayThree[i] = (int)((i + 1) * 100);

    /*
     * Store the pointers in VeC.
     *
     * Only the pointer values are copied into the vector.
     */
    vec_push(arrays, &arrayOne);
    vec_push(arrays, &arrayTwo);
    vec_push(arrays, &arrayThree);

    /*
     * Traverse the pointer vector and then access each
     * dynamically allocated array.
     */
    for (size_t i = 0; i < arrays->length; ++i) {
        int *array;

        vec_get(arrays, i, &array);

        printf("Array %zu: [ ", i);

        for (size_t j = 0; j < lengths[i]; ++j)
            printf("%d ", array[j]);

        printf("]\n");
    }

    /*
     * VeC does not own the pointed-to arrays.
     *
     * The caller must free them.
     */
    free(arrayOne);
    free(arrayTwo);
    free(arrayThree);

    vec_free(arrays);
}

/* ============================================================
 * 12. Pointer vector
 * ============================================================ */

static void demo_generic_pointers(void) {
    printf("\n");
    printf("============================================================\n");
    printf("12. GENERIC POINTERS\n");
    printf("============================================================\n");

    /*
     * A Vector can also store void * pointers.
     *
     * This is useful for heterogeneous object collections where
     * the caller manages the actual object type.
     */
    Vector *pointers = vec_new(sizeof(void *));

    assert(pointers != NULL);

    /*
     * Objects with different types.
     */
    int integer = 42;
    double real = 3.14159;
    char character = 'V';

    /*
     * Convert each object address to void *.
     */
    void *integerPointer = &integer;
    void *realPointer = &real;
    void *characterPointer = &character;

    /*
     * Store the pointers.
     */
    vec_push(pointers, &integerPointer);
    vec_push(pointers, &realPointer);
    vec_push(pointers, &characterPointer);

    /*
     * Retrieve them and cast back to the correct type.
     */
    void *out;

    vec_get(pointers, 0, &out);
    printf("int pointer     : %d\n", *(int *)out);

    vec_get(pointers, 1, &out);
    printf("double pointer  : %.5f\n", *(double *)out);

    vec_get(pointers, 2, &out);
    printf("char pointer    : %c\n", *(char *)out);

    vec_free(pointers);
}

/* ============================================================
 * 13. Dynamic character buffer used as a string and bytes
 * ============================================================ */

static void demo_string_to_bytes(void) {
    printf("\n");
    printf("============================================================\n");
    printf("13. STRING <-> BYTE BUFFER\n");
    printf("============================================================\n");

    /*
     * Start with a character vector.
     *
     * This is our dynamic string.
     */
    Vector *buffer = vec_new(sizeof(char));

    assert(buffer != NULL);

    /*
     * Build ordinary text dynamically.
     */
    string_append(buffer, "Hello");
    string_append(buffer, " ");
    string_append(buffer, "World");

    string_terminate(buffer);

    /*
     * The same memory can be viewed as bytes.
     *
     * No conversion or second allocation is needed.
     */
    unsigned char *rawBytes = buffer->data;

    printf("String: %s\n", (char *)buffer->data);

    printf("Bytes : [");

    for (size_t i = 0; i < buffer->length; ++i) {
        printf("%02X", rawBytes[i]);

        if (i + 1 < buffer->length)
            printf(" ");
    }

    printf("]\n");

    /*
     * Since this is a char vector, modifying the underlying
     * bytes modifies the string as well.
     *
     * Change 'W' -> 'v'.
     */
    char replacement = 'v';

    vec_set(buffer, 6, &replacement);

    printf("Modified string: %s\n", (char *)buffer->data);

    /*
     * This demonstrates the important concept:
     *
     *     dynamic string == dynamic byte buffer
     *
     * when each element is one byte.
     *
     * The interpretation is what changes.
     */

    vec_free(buffer);
}

/* ============================================================
 * 14. Explicit binary buffer with zero bytes
 * ============================================================ */

static void demo_binary_data(void) {
    printf("\n");
    printf("============================================================\n");
    printf("14. ARBITRARY BINARY DATA\n");
    printf("============================================================\n");

    /*
     * Unlike a string, binary data may freely contain 0x00.
     *
     * Therefore there is no '\0' terminator concept here.
     */
    Vector *data = vec_new(sizeof(unsigned char));

    assert(data != NULL);

    /*
     * Example binary representation:
     *
     *     AA 00 FF 13 00 7F
     *
     * Both zero bytes are ordinary data.
     */
    unsigned char binary[] = {0xAA, 0x00, 0xFF, 0x13, 0x00, 0x7F};

    /*
     * Add every byte.
     */
    for (size_t i = 0; i < sizeof(binary); ++i)
        vec_push(data, &binary[i]);

    print_bytes("binary", data);

    /*
     * A binary vector can grow exactly like any other vector.
     */
    unsigned char extra[] = {0x10, 0x20, 0x30, 0x40};

    for (size_t i = 0; i < sizeof(extra); ++i)
        vec_push(data, &extra[i]);

    print_bytes("after growth", data);

    /*
     * Reserve even more room if a larger packet is expected.
     */
    vec_reserve(data, 128);

    printf("Reserved capacity: %zu\n", data->capacity);

    /*
     * Once the final length is known, remove unused capacity.
     */
    vec_chop(data);

    printf("Capacity after chop: %zu\n", data->capacity);

    vec_free(data);
}

/* ============================================================
 * 15. Every core operation together
 * ============================================================ */

static void demo_all_operations(void) {
    printf("\n");
    printf("============================================================\n");
    printf("15. CORE OPERATIONS\n");
    printf("============================================================\n");

    /*
     * Start with a normal integer vector.
     */
    Vector *numbers = vec_new(sizeof(int));

    assert(numbers != NULL);

    /*
     * PUSH
     *
     * Append to the end.
     */
    for (int i = 1; i <= 5; ++i)
        vec_push(numbers, &i);

    print_int_vector("initial", numbers);

    /*
     * INSERT
     *
     * Insert an element at an arbitrary index.
     */
    int inserted = 99;

    vec_insert(numbers, 2, &inserted);

    print_int_vector("after insert", numbers);

    /*
     * GET
     *
     * Copy one element into caller-owned storage.
     */
    int fetched;

    vec_get(numbers, 3, &fetched);

    printf("Fetched index 3: %d\n", fetched);

    /*
     * SET
     *
     * Replace an existing element.
     */
    int replacement = 55;

    vec_set(numbers, 0, &replacement);

    print_int_vector("after set", numbers);

    /*
     * DELETE
     *
     * Remove an arbitrary element and retrieve its old value.
     */
    int deleted;

    vec_delete(numbers, 1, &deleted);

    printf("Deleted index 1: %d\n", deleted);

    print_int_vector("after delete", numbers);

    /*
     * POP
     *
     * Remove the last element.
     */
    int popped;

    vec_pop(numbers, &popped);

    printf("Popped element: %d\n", popped);

    print_int_vector("after pop", numbers);

    /*
     * RESERVE
     *
     * Force the allocation to have room for at least 50 elements.
     */
    printf("Capacity before reserve: %zu\n", numbers->capacity);

    vec_reserve(numbers, 50);

    printf("Capacity after reserve : %zu\n", numbers->capacity);

    /*
     * CHOP
     *
     * Remove unused capacity.
     */
    vec_chop(numbers);

    printf("Capacity after chop    : %zu\n", numbers->capacity);

    /*
     * CLEAR
     *
     * Remove all logical elements while keeping the allocation.
     */
    vec_clear(numbers);

    printf("Length after clear     : %zu\n", numbers->length);

    printf("Capacity after clear   : %zu\n", numbers->capacity);

    /*
     * FREE
     *
     * Release the Vector and its internal allocation.
     */
    vec_free(numbers);
}

/* ============================================================
 * Main
 * ============================================================ */

int main(void) {
    /*
     * This file is a showcase rather than a correctness test.
     *
     * tests/test_VeC.c is responsible for correctness assertions.
     *
     * main.c demonstrates the different ways VeC can be used.
     */

    printf("============================================================\n");
    printf("                         VeC DEMO\n");
    printf("       Generic Dynamic Vector Library for C\n");
    printf("============================================================\n");

    /*
     * Scalar C data types.
     */
    demo_builtin_types();

    /*
     * Normal dynamic array.
     */
    demo_dynamic_array();

    /*
     * Arrays themselves as vector elements.
     */
    demo_static_arrays();

    /*
     * Dynamic C string implemented using Vector<char>.
     */
    demo_dynamic_string();

    /*
     * Dynamic byte buffer implemented using Vector<unsigned char>.
     */
    demo_byte_array();

    /*
     * Vector of char *.
     */
    demo_string_pointers();

    /*
     * Vector of structs.
     */
    demo_structs();

    /*
     * Structs containing pointers.
     */
    demo_complex_struct();

    /*
     * Enums and unions.
     */
    demo_enum_and_union();

    /*
     * Vector containing other Vector objects through pointers.
     */
    demo_nested_vectors();

    /*
     * Vector containing pointers to dynamic arrays.
     */
    demo_dynamic_arrays_inside_vector();

    /*
     * Vector containing generic void * pointers.
     */
    demo_generic_pointers();

    /*
     * Show that a character vector can simultaneously be treated
     * as a text buffer or as raw bytes.
     */
    demo_string_to_bytes();

    /*
     * Show a true arbitrary binary buffer containing zero bytes.
     */
    demo_binary_data();

    /*
     * Demonstrate all core VeC operations together.
     */
    demo_all_operations();

    /*
     * Reaching this point means the complete showcase finished.
     */
    printf("\n");
    printf("============================================================\n");
    printf("                      DEMO COMPLETE\n");
    printf("============================================================\n");

    return 0;
}
