# VeC

VeC is a small generic dynamic vector library written in C.

It provides a contiguous, dynamically growing array that can store elements of arbitrary fixed size. VeC does not need to know the element's C type; it only needs the size of one element.

## Features

- Generic element storage through `void *`
- Configurable element size
- Automatic capacity growth
- Explicit capacity reservation
- Capacity shrinking with `vec_chop`
- Append and remove operations
- Random access
- Element replacement
- Insertion and deletion at valid indexes
- Clear without freeing the underlying allocation
- Support for built-in C types
- Support for structures, arrays, pointers, and nested vectors
- Can be used as a dynamic string or byte buffer

## Project Structure

```text
VeC/
├── src/
│   ├── VeC.c
│   └── VeC.h
├── tests/
│   └── test_VeC.c
├── LICENSE
├── README.md
├── build.sh
└── main.c
```

`src/VeC.c`
: Core library implementation containing VeC's dynamic vector operations and memory management.

`src/VeC.h`
: Public API and `Vector` structure definition.

`tests/test_VeC.c`
: Automated test suite covering VeC's core operations, edge cases, and various element types.

`main.c`
: Demonstration program showcasing different ways VeC can be used.

## Vector Structure

The core type is:

```c
typedef struct {
    void *data;
    size_t length;
    size_t capacity;
    size_t element_size;
} Vector;
```

### Fields

`data`
: Pointer to the dynamically allocated element storage.

`length`
: Number of elements currently stored.

`capacity`
: Number of elements that can be stored before the allocation has to grow.

`element_size`
: Size in bytes of one element.

The basic invariant is:

```text
0 <= length <= capacity
```

## Basic Usage

Create a vector by specifying the size of one element:

```c
Vector *numbers = vec_new(sizeof(int));
```

Push values into it:

```c
int value = 42;

if (!vec_push(numbers, &value))
    return 1;
```

Retrieve a value:

```c
int result;

if (!vec_get(numbers, 0, &result))
    return 1;
```

Release the vector:

```c
vec_free(numbers);
```

A minimal complete example:

```c
#include "src/VeC.h"

#include <stdio.h>

int main(void) {
    Vector *numbers = vec_new(sizeof(int));

    if (numbers == NULL)
        return 1;

    for (int i = 0; i < 10; ++i) {
        if (!vec_push(numbers, &i)) {
            vec_free(numbers);
            return 1;
        }
    }

    for (size_t i = 0; i < numbers->length; ++i) {
        int value;

        if (!vec_get(numbers, i, &value)) {
            vec_free(numbers);
            return 1;
        }

        printf("%d\n", value);
    }

    vec_free(numbers);

    return 0;
}
```

## API

Operations that can fail return `true` on success and `false` on failure.

Failure can occur because of invalid indexes or memory allocation failure.

`vec_reserve()` also returns `true` when the requested capacity is already available and no reallocation is necessary.

### `vec_new`

```c
Vector *vec_new(size_t element_size);
```

Creates an empty vector where every element occupies `element_size` bytes.

Returns `NULL` if `element_size` is zero or memory allocation fails.

Example:

```c
Vector *v = vec_new(sizeof(double));

if (v == NULL)
    return 1;
```

The vector starts empty with an initial capacity of 4 elements.

### `vec_reserve`

```c
bool vec_reserve(Vector *v, size_t new_capacity);
```

Increases the vector's capacity when `new_capacity` is larger than the current capacity.

It does not change `length`.

Returns `true` when the requested capacity is already available or when the allocation succeeds. Returns `false` if the allocation fails.

```c
if (!vec_reserve(v, 1024))
    return 1;
```

This is useful when the approximate number of elements is known in advance and repeated reallocations should be reduced.

### `vec_chop`

```c
bool vec_chop(Vector *v);
```

Shrinks the allocation so that capacity matches the current length.

For example:

```text
length   = 20
capacity = 64

vec_chop()

length   = 20
capacity = 20
```

If the vector is empty, `vec_chop()` releases its data allocation and sets the capacity to zero:

```text
length   = 0
capacity = 4

vec_chop()

length   = 0
capacity = 0
data     = NULL
```

Returns `true` when the operation succeeds and `false` if shrinking the allocation fails.

### `vec_push`

```c
bool vec_push(Vector *v, const void *data);
```

Appends one element to the end of the vector.

VeC copies `element_size` bytes from `data` into its own storage.

```c
int value = 100;

if (!vec_push(v, &value))
    return 1;
```

Capacity grows automatically when the current allocation is full.

### `vec_pop`

```c
bool vec_pop(Vector *v, void *out);
```

Removes the final element and copies it into caller-provided storage.

Returns `false` when the vector is empty.

```c
int value;

if (!vec_pop(v, &value))
    return 1;
```

### `vec_get`

```c
bool vec_get(Vector *v, size_t index, void *out);
```

Copies the element at `index` into caller-provided storage.

Returns `false` when `index` is outside the current length.

```c
double value;

if (!vec_get(v, 3, &value))
    return 1;
```

### `vec_set`

```c
bool vec_set(Vector *v, size_t index, const void *replacement);
```

Replaces the element at `index`.

Returns `false` when `index` is outside the current length.

```c
int replacement = 50;

if (!vec_set(v, 2, &replacement))
    return 1;
```

### `vec_insert`

```c
bool vec_insert(Vector *v, size_t index, const void *data);
```

Inserts an element at `index`.

Existing elements at and after that index are shifted toward the end.

```c
int value = 99;

if (!vec_insert(v, 2, &value))
    return 1;
```

`index == length` is valid and behaves like appending an element.

Indexes greater than `length` are rejected, preventing gaps in the vector.

### `vec_delete`

```c
bool vec_delete(Vector *v, size_t index, void *out);
```

Removes the element at `index` and copies the removed value into caller-provided storage.

Returns `false` when `index` is outside the current length.

```c
int deleted;

if (!vec_delete(v, 2, &deleted))
    return 1;
```

### `vec_clear`

```c
void vec_clear(Vector *v);
```

Removes all logical elements while keeping the allocated capacity.

```text
before:
length   = 40
capacity = 64

vec_clear()

after:
length   = 0
capacity = 64
```

Unlike `vec_chop()`, `vec_clear()` does not release unused capacity.

### `vec_free`

```c
void vec_free(Vector *v);
```

Releases the vector's data allocation and the `Vector` object itself.

## Generic Data

VeC works by copying a fixed number of bytes for each element.

For example:

```c
Vector *v = vec_new(sizeof(int));
```

means:

```text
one element = sizeof(int) bytes
```

Likewise:

```c
Vector *v = vec_new(sizeof(Point));
```

stores complete `Point` structures.

The library does not need a separate implementation for each type.

## Built-in Types

VeC can store normal C scalar types, including:

```c
Vector *chars = vec_new(sizeof(char));
Vector *ints = vec_new(sizeof(int));
Vector *floats = vec_new(sizeof(float));
Vector *doubles = vec_new(sizeof(double));
```

It can also store unsigned and wider integer types, `_Bool`, and other ordinary fixed-size C objects.

## Structures

Structures are stored as complete elements:

```c
typedef struct {
    int x;
    int y;
} Point;

Vector *points = vec_new(sizeof(Point));

Point p = {10, 20};

vec_push(points, &p);
```

The complete structure is copied into VeC's storage.

## Arrays as Elements

Fixed-size arrays can also be elements:

```c
Vector *triples = vec_new(sizeof(int[3]));

int value[3] = {1, 2, 3};

vec_push(triples, &value);
```

Here one vector element is the entire three-integer array.

## Strings

A C string can be represented as a vector of `char`:

```c
Vector *string = vec_new(sizeof(char));
```

Characters can be appended dynamically:

```c
char c = 'H';

vec_push(string, &c);
```

After adding a terminating `'\0'`, the underlying buffer can be treated as a normal C string.

Conceptually:

```text
Vector<char>
    |
    +-- 'H'
    +-- 'e'
    +-- 'l'
    +-- 'l'
    +-- 'o'
    +-- '\0'
```

This makes a dynamic string essentially a dynamic vector specialized to one-byte elements.

## Byte Arrays

The same idea can be used for arbitrary binary data:

```c
Vector *bytes = vec_new(sizeof(unsigned char));
```

Unlike a C string, a byte vector does not require a `'\0'` terminator.

It can therefore contain values such as:

```text
AA 00 FF 13 00 7F
```

This makes VeC suitable as a general-purpose dynamic byte buffer for applications such as packet construction, serialization, encoded data, and file data.

A character vector and a byte vector use the same underlying idea; the application decides how the bytes are interpreted.

## String Pointers

VeC can store string pointers:

```c
Vector *strings = vec_new(sizeof(char *));

char *a = "hello";
char *b = "world";

vec_push(strings, &a);
vec_push(strings, &b);
```

In this case VeC stores the pointer values.

It does not copy the contents of the strings.

## Pointers and Nested Vectors

Pointers can be used to build larger data structures.

For example, an outer vector can store pointers to inner vectors:

```c
Vector *matrix = vec_new(sizeof(Vector *));
```

Each inner vector can then contain its own elements.

This can be used to construct structures such as:

```text
matrix
├── row 0 -> Vector<int>
├── row 1 -> Vector<int>
└── row 2 -> Vector<int>
```

The same pattern works for pointers to dynamically allocated arrays and other user-managed objects.

## Ownership Model

VeC owns:

- The `Vector` object created by `vec_new`
- The internal data allocation managed by the vector

The caller owns objects passed into the vector.

For normal values such as:

```c
int value = 42;
vec_push(v, &value);
```

VeC copies the value's bytes into its own storage.

For pointers such as:

```c
char *string = malloc(...);

vec_push(v, &string);
```

VeC copies the pointer value, but it does not automatically take ownership of or free the pointed-to object.

The caller remains responsible for the lifetime of that object.

## Insert and Delete

Insertion and deletion operate on a contiguous array.

For example:

```text
Before insert at index 2:

[ 10 20 30 40 ]

After inserting 99:

[ 10 20 99 30 40 ]
```

Elements after the insertion point are shifted to make room.

Deletion performs the opposite operation:

```text
Before:

[ 10 20 30 40 ]

Delete index 1:

[ 10 30 40 ]
```

The implementation uses `memmove()` where overlapping memory regions are possible.

## Building

The project can be built with the included script:

```bash
./build.sh
```

The current build command is equivalent to:

```bash
gcc -Wall -Wextra -O3 -march=native \
    -o bin/build \
    main.c src/VeC.c
```

## Running the Demo

The main program is a usage showcase:

```bash
./bin/build
```

The demo covers:

- Built-in scalar types
- Dynamic arrays
- Fixed-size arrays
- Dynamic strings
- Byte arrays
- String pointers
- Structures
- Structures containing pointers
- Enums and unions
- Nested vectors
- Pointers to dynamically allocated arrays
- Generic pointers
- Core vector operations

## Running the Test Suite

The test suite is separate from the demo.

Build it with:

```bash
gcc -Wall -Wextra -O3 -march=native \
    -o bin/test_VeC \
    tests/test_VeC.c src/VeC.c
```

Run it with:

```bash
./bin/test_VeC
```

The test suite exercises the core API across normal cases, boundary cases, invalid operations, zero-capacity behavior, generic structures, string pointers, and mixed-operation workloads.

## Design Philosophy

VeC intentionally keeps the core abstraction small.

The library manages:

```text
memory
length
capacity
element size
```

The caller manages:

```text
type
interpretation
ownership of pointed-to objects
printing
serialization
```

This separation keeps the vector implementation independent of the actual type being stored.

That same mechanism can represent an integer vector, structure array, dynamic string, binary buffer, pointer list, or nested data structure.

## License

VeC is licensed under the MIT License. See [LICENSE](LICENSE) for the full license text.
