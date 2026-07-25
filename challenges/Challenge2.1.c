# include <stdio.h>
# include <stdlib.h>    // Needed to use malloc

// Working with malloc

/*
Every variable you've used so far (int x;, Point3D p;, counter mc;)
lives on the stack — memory that's automatically created when a function is called
and automatically destroyed when it returns. You never had to ask for that memory or
give it back; the compiler handled it.

Malloc is different: it asks the OS for a chunk of memory on the heap — memory that
stays allocated until you explicitly free it.

Heap memory outlives the function that created it (which is exactly what you need here),
but you now own the responsibility of freeing it later.

*/

// If a function needs to change the pointer variable itself, you must pass a pointer to that pointer.
void make_it_five(int **p){ //int** declares a pointer to a pointer to an int // This means the variable stores the memory address of another pointer (which itself points to an integer)
    *p = malloc(sizeof(int));   // Cool, sizeof(int) works! Awesome!
    **p = 5;
}

//What I still don't understand is why the function needs a pointer to a pointer... I guess since I am calling make_it_five with a pointer, the pointer *p points at that pointer, thus a pointer for a pointer. maybe???
//ptr is a pointer variable. Since I want the function to modify that pointer variable itself, I pass its address (&ptr). The parameter must therefore be an int **, because it points to an int *.

int main(){

    //int num = 10;
    //int *ptr = &num;

    int *ptr = NULL;
    // ptr = malloc(sizeof(*ptr)); // Allocate memory for one integer, it even works when *ptr = NULL, essentially when the pointer, I think, is pointing at nothing. I think it still takes up 8 bytes of memory.
    // this code, I think, essentially opens up 8 bytes of memory for use...

    make_it_five(&ptr); //Then that memory is used in this function call and it outputs what it should...
    printf("%d\n", *ptr);  // should print 5

return 0;
}
