#include <stdio.h>
#include <math.h>

// apparently structs cannot have variables equal to anything, only initialize the variables.
typedef struct {
    double x;
    double y;
    double z;
} Point3D;


double magnitude(Point3D p){
    printf("%lf\n", p.x);   // I can print p.x from magnitude, indicating that my conclusion was correct... Interesting...

return sqrt(pow(p.x, 2) + pow(p.y, 2) + pow(p.z, 2));
};

int main(){
    // What main does is it assigns the x, y and z values first then uses those for the function. Interesting.... C is definitely a challenge...
    Point3D p;

    p.x = 10;
    p.y = 20;
    p.z = 30;


    double mag = magnitude(p);

    printf("Magnitude is: %lf", mag);

return 0;}
