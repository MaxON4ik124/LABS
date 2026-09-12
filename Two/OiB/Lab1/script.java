package OiB.Lab1;
import java.io.UnsupportedEncodingException;
import java.math.BigInteger;
public class script {
    public static int Ng = 40002; // Ng - номер группы
    public static int Nl = 7; // (4 + 10) / 2 = 7; Nl - порядковый номер студента в группе, так как студентов двое, взято среднее арифметическое от порядковых номеров обоих студентов с округлением
    public static int Sr = 13; // ("Х" = 23; "Б" = 2; (23+2) / 2 = 12.5 ~ 13) Sr - порядковый номер 3 буквы фамилии, так как студентов двое, взято среднее арифметическое от порядковых номеров обоих студентов с округлением
    static BigInteger pow(BigInteger base, BigInteger index)
    {
        BigInteger res = base;
        if(index.equals(BigInteger.ZERO)) return BigInteger.ONE;
        for(int i = 1;i < index.intValue();i++)
        {
            res = res.multiply(base);
        }
        return res;
    }
    static void task_1()
    {
        BigInteger result = pow(BigInteger.valueOf(Nl + Ng), BigInteger.valueOf(11));
        result = result.add(BigInteger.valueOf(Sr));
        result = result.mod(BigInteger.valueOf(11));
        System.out.printf("Result of task 1 is: %d\n", result);
    }
    static void task_2(int k, String Name1, String Name2)
    {
        System.out.println("Result of task 2:");
        String alp = "абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
        char[] encodedName1 = Name1.toCharArray();
        char[] encodedName2 = Name2.toCharArray();
        for(int i = 0;i < Name1.length();i++)
        {
            if(Name1.charAt(i) == ' ') continue;
            encodedName1[i] = alp.charAt((alp.indexOf(Character.toLowerCase(Name1.charAt(i))) + k) % alp.length());
            if(Character.isUpperCase(Name1.charAt(i))) encodedName1[i] = Character.toUpperCase(encodedName1[i]);
        }
        System.out.printf("Encoded 1st name is: %s\n", String.valueOf(encodedName1));
        for(int i = 0;i < Name2.length();i++)
        {
            if(Name2.charAt(i) == ' ') continue;
            encodedName2[i] = alp.charAt((alp.indexOf(Character.toLowerCase(Name2.charAt(i))) + k) % alp.length());
            if(Character.isUpperCase(Name2.charAt(i))) encodedName2[i] = Character.toUpperCase(encodedName2[i]);
        }
        System.out.printf("Encoded 2nd name is: %s\n", String.valueOf(encodedName2));
    }
    static long NOD(long x, long y)
    {
        while(x != 0 && y != 0)
        {
            if(x > y)
            {
                // System.out.println("Вероятный НОД: " + x);
                x = x % y;
            }
            else
            {
                // System.out.println("Вероятный НОД: "+ y);
                y = y % x;
            }
        }
        // System.out.println("НОД: " + (x+y));
        return x + y;
    }
    static long NOD(long x, long y, long z)
    {
        return NOD(NOD(x, y), z);
    }
    static void task_3()
    {
        long A = (long)Math.pow((Ng * (8 + (Nl % 7))), 2);
        long B = (26102006 + 8122006) / 2;
        System.out.println("Solve of task 3:");
        System.out.println("НОД(A, B(mod 95) + 900):" + NOD(A, B % 95 + 900));
        System.out.println("НОД(A, (B+50)(mod 97) + 700): " + NOD(A, ((B + 50) % 97 + 700)));
        System.out.println("НОД(A, (B+50)(mod 101) + 1500, (B - 40)(mod 103) + 2500): " + NOD(A, ((B+50)% 101 + 1500), ((B - 40) % 103 + 2500)));
    }
    static boolean check(BigInteger x, BigInteger a, BigInteger t, int s)
    {
        BigInteger A;
        for(int k = 0;k < s;k++)
        {
            BigInteger ind = t.multiply(pow(BigInteger.TWO, BigInteger.valueOf(k)));
            A = pow(a, ind);
            if((A.mod(x)).equals(x.subtract(BigInteger.ONE))) return false;
        }
        return true;
    }
    static void task_4(int x)
    {
        System.out.println("Solve of task 4:");
        if(x % 2 == 0 && x != 2) System.out.printf("Число %d составное\n", x);
        else
        {
            int s = 0;
            int t = x-1;
            while(t % 2 == 0)
            {
                s++;
                t /= 2;
            }
            BigInteger X = BigInteger.valueOf(x);
            BigInteger T = BigInteger.valueOf(t);
            for(int a = 2;a < x;a++)
            {
                BigInteger A = BigInteger.valueOf(a);
                BigInteger A_1 = A.modPow(T, X);
                if(x % a == 0)
                {
                    System.out.printf("Число %d составное\n", x);
                    return;
                }
                if((A_1.equals(X.subtract(BigInteger.ONE))) && check(X, A, T, s))
                {
                    System.out.printf("Число %d составное\n", x);
                    return;
                }
            }
            System.out.printf("Число %d простое\n", x);
        }
    }
    static long decrypt(long n, int e)
    {
        long matrix[][] = {{1, 0, eiler(n)}, {0, 1, e}};
        while(matrix[1][2] != 1)
        {
            long Q = matrix[0][2] / matrix[1][2];
            long row[] = matrix[1];
            long iterrow[] = matrix[0];
            for(int i = 0;i < 3;i++) iterrow[i] -= Q * row[i];
            matrix[0] = row;
            matrix[1] = iterrow;
        }
        long d = matrix[1][1];
        return d;
    }
    static long eiler(long x)
    {
        long res = 0;
        for(int i = 0;i < x;i++)
            if(NOD(x, i) == 1) res++;
        return res;
    }
    static void task_5(int p, int q, int e)
    {
        System.out.println("Solve of task 5:");
        long n = p*q;
        System.out.println("n = " + n);
        long f = eiler(n);
        System.out.println("ф(n) = " + f);
        long d = decrypt(n, e);
        System.out.println("d = " + d);
    }
    static BigInteger[] encode(int[] message, int e, int n, int len)
    {
        BigInteger[] encoded = new BigInteger[len];
        for(int i = 0;i < len;i++) encoded[i] = BigInteger.valueOf(message[i]).modPow(BigInteger.valueOf(e), BigInteger.valueOf(n));
        return encoded;
    }
    static int[] decode(BigInteger[] encoded, long d, int n, int len)
    {
        BigInteger[] decoded = new BigInteger[len];
        int[] message = new int[len];
        for(int i = 0;i < len;i++) 
        {
            decoded[i] = encoded[i].modPow(BigInteger.valueOf(d), BigInteger.valueOf(n));
            message[i] = decoded[i].intValue();
        }
        return message;
    }
    static void task_6(String message, int e, int n)
    {
        System.out.println("Solve of task 6:");
        char text[] = message.toCharArray();
        int[] intmess = new int[message.length()];
        for(int i = 0;i < message.length();i++) intmess[i] = (int)text[i];
        BigInteger[] encoded = encode(intmess, e, n, message.length());
        System.out.println("Перевод в числа:");
        for(int i = 0;i < message.length();i++) System.out.print(text[i] + " -> " + intmess[i] + " ");
        System.out.println("\n Шифрование: ");
        for(int i = 0;i < message.length();i++) System.out.print(encoded[i] + " ");
        System.out.println("\n Расшифрование:");
        long d = decrypt(n, e);
        int[] decoded = decode(encoded, d, n, message.length());
        for(int i = 0;i < message.length();i++) System.out.print(decoded[i] + " -> " + (char)decoded[i] + " ");
        System.out.print("\n");
    }
    static int create_sign(long x, long d, long n)
    {
        BigInteger S = BigInteger.valueOf(x);
        S = S.modPow(BigInteger.valueOf(d), BigInteger.valueOf(n));
        return S.intValue();
    }
    static boolean check_sign(int s, int e, long x, long n)
    {
        BigInteger X = BigInteger.valueOf(s);
        X = X.modPow(BigInteger.valueOf(e), BigInteger.valueOf(n));
        return X.longValue() == x;
    }
    static void task_7(String message, long n, int e)
    {
        System.out.println("Solve of task 7:");
        char[] text = message.toCharArray();
        long d = decrypt(n, e);
        long x = 0;
        for(int i = 0;i < message.length();i++) x += (int)text[i];
        int s = create_sign(x, d, n);
        System.out.println("s = " + s);
        if(check_sign(s, e, x, n)) System.out.println("Подпись подтверждена");
        else System.out.println("Подпись не подтверждена");
    }
    static void task_8(int a, int x, int y, long n)
    {
        BigInteger A = BigInteger.valueOf(a).modPow(BigInteger.valueOf(x), BigInteger.valueOf(n));
        BigInteger B = BigInteger.valueOf(a).modPow(BigInteger.valueOf(y), BigInteger.valueOf(n));
        System.out.println("Solve of task 8:");
        System.out.println("A: " + A + " B: " + B);
        BigInteger X_1 = B.modPow(BigInteger.valueOf(x), BigInteger.valueOf(n));
        BigInteger X_2 = A.modPow(BigInteger.valueOf(y), BigInteger.valueOf(n));
        System.out.println("Сеансовые ключи: " + X_1 + " " + X_2);
        BigInteger ex_1 = BigInteger.valueOf(a).modPow(BigInteger.valueOf(x*y), BigInteger.valueOf(n));
        BigInteger ex_2 = (pow(pow(BigInteger.valueOf(a), BigInteger.valueOf(x)), BigInteger.valueOf(y))).mod(BigInteger.valueOf(n));
        BigInteger ex_3 = (pow(pow(BigInteger.valueOf(a), BigInteger.valueOf(y)), BigInteger.valueOf(x))).mod(BigInteger.valueOf(n));
        System.out.println("a ^ (xy) (mod n) = " + ex_1);
        System.out.println("(a ^ x) ^ y (mod n) = " + ex_2);
        System.out.println("(a ^ y) ^ x (mod n) = " + ex_3);
        if(ex_1.equals(ex_2) && ex_1.equals(ex_3)) System.out.println("Равенство: a ^ (xy) = (a ^ x) ^ y = (a ^ y) ^ x (mod n) верно");
        else System.out.println("Равенство: a ^ (xy) = (a ^ x) ^ y = (a ^ y) ^ x (mod n) неверно");
    }
    static int[] extendedNOD(int a, int b)
    {
        if(b == 0) return new int[]{a, 1, 0};
        else
        {
            int[] res = extendedNOD(b, a % b);
            int nod = res[0];
            int x1 = res[1];
            int y1 = res[2];
            int x = y1;
            int y = x1 - (a/b) * y1;
            return new int[]{nod, x, y};
        }
    }
    static int mod_op(int a, int m)
    {
        int[] res = extendedNOD(a, m);
        int nod = res[0];
        int x = res[1];

        if(nod != 1) return 0;
        else return (x % m + m) % m;
    }
    static int encodeBP(char letter, int[] keyO)
    {   
        int sum = 0;
        try 
        {
            byte[] bytes = String.valueOf(letter).getBytes("Windows-1251");
            byte b = bytes[0];
            String binc = String.format("%8s", Integer.toBinaryString(b & 0xFF)).replace(' ', '0');
            char[] binar = binc.toCharArray();
            for(int i = 0;i < keyO.length;i++)
            {
                if(binar[i] == '1') sum += keyO[i];
            }
            // System.out.println(binar);
            return sum;
            
        }
        catch (UnsupportedEncodingException e)
        { 
            System.out.println("ERROR!"); 
            return 0;
        }
    }
    static char[] permutate_bits(int[] key, int sum)
    {
        char[] res = {'0','0','0','0','0','0','0','0'};
        int remaind = sum;
        int i = key.length-1;
        while(remaind >= key[0])
        {
            if(key[i] > remaind) i--;
            else
            {
                remaind -= key[i];
                res[i] = '1';
            }
            if(i == -1) i = key.length-1;
        }
        return res;
    }
    static char decodeBP(int encoded, int[] keyC, int m, int n)
    {
        try
        {
            char res;
            int n_op = mod_op(n, m);
            int sum = (encoded * n_op) % m;
            String binar = new String(permutate_bits(keyC, sum));
            byte b = (byte)Integer.parseInt(binar, 2);
            byte[] barr = {b};
            String r = new String(barr, "Windows-1251");
            res = r.charAt(0);
            return res;
        }
        catch(UnsupportedEncodingException e)
        {
            System.out.println("ERROR!");
            return 0;
        }
    }
    static void task_9(String message)
    {
        System.out.println("Solve of task 9:");
        int[] keyC = {1, 3, 5, 10, 21, 42, 84, 167};
        int[] keyO = {19, 57, 95, 190, 64, 128, 256, 158};
        char[] text = message.toCharArray();
        int[] encoded = new int[message.length()];
        char[] decoded = new char[message.length()];
        for(int i = 0;i < message.length();i++) encoded[i] = encodeBP(text[i], keyO);
        System.out.println("Закодированное сообщение: ");
        for(int i = 0;i < message.length();i++) System.out.printf("%d ", encoded[i]);
        for(int i = 0;i < message.length();i++) decoded[i] = decodeBP(encoded[i], keyC, 335, 19);
        System.out.println("\nДекодированное сообщение: ");
        for(int i = 0;i < message.length();i++) System.out.print(decoded[i]);
    }
    public static void main(String[] args)
    {
        task_1();
        task_2(3, "Михайлич Максим Игоревич", "Библив Александр Игоревич");
        task_3();
        task_4(23);
        task_5(443, 941,31);
        task_6("apple", 31, 416863);
        task_7("Политех", 416863,  31);
        task_8(12, 34, 11, 416863);
        task_9("C");
    }
}