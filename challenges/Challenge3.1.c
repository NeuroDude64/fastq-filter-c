#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int *data;
    size_t size;      // how many ints are actually stored right now
    size_t capacity;  // how many ints there's currently room for
} IntList;

IntList *intlist_create(void){                  //IntList is the type, like int or long, it is a struct datatype
    IntList *list = malloc(sizeof(IntList));    //malloc() always returns an address, so *list must be a pointer.
    if (list == NULL) {
        printf("Failed to allocate IntList!\n");
        exit(EXIT_FAILURE);
    }
    list->data = NULL;
    list->size = 0;
    list->capacity = 0;
    return list;
}
/*
First try...

void intlist_push(IntList *list, int value){

    if (list->size == 0){
        list->size = realloc(list->size, 4*sizeof(value))
    }
    else if (list->size >= 4){
        list->size = realloc(list->size, list->capacity * 2 * *sizeof(value))
    }
    list->capacity = list->size;
*/

void intlist_push(IntList *list, int value){
        if (list->size == list->capacity)
        {
            size_t new_capacity;

            if (list->capacity == 0)
            {
                new_capacity = 4;
            }
            else
            {
                new_capacity = list->capacity * 2;
            }

            int *temp = realloc(
                list->data,
                new_capacity * sizeof(int)
            );

            if (temp == NULL)
            {
                printf("Memory allocation failed!\n");
                exit(EXIT_FAILURE);
            }

            list->data = temp;
            list->capacity = new_capacity;
        }

        list->data[list->size] = value;
        list->size++;
    }


    // TODO 1: if list->size has reached list->capacity, there's no room
    // left -- you need to grow list->data before you can store anything.
    //
    // A common strategy: if capacity is 0, start at some small number
    // (e.g. 4). Otherwise, double it. This is the "amortized growth"
    // idea from last time -- growing by a multiple instead of by exactly
    // what's needed means you're not calling realloc on almost every push.
    //
    // Careful with the realloc call here vs. grow_buffer: your buffer
    // there was char*, so byte count == element count. Here list->data
    // is int*, so the byte count realloc needs is
    // (new_capacity * sizeof(int)), not just new_capacity.
    //
    // Don't forget to update list->capacity after a successful realloc.

    // TODO 2: now that you're sure there's room, store `value` at the
    // next open slot, and update list->size to reflect the new count.
    // Think about exactly which index the "next open slot" is, in terms
    // of list->size before vs. after you increment it.
}

void intlist_free(IntList *list){
    // TODO 3: this struct owns two separate allocations -- the IntList
    // struct itself (from intlist_create's malloc) AND the int array
    // (list->data, from realloc inside intlist_push). Both need freeing.
    // Think about what would go wrong if you freed them in the wrong order.
}

int main(){
    IntList *list = intlist_create();

    for (int i = 0; i < 10; i++) {
        intlist_push(list, i * i);
    }

    printf("Contents: ");
    for (size_t i = 0; i < list->size; i++) {
        printf("%d ", list->data[i]);
    }
    printf("\n");
    printf("size = %zu, capacity = %zu\n", list->size, list->capacity);

    intlist_free(list);
    return 0;
}
