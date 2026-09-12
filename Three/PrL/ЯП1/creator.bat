gcc -S test1.c
gcc -S -ffloat-store -fno-defer-pop -O3 -fno-inline -finline-functions -fkeep-inline-functions -fno-function-cse -ffast-math -funroll-loops -funroll-all-loops -fverbose-asm -o test1_opt.S test1.c
gcc -S test2.c
gcc -S -ffloat-store -fno-defer-pop -O3 -fno-inline -finline-functions -fkeep-inline-functions -fno-function-cse -ffast-math -funroll-loops -funroll-all-loops -fverbose-asm -o test2_opt.S test2.c
gcc -S test3.c
gcc -S -ffloat-store -fno-defer-pop -O3 -fno-inline -finline-functions -fkeep-inline-functions -fno-function-cse -ffast-math -funroll-loops -funroll-all-loops -fverbose-asm -o test3_opt.S test3.c
gcc -S test4.c
gcc -S -ffloat-store -fno-defer-pop -O3 -fno-inline -finline-functions -fkeep-inline-functions -fno-function-cse -ffast-math -funroll-loops -funroll-all-loops -fverbose-asm -o test4_opt.S test4.c
gcc -S test5.c
gcc -S -ffloat-store -fno-defer-pop -O3 -fno-inline -finline-functions -fkeep-inline-functions -fno-function-cse -ffast-math -funroll-loops -funroll-all-loops -fverbose-asm -o test5_opt.S test5.c
gcc -S test6.c
gcc -S -ffloat-store -fno-defer-pop -O3 -fno-inline -finline-functions -fkeep-inline-functions -fno-function-cse -ffast-math -funroll-loops -funroll-all-loops -fverbose-asm -o test6_opt.S test6.c
gcc -S test7.c
gcc -S -ffloat-store -fno-defer-pop -O3 -fno-inline -finline-functions -fkeep-inline-functions -fno-function-cse -ffast-math -funroll-loops -funroll-all-loops -fverbose-asm -o test7_opt.S test7.c
gcc -S test8.c
gcc -S -ffloat-store -fno-defer-pop -O3 -fno-inline -finline-functions -fkeep-inline-functions -fno-function-cse -ffast-math -funroll-loops -funroll-all-loops -fverbose-asm -o test8_opt.S test8.c
gcc -S test9.c
gcc -S -ffloat-store -fno-defer-pop -O3 -fno-inline -finline-functions -fkeep-inline-functions -fno-function-cse -ffast-math -funroll-loops -funroll-all-loops -fverbose-asm -o test9_opt.S test9.c
gcc -S test10.c
gcc -S -ffloat-store -fno-defer-pop -O3 -fno-inline -finline-functions -fkeep-inline-functions -fno-function-cse -ffast-math -funroll-loops -funroll-all-loops -fverbose-asm -o test10_opt.S test10.c
