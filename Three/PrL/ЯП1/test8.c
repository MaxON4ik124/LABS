int i;
short ivector4[6]; 
void loop_unrolling( x )
int x;
{
    for( i = 0; i < 6; i++ )
        ivector4[ i ] = 0;
} /* Конец loop_unrolling */
int main()
{
    loop_unrolling(7);
}
