package Five.TCMK.Lab3;

import java.math.BigInteger;
import java.security.SecureRandom;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Deque;
import java.util.List;
import java.util.concurrent.CancellationException;
import java.util.function.Function;

public class Decompose {

    private static final SecureRandom RANDOM = new SecureRandom();
    private static final int BATCH_SIZE = 128;
    private static final int MAX_ATTEMPT_STEPS = 6_400_000;
    private static final int PMINUS1_START_BOUND = 1_000;
    private static final int PMINUS1_MAX_BOUND = 1_000_000;


    private static int[] sievePrimes = new int[0];

    private static int fillPrimesUpTo(int bound)
    {
        if(bound <= sievePrimes.length)
        {
            return sievePrimes.length;
        }

        boolean[] composite = new boolean[bound + 1];
        for(int i = 2; (long) i * i <= bound; i++)
        {
            if(!composite[i])
            {
                for(int j = i * i; j <= bound; j += i)
                {
                    composite[j] = true;
                }
            }
        }

        List<Integer> primes = new ArrayList<>();
        for(int i = 2; i <= bound; i++)
        {
            if(!composite[i])
            {
                primes.add(i);
            }
        }

        int[] result = new int[primes.size()];
        for(int i = 0; i < result.length; i++)
        {
            result[i] = primes.get(i);
        }
        sievePrimes = result;
        return result.length;
    }


    private static BigInteger pMinus1Divisor(BigInteger n, boolean verbose)
    {
        for(int bound = PMINUS1_START_BOUND; bound <= PMINUS1_MAX_BOUND; bound *= 4)
        {
            checkInterrupted();
            BigInteger a = BigInteger.TWO;
            boolean failed = false;

            if(verbose)
            {
                System.out.println("База разложения: " + bound);
            }

            int count = fillPrimesUpTo(bound);
            for(int index = 0; index < count; index++)
            {
                int prime = sievePrimes[index];
                if(prime > bound)
                {
                    break;
                }

                long power = prime;
                while(power * prime <= bound)
                {
                    power *= prime;
                }
                BigInteger exponent = BigInteger.valueOf(power);
                a = a.modPow(exponent, n);

                if(verbose)
                {
                    System.out.println("l = " + exponent + ", a = " + a);
                }

                if((index & 31) == 0)
                {
                    checkInterrupted();
                }

                BigInteger divisor = a.subtract(BigInteger.ONE).gcd(n);
                if(!divisor.equals(BigInteger.ONE))
                {
                    if(divisor.equals(n))
                    {

                        failed = true;
                        break;
                    }
                    return divisor;
                }
            }

            if(failed)
            {
                return null;
            }
        }
        return null;
    }
    private static final BigInteger[] SMALL_PRIMES = {
        BigInteger.valueOf(3), BigInteger.valueOf(5), BigInteger.valueOf(7),
        BigInteger.valueOf(11), BigInteger.valueOf(13), BigInteger.valueOf(17),
        BigInteger.valueOf(19), BigInteger.valueOf(23), BigInteger.valueOf(29),
        BigInteger.valueOf(31), BigInteger.valueOf(37), BigInteger.valueOf(41),
        BigInteger.valueOf(43), BigInteger.valueOf(47), BigInteger.valueOf(53),
        BigInteger.valueOf(59), BigInteger.valueOf(61), BigInteger.valueOf(67),
        BigInteger.valueOf(71), BigInteger.valueOf(73), BigInteger.valueOf(79),
        BigInteger.valueOf(83), BigInteger.valueOf(89), BigInteger.valueOf(97)
    };

    private static void checkInterrupted()
    {
        if(Thread.currentThread().isInterrupted())
        {
            throw new CancellationException("Factorization interrupted");
        }
    }

    private static BigInteger rhoDivisorFloyd(BigInteger n, boolean verbose)
    {
        BigInteger ONE = BigInteger.ONE;
        BigInteger x0 = ONE;
        int limit = 10_000;

        while(true)
        {
            checkInterrupted();
            BigInteger c = randomBelow(n.subtract(ONE)).add(ONE);
            BigInteger a = x0;
            BigInteger b = x0;
            BigInteger d = ONE;

            for(int step = 0; step < limit && d.equals(ONE); step++)
            {
                if((step & 31) == 0)
                {
                    checkInterrupted();
                }

                a = F(a, c, n);
                b = F(F(b, c, n), c, n);
                d = a.subtract(b).abs().gcd(n);

                if(verbose)
                {
                    System.out.println("a = " + a + ", b = " + b + ", НОД(a-b, n) = " + d);
                }
            }

            if(!d.equals(n))
            {
                return d;
            }

            limit = Math.min(limit * 2, MAX_ATTEMPT_STEPS);
        }
    }

    private static BigInteger brentDivisor(BigInteger n)
    {
        int budget = 100_000;
        while(true)
        {
            checkInterrupted();
            BigInteger c = randomBelow(n.subtract(BigInteger.ONE)).add(BigInteger.ONE);
            BigInteger y = randomBelow(n);
            int steps = 0;
            int length = 1;

            while(steps < budget)
            {
                BigInteger x = y;
                for(int i = 0; i < length && steps < budget; i++)
                {
                    if((i & (BATCH_SIZE - 1)) == 0)
                    {
                        checkInterrupted();
                    }
                    y = F(y, c, n);
                    steps++;
                }

                int offset = 0;
                boolean retry = false;
                while(offset < length && steps < budget)
                {
                    checkInterrupted();
                    BigInteger batchStart = y;
                    BigInteger product = BigInteger.ONE;
                    int count = Math.min(BATCH_SIZE, Math.min(length - offset, budget - steps));
                    for(int i = 0; i < count; i++)
                    {
                        y = F(y, c, n);
                        product = product.multiply(x.subtract(y).abs()).mod(n);
                    }
                    steps += count;
                    offset += count;
                    BigInteger divisor = product.gcd(n);

                    if(divisor.equals(n))
                    {
    
                        for(int i = 0; i < count; i++)
                        {
                            batchStart = F(batchStart, c, n);
                            divisor = x.subtract(batchStart).abs().gcd(n);
                            if(!divisor.equals(BigInteger.ONE) && !divisor.equals(n))
                            {
                                return divisor;
                            }
                        }
                        retry = true;
                        break;
                    }
                    if(!divisor.equals(BigInteger.ONE))
                    {
                        return divisor;
                    }
                }
                if(retry)
                {
                    break;
                }
                length *= 2;
            }
            budget = Math.min(budget * 2, MAX_ATTEMPT_STEPS);
        }
    }

    public static BigInteger F(BigInteger x, BigInteger c, BigInteger n)
    {
        return x.multiply(x).add(c).mod(n);
    }

    public static BigInteger randomBelow(BigInteger n)
    {
        BigInteger r;
        do
        {
            r = new BigInteger(n.bitLength(), RANDOM);
        }
        while(r.compareTo(n) >= 0);
        return r;
    }

    public static Boolean RhoPollard(BigInteger n)
    {
        return RhoPollard(n, false);
    }

    public static Boolean RhoPollard(BigInteger n, boolean verbose)
    {

        BigInteger ONE = BigInteger.ONE;
        BigInteger TWO = BigInteger.valueOf(2);
        List<BigInteger> factors = new ArrayList<>();
        Deque<BigInteger> pending = new ArrayDeque<>();
        pending.push(n);

        while(!pending.isEmpty())
        {
            checkInterrupted();
            BigInteger current = pending.pop();
            int twos = current.getLowestSetBit();
            for(int i = 0; i < twos; i++)
            {
                factors.add(TWO);
            }
            current = current.shiftRight(twos);
            for(BigInteger prime : SMALL_PRIMES)
            {
                while(true)
                {
                    BigInteger[] division = current.divideAndRemainder(prime);
                    if(division[1].signum() != 0)
                    {
                        break;
                    }
                    factors.add(prime);
                    current = division[0];
                }
                if(current.equals(ONE))
                {
                    break;
                }
            }
            if(current.equals(ONE))
            {
                continue;
            }
            if(current.isProbablePrime(100))
            {
                factors.add(current);
                continue;
            }

            BigInteger divisor = rhoDivisorFloyd(current, verbose);
            if(divisor == null)
            {
                divisor = brentDivisor(current);
            }
            pending.push(divisor);
            pending.push(current.divide(divisor));
        }

        Collections.sort(factors);
        System.out.println(factors);
        return true;
    }


    public static Boolean PMinus1(BigInteger n)
    {
        return PMinus1(n, false);
    }

    public static Boolean PMinus1(BigInteger n, boolean verbose)
    {
        if(n.signum() <= 0)
        {
            throw new IllegalArgumentException("n must be positive");
        }

        BigInteger ONE = BigInteger.ONE;
        BigInteger TWO = BigInteger.valueOf(2);
        List<BigInteger> factors = new ArrayList<>();
        Deque<BigInteger> pending = new ArrayDeque<>();
        pending.push(n);

        while(!pending.isEmpty())
        {
            checkInterrupted();
            BigInteger current = pending.pop();
            int twos = current.getLowestSetBit();
            for(int i = 0; i < twos; i++)
            {
                factors.add(TWO);
            }
            current = current.shiftRight(twos);
            for(BigInteger prime : SMALL_PRIMES)
            {
                while(true)
                {
                    BigInteger[] division = current.divideAndRemainder(prime);
                    if(division[1].signum() != 0)
                    {
                        break;
                    }
                    factors.add(prime);
                    current = division[0];
                }
                if(current.equals(ONE))
                {
                    break;
                }
            }
            if(current.equals(ONE))
            {
                continue;
            }
            if(current.isProbablePrime(100))
            {
                factors.add(current);
                continue;
            }

            BigInteger divisor = pMinus1Divisor(current, verbose);
            if(divisor == null)
            {
                divisor = brentDivisor(current);
            }
            pending.push(divisor);
            pending.push(current.divide(divisor));
        }

        Collections.sort(factors);
        System.out.println(factors);
        return true;
    }

    public static Boolean timedTest(String name, BigInteger n, Function<BigInteger, Boolean> test)
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
        BigInteger val1 = new BigInteger("3692655664584345509");
        BigInteger val2 = new BigInteger("491567273268656308119212755636928578489");
        BigInteger val3 = new BigInteger("15001130225340576213314924303539086147166581600235267913822957208411676781245489");
        System.out.println("-----------------1 число--------------------------------------");
        timedTest("Po-Метод Полларда", val1, n -> Decompose.RhoPollard(n, false));
        timedTest("P-1 Метод Полларда", val1, n -> Decompose.PMinus1(n, false));
        
        System.out.println("-----------------2 число--------------------------------------");
        timedTest("P-1 Метод Полларда", val2, n -> Decompose.PMinus1(n, false));
        timedTest("Po-Метод Полларда", val2, n -> Decompose.RhoPollard(n, false));

        System.out.println("-----------------3 число---------------------------------------");
        timedTest("Po-Метод Полларда", val3, n -> Decompose.RhoPollard(n, false));
        timedTest("P-1 Метод Полларда", val3, n -> Decompose.PMinus1(n, false));
    }

}
