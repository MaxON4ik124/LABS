int i, k5, j5 = 5, i5;


void loop_jamming( x )
int x;
{
    for( i = 0; i < 5; i++ )
    k5 = x + j5 * i;
    for( i = 0; i < 5; i++ )
    i5 = x * k5 * i;
} /* Конец loop_jamming */
int main()
{
    loop_jamming(7);
}