int jump_compression(int, int, int, int, int);

int main()
{
    jump_compression(1, 2, 3, 4, 5);
}
int jump_compression( i, j, k, l, m )
int i, j, k, l, m;
{
       beg_1:
               if( i < j )
                   if( j < k )
                       if( k < l )
						   if( l < m )
                               l += m;
                           else
                               goto end_1;
                       else
                           k += l;
                   else {
                       j += k;
                        end_1:
                       goto beg_1;
                   }
               else
                   i += j;
               return( i + j + k + l + m );
           } /* Конец jump_compression */