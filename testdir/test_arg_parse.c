#include <ParseArgv.h>
#include <stdio.h>

int const_a = 0;
int const_b = 0;

int int_a = 0;
int int_b = 0;

long long_a = 0;
long long_b = 0;


ArgvInfo argTable[] = 
{
    {"-const_a", ARGV_CONSTANT, (char *)1, (char *)&const_a, "const_a option"},
    {"-const_b", ARGV_CONSTANT, (char *)1, (char *)&const_b, "const_b option"},

    {"-int_a", ARGV_INT, (char *)1, (char *)&int_a, "int_a option"},
    {"-int_b", ARGV_INT, (char *)1, (char *)&int_b, "int_b option"},

    {"-long_a", ARGV_LONG, (char *)1, (char *)&long_a, "long_a option"},
    {"-long_b", ARGV_LONG, (char *)1, (char *)&long_b, "long_b option"},

    {NULL, ARGV_END, NULL, NULL, NULL}
};



int main (int argc, char *argv[])
{
   ParseArgv(&argc, argv, argTable, 0);

   printf( "const_a:%d const_b:%d int_a:%d int_b:%d long_a:%ld long_b:%ld\n", 
	   const_a, const_b, int_a, int_b, long_a, long_b );

   return(0);
}
