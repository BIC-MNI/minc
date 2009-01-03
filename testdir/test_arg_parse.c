#include <ParseArgv.h>
#include <stdio.h>

int const_a = 0;
int const_b = 0;


ArgvInfo argTable[] = 
{
    {"-const_a", ARGV_CONSTANT, (char *)1, (char *)&const_a, "const_a option"},
    {"-const_b", ARGV_CONSTANT, (char *)1, (char *)&const_b, "const_b option"},
    {NULL, ARGV_END, NULL, NULL, NULL}
};



int main (int argc, char *argv[])
{
   ParseArgv(&argc, argv, argTable, 0);

   printf("const_a:%d const_b:%d\n", const_a, const_b);
   return(0);
}
