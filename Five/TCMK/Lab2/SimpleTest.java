package Five.TCMK.Lab2;

import java.math.BigInteger;
import java.security.SecureRandom;
import java.util.ArrayList;
import java.util.function.Function;

public class SimpleTest 
{    
    private static final int TESTS_COUNT = 5;
    private static final SecureRandom RANDOM = new SecureRandom();
    
    static Boolean FermaTest(BigInteger n)
    {
        ArrayList<BigInteger> PassedBases = new ArrayList<>();
        BigInteger exponent = n.subtract(BigInteger.ONE);

        for (int i = 0; i < TESTS_COUNT; i++)
        {

            BigInteger a;
            do 
            {
                a = randomBase(n);
            } 
            while (!a.gcd(n).equals(BigInteger.ONE));

            if (!a.modPow(exponent, n).equals(BigInteger.ONE))
            {
                System.out.printf("Число %s составное. a = %s\n", n, a);
                return false;
            }
            PassedBases.add(a);
        }

        System.out.printf("Число %s Вероятно простое. Основания:\n", n);
        for(BigInteger base : PassedBases)
        {
            System.out.printf("a = %s\n", base);
            System.out.printf("Проверка: %s ^ %s (mod %s) = 1\n", base, exponent, n);
        }

        return true;
    }

    static int jacobi(BigInteger a, BigInteger n) 
    {
        a = a.mod(n);
        int result = 1;
        BigInteger two = BigInteger.valueOf(2);
        BigInteger four = BigInteger.valueOf(4);
        BigInteger eight = BigInteger.valueOf(8);

        while (!a.equals(BigInteger.ZERO)) 
        {
            while (!a.testBit(0)) {
                a = a.divide(two);

                int nMod8 = n.mod(eight).intValue();
                if (nMod8 == 3 || nMod8 == 5) {
                    result = -result;
                }
            }

            BigInteger temp = a;
            a = n;
            n = temp;

            if (a.mod(four).equals(BigInteger.valueOf(3)) &&
                n.mod(four).equals(BigInteger.valueOf(3))) {
                result = -result;
            }

            a = a.mod(n);
        }

        return n.equals(BigInteger.ONE) ? result : 0;
    }

    static Boolean SolovayShtrassenTest(BigInteger n)
    {
        ArrayList<BigInteger> PassedBases = new ArrayList<>();
        ArrayList<BigInteger> Jakobies = new ArrayList<>();
        BigInteger exponent = n.subtract(BigInteger.ONE)
                .divide(BigInteger.TWO);

        for (int i = 0; i < TESTS_COUNT; i++)
        {

            BigInteger a;
            do 
            {
                a = randomBase(n);
            } 
            while (!a.gcd(n).equals(BigInteger.ONE));
            int Jakobi = jacobi(a, n);

            if (Jakobi == 0)
            {
                System.out.printf("Число %s составное. a = %s\n", n, a);
                return false;
            }

            BigInteger x = a.modPow(exponent, n);

            BigInteger expect = (Jakobi == 1) ?  BigInteger.ONE : n.subtract(BigInteger.ONE);

            if(!x.equals(expect))
            {
                System.out.printf("Число %s составное. a = %s\n", n, a);
                return false;
            }
            Jakobies.add(expect);
            PassedBases.add(a);
        }
        System.out.printf("Число %s вероятно простое. Основания:\n", n);
        for(int i = 0;i < 5;i++)
        {
            System.out.printf("a = %s\nПроверка: %s ^ %s (mod %s) = %s\n", PassedBases.get(i), PassedBases.get(i), exponent, n, Jakobies.get(i));
        }
        return true;
    }

    private static BigInteger randomBase(BigInteger n)
    {
        BigInteger range = n.subtract(BigInteger.valueOf(3));
        BigInteger offset;

        do
        {
            offset = new BigInteger(range.bitLength(), RANDOM);
        }
        while (offset.compareTo(range) >= 0);

        return offset.add(BigInteger.TWO);
    }

    static Boolean MillerRabinTest(BigInteger n)
    {
        ArrayList<BigInteger> PassedBases = new ArrayList<>();
        ArrayList<Integer> PassedR = new ArrayList<>();
        ArrayList<Integer> ReasonPass = new ArrayList<>();

        BigInteger d = n.subtract(BigInteger.ONE);
        int s = 0;
        
        while (!d.testBit(0))
        {
            d = d.divide(BigInteger.TWO);
            s++;
        }

        BigInteger nMinusOne = n.subtract(BigInteger.ONE);

        for (int i = 0; i < TESTS_COUNT; i++)
        {

            BigInteger a;;
            do 
            {
                a = randomBase(n);
            }
            while (!a.gcd(n).equals(BigInteger.ONE));
            BigInteger x = a.modPow(d, n);

            if (x.equals(BigInteger.ONE))
            {
                ReasonPass.add(1);
                PassedBases.add(a);
                PassedR.add(-1);
                continue;
            }
            else if (x.equals(nMinusOne))
            {
                ReasonPass.add(2);
                PassedBases.add(a);
                PassedR.add(0);
                continue;
            }   


            boolean passed = false;

            for (int r = 1; r < s; r++)
            {
                x = x.multiply(x).mod(n);

                if (x.equals(nMinusOne))
                {
                    PassedR.add(r);
                    ReasonPass.add(2);
                    PassedBases.add(a);
                    passed = true;
                    break;
                }
            }

            if (!passed)
            {
                System.out.printf("Число %d составное. a = %s\n", n, a);
                return false;
            }
        }
        System.out.printf("Число %s вероятно простое. Основания: \n", n);
        for(int i = 0;i < 5;i++)
        {
            System.out.printf("a = %s\n", PassedBases.get(i));
            if(ReasonPass.get(i) == 1)
                System.out.printf("Проверка: %s ^ %s (mod %s) = 1\n", PassedBases.get(i), d, n);
            if(ReasonPass.get(i) == 2)
                System.out.printf("Проверка: %s ^ ((2 ^ %d) * %s) = %s (mod %s)\n", PassedBases.get(i), PassedR.get(i), d, nMinusOne, n);
        }
        return true;
    }



    private static Boolean timedTest(String name, BigInteger n,
                                     Function<BigInteger, Boolean> test)
    {
        long start = System.nanoTime();
        Boolean result = test.apply(n);
        long elapsed = System.nanoTime() - start;
        System.out.printf("Время теста %s: %d нс (%.3f мс)%n",
                name, elapsed, elapsed / 1_000_000.0);
        return result;
    }

    public static void main(String[] args)
    {
        long start = System.nanoTime();
        RANDOM.nextBytes(new byte[32]);
        BigInteger num1 = new BigInteger("13217255198004962887");
        BigInteger num2 = new BigInteger("1752846681491936797148622077603952008089");
        BigInteger num3 = new BigInteger("317565151248733323062037418099788024733");
        BigInteger num4 = new BigInteger("10290608798586102391723793837292981274471443732802263657809874438237047137603303");
        
        System.out.println("============================Тест Ферма=========================================");
        timedTest("Ферма", num1, SimpleTest::FermaTest);
        System.out.println("===============================================================================");
        timedTest("Ферма", num2, SimpleTest::FermaTest);
        System.out.println("===============================================================================");
        timedTest("Ферма", num3, SimpleTest::FermaTest);
        System.out.println("===============================================================================");
        timedTest("Ферма", num4, SimpleTest::FermaTest);
        System.out.println("=============================Тест Соловэя-Штрассена============================");
        timedTest("Соловэя-Штрассена", num1, SimpleTest::SolovayShtrassenTest);
        System.out.println("===============================================================================");
        timedTest("Соловэя-Штрассена", num2, SimpleTest::SolovayShtrassenTest);
        System.out.println("===============================================================================");
        timedTest("Соловэя-Штрассена", num3, SimpleTest::SolovayShtrassenTest);
        System.out.println("===============================================================================");
        timedTest("Соловэя-Штрассена", num4, SimpleTest::SolovayShtrassenTest);
        System.out.println("=============================Тест Миллера-Рабина===============================");
        timedTest("Миллера-Рабина", num1, SimpleTest::MillerRabinTest);
        System.out.println("===============================================================================");
        timedTest("Миллера-Рабина", num2, SimpleTest::MillerRabinTest);
        System.out.println("===============================================================================");
        timedTest("Миллера-Рабина", num3, SimpleTest::MillerRabinTest);
        System.out.println("===============================================================================");
        timedTest("Миллера-Рабина", num4, SimpleTest::MillerRabinTest);
        System.out.println("===============================================================================");

        BigInteger Carm1 = new BigInteger("1334733877147062382486934807105197899496002201113849920496510541601");
        BigInteger Carm2 = new BigInteger("260849323075371835669784094383812120359260783810157225730623388382401");

        System.out.println("============================Тест Чисел Кармайкла===============================");
        System.out.println("============================Тест Ферма=========================================");
        timedTest("Ферма", Carm1, SimpleTest::FermaTest);
        System.out.println("===============================================================================");
        timedTest("Ферма", Carm2, SimpleTest::FermaTest);
        System.out.println("=============================Тест Соловэя-Штрассена============================");
        timedTest("Соловэя-Штрассена", Carm1, SimpleTest::SolovayShtrassenTest);
        System.out.println("===============================================================================");
        timedTest("Соловэя-Штрассена", Carm2, SimpleTest::SolovayShtrassenTest);
        System.out.println("=============================Тест Миллера-Рабина===============================");
        timedTest("Миллера-Рабина", Carm1, SimpleTest::MillerRabinTest);
        System.out.println("===============================================================================");
        timedTest("Миллера-Рабина", Carm2, SimpleTest::MillerRabinTest);
    }
}
