#include "src/VeC.h"

int main(void) {

    Vector *my_vec = vec_new(sizeof(int));
    int my_data = 10;
    if (my_vec == NULL) {
        return 1;
    }

    vec_push(my_vec, &my_data);
    vec_display_metadata(my_vec);

    my_data++;
    vec_push(my_vec, &my_data);
    vec_display_metadata(my_vec);

    my_data++;
    vec_push(my_vec, &my_data);
    vec_display_metadata(my_vec);

    my_data++;
    vec_push(my_vec, &my_data);
    vec_display_metadata(my_vec);

    my_data++;
    vec_push(my_vec, &my_data);
    vec_display_metadata(my_vec);

    vec_display_elements_as_int(my_vec);

    int popped_element;
    vec_pop(my_vec, &popped_element);

    printf("Popped Element: %d\n", popped_element);

    vec_display_elements_as_int(my_vec);

    int fetched_element;
    vec_get(my_vec, 0, &fetched_element);
    printf("Fetched Element: %d\n", fetched_element);

    int replacement = 20;
    vec_set(my_vec, 0, &replacement);

    vec_display_elements_as_int(my_vec);

    my_data = 10;
    vec_insert(my_vec, 0, &my_data);
    vec_display_elements_as_int(my_vec);

    vec_delete(my_vec, 0, &my_data);
    vec_display_elements_as_int(my_vec);

    vec_chop(my_vec);
    vec_display_metadata(my_vec);

    vec_clear(my_vec);
    vec_display_metadata(my_vec);
    vec_display_elements_as_int(my_vec);

    vec_free(my_vec);

    return 0;
}
