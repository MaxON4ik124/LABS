package Five.TCMK.Lab1;


import java.math.BigInteger;
import java.util.Arrays;

public class algos {

    public static BigInteger[] ExtendedGcd(BigInteger a,BigInteger b, boolean comment) {
        BigInteger oldR = a;
        BigInteger r = b;

        BigInteger oldX = BigInteger.ONE;
        BigInteger x = BigInteger.ZERO;

        BigInteger oldY = BigInteger.ZERO;
        BigInteger y = BigInteger.ONE;

        int iteration = 0;
        if(comment)
            System.out.printf("%d iteration: g=%s, x=%s, y=%s%n",iteration, oldR, oldX, oldY);

        while (!r.equals(BigInteger.ZERO)) {
            BigInteger quotient = oldR.divide(r);

            BigInteger nextR = oldR.subtract(quotient.multiply(r));
            oldR = r;
            r = nextR;

            BigInteger nextX = oldX.subtract(quotient.multiply(x));
            oldX = x;
            x = nextX;

            BigInteger nextY = oldY.subtract(quotient.multiply(y));
            oldY = y;
            y = nextY;

            iteration++;

            if(comment)
                System.out.printf("%d итерация: g=%s, x=%s, y=%s%n",iteration, oldR, oldX, oldY);
        }

        return new BigInteger[] { oldR, oldX, oldY };
    }

    private static BigInteger[] HalfCoefficients(
            BigInteger x,
            BigInteger y,
            BigInteger a,
            BigInteger b
    ) {
        if (x.mod(BigInteger.TWO).equals(BigInteger.ZERO)
                && y.mod(BigInteger.TWO).equals(BigInteger.ZERO)) {
            return new BigInteger[] {
                x.divide(BigInteger.TWO),
                y.divide(BigInteger.TWO)
            };
        }

        return new BigInteger[] {
            x.add(b).divide(BigInteger.TWO),
            y.subtract(a).divide(BigInteger.TWO)
        };
    }

    public static BigInteger[] BinaryExtendedGcd(BigInteger a,BigInteger b, boolean comment) {
        if (a.signum() < 0 || b.signum() < 0) {
            throw new IllegalArgumentException("Numbers must be non-negative");
        }
        if (a.equals(BigInteger.ZERO)) {
            return new BigInteger[] { b, BigInteger.ZERO, BigInteger.ONE };
        }
        if (b.equals(BigInteger.ZERO)) {
            return new BigInteger[] { a, BigInteger.ONE, BigInteger.ZERO };
        }

        BigInteger common = BigInteger.ONE;
        while (a.mod(BigInteger.TWO).equals(BigInteger.ZERO)
                && b.mod(BigInteger.TWO).equals(BigInteger.ZERO)) {
            a = a.divide(BigInteger.TWO);
            b = b.divide(BigInteger.TWO);
            common = common.multiply(BigInteger.TWO);
        }

        BigInteger u = a;
        BigInteger v = b;
        BigInteger x1 = BigInteger.ONE;
        BigInteger y1 = BigInteger.ZERO;
        BigInteger x2 = BigInteger.ZERO;
        BigInteger y2 = BigInteger.ONE;
        int iteration = 0;

        while (!u.equals(BigInteger.ZERO)) {
            while (u.mod(BigInteger.TWO).equals(BigInteger.ZERO)) {
                u = u.divide(BigInteger.TWO);
                BigInteger[] coefficients = HalfCoefficients(x1, y1, a, b);
                x1 = coefficients[0];
                y1 = coefficients[1];
            }

            while (v.mod(BigInteger.TWO).equals(BigInteger.ZERO)) {
                v = v.divide(BigInteger.TWO);
                BigInteger[] coefficients = HalfCoefficients(x2, y2, a, b);
                x2 = coefficients[0];
                y2 = coefficients[1];
            }

            if (u.compareTo(v) >= 0) {
                u = u.subtract(v);
                x1 = x1.subtract(x2);
                y1 = y1.subtract(y2);
            } else {
                v = v.subtract(u);
                x2 = x2.subtract(x1);
                y2 = y2.subtract(y1);
            }

            iteration++;
            if(comment)
                System.out.printf("%d итерация: g=%s, x=%s, y=%s%n", iteration, v, x2, y2);
        }
        return new BigInteger[] {common.multiply(v), x2, y2};
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

    public static void main(String[] args) {
        BigInteger a = new BigInteger("9190812423861359177");
        BigInteger b = new BigInteger("5665686725157642793");
        System.out.println("Расширенный алгоритм Евклида");
        BigInteger[] result = ExtendedGcd(a, b, false);
        System.out.println("Ответ: " + Arrays.toString(result));
        System.out.println("Бинарный расширенный алгоритм Евклида:");
        result = BinaryExtendedGcd(a, b, false);
        System.out.println("Ответ: " + Arrays.toString(result));
        System.out.println("Расширенный алгоритм Евклида с усеченными остатками:");
        result = ExtendedGcdRR(a, b, false);
        System.out.println("Ответ: " + Arrays.toString(result));
    }
}
