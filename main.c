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

    vec_free(my_vec);

    return 0;
}
