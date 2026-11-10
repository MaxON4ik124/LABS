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
    private static final int TIMEOUT = 3;

    private static long timeoutDeadline = Long.MAX_VALUE;

    private static void startTimeout()
    {
        timeoutDeadline = System.currentTimeMillis() + TIMEOUT * 1000L;
    }

    private static boolean timedOut()
    {
        return System.currentTimeMillis() >= timeoutDeadline;
    }

    private static final class Trace
    {
        private static final int HEAD = 5;
        private static final int TAIL = 5;
        private final List<String> head = new ArrayList<>();
        private final Deque<String> tail = new ArrayDeque<>();
        private final boolean enabled;
        private int count;

        Trace(boolean enabled)
        {
            this.enabled = enabled;
        }

        void add(String line)
        {
            count++;
            if(!enabled)
            {
                return;
            }
            if(head.size() < HEAD)
            {
                head.add(line);
            }
            else
            {
                if(tail.size() == TAIL)
                {
                    tail.removeFirst();
                }
                tail.addLast(line);
            }
        }

        void print(boolean completed, boolean verbose)
        {
            if(!verbose)
            {
                return;
            }
            if(count == 0)
            {
                System.out.println(completed
                        ? "--- разбиение получено, итераций нет ---"
                        : "--- таймаут до первой итерации ---");
                return;
            }
            List<String> tailList = new ArrayList<>(tail);
            if(count <= HEAD + TAIL)
            {
                for(String line : head)
                {
                    System.out.println(line);
                }
                for(String line : tailList)
                {
                    System.out.println(line);
                }
            }
            else
            {
                for(String line : head)
                {
                    System.out.println(line);
                }
                System.out.println("... (" + (count - HEAD - TAIL) + " итераций пропущено) ...");
                for(String line : tailList)
                {
                    System.out.println(line);
                }
            }
            if(completed)
            {
                System.out.println("--- разбиение получено, всего итераций: " + count + " ---");
            }
            else
            {
                System.out.println("--- таймаут, последние итерации, всего: " + count + " ---");
            }
        }
    }

    // Trace for CFRAC: numerA = A_i (numerator of the convergent),
    // residue = (A_i)^2 mod n, exponent = full exponent vector over the base.
    private static final class CfracTrace
    {
        private static final int HEAD = 5;
        private static final int TAIL = 5;
        private final List<String> head = new ArrayList<>();
        private final Deque<String> tail = new ArrayDeque<>();
        private final boolean enabled;
        private int count;

        CfracTrace(boolean enabled)
        {
            this.enabled = enabled;
        }

        void add(String numerA, BigInteger residue, int[] exponent)
        {
            count++;
            if(!enabled)
            {
                return;
            }
            StringBuilder line = new StringBuilder();
            line.append(count).append(") A = ").append(numerA)
                    .append(", (A)^2 mod n = ").append(residue)
                    .append(", вектор = [");
            for(int i = 0; i < exponent.length; i++)
            {
                if(i > 0)
                {
                    line.append(", ");
                }
                line.append(exponent[i]);
            }
            line.append("]");

            if(head.size() < HEAD)
            {
                head.add(line.toString());
            }
            else
            {
                if(tail.size() == TAIL)
                {
                    tail.removeFirst();
                }
                tail.addLast(line.toString());
            }
        }

        void print(boolean completed, boolean verbose)
        {
            if(!verbose)
            {
                return;
            }
            if(count == 0)
            {
                System.out.println(completed
                        ? "--- разбиение получено (D-гладких значений нет) ---"
                        : "--- таймаут (D-гладких значений не найдено) ---");
                return;
            }
            List<String> tailList = new ArrayList<>(tail);
            for(String line : head)
            {
                System.out.println(line);
            }
            if(count > HEAD + TAIL)
            {
                System.out.println("... (" + (count - HEAD - TAIL) + " итераций пропущено) ...");
            }
            for(String line : tailList)
            {
                System.out.println(line);
            }
            if(completed)
            {
                System.out.println("--- разбиение получено, всего D-гладких значений: " + count + " ---");
            }
            else
            {
                System.out.println("--- таймаут, последние D-гладкие значения, всего: " + count + " ---");
            }
        }
    }


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
        int counter = 0;
        Trace trace = new Trace(verbose);

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
                counter++;
                if(verbose)
                {
                    trace.add(counter + ") l = " + exponent + ", a = " + a);
                }

                if((index & 31) == 0)
                {
                    checkInterrupted();
                    if(timedOut())
                    {
                        trace.print(false, verbose);
                        return null;
                    }
                }

                BigInteger divisor = a.subtract(BigInteger.ONE).gcd(n);
                if(!divisor.equals(BigInteger.ONE))
                {
                    if(divisor.equals(n))
                    {
                        failed = true;
                        break;
                    }
                    trace.print(true, verbose);
                    return divisor;
                }
            }

            if(failed)
            {
                trace.print(false, verbose);
                return null;
            }
        }

        trace.print(false, verbose);
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
        int counter = 0;
        Trace trace = new Trace(verbose);
        BigInteger c = randomBelow(n.subtract(ONE)).add(ONE);
        if(verbose)
        {
            System.out.printf("Отображение: x^2 + %s\n", c);
        }

        while(true)
        {
            checkInterrupted();
            c = randomBelow(n.subtract(ONE)).add(ONE);
            BigInteger a = x0;
            BigInteger b = x0;
            BigInteger d = ONE;

            for(int step = 0; step < limit && d.equals(ONE); step++)
            {
                if((step & 31) == 0)
                {
                    checkInterrupted();
                    if(timedOut())
                    {
                        trace.print(false, verbose);
                        return null;
                    }
                }

                a = F(a, c, n);
                b = F(F(b, c, n), c, n);
                d = a.subtract(b).abs().gcd(n);
                counter++;
                if(verbose)
                {
                    trace.add(counter + ") a = " + a + ", b = " + b + ", НОД(a-b, n) = " + d);
                }
            }

            if(!d.equals(ONE) && !d.equals(n))
            {
                trace.print(true, verbose);
                return d;
            }

            if(limit == MAX_ATTEMPT_STEPS && d.equals(ONE))
            {
                // No collision within the step budget: give up this run.
                trace.print(false, verbose);
                return null;
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
            if(timedOut())
            {
                return null;
            }
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
                        if(timedOut())
                        {
                            return null;
                        }
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


    private static BigInteger cfracDivisor(BigInteger n, boolean verbose)
    {
        fillPrimesUpTo(500);
        BigInteger sqrtN = n.sqrt();

        List<Integer> base = new ArrayList<>();
        for(int i = 0; i < sievePrimes.length && base.size() < 30; i++)
        {
            int prime = sievePrimes[i];
            if(prime == 2 || legendre(n, prime) == 1)
            {
                base.add(prime);
            }
        }

        int relationLimit = base.size() + 5;
        List<BigInteger> aList = new ArrayList<>();
        List<int[]> relations = new ArrayList<>();
        int counter = 0;
        CfracTrace trace = new CfracTrace(verbose);


        BigInteger P = BigInteger.ZERO;
        BigInteger Q = BigInteger.ONE;
        BigInteger aPrev = BigInteger.ZERO;
        BigInteger aCurr = BigInteger.ONE;

        for(int i = 0; i < 500_000; i++)
        {
            checkInterrupted();
            if(timedOut())
            {
                trace.print(false, verbose);
                return null;
            }
            BigInteger a = sqrtN.add(P).divide(Q);

            BigInteger nextP = a.multiply(Q).subtract(P);
            BigInteger nextQ = n.subtract(nextP.multiply(nextP)).divide(Q);
            // A_i may be reduced modulo n: all CFRAC comparisons are modulo n.
            BigInteger nextA = a.multiply(aCurr).add(aPrev).mod(n);

            P = nextP;
            Q = nextQ;
            aPrev = aCurr;
            aCurr = nextA;

            if(Q.signum() == 0)
            {
                return aCurr.subtract(aPrev).abs().gcd(n);
            }

            BigInteger qValue = Q.abs();
            int[] exponent = factorOverBase(qValue, base);
            if(exponent == null)
            {
                continue;
            }

            aList.add(aCurr);
            relations.add(exponent);
            counter++;
            int[] parity = new int[exponent.length];
            for(int j = 0; j < exponent.length; j++)
            {
                parity[j] = exponent[j] & 1;
            }
            trace.add(aCurr.toString(), qValue, parity);

            if(relations.size() >= relationLimit)
            {
                BigInteger divisor = solveRelations(n, base, relations, aList, verbose);
                if(divisor != null)
                {
                    trace.print(true, verbose);
                    return divisor;
                }
                relations.clear();
                aList.clear();
            }
        }

        trace.print(false, verbose);
        return null;
    }


    private static int legendre(BigInteger n, int prime)
    {
        int value = n.mod(BigInteger.valueOf(prime)).intValue();
        if(value == 0)
        {
            return 0;
        }
        BigInteger exponent =
                BigInteger.valueOf((prime - 1) / 2);
        BigInteger result =
                BigInteger.valueOf(value).modPow(
                        exponent, BigInteger.valueOf(prime));
        return result.equals(BigInteger.valueOf(prime - 1)) ? -1 : 1;
    }

    private static int[] factorOverBase(
            BigInteger q, List<Integer> base)
    {
        int[] exponent = new int[base.size()];
        BigInteger value = q;

        for(int i = 0; i < base.size(); i++)
        {
            int prime = base.get(i);
            BigInteger p = BigInteger.valueOf(prime);
            while(value.mod(p).signum() == 0)
            {
                value = value.divide(p);
                exponent[i]++;
            }
        }

        return value.equals(BigInteger.ONE) ? exponent : null;
    }

    // Eliminates parities over GF(2) and tests each dependency for a factor.
    private static BigInteger solveRelations(
            BigInteger n,
            List<Integer> base,
            List<int[]> relations,
            List<BigInteger> aList,
            boolean verbose)
    {
        int size = relations.size();
        int columns = base.size();

        int[][] rowBits = new int[size][(columns + 31) >>> 5];
        BigInteger[] rowMasks = new BigInteger[size];
        for(int i = 0; i < size; i++)
        {
            int[] exponent = relations.get(i);
            for(int j = 0; j < columns; j++)
            {
                if((exponent[j] & 1) == 1)
                {
                    rowBits[i][j >>> 5] |= 1 << (j & 31);
                }
            }
            rowMasks[i] = BigInteger.ONE.shiftLeft(i);
        }

        int[] pivotRow = new int[columns];
        java.util.Arrays.fill(pivotRow, -1);

        for(int column = 0; column < columns; column++)
        {
            int selected = -1;
            for(int row = 0; row < size; row++)
            {
                if(rowMasks[row].signum() != 0
                        && (rowBits[row][column >>> 5] & (1 << (column & 31))) != 0)
                {
                    selected = row;
                    break;
                }
            }
            if(selected == -1)
            {
                continue;
            }

            for(int row = 0; row < size; row++)
            {
                if(row != selected
                        && (rowBits[row][column >>> 5] & (1 << (column & 31))) != 0)
                {
                    for(int k = 0; k < rowBits[row].length; k++)
                    {
                        rowBits[row][k] ^= rowBits[selected][k];
                    }
                    rowMasks[row] = rowMasks[row].xor(rowMasks[selected]);
                }
            }
            pivotRow[column] = selected;
        }

        for(int row = 0; row < size; row++)
        {
            boolean empty = true;
            for(int k = 0; k < rowBits[row].length && empty; k++)
            {
                if(rowBits[row][k] != 0)
                {
                    empty = false;
                }
            }
            if(!empty)
            {
                continue;
            }

            BigInteger x = BigInteger.ONE;
            BigInteger y = BigInteger.ONE;
            for(int bit = 0; bit < size; bit++)
            {
                if(rowMasks[row].testBit(bit))
                {
                    x = x.multiply(aList.get(bit)).mod(n);
                }
            }

            int[] total = new int[columns];
            for(int bit = 0; bit < size; bit++)
            {
                if(rowMasks[row].testBit(bit))
                {
                    int[] exponent = relations.get(bit);
                    for(int j = 0; j < columns; j++)
                    {
                        total[j] += exponent[j];
                    }
                }
            }

            boolean valid = true;
            for(int j = 0; j < columns; j++)
            {
                if((total[j] & 1) != 0)
                {
                    valid = false;
                    break;
                }
                if(total[j] > 0)
                {
                    y = y.multiply(BigInteger.valueOf(base.get(j))
                            .pow(total[j] / 2)).mod(n);
                }
            }
            if(!valid)
            {
                continue;
            }

            BigInteger divisor = x.subtract(y).abs().gcd(n);
            if(!divisor.equals(BigInteger.ONE) && !divisor.equals(n))
            {
                return divisor;
            }
            divisor = x.add(y).gcd(n);
            if(!divisor.equals(BigInteger.ONE) && !divisor.equals(n))
            {
                return divisor;
            }
        }

        return null;
    }

    public static Boolean ContinuedFractions(BigInteger n)
    {
        return ContinuedFractions(n, false);
    }

    public static Boolean ContinuedFractions(BigInteger n, boolean verbose)
    {
        if(n.signum() <= 0)
        {
            throw new IllegalArgumentException("n must be positive");
        }

        startTimeout();

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

            BigInteger divisor = cfracDivisor(current, verbose);
            if(divisor == null)
            {
                if(timedOut())
                {
                    break;
                }
                divisor = rhoDivisorFloyd(current, false);
            }
            if(divisor == null)
            {
                if(timedOut())
                {
                    break;
                }
                divisor = brentDivisor(current);
            }
            if(divisor == null || divisor.equals(BigInteger.ONE))
            {
                break;
            }
            pending.push(divisor);
            pending.push(current.divide(divisor));
        }

        Collections.sort(factors);
        System.out.println(factors);
        return true;
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
        startTimeout();

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
                if(timedOut())
                {
                    break;
                }
                divisor = brentDivisor(current);
            }
            if(divisor == null || divisor.equals(ONE))
            {
                break;
            }
            pending.push(divisor);
            pending.push(current.divide(divisor));
        }

        Collections.sort(factors);
        System.out.println(factors);
        return true;
    }

    public static Boolean PMinus1(BigInteger n, boolean verbose)
    {
        if(n.signum() <= 0)
        {
            throw new IllegalArgumentException("n must be positive");
        }

        startTimeout();

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
                if(timedOut())
                {
                    break;
                }
                divisor = brentDivisor(current);
            }
            if(divisor == null || divisor.equals(ONE))
            {
                break;
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
        timedTest("Po-Метод Полларда", val1, n -> Decompose.RhoPollard(n, true));
        timedTest("P-1 Метод Полларда", val1, n -> Decompose.PMinus1(n, true));
        timedTest("непрерывные дроби", val1, n -> Decompose.ContinuedFractions(n, true));
        
        System.out.println("-----------------2 число--------------------------------------");
        timedTest("P-1 Метод Полларда", val2, n -> Decompose.PMinus1(n, true));
        timedTest("Po-Метод Полларда", val2, n -> Decompose.RhoPollard(n, true));
        timedTest("непрерывные дроби", val2, n -> Decompose.ContinuedFractions(n, true));

        System.out.println("-----------------3 число---------------------------------------");
        timedTest("Po-Метод Полларда", val3, n -> Decompose.RhoPollard(n, true));
        timedTest("P-1 Метод Полларда", val3, n -> Decompose.PMinus1(n, true));
        timedTest("непрерывные дроби", val3, n -> Decompose.ContinuedFractions(n, true));
    }
}
