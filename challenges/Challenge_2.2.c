#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void grow_buffer(char **buf, size_t *cap, size_t new_size){
    if (new_size <= *cap) {
        return;   // buffer's already big enough, nothing to do
    }
    // so this realloc is okay, but it is better to use a temporary variable in case it returns NULL
    //*buf = realloc(*buf, new_size);
    //*cap = new_size;
    char *temp = realloc(*buf, new_size);
    if (temp == NULL){
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    *buf = temp;
    *cap = new_size;
    }


int main(){
    char *buffer = NULL;    // this pointer is a dereference, buffer will hold the stings...
    size_t capacity = 0;

    const char *test_strings[4];    //You wanted an array of pointers to strings, not an array of 4 characters. And since I am not going to be modifying them, I should use const.
    test_strings[0] = "Hi";
    test_strings[1] = "This is a longer string...";
    test_strings[2] = "Bye";
    test_strings[3] = "My favorite number is the number 4";

    for (int i = 0; i < 4; i++) {
        size_t needed = strlen(test_strings[i]) + 1;  // +1 for '\0'

        grow_buffer(&buffer, &capacity, needed);

        strcpy(buffer, test_strings[i]);    //strcpy() copies the characters into the memory that buffer points to

        printf("buffer = \"%s\", capacity = %zu\n", buffer, capacity);
    }

    free(buffer);
    return 0;
}
