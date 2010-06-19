/*
 * Fixed-point aritmetika za MIDletPascal
 * */

public class F
{

    private F()
    {
    }

    // fromInt
    public static int fI(int i)
    {
        return i << 12;
    }

    // fromint
    public static int fI(int a, int b)
    {
	StringBuffer s = new StringBuffer();
	s.append(a);
	s.append('.');
	s.append(b);
    	return fI(s.toString(),10);
    }

    // fromInt
    public static int fI(String s, int radix)
    {
        int i = 0;
        if(s.charAt(0) == '-')
            i = 1;
        String s1 = "-1";
        int j = s.indexOf('.');
        if(j >= 0)
        {
            for(s1 = s.substring(j + 1, s.length()); s1.length() < 4; s1 = s1 + "0");
            if(s1.length() > 4)
                s1 = s1.substring(0, 4);
        } else
        {
            j = s.length();
        }
        int k = Integer.parseInt(s.substring(i, j), radix);
        int l = Integer.parseInt(s1, radix) + 1;
        int i1 = (k << 12) + (l << 12) / 10000;
        if(i == 1)
            i1 = -i1;
        return i1;
    }

    // toString
    public static String tS(int i)
    {
        boolean flag = false;
        if(i < 0)
        {
            flag = true;
            i = -i;
        }
        int j = i >> 12;
        int k = 10000 * (i & 0xfff) >> 12;
        String s;
        for(s = Integer.toString(k); s.length() < 4; s = "0" + s);
        return (flag ? "-" : "") + Integer.toString(j) + "." + s;
    }

    // toInt
    public static int tI(int i)
    {
        if(i >= 0)
            i += 2048;
        else
            i -= 2048;
        return i >> 12;
    }

    // div
    public static int D(int i, int j)
    {
        boolean flag = false;
        if(j == 4096)
            return i;
        if((j & 0xfff) == 0)
            return i / (j >> 12);
        if(i < 0)
        {
            i = -i;
            flag = true;
        }
        if(j < 0)
        {
            j = -j;
            if(flag)
                flag = false;
            else
                flag = true;
        }
        byte byte0 = 0;
        if(i > 0x64fff)
            byte0 = 3;
        if(i > 0x3e8fff)
            byte0 = 4;
        if(i > 0x7dffff)
            byte0 = 6;
        if(i > 0x1f4ffff)
            byte0 = 8;
        if(i > 0x7dfffff)
            byte0 = 10;
        if(byte0 > 0)
        {
            int k = 2 << byte0 - 1;
            i += k;
            j += k;
        }
        int l = (i << 12 - byte0) / (j >> byte0);
        return flag ? -l : l;
    }

    // mult
    public static int M(int i, int j)
    {
        boolean flag = false;
        if((i & 0xfff) == 0)
            return (i >> 12) * j;
        if((j & 0xfff) == 0)
            return i * (j >> 12);
        if(i < 0 && j > 0 || i > 0 && j < 0)
            flag = true;
        if(i < 0)
            i = -i;
        if(j < 0)
            j = -j;
        byte byte0 = 0;
        if(i > 0x64fff || j > 0x64fff)
            byte0 = 2;
        if(i > 0x3e8fff || j > 0x3e8fff)
            byte0 = 4;
        if(i > 0x271ffff || j > 0x271ffff)
            byte0 = 6;
        if(byte0 > 0)
        {
            int k = 2 << byte0 - 1;
            i += k;
            j += k;
        }
        int l = (i >> 12) * (j >> 12) << 12;
        int i1 = (i & 0xfff) * (j & 0xfff) >> 12;
        i1 += ((i & 0xfffff000) >> byte0) * ((j & 0xfff) >> byte0) >> 12 - (byte0 << 1);
        l = l + i1 + (((i & 0xfff) >> byte0) * ((j & 0xfffff000) >> byte0) >> 12 - (byte0 << 1));
        if(l < 0)
            return 0;
        else
            return flag ? -l : l;
    }

    // abs
    public static int A(int i)
    {
        if(i < 0)
            return -i;
        else
            return i;
    }

    // sqrt
    public static int S(int i, int j)
    {
        if(i < 0)
            return 0;
        if(i == 0)
            return 0;
        int k = i + 4096 >> 1;
        for(int l = 0; l < j; l++)
            k = k + D(i, k) >> 1;

        if(k < 0)
            return 0;
        else
            return k;
    }

    // sqrt
    public static int S(int i)
    {
        byte byte0 = 8;
        if(i > 0x64000)
            byte0 = 12;
        if(i > 0x3e8000)
            byte0 = 16;
        return S(i, byte0);
    }

    // sin
    public static int s(int i)
    {
        int j = 0;
        for(; i < 0; i += 25736);
        if(i > 25736)
            i %= 25736;
        int k = (i * 10) / 714;
        if(i != 0 && i != 6434 && i != 12868 && i != 19302 && i != 25736)
            j = (i * 100) / 714 - k * 10;
        if(k <= 90)
            return sin_lookup(k, j);
        if(k <= 180)
            return sin_lookup(180 - k, j);
        if(k <= 270)
            return -sin_lookup(k - 180, j);
        else
            return -sin_lookup(360 - k, j);
    }

    private static int sin_lookup(int i, int j)
    {
        if(j > 0 && j < 10 && i < 90)
            return SIN_TABLE[i] + ((SIN_TABLE[i + 1] - SIN_TABLE[i]) / 10) * j;
        else
            return SIN_TABLE[i];
    }

	// cos
    public static int C(int i)
    {
        return s(i + 6435);
    }

    public static int tan(int i)
    {
        int j = D(s(i), C(i));
        return j;
    }

    public static int cot(int i)
    {
        int j = D(fI(1), tan(i));
        return j;
    }

    // asin
    public static int AS(int i)
    {
        boolean flag = false;
        if(i < 0)
            flag = true;
        if(A(i) > 4096)
	    return 0;
        i = A(i);
        int j = 0;
        int k = SIN_TABLE.length;
        for(int l = 0; l < SIN_TABLE.length; l++)
        {
            int i1 = A(i - SIN_TABLE[l]);
            if(i1 < k)
            {
                k = i1;
                j = l;
            }
        }

        if(flag)
            return -(j * 72);
        else
            return j * 72;
    }

    // acos
    public static int AC(int i)
    {
        return 6434 - AS(i);
    }

    // exp
    public static int e(int i)
    {
        int j = A(i) >> 12;
        if(j > 13)
            return 0;
        int ai[] = {
            4096, 11134, 30266, 0x1415e, 0x36992, 0x9469c, 0x1936dc, 0x448a21, 0xba4f54, 0x1fa7158, 
            0x560a774, 0xe9e2244, 0x27bc2ca9, 0x6c02d646
        };
        int k = ai[j];
        int l = A(i) & 0xfff;
        if(l > 0)
        {
            int i1 = 0;
            int j1 = 4096;
            int k1 = 1;
            for(int l1 = 0; l1 < 6; l1++)
            {
                i1 += j1 / k1;
                j1 = M(j1, l);
                k1 *= l1 + 1;
            }

            k = M(k, i1);
        }
        if(i < 0)
            return D(4096, k);
        else
            return k;
    }

    public static int log(int i)
    {
        if(i <= 0)
            return 0;
        int j = 0;
        boolean flag = false;
        int l;
        for(l = 0; i > 8192; l++)
            i >>= 1;

        int i1 = l * 2839;
        int j1 = 0;
        i -= 4096;
        for(int k1 = 1; k1 < 11; k1++)
        {
            int k;
            if(j == 0)
                k = i;
            else
                k = M(j, i);
            if(k == 0)
                break;
            j1 += ((k1 % 2 != 0 ? 1 : -1) * k) / k1;
            j = k;
        }

        return i1 + j1;
    }

    public static int log10(int i)
    {
    	return D(log(i), log(2));
    }

    // pow
    public static int p(int i, int j)
    {
        boolean flag = false;
        int k = 1;
        if(j < 0)
        {
            flag = true;
            j = -j;
        }
        if(A(i) < 4096 && j > 12288)
        {
            k = e(M(log(i), j));
        } else
        {
            k = pi(i, j >> 12);
            if((j & 0xfff) != 0)
                k = M(k, e(M(log(i), j & 0xfff)));
        }
        if(flag)
            return D(4096, k);
        else
            return k;
    }

    private static int pi(int i, int j)
    {
        int k = 4096;
        if(i == 0)
            return 0;
        for(int l = 0; l < j; l++)
            k = M(k, i);

        if(k < 0)
            return 0;
        else
            return k;
    }

    // to degrees
    public static int tD(int i)
    {
    	return 180*D(i, PI);
    }

    // to radians
    public static int tR(int i)
    {
    	return M(i/180, PI);
    }

    public static int atan(int i)
    {
    	return AC(D(fI(1), S(1+M(i, i))));
    }

    public static int atan2(int y, int x)
    {
    	int res;
	if (x == 0)
	{
		if (y == 0)
			return 0;
		if (y > 0)
			return PI/2;
		return -PI/2;
	}
	
	if (x>0)
		return atan(D(y,x));

	res = PI - atan(A(D(y,x)));

	if (y>=0)
		return res;
	return -res;
    }

    public static final int MAX_VALUE = 0x7fffffff;
    public static final int MIN_VALUE = 0x80000001;
    public static final int E = 11134;
    public static final int PI = 12868;
    private static final int SIN_TABLE[] = {
        0, 71, 142, 214, 285, 357, 428, 499, 570, 641, 
        711, 781, 851, 921, 990, 1060, 1128, 1197, 1265, 1333, 
        1400, 1468, 1534, 1600, 1665, 1730, 1795, 1859, 1922, 1985, 
        2048, 2109, 2170, 2230, 2290, 2349, 2407, 2464, 2521, 2577, 
        2632, 2686, 2740, 2793, 2845, 2896, 2946, 2995, 3043, 3091, 
        3137, 3183, 3227, 3271, 3313, 3355, 3395, 3434, 3473, 3510, 
        3547, 3582, 3616, 3649, 3681, 3712, 3741, 3770, 3797, 3823, 
        3849, 3872, 3895, 3917, 3937, 3956, 3974, 3991, 4006, 4020, 
        4033, 4045, 4056, 4065, 4073, 4080, 4086, 4090, 4093, 4095, 
        4096
    };

}
