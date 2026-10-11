package Five.TCMK.Lab2;

import java.math.BigInteger;
import java.security.SecureRandom;

public class SimpleTest 
{    
    private static final int TESTS_COUNT = 5;
    private static final SecureRandom RANDOM = new SecureRandom();

    static Boolean FermaTest(BigInteger n)
    {

        BigInteger exponent = n.subtract(BigInteger.ONE);

        for (int i = 0; i < TESTS_COUNT; i++)
        {
            BigInteger a = randomBase(n);

            if (!a.gcd(n).equals(BigInteger.ONE) ||
                !a.modPow(exponent, n).equals(BigInteger.ONE))
            {
                return false;
            }
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

        BigInteger exponent = n.subtract(BigInteger.ONE)
                .divide(BigInteger.TWO);

        for (int i = 0; i < TESTS_COUNT; i++)
        {
            BigInteger a = randomBase(n);
            int Jakobi = jacobi(a, n);

            if (Jakobi == 0) 
                return false;

            BigInteger x = a.modPow(exponent, n);

            BigInteger expect = (Jakobi == 1) ?  BigInteger.ONE : n.subtract(BigInteger.ONE);

            if(!x.equals(expect))
                return false;
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
        if (n.compareTo(BigInteger.TWO) < 0)
            return false;

        if (n.equals(BigInteger.TWO) || n.equals(BigInteger.valueOf(3)))
            return true;

        if (!n.testBit(0))
            return false;

       
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
            BigInteger a = randomBase(n);
            BigInteger x = a.modPow(d, n);

            if (x.equals(BigInteger.ONE) || x.equals(nMinusOne))
                continue;

            boolean passed = false;

            for (int r = 1; r < s; r++)
            {
                x = x.multiply(x).mod(n);

                if (x.equals(nMinusOne))
                {
                    passed = true;
                    break;
                }
            }

            if (!passed)
                return false;
        }

        return true;
    }



    public static void main(String[] args)
    {
        BigInteger num1 = new BigInteger("13217255198004962887");
        BigInteger num2 = new BigInteger("1752846681491936797148622077603952008089");
        BigInteger num3 = new BigInteger("317565151248733323062037418099788024733");
        BigInteger num4 = new BigInteger("10290608798586102391723793837292981274471443732802263657809874438237047137603303");
        
    }
}
