#include <stdio.h>

void dead_code( a, b )
    int a;
    char *b;
    {
        int idead_store;

        idead_store = a;
        if( 0 )
            printf( "%s\n", b );
    } /* Конец dead_code */

int main()
{
    dead_code( 1, "This line should not be printed" );
}