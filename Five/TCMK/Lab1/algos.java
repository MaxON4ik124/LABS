package Five.TCMK.Lab1;


import java.math.BigInteger;
import java.util.Arrays;

public class algos 
{
    public static BigInteger[] ExtendedGcd(BigInteger a,BigInteger b, boolean comment) 
    {
        BigInteger oldR = a;
        BigInteger r = b;

        BigInteger oldX = BigInteger.ONE;
        BigInteger x = BigInteger.ZERO;

        BigInteger oldY = BigInteger.ZERO;
        BigInteger y = BigInteger.ONE;

        int iteration = 0;

        while (!r.equals(BigInteger.ZERO)) 
        {
            BigInteger q = oldR.divide(r);

            BigInteger nextR = oldR.subtract(q.multiply(r));
            oldR = r;
            r = nextR;

            BigInteger nextX = oldX.subtract(q.multiply(x));
            oldX = x;
            x = nextX;

            BigInteger nextY = oldY.subtract(q.multiply(y));
            oldY = y;
            y = nextY;

            iteration++;

            if(comment)
                System.out.printf("%d итерация: g=%s, x=%s, y=%s%n",iteration, oldR, oldX, oldY);
        }

        return new BigInteger[] { oldR, oldX, oldY };
    }

    private static BigInteger[] HalfCoefficients(BigInteger x, BigInteger y, BigInteger a, BigInteger b) 
    {
        if (x.mod(BigInteger.TWO).equals(BigInteger.ZERO) && y.mod(BigInteger.TWO).equals(BigInteger.ZERO)) 
        {
            return new BigInteger[] { x.divide(BigInteger.TWO), y.divide(BigInteger.TWO) };
        }

        return new BigInteger[] { x.add(b).divide(BigInteger.TWO), y.subtract(a).divide(BigInteger.TWO) };
    }

    public static BigInteger[] BinaryExtendedGcd(BigInteger a,BigInteger b, boolean comment) 
    {
        BigInteger common = BigInteger.ONE;
        while (a.mod(BigInteger.TWO).equals(BigInteger.ZERO) && b.mod(BigInteger.TWO).equals(BigInteger.ZERO)) 
        {
            a = a.divide(BigInteger.TWO);
            b = b.divide(BigInteger.TWO);
            common = common.multiply(BigInteger.TWO);
        }

        BigInteger oldR = a;
        BigInteger r = b;
        BigInteger x1 = BigInteger.ONE;
        BigInteger y1 = BigInteger.ZERO;
        BigInteger x2 = BigInteger.ZERO;
        BigInteger y2 = BigInteger.ONE;
        int iteration = 0;

        while (!oldR.equals(BigInteger.ZERO)) 
        {
            while (oldR.mod(BigInteger.TWO).equals(BigInteger.ZERO)) 
            {
                oldR = oldR.divide(BigInteger.TWO);
                BigInteger[] coefficients = HalfCoefficients(x1, y1, a, b);
                x1 = coefficients[0];
                y1 = coefficients[1];
            }

            while (r.mod(BigInteger.TWO).equals(BigInteger.ZERO)) {
                r = r.divide(BigInteger.TWO);
                BigInteger[] coefficients = HalfCoefficients(x2, y2, a, b);
                x2 = coefficients[0];
                y2 = coefficients[1];
            }

            if (oldR.compareTo(r) >= 0) {
                oldR = oldR.subtract(r);
                x1 = x1.subtract(x2);
                y1 = y1.subtract(y2);
            } else {
                r = r.subtract(oldR);
                x2 = x2.subtract(x1);
                y2 = y2.subtract(y1);
            }

            iteration++;
            if(comment)
                System.out.printf("%d итерация: g=%s, x=%s, y=%s%n", iteration, r, x2, y2);
        }
        return new BigInteger[] {common.multiply(r), x2, y2};
    }

    public static BigInteger[] ExtendedGcdRR(BigInteger a, BigInteger b, boolean comment)
    {
        if (a.equals(BigInteger.ZERO)) {
            return new BigInteger[] { b, BigInteger.ZERO, BigInteger.ONE };
        }
        if (b.equals(BigInteger.ZERO)) {
            return new BigInteger[] { a, BigInteger.ONE, BigInteger.ZERO };
        }

        BigInteger oldR = a;
        BigInteger r = b;
        BigInteger oldX = BigInteger.ONE;
        BigInteger oldY = BigInteger.ZERO;
        BigInteger x = BigInteger.ZERO;
        BigInteger y = BigInteger.ONE;
        int iteration = 0;


        while(!r.equals(BigInteger.ZERO))
        {

            BigInteger q =
            (oldR.multiply(BigInteger.valueOf(2L)).add(r)).
            divide
            (r.multiply(BigInteger.valueOf(2L)));

            BigInteger nextR = oldR.subtract(q.multiply(r));

            oldR = r.abs();
            r = nextR.abs();

            BigInteger nextX = oldX.subtract(q.multiply(x));
            BigInteger nextY = oldY.subtract(q.multiply(y));


            oldX = x;
            x = nextX;

            oldY = y;
            y = nextY;

            iteration++;
            if(comment)
                System.out.printf("%d итерация: r=%s, x=%s, y=%s%n", iteration, oldR, oldX, oldY);
        }
        return new BigInteger[] { oldR.abs(), oldX, oldY };
    }

    public static void CalcGcd(BigInteger a, BigInteger b, boolean com1, boolean com2, boolean com3)
    {
        System.out.println("Расширенный алгоритм Евклида");
        long start = System.nanoTime();
        BigInteger[] result = ExtendedGcd(a, b, com1);
        long end = System.nanoTime();
        double timeSec = (end - start) / 1_000_000_000.0;
        System.out.println("Ответ: " + Arrays.toString(result));
        System.out.printf("Время выполнения %.7f с.%n", timeSec);


        System.out.println("Бинарный расширенный алгоритм Евклида:");
        start = System.nanoTime();
        result = BinaryExtendedGcd(a, b, com2);
        end = System.nanoTime();
        timeSec = (end - start) / 1_000_000_000.0;
        System.out.println("Ответ: " + Arrays.toString(result));
        System.out.printf("Время выполнения %.7f с.%n", timeSec);

        System.out.println("Расширенный алгоритм Евклида с усеченными остатками:");
        start = System.nanoTime();
        result = ExtendedGcdRR(a, b, com3);
        end = System.nanoTime();
        timeSec = (end - start) / 1_000_000_000.0;
        System.out.println("Ответ: " + Arrays.toString(result));
        System.out.printf("Время выполнения %.7f с.%n", timeSec);
    }
    public static void main(String[] args) 
    {
        BigInteger a1 = new BigInteger("9190812423861359177");
        BigInteger b1 = new BigInteger("5665686725157642793");

        BigInteger a2 = new BigInteger("382875700183783912180972380599677237411");
        BigInteger b2 = new BigInteger("850028528056170803249499731597831491891");

        BigInteger a3 = new BigInteger("6568591084326103188389903689528446432238935028773467398030238114964024225816689");
        BigInteger b3 = new BigInteger("8213990379675158500374799385935382213186265977913450415675510020284382035371583");

        CalcGcd(a1, b1, false, false, false);

        CalcGcd(a2, b2, false, false, false);

        CalcGcd(a3, b3, false, false, false);
    }
}
