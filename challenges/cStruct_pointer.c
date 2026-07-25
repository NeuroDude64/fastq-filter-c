#include <stdio.h>
#include <string.h> // need for strcpy

typedef struct{
int count;
char label[32];
} Counter;

void counter_increment_bad(Counter c){;    // pass by value
c.count++;
}
void counter_increment(Counter *c){;       // The pointer can update the counter.count number, but the previous function can't which is why it just returns 0
c->count++;
}


int main(){

    Counter mc;
    mc.count = 0;
    strcpy(mc.label, "This is my counter"); // need to use strcpy to copy the string and replace it with this new string. That's how it works in C, working directly with memory.

    // non-pointer
    counter_increment_bad(mc);
    counter_increment_bad(mc);
    printf("My non-pointer counter %d\n", mc.count);

    //pointer
    counter_increment(&mc); // Needs & symbol indicating mc is a pointer not the value directly
    counter_increment(&mc);
    printf("My counter %d\n", mc.count);    // need the %d because I am trying to print the integer in the struct

return 0;}
