// Real: Java integer implementation of 63-bit precision floating point
// Version 1.07
//
// Copyright 2003-2005 Roar Lauritzsen <roarl@pvv.org>
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// The following link provides a copy of the GNU General Public License:
//     http://www.gnu.org/licenses/gpl.txt
// If you are unable to obtain the copy from this address, write to
// the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
// Boston, MA 02111-1307 USA
//
//
// ----------------------------------------------------------------------------
//
// class Real
//
// Public fields:
//   long mantissa
//   int exponent
//   byte sign
//
// Constructors/assignment:
//   Real()                                 <==  0
//   Real(Real)                             <==  Real
//   Real(int)                              <==  int
//   Real(long)                             <==  long
//   Real(String)                           <==  "-1.234e56"
//   Real(String, int base)                 <==  "-1.234e56" / "/FFF3.2e-10"
//   Real(int s, int e, long m)             <==  (-1)**s * 2**(e-62) * m
//   Real(byte[] data, int offset)          <==  data[offset]..data[offset+11]
//   assign(Real)
//   assign(int)
//   assign(long)
//   assign(String)
//   assign(String, int base)
//   assign(int s, int e, long m)
//   assign(byte[] data, int offset)
//   assignFloatBits(int)                   <==  IEEE754 32-bits float format
//   assignDoubleBits(long)                 <==  IEEE754 64-bits double format
//
// Output:
//   String toString()                      ==>  "-1.234e56"
//   String toString(int base)              ==>  "-1.234e56" / "03.FEe56"
//   String toString(NumberFormat)          ==>  e.g. 
//   int  toInteger()                       ==>  int
//   long toLong()                          ==>  long
//   void toBytes(byte[] data, int offset)  ==>  data[offset]..data[offset+11]
//   int  toFloatBits()                     ==>  IEEE754 32-bits float format
//   long toDoubleBits()                    ==>  IEEE754 64-bits double format
//
//
// (Error bounds are calculated using William Rossi's rossi.dfp.dfp at
// 40 decimal digits accuracy. Error bounds may increase when results
// approach zero or infinity. Error bound of ½ ULP means that the
// result is correctly rounded)
//
//                                               Approx error  Execution time
//                        "Explanation"          bound (ULPs)  (rel. to add)
// Binary operators:
//   x.add(Real y)       : x+=y                       ½         ««« 1.0 »»»
//   sub(Real)           : x-=y                       ½             2.0
//   mul(Real)           : x*=y                       ½             1.2
//   div(Real)           : x/=y                       ½             1.8
//   rdiv(Real)          : x=y/x                      ½             2.2
//   mod(Real)           : x=x-y*floor(x/y)           0              20
//   divf(Real)          : x=floor(x/y)               0              16
//   and(Real)           : x&=y                       0             1.5
//   or(Real)            : x|=y                       0             1.6
//   xor(Real)           : x^=y                       0             1.5
//   bic(Real)           : x&=~y                      0             1.4
//   swap(Real)          : tmp=x, x=y, y=tmp          0             0.4
//
// Functions:
//   x.abs()             : x=|x|                      0             0.1
//   neg()               : x=-x                       0             0.2
//   sqr()               : x=x*x                      ½             0.9
//   recip()             : x=1/x                      ½             1.5
//   rsqrt()             : x=1/sqrt(x)                2              29
//   sqrt()              : (square root)              2              31
//   cbrt()              : (cube root)                2              31
//   exp()               : x=e**x                     2              75
//   exp2()              : x=2**x                     1              70
//   exp10()             : x=10**x                    2              75
//   ln()                                             2             100
//   log2()                                           1             100
//   log10()                                          2             100
//   sin()                                            2              40
//   cos()                                            2              40
//   tan()                                            3              80
//   asin()                                           3             160
//   acos()                                           3             160
//   atan()                                           2             130
//   sinh()                                           2             150
//   cosh()                                           2             150
//   tanh()                                           2             150
//   asinh()                                          2             130
//   acosh()                                          2             130
//   atanh()                                          2             100
//   fact()              : x=x!                      15           6-300
//   gamma()             : x=(x-1)!                 100+            300
//   erfc()              : (compl error func)     2**19        120-4000
//   toDHMS()            : H -> yyyymmddHH.MMSS       ?              20
//   fromDHMS()          : yyyymmddHH.MMSS -> H       ?              17
//   random()            : random number [0,1)        -              50
//
// Binary functions:
//   hypot(Real)         : x=sqrt(x*x+y*y)            2              34
//   y.atan2(Real x)     : y=atan(y/x)   (-pi,pi]     2             125
//   pow(Real)           : x**=y                      3             200
//   pow(int)            : x**=y                      ½              55
//   nroot(Real)         : x**=1/y                    3             175
//
// Integral values:
//   floor()                                          0             0.4
//   ceil()                                           0             1.6
//   round()                                          0             1.2
//   trunc()                                          0             1.1
//   frac()                                           0             1.1
//
// Utility functions:
//   copysign(Real)      : x=|x|*y/|y|                0             0.2
//   nextafter(Real)     : x+=(y-x)*epsilon           0             1.5
//   scalbn(int)         : x<<=y                      0             0.3
//
// Make special values:
//   makeZero()                                       0             0.2
//   makeZero(int sign)                               0             0.2
//   makeInfinity(int sign)                           0             0.2
//   makeNan()                                        0             0.2
//
// Comparisons:
//   boolean equalTo(Real)      : x==y                              0.7
//   boolean notEqualTo(Real)   : x!=y                              0.7
//   boolean lessThan(Real)     : x<y                               0.7
//   boolean lessEqual(Real)    : x<=y                              0.7
//   boolean greaterThan(Real)  : x>y                               0.7
//   boolean greaterEqual(Real) : x>=y                              0.7
//   boolean absLessThan(Real)  : |x|<|y|                           0.3
//
// Query state:
//   boolean isZero()                                               0.1
//   boolean isInfinity()                                           0.1
//   boolean isNan()                                                0.1
//   boolean isFinite()                                             0.1
//   boolean isFiniteNonZero()                                      0.1
//   boolean isNegative()                                           0.1
//   boolean isIntegral()                                           0.4
//   boolean isOdd()                                                0.3
//
// Overloaded methods, integer arguments:
//   add(int)                                         ½             2.1
//   sub(int)                                         ½             2.4
//   mul(int)                                         ½             1.3
//   div(int)                                         ½             1.5
//   rdiv(int)                                        ½             3.0
//   boolean equalTo(int)                                           1.4
//   boolean notEqualTo(int)                                        1.4
//   boolean lessThan(int)                                          1.4
//   boolean lessEqual(int)                                         1.4
//   boolean greaterThan(int)                                       1.4
//   boolean greaterEqual(int)                                      1.4
//
// Constants:
//   ZERO     = 0
//   ONE      = 1
//   TWO      = 2
//   THREE    = 3
//   FIVE     = 5
//   TEN      = 10
//   HUNDRED  = 100
//   HALF     = 1/2
//   THIRD    = 1/3
//   TENTH    = 1/10
//   PERCENT  = 1/100
//   SQRT2    = sqrt(2)
//   SQRT1_2  = sqrt(1/2)
//   CBRT1_2  = cbrt(1/2)
//   PI2      = pi*2
//   PI       = pi
//   PI_2     = pi/2
//   PI_4     = pi/4
//   PI_8     = pi/8
//   E        = e
//   LN2      = ln(2)
//   LN10     = ln(10)
//   LOG2E    = log2(e)  = 1/ln(2)
//   LOG10E   = log10(e) = 1/ln(10)
//   MAX      = max non-infinite positive number = 4.197e323228496
//   MIN      = min non-zero positive number     = 2.383e-323228497
//   NAN      = not a number
//   INF      = infinity
//   INF_N    = -infinity
//   ZERO_N   = -0
//   ONE_N    = -1


public final class Real
{
  long mantissa;
  int exponent;
  byte sign;

  // Set the magicRounding to false during numerical algorithms to
  // favour accuracy over prettyness
  public static boolean magicRounding = true;

  public static final Real ZERO = new Real(0,0x00000000,0x0000000000000000L);
  public static final Real ONE = new Real(0,0x40000000,0x4000000000000000L);
//  public static final Real TWO = new Real(0,0x40000001,0x4000000000000000L);
  public static final Real THREE = new Real(0,0x40000001,0x6000000000000000L);
  public static final Real FIVE = new Real(0,0x40000002,0x5000000000000000L);
  public static final Real TEN = new Real(0,0x40000003,0x5000000000000000L);
//  public static final Real HUNDRED= new Real(0,0x40000006,0x6400000000000000L);
//  public static final Real HALF = new Real(0,0x3fffffff,0x4000000000000000L);
//  public static final Real THIRD = new Real(0,0x3ffffffe,0x5555555555555555L);
//  public static final Real TENTH = new Real(0,0x3ffffffc,0x6666666666666666L);
//  public static final Real PERCENT= new Real(0,0x3ffffff9,0x51eb851eb851eb85L);
  public static final Real SQRT2 = new Real(0,0x40000000,0x5a827999fcef3242L);
  public static final Real SQRT1_2= new Real(0,0x3fffffff,0x5a827999fcef3242L);
  public static final Real CBRT1_2= new Real(0,0x3fffffff,0x6597fa94f5b8f20bL);
  public static final Real PI2 = new Real(0,0x40000002,0x6487ed5110b4611aL);
  public static final Real PI = new Real(0,0x40000001,0x6487ed5110b4611aL);
  public static final Real PI_2 = new Real(0,0x40000000,0x6487ed5110b4611aL);
  public static final Real PI_4 = new Real(0,0x3fffffff,0x6487ed5110b4611aL);
  public static final Real PI_8 = new Real(0,0x3ffffffe,0x6487ed5110b4611aL);
  public static final Real E = new Real(0,0x40000001,0x56fc2a2c515da54dL);
  public static final Real LN2 = new Real(0,0x3fffffff,0x58b90bfbe8e7bcd6L);
  public static final Real LN10 = new Real(0,0x40000001,0x49aec6eed554560bL);
//  public static final Real LOG2E = new Real(0,0x40000000,0x5c551d94ae0bf85eL);
  public static final Real LOG10E = new Real(0,0x3ffffffe,0x6f2dec549b9438cbL);
  public static final Real MAX = new Real(0,0x7fffffff,0x7fffffffffffffffL);
  public static final Real MIN = new Real(0,0x00000000,0x4000000000000000L);
//  public static final Real NAN = new Real(0,0x80000000,0x4000000000000000L);
//  public static final Real INF = new Real(0,0x80000000,0x0000000000000000L);
//  public static final Real INF_N = new Real(1,0x80000000,0x0000000000000000L);
//  public static final Real ZERO_N = new Real(1,0x00000000,0x0000000000000000L);
//  public static final Real ONE_N = new Real(1,0x40000000,0x4000000000000000L);

  private static final int clz_magic = 0x7c4acdd;
  private static final byte[] clz_tab =
    { 31,22,30,21,18,10,29, 2,20,17,15,13, 9, 6,28, 1,
      23,19,11, 3,16,14, 7,24,12, 4, 8,25, 5,26,27, 0 };

  public Real() {
  }

  public Real(final Real a) {
    { this.mantissa = a.mantissa; this.exponent = a.exponent; this.sign = a.sign; };
  }

  public Real(int a) {
    assign(a);
  }

  /*public Real(long a) {
    assign(a);
  }*/

  public Real(String a) {
    assign(a,10);
  }

  public Real(String a, int base) {
    assign(a,base);
  }

  public Real(int s, int e, long m) {
    assign(s,e,m);
  }

 /* public Real(byte [] data, int offset) {
    assign(data,offset);
  }*/

  public void assign(final Real a) {
    if (a == null) {
      makeZero();
      return;
    }
    sign = a.sign;
    exponent = a.exponent;
    mantissa = a.mantissa;
  }

  public void assign(int a) {
    if (a==0) {
      makeZero();
      return;
    }
    sign = 0;
    if (a<0) {
      sign = 1;
      a = -a; // Also works for 0x80000000
    }
    // Normalize int
    int t=a; t|=t>>1; t|=t>>2; t|=t>>4; t|=t>>8; t|=t>>16;
    t = clz_tab[(t*clz_magic)>>>27]-1;

    exponent = 0x4000001E-t;
    mantissa = ((long)a)<<(32+t);
  }

 /* public void assign(long a) {
    sign = 0;
    if (a<0) {
      sign = 1;
      a = -a; // Also works for 0x8000000000000000
    }
    exponent = 0x4000003E;
    mantissa = a;
    normalize();
  }*/

  public void assign(String a) {
    assign(a,10);
  }

  public void assign(String a, int base) {
    if (a==null || a.length()==0) {
      assign(ZERO);
      return;
    }
    atof(a,base);
  }

  public void assign(int s, int e, long m) {
    sign = (byte)s;
    exponent = e;
    mantissa = m;
  }

 /* public void assign(byte [] data, int offset) {
    sign = (byte)((data[offset+4]>>7)&1);
    exponent = (((data[offset ]&0xff)<<24)+
                ((data[offset +1]&0xff)<<16)+
                ((data[offset +2]&0xff)<<8)+
                ((data[offset +3]&0xff)));
    mantissa = (((long)(data[offset+ 4]&0x7f)<<56)+
                ((long)(data[offset+ 5]&0xff)<<48)+
                ((long)(data[offset+ 6]&0xff)<<40)+
                ((long)(data[offset+ 7]&0xff)<<32)+
                ((long)(data[offset+ 8]&0xff)<<24)+
                ((long)(data[offset+ 9]&0xff)<<16)+
                ((long)(data[offset+10]&0xff)<< 8)+
                ((long)(data[offset+11]&0xff)));
  }*/

 /* public void toBytes(byte [] data, int offset) {
    data[offset ] = (byte)(exponent>>24);
    data[offset+ 1] = (byte)(exponent>>16);
    data[offset+ 2] = (byte)(exponent>>8);
    data[offset+ 3] = (byte)(exponent);
    data[offset+ 4] = (byte)((sign<<7)+(mantissa>>56));
    data[offset+ 5] = (byte)(mantissa>>48);
    data[offset+ 6] = (byte)(mantissa>>40);
    data[offset+ 7] = (byte)(mantissa>>32);
    data[offset+ 8] = (byte)(mantissa>>24);
    data[offset+ 9] = (byte)(mantissa>>16);
    data[offset+10] = (byte)(mantissa>>8);
    data[offset+11] = (byte)(mantissa);
  }*/

  public void assignFloatBits(int c) {
    sign = (byte)(c>>>31);
    exponent = (c>>23)&0xff;
    mantissa = (long)(c&0x007fffff)<<39;
    if (exponent == 0 && mantissa == 0)
      return; // Valid zero
    if (exponent == 0 && mantissa != 0) {
      // Degenerate small float
      exponent = 0x40000000-126;
      normalize();
      return;
    }
    if (exponent <= 254) {
      // Normal IEEE 754 float
      exponent += 0x40000000-127;
      mantissa |= 1L<<62;
      return;
    }
    if (mantissa == 0)
      makeInfinity(sign);
    else
      makeNan();
  }

 /* public void assignDoubleBits(long c) {
    sign = (byte)(c>>>63);
    exponent = (int)((c>>52)&0x7ff);
    mantissa = (c&0x000fffffffffffffL)<<10;
    if (exponent == 0 && mantissa == 0)
      return; // Valid zero
    if (exponent == 0 && mantissa != 0) {
      // Degenerate small float
      exponent = 0x40000000-1022;
      normalize();
      return;
    }
    if (exponent <= 2046) {
      // Normal IEEE 754 float
      exponent += 0x40000000-1023;
      mantissa |= 1L<<62;
      return;
    }
    if (mantissa == 0)
      makeInfinity(sign);
    else
      makeNan();
  }*/

 /* public int toFloatBits() {
    if ((this.exponent < 0 && this.mantissa != 0))
      return 0x7fffffff; // nan
    int e = exponent-0x40000000+127;
    long m = mantissa;
    // Round properly!
    m += 1L<<38;
    if (m<0) {
      m >>>= 1;
      e++;
      if (exponent < 0) // Overflow
        return (sign<<31)|0x7f800000; // inf
    }
    if ((this.exponent < 0 && this.mantissa == 0) || e > 254)
      return (sign<<31)|0x7f800000; // inf
    if ((this.exponent == 0 && this.mantissa == 0) || e < -22)
      return (sign<<31); // zero
    if (e > 0)
      // Normal IEEE 754 float
      return (sign<<31)|(e<<23)|((int)(m>>>39)&0x007fffff);
    // Degenerate small float
    return (sign<<31)|((int)(m>>>(40-e))&0x007fffff);
  }*/

 /* public long toDoubleBits() {
    if ((this.exponent < 0 && this.mantissa != 0))
      return 0x7fffffffffffffffL; // nan
    int e = exponent-0x40000000+1023;
    long m = mantissa;
    // Round properly!
    m += 1L<<9;
    if (m<0) {
      m >>>= 1;
      e++;
      if (exponent < 0)
        return ((long)sign<<63)|0x7ff0000000000000L; // inf
    }
    if ((this.exponent < 0 && this.mantissa == 0) || e > 2046)
      return ((long)sign<<63)|0x7ff0000000000000L; // inf
    if ((this.exponent == 0 && this.mantissa == 0) || e < -51)
      return ((long)sign<<63); // zero
    if (e > 0)
      // Normal IEEE 754 double
      return ((long)sign<<63)|((long)e<<52)|((m>>>10)&0x000fffffffffffffL);
    // Degenerate small double
    return ((long)sign<<63)|((m>>>(11-e))&0x000fffffffffffffL);
  }*/

  public void makeZero() {
    sign = 0;
    mantissa = 0;
    exponent = 0;
  }

  public void makeZero(int s) {
    sign = (byte)s;
    mantissa = 0;
    exponent = 0;
  }

  public void makeInfinity(int s) {
    sign = (byte)s;
    mantissa = 0;
    exponent = 0x80000000;
  }

  public void makeNan() {
    sign = 0;
    mantissa = 0x4000000000000000L;
    exponent = 0x80000000;
  }

  public boolean isZero() {
    return (exponent == 0 && mantissa == 0);
  }

  public boolean isInfinity() {
    return (exponent < 0 && mantissa == 0);
  }

  public boolean isNan() {
    return (exponent < 0 && mantissa != 0);
  }

  public boolean isFinite() {
    // That is, non-infinite and non-nan
    return (exponent >= 0);
  }

  public boolean isFiniteNonZero() {
    // That is, non-infinite and non-nan and non-zero
    return (exponent >= 0 && mantissa != 0);
  }

  public boolean isNegative() {
    return sign!=0;
  }

  public void abs() {
    sign = 0;
  }

  public void neg() {
    if (!(this.exponent < 0 && this.mantissa != 0))
      sign ^= 1;
  }

  public void copysign(final Real a) {
    if ((this.exponent < 0 && this.mantissa != 0) || (a.exponent < 0 && a.mantissa != 0)) {
      makeNan();
      return;
    }
    sign = a.sign;
  }

  public void normalize() {
    if ((this.exponent >= 0)) {
      if (mantissa > 0)
      {
        int clz = 0;
        int t = (int)(mantissa>>>32);
        if (t == 0) { clz = 32; t = (int)mantissa; }
        t|=t>>1; t|=t>>2; t|=t>>4; t|=t>>8; t|=t>>16;
        clz += clz_tab[(t*clz_magic)>>>27]-1;

        mantissa <<= clz;
        exponent -= clz;
        if (exponent < 0) // Underflow
          makeZero(sign);
      }
      else if (mantissa < 0)
      {
        mantissa = (mantissa+1)>>>1;
        exponent ++;
        if (mantissa == 0) { // Ooops, it was 0xffffffffffffffffL
          mantissa = 0x4000000000000000L;
          exponent ++;
        }
        if (exponent < 0) // Overflow
          makeInfinity(sign);
      }
      else // mantissa == 0
      {
        exponent = 0;
      }
    }
  }

  public long normalize128(long extra) {
    if (!(this.exponent >= 0))
      return 0;
    if (mantissa==0 && extra==0) {
      exponent = 0;
      return 0;
    }
    if (mantissa < 0) {
      extra = (mantissa<<63)+(extra>>>1);
      mantissa >>>= 1;
      exponent ++;
      if (exponent < 0) // Overflow
        makeInfinity(sign);
      return extra;
    }
    while ((mantissa >>> 47) == 0) {
      mantissa = (mantissa<<16)+(extra>>>48);
      extra <<= 16;
      exponent -= 16;
    }
    while ((mantissa >>> 59) == 0) {
      mantissa = (mantissa<<4)+(extra>>>60);
      extra <<= 4;
      exponent -= 4;
    }
    while ((mantissa >>> 62) == 0) {
      mantissa = (mantissa<<1)+(extra>>>63);
      extra <<= 1;
      exponent --;
    }
    if (exponent < 0) { // Underflow
      makeZero(sign);
      return 0;
    }
    return extra;
  }

  public void roundFrom128(long extra) {
    mantissa += (extra)>>>63;
    normalize();
  }

  private int compare(final Real a) {
    // Compare of normal floats, zeros, but not nan or equal-signed inf
    if ((this.exponent == 0 && this.mantissa == 0) && (a.exponent == 0 && a.mantissa == 0))
      return 0;
    if (sign != a.sign)
      return a.sign-sign;
    int s = (this.sign==0) ? 1 : -1;
    if ((this.exponent < 0 && this.mantissa == 0))
      return s;
    if ((a.exponent < 0 && a.mantissa == 0))
      return -s;
    if (exponent != a.exponent)
      return exponent<a.exponent ? -s : s;
    if (mantissa != a.mantissa)
      return mantissa<a.mantissa ? -s : s;
    return 0;
  }

  private boolean invalidCompare(final Real a) {
    return ((this.exponent < 0 && this.mantissa != 0) || (a.exponent < 0 && a.mantissa != 0) ||
            ((this.exponent < 0 && this.mantissa == 0) && (a.exponent < 0 && a.mantissa == 0) && sign == a.sign));
  }

  public boolean equalTo(final Real a) {
    if (invalidCompare(a))
      return false;
    return compare(a) == 0;
  }

  public boolean notEqualTo(final Real a) {
    if (invalidCompare(a))
      return false;
    return compare(a) != 0;
  }

  public boolean lessThan(final Real a) {
    if (invalidCompare(a))
      return false;
    return compare(a) < 0;
  }

  public boolean lessEqual(final Real a) {
    if (invalidCompare(a))
      return false;
    return compare(a) <= 0;
  }

  public boolean greaterThan(final Real a) {
    if (invalidCompare(a))
      return false;
    return compare(a) > 0;
  }

  public boolean greaterEqual(final Real a) {
    if (invalidCompare(a))
      return false;
    return compare(a) >= 0;
  }

  public boolean absLessThan(final Real a) {
    if ((this.exponent < 0 && this.mantissa != 0) || (a.exponent < 0 && a.mantissa != 0) || (this.exponent < 0 && this.mantissa == 0))
      return false;
    if ((a.exponent < 0 && a.mantissa == 0))
      return true;
    if (exponent != a.exponent)
      return exponent<a.exponent;
    return mantissa<a.mantissa;
  }

  public void scalbn(int n) {
    if (!(this.exponent >= 0 && this.mantissa != 0))
      return;
    exponent += n;
    if (exponent < 0) {
      if (n<0)
        makeZero(sign); // Underflow
      else
        makeInfinity(sign); // Overflow
    }
  }

  public void nextafter(final Real a) {
    if ((this.exponent < 0 && this.mantissa != 0) || (a.exponent < 0 && a.mantissa != 0)) {
      makeNan();
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0) && (a.exponent < 0 && a.mantissa == 0) && sign == a.sign)
      return;
    int dir = -compare(a);
    if (dir == 0)
      return;
    if ((this.exponent == 0 && this.mantissa == 0)) {
      { this.mantissa = MIN.mantissa; this.exponent = MIN.exponent; this.sign = MIN.sign; };
      sign = (byte)(dir<0 ? 1 : 0);
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0)) {
      { this.mantissa = MAX.mantissa; this.exponent = MAX.exponent; this.sign = MAX.sign; };
      sign = (byte)(dir<0 ? 0 : 1);
      return;
    }
    if ((this.sign==0) ^ dir<0) {
      mantissa ++;
    } else {
      if (mantissa == 0x4000000000000000L) {
        mantissa <<= 1;
        exponent--;
      }
      mantissa --;
    }
    normalize();
  }

  public void floor() {
    if (!(this.exponent >= 0 && this.mantissa != 0))
      return;
    if (exponent < 0x40000000) {
      if ((this.sign==0))
        makeZero(sign);
      else {
        exponent = ONE.exponent;
        mantissa = ONE.mantissa;
        // sign unchanged!
      }
      return;
    }
    int shift = 0x4000003e-exponent;
    if (shift <= 0)
      return;

    if ((this.sign!=0))
      mantissa += ((1L<<shift)-1);

    mantissa &= ~((1L<<shift)-1);

    if ((this.sign!=0))
      normalize();
  }

  public void ceil() {
    neg();
    floor();
    neg();
  }

  public void round() {
    if (!(this.exponent >= 0 && this.mantissa != 0))
      return;
    if (exponent < 0x3fffffff) {
      makeZero(sign);
      return;
    }
    int shift = 0x4000003e-exponent;
    if (shift <= 0)
      return;

    mantissa += 1L<<(shift-1); // Bla-bla, this works almost
    mantissa &= ~((1L<<shift)-1);
    normalize();
  }

  public void trunc() {
    if (!(this.exponent >= 0 && this.mantissa != 0))
      return;
    if (exponent < 0x40000000) {
      makeZero(sign);
      return;
    }
    int shift = 0x4000003e-exponent;
    if (shift <= 0)
      return;

    mantissa &= ~((1L<<shift)-1);
    normalize();
  }

  public void frac() {
    if (!(this.exponent >= 0 && this.mantissa != 0) || exponent < 0x40000000)
      return;

    int shift = 0x4000003e-exponent;
    if (shift <= 0) {
      makeZero(sign);
      return;
    }

    mantissa &= ((1L<<shift)-1);
    normalize();
  }

  public int toInteger() {
    if ((this.exponent == 0 && this.mantissa == 0) || (this.exponent < 0 && this.mantissa != 0))
      return 0;
    if ((this.exponent < 0 && this.mantissa == 0)) {
      if ((this.sign==0))
        return 0x7fffffff;
      else
        return 0x80000001; // So that you can take -x.toInteger()
    }
    if (exponent < 0x40000000)
      return 0;
    int shift = 0x4000003e-exponent;
    if (shift < 32) {
      if ((this.sign==0))
        return 0x7fffffff;
      else
        return 0x80000001; // So that you can take -x.toInteger()
    }
    return (this.sign==0) ?
      (int)(mantissa>>>shift) : -(int)(mantissa>>>shift);
  }

/*  public long toLong() {
    if ((this.exponent == 0 && this.mantissa == 0) || (this.exponent < 0 && this.mantissa != 0))
      return 0;
    if ((this.exponent < 0 && this.mantissa == 0)) {
      if ((this.sign==0))
        return 0x7fffffffffffffffL;
      else
        return 0x8000000000000001L; // So that you can take -x.toLong()
    }
    if (exponent < 0x40000000)
      return 0;
    int shift = 0x4000003e-exponent;
    if (shift < 0) {
      if ((this.sign==0))
        return 0x7fffffffffffffffL;
      else
        return 0x8000000000000001L; // So that you can take -x.toLong()
    }
    return (this.sign==0) ? (mantissa>>>shift) : -(mantissa>>>shift);
  }
*/
  public boolean isIntegral() {
    if ((this.exponent < 0 && this.mantissa != 0))
      return false;
    if ((this.exponent == 0 && this.mantissa == 0) || (this.exponent < 0 && this.mantissa == 0))
      return true;
    if (exponent < 0x40000000)
      return false;
    int shift = 0x4000003e-exponent;
    if (shift <= 0)
      return true;
    return (mantissa&((1L<<shift)-1)) == 0;
  }

  public boolean isOdd() {
    if (!(this.exponent >= 0 && this.mantissa != 0) ||
        exponent < 0x40000000 || exponent > 0x4000003e)
      return false;
    int shift = 0x4000003e-exponent;
    return ((mantissa>>>shift)&1) != 0;
  }

 /* public void swap(Real a) {
    long tmpMantissa=mantissa; mantissa=a.mantissa; a.mantissa=tmpMantissa;
    int tmpExponent=exponent; exponent=a.exponent; a.exponent=tmpExponent;
    byte tmpSign =sign; sign =a.sign; a.sign =tmpSign;
  }*/

  // Temporary values used by functions (to avoid "new" inside functions)
  private static Real tmp0 = new Real(); // tmp for basic functions
  private static Real recipTmp = new Real();
  private static Real recipTmp2 = new Real();
  private static Real sqrtTmp = new Real();
  private static Real expTmp = new Real();
  private static Real expTmp2 = new Real();
  private static Real expTmp3 = new Real();
  private static Real tmp1 = new Real();
  private static Real tmp2 = new Real();
  private static Real tmp3 = new Real();
  private static Real tmp4 = new Real();
  private static Real tmp5 = new Real();

  public void add(final Real a) {
    if ((this.exponent < 0 && this.mantissa != 0) || (a.exponent < 0 && a.mantissa != 0)) {
      makeNan();
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0) || (a.exponent < 0 && a.mantissa == 0)) {
      if ((this.exponent < 0 && this.mantissa == 0) && (a.exponent < 0 && a.mantissa == 0) && sign != a.sign)
        makeNan();
      else
        makeInfinity((this.exponent < 0 && this.mantissa == 0) ? sign : a.sign);
      return;
    }
    if ((this.exponent == 0 && this.mantissa == 0) || (a.exponent == 0 && a.mantissa == 0)) {
      if ((this.exponent == 0 && this.mantissa == 0))
        { this.mantissa = a.mantissa; this.exponent = a.exponent; this.sign = a.sign; };
      if ((this.exponent == 0 && this.mantissa == 0))
        sign=0;
      return;
    }

    byte s;
    int e;
    long m;
    if (exponent > a.exponent ||
        (exponent == a.exponent && mantissa>=a.mantissa))
    {
      s = a.sign;
      e = a.exponent;
      m = a.mantissa;
    } else {
      s = sign;
      e = exponent;
      m = mantissa;
      sign = a.sign;
      exponent = a.exponent;
      mantissa = a.mantissa;
    }
    int shift = exponent-e;
    if (shift>=64)
      return;

    if (sign == s) {
      mantissa += m>>>shift;
      if (mantissa >= 0 && shift>0 && ((m>>>(shift-1))&1) != 0)
        mantissa ++; // We don't need normalization, so round now
      if (mantissa < 0) {
        // Simplified normalize()
        mantissa = (mantissa+1)>>>1;
        exponent ++;
        if (exponent < 0) { // Overflow
          makeInfinity(sign);
          return;
        }
      }
    } else {
      if (shift>0) {
        // Shift mantissa up to increase accuracy
        mantissa <<= 1;
        exponent --;
        shift --;
      }
      m = -m;
      mantissa += m>>shift;
      if (mantissa >= 0 && shift>0 && ((m>>>(shift-1))&1) != 0)
        mantissa ++; // We don't need to shift down, so round now
      if (mantissa < 0) {
        // Simplified normalize()
        mantissa = (mantissa+1)>>>1;
        exponent ++; // Can't overflow
      } else {
        // Magic rounding: if result of subtract leaves only a few bits
        // standing, the result should most likely be 0...
        if (magicRounding && mantissa >= 0 && mantissa <= 7)
          mantissa = 0;
        normalize();
      }
    }

    if ((this.exponent == 0 && this.mantissa == 0))
      sign=0;
  }

  public long add128(long extra, final Real a, long aExtra) {
    if ((this.exponent < 0 && this.mantissa != 0) || (a.exponent < 0 && a.mantissa != 0)) {
      makeNan();
      return 0;
    }
    if ((this.exponent < 0 && this.mantissa == 0) || (a.exponent < 0 && a.mantissa == 0)) {
      if ((this.exponent < 0 && this.mantissa == 0) && (a.exponent < 0 && a.mantissa == 0) && sign != a.sign)
        makeNan();
      else
        makeInfinity((this.exponent < 0 && this.mantissa == 0) ? sign : a.sign);
      return 0;
    }
    if ((this.exponent == 0 && this.mantissa == 0) || (a.exponent == 0 && a.mantissa == 0)) {
      if ((this.exponent == 0 && this.mantissa == 0)) {
        { this.mantissa = a.mantissa; this.exponent = a.exponent; this.sign = a.sign; };
        extra = aExtra;
      }
      if ((this.exponent == 0 && this.mantissa == 0))
        sign=0;
      return extra;
    }

    byte s;
    int e;
    long m;
    long x;
    if (exponent > a.exponent ||
        (exponent == a.exponent && mantissa>a.mantissa) ||
        (exponent == a.exponent && mantissa==a.mantissa &&
         (extra>>>1)>=(aExtra>>>1)))
    {
      s = a.sign;
      e = a.exponent;
      m = a.mantissa;
      x = aExtra;
    } else {
      s = sign;
      e = exponent;
      m = mantissa;
      x = extra;
      sign = a.sign;
      exponent = a.exponent;
      mantissa = a.mantissa;
      extra = aExtra;
    }

    int shift = exponent-e;
    if (shift>=127)
      return extra;
    if (shift>=64) {
      x = m>>>(shift-64);
      m = 0;
    } else if (shift>0) {
      x = (x>>>shift)+(m<<(64-shift));
      m >>>= shift;
    }
    extra >>>= 1;
    x >>>= 1;

    if (sign == s) {
      extra += x;
      mantissa += extra>>>63;
      mantissa += m;
    } else {
      extra -= x;
      mantissa -= extra>>>63;
      mantissa -= m;
      // Magic rounding: if result of subtract leaves only a few bits
      // standing, the result should most likely be 0...
      if (mantissa == 0 && extra >= 0 && extra <= 0x1f)
        extra = 0;
    }
    extra <<= 1;

    extra = normalize128(extra);

    if ((this.exponent == 0 && this.mantissa == 0))
      sign=0;

    return extra;
  }

  public void sub(final Real a) {
    tmp0.mantissa = a.mantissa;
    tmp0.exponent = a.exponent;
    tmp0.sign = (byte)(a.sign^1);
    add(tmp0);
  }

  public void mul(int b) {
    if ((this.exponent < 0 && this.mantissa != 0))
      return;
    if (b<0) {
      sign ^= 1;
      b = -b;
    }
    if ((this.exponent == 0 && this.mantissa == 0) || b==0) {
      if ((this.exponent < 0 && this.mantissa == 0))
        makeNan();
      else
        makeZero(sign);
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0))
      return;

    // Normalize int
    int t=b; t|=t>>1; t|=t>>2; t|=t>>4; t|=t>>8; t|=t>>16;
    t = clz_tab[(t*clz_magic)>>>27];
    exponent += 0x1F-t;
    b <<= t;

    if (exponent < 0) {
      makeInfinity(sign); // Overflow
      return;
    }

    long a0 = mantissa & 0x7fffffff;
    long a1 = mantissa >>> 31;
    long b0 = b & 0xffffffffL;

    mantissa = a1*b0;
    // If we're going to need normalization, we don't want to round twice
    int round = (mantissa<0) ? 0 : 0x40000000;
    mantissa += ((a0*b0 + round)>>>31);

    // Simplified normalize()
    if (mantissa < 0) {
      mantissa = (mantissa+1)>>>1;
      exponent ++;
      if (exponent < 0) // Overflow
        makeInfinity(sign);
    }
  }

  public void mul(final Real a) {
    if ((this.exponent < 0 && this.mantissa != 0) || (a.exponent < 0 && a.mantissa != 0)) {
      makeNan();
      return;
    }
    sign ^= a.sign;
    if ((this.exponent == 0 && this.mantissa == 0) || (a.exponent == 0 && a.mantissa == 0)) {
      if ((this.exponent < 0 && this.mantissa == 0) || (a.exponent < 0 && a.mantissa == 0))
        makeNan();
      else
        makeZero(sign);
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0) || (a.exponent < 0 && a.mantissa == 0)) {
      makeInfinity(sign);
      return;
    }

    long a0 = mantissa & 0x7fffffff;
    long a1 = mantissa >>> 31;
    long b0 = a.mantissa & 0x7fffffff;
    long b1 = a.mantissa >>> 31;

    mantissa = a1*b1;
    // If we're going to need normalization, we don't want to round twice
    int round = (mantissa<0) ? 0 : 0x40000000;
    mantissa += ((a0*b1 + a1*b0 + ((a0*b0)>>>31) + round)>>>31);

    int aExp = a.exponent;
    exponent += aExp-0x40000000;
    if (exponent < 0) {
      if (exponent == -1 && aExp < 0x40000000 && mantissa < 0) {
        // Not underflow after all, it will be corrected in the
        // normalization below
      } else {
        if (aExp < 0x40000000)
          makeZero(sign); // Underflow
        else
          makeInfinity(sign); // Overflow
        return;
      }
    }

    // Simplified normalize()
    if (mantissa < 0) {
      mantissa = (mantissa+1)>>>1;
      exponent ++;
      if (exponent < 0) // Overflow
        makeInfinity(sign);
    }
  }

  public long mul128(long extra, final Real a, long aExtra) {
    if ((this.exponent < 0 && this.mantissa != 0) || (a.exponent < 0 && a.mantissa != 0)) {
      makeNan();
      return 0;
    }
    sign ^= a.sign;
    if ((this.exponent == 0 && this.mantissa == 0) || (a.exponent == 0 && a.mantissa == 0)) {
      if ((this.exponent < 0 && this.mantissa == 0) || (a.exponent < 0 && a.mantissa == 0))
        makeNan();
      else
        makeZero(sign);
      return 0;
    }
    if ((this.exponent < 0 && this.mantissa == 0) || (a.exponent < 0 && a.mantissa == 0)) {
      makeInfinity(sign);
      return 0;
    }
    int aExp = a.exponent;
    exponent += aExp-0x40000000;
    if (exponent < 0) {
      if (aExp < 0x40000000)
        makeZero(sign); // Underflow
      else
        makeInfinity(sign); // Overflow
      return 0;
    }
    long ffffffffL = 0xffffffffL;
    long a0 = extra & ffffffffL;
    long a1 = extra >>> 32;
    long a2 = mantissa & ffffffffL;
    long a3 = mantissa >>> 32;
    long b0 = aExtra & ffffffffL;
    long b1 = aExtra >>> 32;
    long b2 = a.mantissa & ffffffffL;
    long b3 = a.mantissa >>> 32;

    a0 = ((a3*b0>>>2)+
          (a2*b1>>>2)+
          (a1*b2>>>2)+
          (a0*b3>>>2)+
          0x60000000)>>>28;
          //(a2*b0>>>34)+(a1*b1>>>34)+(a0*b2>>>34)+0x08000000)>>>28;
    a1 *= b3;
    b0 = a2*b2;
    b1 *= a3;

    a0 += ((a1<<2)&ffffffffL) + ((b0<<2)&ffffffffL) + ((b1<<2)&ffffffffL);
    a1 = (a0>>>32) + (a1>>>30) + (b0>>>30) + (b1>>>30);
    a0 &= ffffffffL;

    a2 *= b3;
    b2 *= a3;
    a1 += ((a2<<2)&ffffffffL) + ((b2<<2)&ffffffffL);

    extra = (a1<<32) + a0;
    mantissa = ((a3*b3)<<2) + (a1>>>32) + (a2>>>30) + (b2>>>30);

    extra = normalize128(extra);
    return extra;
  }

  private void mul10() {
    if (!(this.exponent >= 0 && this.mantissa != 0))
      return;
    mantissa += (mantissa+2)>>>2;
    exponent += 3;
    if (mantissa < 0) {
      mantissa = (mantissa+1)>>>1;
      exponent++;
    }
    if (exponent < 0)
      makeInfinity(sign); // Overflow
  }

  public void sqr() {
    sign = 0;
    if (!(this.exponent >= 0 && this.mantissa != 0))
      return;
    int e = exponent;
    exponent += exponent-0x40000000;
    if (exponent < 0) {
      if (e < 0x40000000)
        makeZero(sign); // Underflow
      else
        makeInfinity(sign); // Overflow
      return;
    }
    long a0 = mantissa&0x7fffffff;
    long a1 = mantissa>>>31;

    mantissa = a1*a1;
    // If we're going to need normalization, we don't want to round twice
    int round = (mantissa<0) ? 0 : 0x40000000;
    mantissa += ((((a0*a1)<<1) + ((a0*a0)>>>31) + round)>>>31);

    // Simplified normalize()
    if (mantissa < 0) {
      mantissa = (mantissa+1)>>>1;
      exponent ++;
      if (exponent < 0) // Overflow
        makeInfinity(sign);
    }
  }

  public void div(int a) {
    if ((this.exponent < 0 && this.mantissa != 0))
      return;
    if (a<0) {
      sign ^= 1;
      a = -a;
    }
    if ((this.exponent < 0 && this.mantissa == 0))
      return;
    if ((this.exponent == 0 && this.mantissa == 0)) {
      if (a==0)
        makeNan();
      return;
    }
    if (a==0) {
      makeInfinity(sign);
      return;
    }
    long denom = a & 0xffffffffL;
    long remainder = mantissa%denom;
    mantissa /= denom;

    // Normalizing mantissa and scaling remainder accordingly
    int clz = 0;
    int t = (int)(mantissa>>>32);
    if (t == 0) { clz = 32; t = (int)mantissa; }
    t|=t>>1; t|=t>>2; t|=t>>4; t|=t>>8; t|=t>>16;
    clz += clz_tab[(t*clz_magic)>>>27]-1;
    mantissa <<= clz;
    remainder <<= clz;
    exponent -= clz;

    // Final division, correctly rounded
    remainder = (remainder+denom/2)/denom;
    mantissa += remainder;

    if (exponent < 0) // Underflow
      makeZero(sign);
  }

  public static long ldiv(long a, long b) {
    // Calculate (a<<63)/b, where a<2**64, b<2**63, b<=a and a<2*b
    // The result will always be 63 bits, leading to a 3-stage radix-2**21
    // (very high radix) algorithm, as described here:
    // S.F. Oberman and M.J. Flynn, "Division Algorithms and Implementations,"
    // IEEE Trans. Computers, vol. 46, no. 8, pp. 833-854, Aug 1997
    // Section 4: "Very High Radix Algorithms"

    int bInv24; // Approximate 1/b, never more than 24 bits
    int aHi24; // High 24 bits of a (sometimes 25 bits)
    int next21; // The next 21 bits of result, possibly 1 less
    long q; // Resulting quotient: round((a<<63)/b)
    // Preparations
    bInv24 = (int)(0x400000000000L/((b>>>40)+1));
    aHi24 = (int)(a>>>40);
    a <<= 20; // aHi24 and a overlap by 4 bits
    // Now perform the division
    next21 = (int)(((long)aHi24*(long)bInv24)>>>26);
    a -= next21*b; // Bits above 2**64 will always be cancelled
    // No need to remove remainder since this will be cared for in next block
    q = next21;
    aHi24 = (int)(a>>>39);
    a <<= 21;
    // Two more almost identical blocks...
    next21 = (int)(((long)aHi24*(long)bInv24)>>>26);
    a -= next21*b;
    q = (q<<21)+next21;
    aHi24 = (int)(a>>>39);
    a <<= 21;
    next21 = (int)(((long)aHi24*(long)bInv24)>>>26);
    a -= next21*b;
    q = (q<<21)+next21;
    // Remove final remainder
    if (a<0 || a>=b) { q++; a -= b; }
    a <<= 1;
    // Round correctly
    if (a<0 || a>=b) q++;
    return q;
  }

  public void div(final Real a) {
    if ((this.exponent < 0 && this.mantissa != 0) || (a.exponent < 0 && a.mantissa != 0)) {
      makeNan();
      return;
    }
    sign ^= a.sign;
    if ((this.exponent < 0 && this.mantissa == 0)) {
      if ((a.exponent < 0 && a.mantissa == 0))
        makeNan();
      return;
    }
    if ((a.exponent < 0 && a.mantissa == 0)) {
      makeZero(sign);
      return;
    }
    if ((this.exponent == 0 && this.mantissa == 0)) {
      if ((a.exponent == 0 && a.mantissa == 0))
        makeNan();
      return;
    }
    if ((a.exponent == 0 && a.mantissa == 0)) {
      makeInfinity(sign);
      return;
    }
    exponent += 0x40000000-a.exponent;
    if (mantissa < a.mantissa) {
      mantissa <<= 1;
      exponent--;
    }
    if (exponent < 0) {
      if (a.exponent >= 0x40000000)
        makeZero(sign); // Underflow
      else
        makeInfinity(sign); // Overflow
      return;
    }

    if (a.mantissa == 0x4000000000000000L)
      return;
    mantissa = ldiv(mantissa,a.mantissa);
  }

/*  public void rdiv(final Real a) {
    { recipTmp.mantissa = a.mantissa; recipTmp.exponent = a.exponent; recipTmp.sign = a.sign; };
    recipTmp.div(this);
    { this.mantissa = recipTmp.mantissa; this.exponent = recipTmp.exponent; this.sign = recipTmp.sign; };
  }
*/
  public void recip() {
    if ((this.exponent < 0 && this.mantissa != 0))
      return;
    if ((this.exponent < 0 && this.mantissa == 0)) {
      makeZero(sign);
      return;
    }
    if ((this.exponent == 0 && this.mantissa == 0)) {
      makeInfinity(sign);
      return;
    }
    exponent = 0x80000000-exponent;
    if (mantissa == 0x4000000000000000L) {
      if (exponent < 0)
        makeInfinity(sign); // Overflow
      return;
    }
    exponent--;
    mantissa = ldiv(0x8000000000000000L,mantissa);
  }

  public long recip128(long extra) {
    if ((this.exponent < 0 && this.mantissa != 0))
      return 0;
    if ((this.exponent < 0 && this.mantissa == 0)) {
      makeZero(sign);
      return 0;
    }
    if ((this.exponent == 0 && this.mantissa == 0)) {
      makeInfinity(sign);
      return 0;
    }

    byte s = sign;
    sign = 0;

    // Special case, simple power of 2
    if (mantissa == 0x4000000000000000L && extra == 0) {
      exponent = 0x80000000-exponent;
      if (exponent<0) // Overflow
        makeInfinity(s);
      return 0;
    }

    // Normalize exponent
    int exp = 0x40000000-exponent;
    exponent = 0x40000000;

    // Save -A
    { recipTmp.mantissa = this.mantissa; recipTmp.exponent = this.exponent; recipTmp.sign = this.sign; };
    long recipTmpExtra = extra;
    recipTmp.neg();

    // First establish approximate result (actually 63 bit accurate)
    recip();

    // Perform one Newton-Raphson iteration
    // Xn+1 = Xn + Xn*(1-A*Xn)
    { recipTmp2.mantissa = this.mantissa; recipTmp2.exponent = this.exponent; recipTmp2.sign = this.sign; };
    extra = mul128(0,recipTmp,recipTmpExtra);
    extra = add128(extra,ONE,0);
    extra = mul128(extra,recipTmp2,0);
    extra = add128(extra,recipTmp2,0);

    // Fix exponent
    scalbn(exp);
    // Fix sign
    if (!isNan())
      sign = s;

    return extra;
  }

 /* public void divf(final Real a) {
    if ((this.exponent < 0 && this.mantissa != 0) || (a.exponent < 0 && a.mantissa != 0)) {
      makeNan();
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0)) {
      if ((a.exponent < 0 && a.mantissa == 0))
        makeNan();
      return;
    }
    if ((a.exponent < 0 && a.mantissa == 0)) {
      makeZero(sign);
      return;
    }
    if ((this.exponent == 0 && this.mantissa == 0)) {
      if ((a.exponent == 0 && a.mantissa == 0))
        makeNan();
      return;
    }
    if ((a.exponent == 0 && a.mantissa == 0)) {
      makeInfinity(sign);
      return;
    }
    { tmp0.mantissa = a.mantissa; tmp0.exponent = a.exponent; tmp0.sign = a.sign; }; // tmp0 should be free
    // Perform same division as with mod, and don't round up
    long extra = tmp0.recip128(0);
    extra = mul128(0,tmp0,extra);
    if (((tmp0.sign!=0) && (extra < 0 || extra > 0x1f)) ||
        (!(tmp0.sign!=0) && extra < 0 && extra > 0xffffffe0))
    {
      // For accurate floor()
      mantissa++;
      normalize();
    }
    floor();
  }*/

  private void modInternal(/*long thisExtra,*/ final Real a, long aExtra) {
    { tmp0.mantissa = a.mantissa; tmp0.exponent = a.exponent; tmp0.sign = a.sign; }; // tmp0 should be free
    long extra = tmp0.recip128(aExtra);
    extra = tmp0.mul128(extra,this,0/*thisExtra*/); // tmp0 == this/a
    if (tmp0.exponent > 0x4000003e) {
      // floor() will be inaccurate
      makeZero(a.sign); // What else can be done? makeNan?
      return;
    }
    if (((tmp0.sign!=0) && (extra < 0 || extra > 0x1f)) ||
        (!(tmp0.sign!=0) && extra < 0 && extra > 0xffffffe0))
    {
      // For accurate floor() with a bit of "magical rounding"
      tmp0.mantissa++;
      tmp0.normalize();
    }
    tmp0.floor();
    tmp0.neg(); // tmp0 == -floor(this/a)
    extra = tmp0.mul128(0,a,aExtra);
    extra = add128(0/*thisExtra*/,tmp0,extra);
    roundFrom128(extra);
  }

  /*public void mod(final Real a) {
    if ((this.exponent < 0 && this.mantissa != 0) || (a.exponent < 0 && a.mantissa != 0)) {
      makeNan();
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0)) {
      makeNan();
      return;
    }
    if ((this.exponent == 0 && this.mantissa == 0)) {
      if ((a.exponent == 0 && a.mantissa == 0))
        makeNan();
      else
        sign = a.sign;
      return;
    }
    if ((a.exponent < 0 && a.mantissa == 0)) {
      if (sign != a.sign)
        makeInfinity(a.sign);
      return;
    }
    if ((a.exponent == 0 && a.mantissa == 0)) {
      makeZero(a.sign);
      return;
    }
    modInternal(a,0);
  }*/

  /*public void and(final Real a) {
    if ((this.exponent < 0 && this.mantissa != 0) || (a.exponent < 0 && a.mantissa != 0)) {
      makeNan();
      return;
    }
    if ((this.exponent == 0 && this.mantissa == 0) || (a.exponent == 0 && a.mantissa == 0)) {
      makeZero();
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0) || (a.exponent < 0 && a.mantissa == 0)) {
      if (!(this.exponent < 0 && this.mantissa == 0) && (this.sign!=0)) {
        { this.mantissa = a.mantissa; this.exponent = a.exponent; this.sign = a.sign; };
      } else if (!(a.exponent < 0 && a.mantissa == 0) && (a.sign!=0))
        ; // ASSIGN(this,this)
      else if ((this.exponent < 0 && this.mantissa == 0) && (a.exponent < 0 && a.mantissa == 0) &&
               (this.sign!=0) && (a.sign!=0))
        ; // makeInfinity(1)
      else
        makeZero();
      return;
    }

    byte s;
    int e;
    long m;
    if (exponent >= a.exponent) {
      s = a.sign;
      e = a.exponent;
      m = a.mantissa;
    } else {
      s = sign;
      e = exponent;
      m = mantissa;
      sign = a.sign;
      exponent = a.exponent;
      mantissa = a.mantissa;
    }
    int shift = exponent-e;
    if (shift>=64) {
      if (s == 0)
        makeZero(sign);
      return;
    }
    if (s != 0)
      m = -m;
    if ((this.sign!=0))
      mantissa = -mantissa;

    mantissa &= m>>shift;
    sign = 0;
    if (mantissa < 0) {
      mantissa = -mantissa;
      sign = 1;
    }
    normalize();
  }

  public void or(final Real a) {
    if ((this.exponent < 0 && this.mantissa != 0) || (a.exponent < 0 && a.mantissa != 0)) {
      makeNan();
      return;
    }
    if ((this.exponent == 0 && this.mantissa == 0) || (a.exponent == 0 && a.mantissa == 0)) {
      if ((this.exponent == 0 && this.mantissa == 0))
        { this.mantissa = a.mantissa; this.exponent = a.exponent; this.sign = a.sign; };
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0) || (a.exponent < 0 && a.mantissa == 0)) {
      if (!(this.exponent < 0 && this.mantissa == 0) && (this.sign!=0))
        ; // ASSIGN(this,this);
      else if (!(a.exponent < 0 && a.mantissa == 0) && (a.sign!=0)) {
        { this.mantissa = a.mantissa; this.exponent = a.exponent; this.sign = a.sign; };
      } else
        makeInfinity(sign | a.sign);
      return;
    }

    byte s;
    int e;
    long m;
    if (((this.sign!=0) && exponent <= a.exponent) ||
        ((a.sign==0) && exponent >= a.exponent))
    {
      s = a.sign;
      e = a.exponent;
      m = a.mantissa;
    } else {
      s = sign;
      e = exponent;
      m = mantissa;
      sign = a.sign;
      exponent = a.exponent;
      mantissa = a.mantissa;
    }
    int shift = exponent-e;
    if (shift>=64 || shift<=-64)
      return;

    if (s != 0)
      m = -m;
    if ((this.sign!=0))
      mantissa = -mantissa;

    if (shift>=0)
      mantissa |= m>>shift;
    else
      mantissa |= m<<(-shift);

    sign = 0;
    if (mantissa < 0) {
      mantissa = -mantissa;
      sign = 1;
    }
    normalize();
  }

  public void xor(final Real a) {
    if ((this.exponent < 0 && this.mantissa != 0) || (a.exponent < 0 && a.mantissa != 0)) {
      makeNan();
      return;
    }
    if ((this.exponent == 0 && this.mantissa == 0) || (a.exponent == 0 && a.mantissa == 0)) {
      if ((this.exponent == 0 && this.mantissa == 0))
        { this.mantissa = a.mantissa; this.exponent = a.exponent; this.sign = a.sign; };
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0) || (a.exponent < 0 && a.mantissa == 0)) {
      makeInfinity(sign ^ a.sign);
      return;
    }

    byte s;
    int e;
    long m;
    if (exponent >= a.exponent) {
      s = a.sign;
      e = a.exponent;
      m = a.mantissa;
    } else {
      s = sign;
      e = exponent;
      m = mantissa;
      sign = a.sign;
      exponent = a.exponent;
      mantissa = a.mantissa;
    }
    int shift = exponent-e;
    if (shift>=64)
      return;

    if (s != 0)
      m = -m;
    if ((this.sign!=0))
      mantissa = -mantissa;

    mantissa ^= m>>shift;

    sign = 0;
    if (mantissa < 0) {
      mantissa = -mantissa;
      sign = 1;
    }
    normalize();
  }*/

 /* public void bic(final Real a) {
    if ((this.exponent < 0 && this.mantissa != 0) || (a.exponent < 0 && a.mantissa != 0)) {
      makeNan();
      return;
    }
    if ((this.exponent == 0 && this.mantissa == 0) || (a.exponent == 0 && a.mantissa == 0))
      return;
    if ((this.exponent < 0 && this.mantissa == 0) || (a.exponent < 0 && a.mantissa == 0)) {
      if (!(this.exponent < 0 && this.mantissa == 0)) {
        if ((this.sign!=0))
          if ((a.sign!=0))
            makeInfinity(0);
          else
            makeInfinity(1);
      } else if ((a.sign!=0)) {
        if ((a.exponent < 0 && a.mantissa == 0))
          makeInfinity(0);
        else
          makeZero();
      }
      return;
    }

    int shift = exponent-a.exponent;
    if (shift>=64 || (shift<=-64 && (this.sign==0)))
      return;

    long m = a.mantissa;
    if ((a.sign!=0))
      m = -m;
    if ((this.sign!=0))
      mantissa = -mantissa;

    if (shift<0) {
      if ((this.sign!=0)) {
        if (shift<=-64)
          mantissa = ~m;
        else
          mantissa = (mantissa>>(-shift)) & ~m;
        exponent = a.exponent;
      } else
        mantissa &= ~(m<<(-shift));
    } else
      mantissa &= ~(m>>shift);

    sign = 0;
    if (mantissa < 0) {
      mantissa = -mantissa;
      sign = 1;
    }
    normalize();
  }*/

  // Overloaded metohds, int arguments
  public void add(int a) {tmp0.assign(a);add(tmp0); }
//  public void sub(int a) {tmp0.assign(a);sub(tmp0); }
//  public void rdiv(int a) {tmp0.assign(a);rdiv(tmp0); }
//  public boolean equalTo(int a) {tmp0.assign(a);return equalTo(tmp0); }
//  public boolean notEqualTo(int a) {tmp0.assign(a);return notEqualTo(tmp0); }
//  public boolean lessThan(int a) {tmp0.assign(a);return lessThan(tmp0); }
//  public boolean lessEqual(int a) {tmp0.assign(a);return lessEqual(tmp0); }
//  public boolean greaterThan(int a) {tmp0.assign(a);return greaterThan(tmp0); }
//  public boolean greaterEqual(int a){tmp0.assign(a);return greaterEqual(tmp0);}
//  private int compare(int a) {tmp0.assign(a);return compare(tmp0); }

  private void rsqrtInternal() {
    // Calculates recipocal square root of normalized Real,
    // not zero, nan or infinity
    final long start = 0x4e60000000000000L;

    // Save -A
    { recipTmp.mantissa = this.mantissa; recipTmp.exponent = this.exponent; recipTmp.sign = this.sign; };
    recipTmp.neg();

    // First establish approximate result

    mantissa = start-(mantissa>>>2);
    boolean oddexp = ((exponent&1) != 0);
    exponent = 0x60000000-(exponent>>1);
    normalize();
    if (oddexp)
      mul(SQRT1_2);

    // Now perform Newton-Raphson iteration
    // Xn+1 = Xn + Xn*(1-A*Xn*Xn)/2

    for (int i=0; i<4; i++) {
      { recipTmp2.mantissa = this.mantissa; recipTmp2.exponent = this.exponent; recipTmp2.sign = this.sign; };
      sqr();
      mul(recipTmp);
      add(ONE);
      scalbn(-1);
      mul(recipTmp2);
      add(recipTmp2);
    }
  }

  public void rsqrt() {
    if ((this.exponent < 0 && this.mantissa != 0))
      return;
    if ((this.exponent == 0 && this.mantissa == 0)) {
      makeInfinity(0);
      return;
    }
    if ((this.sign!=0)) {
      makeNan();
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0)) {
      makeZero(sign);
      return;
    }

    rsqrtInternal();
  }

  public void sqrt() {
    if ((this.exponent < 0 && this.mantissa != 0))
      return;
    if ((this.exponent == 0 && this.mantissa == 0)) {
      sign=0;
      return;
    }
    if ((this.sign!=0)) {
      makeNan();
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0))
      return;

    { sqrtTmp.mantissa = this.mantissa; sqrtTmp.exponent = this.exponent; sqrtTmp.sign = this.sign; };
    sqrtTmp.rsqrtInternal();
    mul(sqrtTmp);
  }

 /* private void rcbrtInternal() {
    // Calculates recipocal cube root of normalized Real,
    // not zero, nan or infinity
    final long start = 0x5120000000000000L;

    // Save -A
    { recipTmp.mantissa = this.mantissa; recipTmp.exponent = this.exponent; recipTmp.sign = this.sign; };
    recipTmp.neg();

    // First establish approximate result

    mantissa = start-(mantissa>>>2);
    int expRmd = exponent==0 ? 2 : (exponent-1)%3;
    exponent = 0x40000000-(exponent-0x40000000-expRmd)/3;
    normalize();
    if (expRmd>0) {
      mul(CBRT1_2);
      if (expRmd>1)
        mul(CBRT1_2);
    }

    // Now perform Newton-Raphson iteration
    // Xn+1 = (4*Xn - A*Xn**4)/3

    for (int i=0; i<4; i++) {
      { recipTmp2.mantissa = this.mantissa; recipTmp2.exponent = this.exponent; recipTmp2.sign = this.sign; };
      sqr();
      sqr();
      mul(recipTmp);
      recipTmp2.scalbn(2);
      add(recipTmp2);
      mul(THIRD);
    }
  }*/

 /* public void cbrt() {
    if (!(this.exponent >= 0 && this.mantissa != 0))
      return;
    byte s = sign;
    sign = 0;
    rcbrtInternal();
    recip();
    if (!(this.exponent < 0 && this.mantissa != 0))
      sign = s;
  }*/

 /* public void nroot(Real root) {
    if ((root.exponent < 0 && root.mantissa != 0)) {
      makeNan();
      return;
    }
    if (root.compare(THREE)==0) {
      cbrt(); // Most probable application of nroot...
      return;
    } else if (root.compare(TWO)==0) {
      sqrt(); // Also possible, should be optimized like this
      return;
    }
    boolean negative = false;
    if ((this.sign!=0) && root.isIntegral() && root.isOdd()) {
      negative = true;
      abs();
    }
    { tmp2.mantissa = root.mantissa; tmp2.exponent = root.exponent; tmp2.sign = root.sign; }; // Copy to temporary location in case of x.nroot(x)
    tmp2.recip();
    pow(tmp2);
    if (negative)
      neg();
  }*/

/*  public void hypot(final Real a) {
    { tmp1.mantissa = a.mantissa; tmp1.exponent = a.exponent; tmp1.sign = a.sign; }; // Copy to temporary location in case of x.hypot(x)
    tmp1.sqr();
    sqr();
    add(tmp1);
    sqrt();
  }*/

  private void exp2Internal(long extra) {
    if ((this.exponent < 0 && this.mantissa != 0))
      return;
    if ((this.exponent < 0 && this.mantissa == 0)) {
      if ((this.sign!=0))
        makeZero(0);
      return;
    }
    if ((this.exponent == 0 && this.mantissa == 0)) {
      { this.mantissa = ONE.mantissa; this.exponent = ONE.exponent; this.sign = ONE.sign; };
      return;
    }

    // Extract integer part
    { expTmp.mantissa = this.mantissa; expTmp.exponent = this.exponent; expTmp.sign = this.sign; };
    if ((expTmp.sign!=0) && extra != 0) {
      // For accurate floor()
      expTmp.mantissa++;
      expTmp.normalize();
    }
    expTmp.floor();
    int exp = expTmp.toInteger();
    if (exp >= 0x40000000) {
      makeInfinity(sign);
      return;
    }
    if (exp < -0x40000000) {
      makeZero(sign);
      return;
    }

    // Subtract integer part (this is where we most need the extra accuracy)
    expTmp.neg();
    extra = add128(extra,expTmp,0);

    // Now 0<X<1. To calculate exp2(X), first multiply by ln(2)
    expTmp.assign(0, 0x3fffffff, 0x58b90bfbe8e7bcd5L); // ln(2)
    extra = mul128(extra,expTmp,0xe4f1d9cc01f97b58L);
    roundFrom128(extra);

    // Calculate e**x for 0 < x < ln 2
    // Using the classic Taylor series
    //                       2        3        4        5
    //   x                  x        x        x        x
    //  e  =   1  +  x  +  ----  +  ----  +  ----  +  ----  +  ...
    //                      2!       3!       4!       5!
    //
    // , factorized as follows:
    //
    //  x   (((((...)*x + 5...)*x + 4*5...)*x + 3*4*5...)*x + 2*3*4*5...)*x
    // e  = --------------------------------------------------------------- + 1
    //                              2*3*4*5...

    { expTmp.mantissa = this.mantissa; expTmp.exponent = this.exponent; expTmp.sign = this.sign; };
    { expTmp2.mantissa = ONE.mantissa; expTmp2.exponent = ONE.exponent; expTmp2.sign = ONE.sign; };
    for (int i=18; i>=2; i--) {
      expTmp2.mul(i);
      add(expTmp2);
      mul(expTmp);
    }
    div(expTmp2);
    add(ONE);

    exponent += exp;
  }

  public void exp() {
    expTmp.assign(0, 0x40000000, 0x5c551d94ae0bf85dL); // log2(e)
    long extra = mul128(0,expTmp,0xdf43ff68348e9f44L);
    exp2Internal(extra);
  }

  /*public void exp2() {
    exp2Internal(0);
  }*/

/* public void exp10() {
    expTmp.assign(0, 0x40000001, 0x6a4d3c25e68dc57fL); // log2(10)
    long extra = mul128(0,expTmp,0x2495fb7fa6d7eda6L);
    exp2Internal(extra);
  }*/

  private void lnInternal() {
    // Calculates natural logarithm ln(x) for x between 1 and 2, using
    // the series
    //                          3        5        7
    //  1      1+a             a        a        a
    // --- ln ----- =  a  +   ----  +  ----  +  ---- + ...
    //  2      1-a             3        5        7
    //
    // ,where a = (x-1)/(x+1)
    // The expression is factorized as follows:
    //
    // (((((...)*7a² + 9...)*5a² + 7*9...)*3a² + 5*7*9...)*a² + 3*5*7*9...)*a
    // ----------------------------------------------------------------------
    //                              3*5*7*9...

    { expTmp.mantissa = this.mantissa; expTmp.exponent = this.exponent; expTmp.sign = this.sign; };
    expTmp.add(ONE);
    sub(ONE);
    div(expTmp);

    { expTmp.mantissa = this.mantissa; expTmp.exponent = this.exponent; expTmp.sign = this.sign; };
    { expTmp2.mantissa = this.mantissa; expTmp2.exponent = this.exponent; expTmp2.sign = this.sign; };
    expTmp2.sqr();
    { this.mantissa = ONE.mantissa; this.exponent = ONE.exponent; this.sign = ONE.sign; };
    { expTmp3.mantissa = ONE.mantissa; expTmp3.exponent = ONE.exponent; expTmp3.sign = ONE.sign; };
    for (int i=37; i>=3; i-=2) {
      mul(expTmp2);
      mul(i-2);
      expTmp3.mul(i);
      add(expTmp3);
    }
    mul(expTmp);
    div(expTmp3);
    scalbn(1);
  }

  public void ln() {
    if ((this.exponent < 0 && this.mantissa != 0))
      return;
    if ((this.sign!=0)) {
      makeNan();
      return;
    }
    if ((this.exponent == 0 && this.mantissa == 0)) {
      makeInfinity(1);
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0))
      return;

    int exp = exponent-0x40000000;
    exponent = 0x40000000;

    //mul(DIV2_3);  // Comment in for quicker convergence, less precision
    lnInternal();
    //sub(LN2_3);

    expTmp.assign(exp);
    expTmp.mul(LN2);
    add(expTmp);
  }

  /*public void log2() {
    if ((this.exponent < 0 && this.mantissa != 0))
      return;
    if ((this.sign!=0)) {
      makeNan();
      return;
    }
    if ((this.exponent == 0 && this.mantissa == 0)) {
      makeInfinity(1);
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0))
      return;

    int exp = exponent-0x40000000;
    exponent = 0x40000000;

    lnInternal();
    mul(LOG2E);

    add(exp);
  }*/

  public void log10() {
    if ((this.exponent < 0 && this.mantissa != 0))
      return;
    if ((this.sign!=0)) {
      makeNan();
      return;
    }
    if ((this.exponent == 0 && this.mantissa == 0)) {
      makeInfinity(1);
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0))
      return;

    int exp = exponent-0x40000000;
    exponent = 0x40000000;

    lnInternal();

    expTmp.assign(exp);
    expTmp.mul(LN2);
    add(expTmp);
    mul(LOG10E);
  }

  // Convert to power of 10 <= x, return exponent
  public int lowPow10() {
    if (!(this.exponent >= 0 && this.mantissa != 0))
      return 0;
    { tmp2.mantissa = this.mantissa; tmp2.exponent = this.exponent; tmp2.sign = this.sign; };
    // Approximate log10 using exponent only
    int e = exponent - 0x40000000;
    if (e<0) // it's important to achieve floor(exponent*ln2/ln10)
      e = -(int)(((-e)*0x4d104d43L+((1L<<32)-1)) >> 32);
    else
      e = (int)(e*0x4d104d43L >> 32);
    // Now, e < log10(this) < e+1
    { this.mantissa = TEN.mantissa; this.exponent = TEN.exponent; this.sign = TEN.sign; };
    pow(e);
    { tmp3.mantissa = this.mantissa; tmp3.exponent = this.exponent; tmp3.sign = this.sign; };
    tmp3.mul10();
    if (tmp3.compare(tmp2) <= 0) {
      // First estimate of log10 was too low
      e++;
      { this.mantissa = tmp3.mantissa; this.exponent = tmp3.exponent; this.sign = tmp3.sign; };
    }
    return e;
  }

  public void pow(int power) {
    // Calculate power of integer by successive squaring
    boolean recp=false;
    if (power < 0) {
      power = -power; // Also works for 0x80000000
      recp = true;
    }
    long extra = 0, expTmpExtra = 0;
    { expTmp.mantissa = this.mantissa; expTmp.exponent = this.exponent; expTmp.sign = this.sign; };
    { this.mantissa = ONE.mantissa; this.exponent = ONE.exponent; this.sign = ONE.sign; };
    for (; power!=0; power>>>=1) {
      if ((power & 1) != 0)
        extra = mul128(extra,expTmp,expTmpExtra);
      expTmpExtra = expTmp.mul128(expTmpExtra,expTmp,expTmpExtra);
    }
    if (recp)
      extra = recip128(extra);
    roundFrom128(extra);
  }

  public void pow(final Real exp) {
    // Special cases:
    // if y is 0.0 or -0.0 then result is 1.0
    // if y is NaN then result is NaN
    // if x is NaN and y is not zero then result is NaN
    // if y is 1.0 then result is x
    // if |x| > 1.0 and y is +Infinity then result is +Infinity
    // if |x| < 1.0 and y is -Infinity then result is +Infinity
    // if |x| > 1.0 and y is -Infinity then result is +0
    // if |x| < 1.0 and y is +Infinity then result is +0
    // if |x| = 1.0 and y is +/-Infinity then result is NaN
    // if x = +0 and y > 0 then result is +0
    // if x = +0 and y < 0 then result is +Inf
    // if x = -0 and y > 0, and odd integer then result is -0
    // if x = -0 and y < 0, and odd integer then result is -Inf
    // if x = -0 and y > 0, not odd integer then result is +0
    // if x = -0 and y < 0, not odd integer then result is +Inf
    // if x = +Inf and y > 0 then result is +Inf
    // if x = +Inf and y < 0 then result is +0
    // if x = -Inf and y not integer then result is NaN
    // if x = -Inf and y > 0, and odd integer then result is -Inf
    // if x = -Inf and y > 0, not odd integer then result is +Inf
    // if x = -Inf and y < 0, and odd integer then result is -0
    // if x = -Inf and y < 0, not odd integer then result is +0
    // if x < 0 and y not integer then result is NaN
    // if x < 0 and y odd integer then result is -(|x|**y)
    // if x < 0 and y not odd integer then result is |x|**y
    // else result is pow2(log2(x)*y)

    if ((exp.exponent == 0 && exp.mantissa == 0)) {
      { this.mantissa = ONE.mantissa; this.exponent = ONE.exponent; this.sign = ONE.sign; };
      return;
    }
    if ((this.exponent < 0 && this.mantissa != 0) || (exp.exponent < 0 && exp.mantissa != 0)) {
      makeNan();
      return;
    }
    if (exp.compare(ONE)==0)
      return;
    if ((exp.exponent < 0 && exp.mantissa == 0)) {
      { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };
      tmp1.abs();
      int test = tmp1.compare(ONE);
      if (test>0) {
        if ((exp.sign==0))
          makeInfinity(0);
        else
          makeZero();
      } else if (test<0) {
        if ((exp.sign!=0))
          makeInfinity(0);
        else
          makeZero();
      } else {
        makeNan();
      }
      return;
    }
    if ((this.exponent == 0 && this.mantissa == 0)) {
      if ((this.sign==0)) {
        if ((exp.sign==0))
          makeZero();
        else
          makeInfinity(0);
      } else {
        if (exp.isIntegral() && exp.isOdd()) {
          if ((exp.sign==0))
            makeZero(1);
          else
            makeInfinity(1);
        } else {
          if ((exp.sign==0))
            makeZero();
          else
            makeInfinity(0);
        }
      }
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0)) {
      if ((this.sign==0)) {
        if ((exp.sign==0))
          makeInfinity(0);
        else
          makeZero();
      } else {
        if (exp.isIntegral()) {
          if (exp.isOdd()) {
            if ((exp.sign==0))
              makeInfinity(1);
            else
              makeZero(1);
          } else {
            if ((exp.sign==0))
              makeInfinity(0);
            else
              makeZero();
          }
        } else {
          makeNan();
        }
      }
      return;
    }

    if (exp.isIntegral() && exp.exponent <= 0x4000001e) {
      pow(exp.toInteger());
      return;
    }

    byte s=0;
    if ((this.sign!=0)) {
      if (exp.isIntegral()) {
        if (exp.isOdd())
          s = 1;
      } else {
        makeNan();
        return;
      }
      sign = 0;
    }

    { tmp1.mantissa = exp.mantissa; tmp1.exponent = exp.exponent; tmp1.sign = exp.sign; };
    if (tmp1.exponent <= 0x4000001e) {
      // For increased accuracy, exponentiate with integer part of
      // exponent by successive squaring
      // (I really don't know why this works)
      { tmp2.mantissa = tmp1.mantissa; tmp2.exponent = tmp1.exponent; tmp2.sign = tmp1.sign; };
      tmp2.floor();
      { tmp3.mantissa = this.mantissa; tmp3.exponent = this.exponent; tmp3.sign = this.sign; };
      tmp3.pow(tmp2.toInteger());
      tmp1.sub(tmp2);
    } else {
      { tmp3.mantissa = ONE.mantissa; tmp3.exponent = ONE.exponent; tmp3.sign = ONE.sign; };
    }

    // Do log2 and maintain accuracy
    int e = exponent-0x40000000;
    exponent = 0x40000000;
    lnInternal();
    tmp2.assign(0, 0x40000000, 0x5c551d94ae0bf85dL); // log2(e)
    long extra = mul128(0,tmp2,0xdf43ff68348e9f44L);
    tmp2.assign(e);
    extra = add128(extra,tmp2,0);

    // Do exp2 of this multiplied by (fractional part of) exponent
    extra = tmp1.mul128(0,this,extra);
    tmp1.exp2Internal(extra);
    { this.mantissa = tmp1.mantissa; this.exponent = tmp1.exponent; this.sign = tmp1.sign; };
    mul(tmp3);

    if (!(this.exponent < 0 && this.mantissa != 0))
      sign = s;
  }

  private void sinInternal() {
    // Calculate sine for 0 < x < pi/4
    // Using the classic Taylor series
    //                     3        5        7
    //                    x        x        x
    // sin(x)  =   x  -  ----  +  ----  -  ----  +  ...
    //                    3!       5!       7!
    //
    // , factorized as follows:
    //
    //         ((((...)*-x² + 6*7...)*-x² + 4*5*6*7...)*-x² + 2*3*4*5*6*7...)*x
    // sin(x) = ---------------------------------------------------------------
    //                                  2*3*4*5*6*7...

    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };
    { tmp2.mantissa = this.mantissa; tmp2.exponent = this.exponent; tmp2.sign = this.sign; };
    tmp2.sqr();
    tmp2.neg();
    { this.mantissa = ONE.mantissa; this.exponent = ONE.exponent; this.sign = ONE.sign; };
    { tmp3.mantissa = ONE.mantissa; tmp3.exponent = ONE.exponent; tmp3.sign = ONE.sign; };
    for (int i=19; i>=3; i-=2) {
      mul(tmp2);
      tmp3.mul(i*(i-1));
      add(tmp3);
    }
    mul(tmp1);
    div(tmp3);
  }

  private void cosInternal() {
    // Calculate cosine for 0 < x < pi/4
    // Using the classic Taylor series
    //                     2        4        6
    //                    x        x        x
    // cos(x)  =   1  -  ----  +  ----  -  ----  +  ...
    //                    2!       4!       6!
    //
    // , factorized as follows:
    //
    //          (((...)*-x² + 5*6...)*-x² + 3*4*5*6...)*-x²
    // cos(x) = ------------------------------------------- + 1
    //                          1*2*3*4*5*6...

    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };
    { tmp2.mantissa = this.mantissa; tmp2.exponent = this.exponent; tmp2.sign = this.sign; };
    tmp2.sqr();
    tmp2.neg();
    { this.mantissa = tmp2.mantissa; this.exponent = tmp2.exponent; this.sign = tmp2.sign; };
    { tmp3.mantissa = ONE.mantissa; tmp3.exponent = ONE.exponent; tmp3.sign = ONE.sign; };
    for (int i=18; i>=4; i-=2) {
      tmp3.mul(i*(i-1));
      add(tmp3);
      mul(tmp2);
    }
    tmp3.scalbn(1);
    div(tmp3);
    add(ONE);
  }

  public void sin() {
    if (!(this.exponent >= 0 && this.mantissa != 0)) {
      if (!(this.exponent == 0 && this.mantissa == 0))
        makeNan();
      return;
    }

    // Since sin(-x) = -sin(x) we can make sure that x > 0
    boolean negative = false;
    if ((this.sign!=0)) {
      abs();
      negative = true;
    }

    // Then reduce the argument to the range of 0 < x < pi*2
    if (this.compare(PI2) > 0)
      modInternal(PI2,0x62633145c06e0e69L);

    // Since sin(pi*2 - x) = -sin(x) we can reduce the range 0 < x < pi
    if (this.compare(PI) > 0) {
      sub(PI2);
      neg();
      negative = !negative;
    }

    // Since sin(x) = sin(pi - x) we can reduce the range to 0 < x < pi/2
    if (this.compare(PI_2) > 0) {
      sub(PI);
      neg();
    }

    // Since sin(x) = cos(pi/2 - x) we can reduce the range to 0 < x < pi/4
    if (this.compare(PI_4) > 0) {
      sub(PI_2);
      neg();
      cosInternal();
    } else {
      sinInternal();
    }

    if (negative)
      neg();
    if ((this.exponent == 0 && this.mantissa == 0))
      abs(); // Remove confusing "-"
  }

  public void cos() {
    if ((this.exponent == 0 && this.mantissa == 0)) {
      { this.mantissa = ONE.mantissa; this.exponent = ONE.exponent; this.sign = ONE.sign; };
      return;
    }
    if ((this.sign!=0))
      abs();
    if (this.compare(PI_4) < 0) {
      cosInternal();
    } else {
      add(PI_2);
      sin();
    }
  }

  public void tan() {
    { tmp4.mantissa = this.mantissa; tmp4.exponent = this.exponent; tmp4.sign = this.sign; };
    tmp4.cos();
    sin();
    div(tmp4);
  }

  public void asin() {
    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };
    sqr();
    neg();
    add(ONE);
    rsqrt();
    mul(tmp1);
    atan();
  }

  public void acos() {
    boolean negative = (this.sign!=0);
    abs();

    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };
    sqr();
    neg();
    add(ONE);
    sqrt();
    div(tmp1);
    atan();

    if (negative) {
      neg();
      add(PI);
    }
  }

  private void atanInternal() {
    // Calculate atan for 0 < x < sqrt(2)-1
    // Using the classic Taylor series.
    //                      3        5        7
    //                     x        x        x
    // atan(x)  =   x  -  ----  +  ----  -  ----  +  ...
    //                     3        5        7
    //
    // , factorized as follows:
    //
    //           (((...)*-5x² + 7...)*-3x² + 5*7...)*-x² + 3*5*7...)*x
    // atan(x) = -----------------------------------------------------
    //                                 1*3*5*7...

    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };
    { tmp2.mantissa = this.mantissa; tmp2.exponent = this.exponent; tmp2.sign = this.sign; };
    tmp2.sqr();
    tmp2.neg();
    { this.mantissa = ONE.mantissa; this.exponent = ONE.exponent; this.sign = ONE.sign; };
    { tmp3.mantissa = ONE.mantissa; tmp3.exponent = ONE.exponent; tmp3.sign = ONE.sign; };
    for (int i=45; i>=3; i-=2) {
      mul(tmp2);
      mul(i-2);
      tmp3.mul(i);
      add(tmp3);
    }
    mul(tmp1);
    div(tmp3);
  }

  public void atan() {
    if ((this.exponent == 0 && this.mantissa == 0) || (this.exponent < 0 && this.mantissa != 0))
      return;
    if ((this.exponent < 0 && this.mantissa == 0)) {
      byte s = sign;
      { this.mantissa = PI_2.mantissa; this.exponent = PI_2.exponent; this.sign = PI_2.sign; };
      sign = s;
      return;
    }

    byte s = sign;
    sign = 0;

    boolean recp = false;
    if (this.compare(ONE) > 0) {
      recp = true;
      recip();
    }

    // atan(x) = atan[ (x - a) / (1 + x*a) ] + PI/8
    // ,where a = sqrt(2)-1
    { tmp1.mantissa = SQRT2.mantissa; tmp1.exponent = SQRT2.exponent; tmp1.sign = SQRT2.sign; };
    tmp1.sub(ONE);
    boolean sub = false;
    if (this.compare(tmp1) > 0)
    {
      sub = true;
      { tmp2.mantissa = this.mantissa; tmp2.exponent = this.exponent; tmp2.sign = this.sign; };
      sub(tmp1);
      tmp2.mul(tmp1);
      tmp2.add(ONE);
      div(tmp2);
    }

    atanInternal();

    if (sub)
      add(PI_8);

    if (recp) {
      neg();
      add(PI_2);
    }

    sign = s;
  }

  public void atan2(final Real x) {
    if ((this.exponent < 0 && this.mantissa != 0) || (x.exponent < 0 && x.mantissa != 0) || ((this.exponent < 0 && this.mantissa == 0) && (x.exponent < 0 && x.mantissa == 0))) {
      makeNan();
      return;
    }
    if ((this.exponent == 0 && this.mantissa == 0) && (x.exponent == 0 && x.mantissa == 0))
      return;

    byte s = sign;
    byte s2 = x.sign;
    sign = 0;
    x.sign = 0;

    div(x);
    atan();

    if (s2 != 0) {
      neg();
      add(PI);
    }

    sign = s;
  }

/*  public void sinh() {
    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };
    tmp1.neg();
    tmp1.exp();
    exp();
    sub(tmp1);
    scalbn(-1);
  }

  public void cosh() {
    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };
    tmp1.neg();
    tmp1.exp();
    exp();
    add(tmp1);
    scalbn(-1);
  }

  public void tanh() {
    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };
    tmp1.neg();
    tmp1.exp();
    exp();
    { tmp2.mantissa = this.mantissa; tmp2.exponent = this.exponent; tmp2.sign = this.sign; };
    tmp2.add(tmp1);
    sub(tmp1);
    div(tmp2);
  }

  public void asinh() {
    if (!(this.exponent >= 0 && this.mantissa != 0))
      return;

    // Use symmetry to prevent underflow error for very large negative values
    byte s = sign;
    sign = 0;

    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };
    tmp1.sqr();
    tmp1.add(ONE);
    tmp1.sqrt();
    add(tmp1);
    ln();

    if (!(this.exponent < 0 && this.mantissa != 0))
      sign = s;
  }

  public void acosh() {
    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };
    tmp1.sqr();
    tmp1.sub(ONE);
    tmp1.sqrt();
    add(tmp1);
    ln();
  }

  public void atanh() {
    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };
    tmp1.neg();
    tmp1.add(ONE);
    add(ONE);
    div(tmp1);
    ln();
    scalbn(-1);
  }*/

  /*public void fact() {
    if (!(this.exponent >= 0))
      return;

    if (!this.isIntegral() || this.compare(ZERO)<0 || this.compare(200)>0)
    {
      // x<0, x>200 or not integer: fact(x) = gamma(x+1)
      add(ONE);
      gamma();
      return;
    }
    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };
    { this.mantissa = ONE.mantissa; this.exponent = ONE.exponent; this.sign = ONE.sign; };
    while (tmp1.compare(ONE) > 0) {
      mul(tmp1);
      tmp1.sub(ONE);
    }
  }

  public void gamma() {
    if (!(this.exponent >= 0))
      return;

    // x<0: gamma(-x) = -pi/(x*gamma(x)*sin(pi*x))
    boolean negative = (this.sign!=0);
    abs();
    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };

    // x<n: gamma(x) = gamma(x+m)/x*(x+1)*(x+2)*...*(x+m-1)
    // n=20
    { tmp2.mantissa = ONE.mantissa; tmp2.exponent = ONE.exponent; tmp2.sign = ONE.sign; };
    boolean divide = false;
    while (this.compare(20) < 0) {
      divide = true;
      tmp2.mul(this);
      add(ONE);
    }

    // x>n: gamma(x) = exp((x-1/2)*ln(x)-x+ln(2*pi)/2+1/12x-1/360x³+1/1260x**5
    //                     -1/1680x**7+1/1188x**9)
    { tmp3.mantissa = this.mantissa; tmp3.exponent = this.exponent; tmp3.sign = this.sign; }; // x
    { tmp4.mantissa = this.mantissa; tmp4.exponent = this.exponent; tmp4.sign = this.sign; };
    tmp4.sqr(); // x²

    // (x-1/2)*ln(x)-x
    ln(); { tmp5.mantissa = tmp3.mantissa; tmp5.exponent = tmp3.exponent; tmp5.sign = tmp3.sign; }; tmp5.sub(HALF); mul(tmp5); sub(tmp3);
    // + ln(2*pi)/2
    tmp5.assign(0, 0x3fffffff, 0x759fc72192fad29aL); add(tmp5);
    // + 1/12x
    tmp5.assign( 12); tmp5.mul(tmp3); tmp5.recip(); add(tmp5); tmp3.mul(tmp4);
    // - 1/360x³
    tmp5.assign( 360); tmp5.mul(tmp3); tmp5.recip(); sub(tmp5); tmp3.mul(tmp4);
    // + 1/1260x**5
    tmp5.assign(1260); tmp5.mul(tmp3); tmp5.recip(); add(tmp5); tmp3.mul(tmp4);
    // - 1/1680x**7
    tmp5.assign(1680); tmp5.mul(tmp3); tmp5.recip(); sub(tmp5); tmp3.mul(tmp4);
    // + 1/1188x**9
    tmp5.assign(1188); tmp5.mul(tmp3); tmp5.recip(); add(tmp5);

    exp();

    if (divide)
      div(tmp2);

    if (negative) {
      { tmp5.mantissa = tmp1.mantissa; tmp5.exponent = tmp1.exponent; tmp5.sign = tmp1.sign; }; // sin() uses tmp1
      // -pi/(x*gamma(x)*sin(pi*x))
      mul(tmp5);
      tmp5.scalbn(-1); tmp5.frac(); tmp5.mul(PI2); // Fixes integer inaccuracy
      tmp5.sin(); mul(tmp5); recip(); mul(PI); neg();
    }
  }*/

 /* private void erfc1Internal() {
    //                                  3       5        7        9
    //                  2     /        x       x        x        x               // erfc(x) = 1 - -------- | x  -  ---  +  ----  -  ----  +  ----  -  ... |

    //               sqrt(pi) \        3      2!*5     3!*7     4!*9         /
    //
    long extra=0,tmp1Extra,tmp2Extra,tmp3Extra,tmp4Extra;
    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; }; tmp1Extra = 0;
    { tmp2.mantissa = this.mantissa; tmp2.exponent = this.exponent; tmp2.sign = this.sign; };
    tmp2Extra = tmp2.mul128(0,tmp2,0);
    tmp2.neg();
    { tmp3.mantissa = ONE.mantissa; tmp3.exponent = ONE.exponent; tmp3.sign = ONE.sign; }; tmp3Extra = 0;
    int i=1;
    do {
      tmp1Extra = tmp1.mul128(tmp1Extra,tmp2,tmp2Extra);
      tmp4.assign(i);
      tmp3Extra = tmp3.mul128(tmp3Extra,tmp4,0);
      tmp4.assign(2*i+1);
      tmp4Extra = tmp4.mul128(0,tmp3,tmp3Extra);
      tmp4Extra = tmp4.recip128(tmp4Extra);
      tmp4Extra = tmp4.mul128(tmp4Extra,tmp1,tmp1Extra);
      extra = add128(extra,tmp4,tmp4Extra);
      i++;
    } while (exponent - tmp4.exponent < 128);
    tmp1.assign(1,0x40000000,0x48375d410a6db446L); // -2/sqrt(pi)
    extra = mul128(extra,tmp1,0xb8ea453fb5ff61a2L);
    extra = add128(extra,ONE,0);
    roundFrom128(extra);
  }*/

 /* private void erfc2Internal() {
    //             -x² -1
    //            e   x   /      1      3       3*5     3*5*7            // erfc(x) = -------- | 1 - --- + ------ - ------ + ------ - ... |

    //           sqrt(pi) \     2x²        2        3        4       /
    //                                (2x²)    (2x²)    (2x²)

    // Calculate iteration stop criteria
    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };
    tmp1.sqr();
    tmp2.assign(0,0x40000000,0x5c3811b4bfd0c8abL); // 1/0.694
    tmp2.mul(tmp1);
    tmp2.sub(HALF);
    int digits = tmp2.toInteger(); // number of accurate digits = x*x/0.694-0.5
    if (digits > 64)
      digits = 64;
    tmp1.scalbn(1);
    int dxq = tmp1.toInteger()+1;

    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };
    recip();
    { tmp2.mantissa = this.mantissa; tmp2.exponent = this.exponent; tmp2.sign = this.sign; };
    { tmp3.mantissa = this.mantissa; tmp3.exponent = this.exponent; tmp3.sign = this.sign; };
    tmp3.sqr();
    tmp3.neg();
    tmp3.scalbn(-1);
    { this.mantissa = ONE.mantissa; this.exponent = ONE.exponent; this.sign = ONE.sign; };
    { tmp4.mantissa = ONE.mantissa; tmp4.exponent = ONE.exponent; tmp4.sign = ONE.sign; };
    int i=1;
    do {
      tmp4.mul(2*i-1);
      tmp4.mul(tmp3);
      add(tmp4);
      i++;
    } while (tmp4.exponent-0x40000000>-(digits+2) && 2*i-1<dxq);
    mul(tmp2);
    tmp1.sqr();
    tmp1.neg();
    tmp1.exp();
    mul(tmp1);
    tmp1.assign(0,0x3fffffff,0x48375d410a6db447L); // 1/sqrt(pi)
    mul(tmp1);
  }*/
/*
  public void erfc() {
    if ((this.exponent < 0 && this.mantissa != 0))
      return;
    if ((this.exponent == 0 && this.mantissa == 0)) {
      { this.mantissa = ONE.mantissa; this.exponent = ONE.exponent; this.sign = ONE.sign; };
      return;
    }
    if ((this.exponent < 0 && this.mantissa == 0) || toInteger()>27281) {
      if ((this.sign!=0)) {
        { this.mantissa = TWO.mantissa; this.exponent = TWO.exponent; this.sign = TWO.sign; };
      } else
        makeZero(0);
      return;
    }
    byte s = sign;
    sign = 0;

    tmp1.assign(0, 0x40000002, 0x570a3d70a3d70a3dL); // 5.44
    if (this.lessThan(tmp1))
      erfc1Internal();
    else
      erfc2Internal();

    if (s != 0) {
      neg();
      add(TWO);
    }
  }*/

//*****************************************************************************
// Calendar conversions taken from http://www.fourmilab.ch/documents/calendar/

  private static int floorDiv(int a, int b) {
    if (a>=0)
      return a/b;
    return -((-a+b-1)/b);
  }
  private static int floorMod(int a, int b) {
    if (a>=0)
      return a%b;
    return a+((-a+b-1)/b)*b;
  }
/*
  private static boolean leap_gregorian(int year) {
    return ((year % 4) == 0) &&
      (!(((year % 100) == 0) && ((year % 400) != 0)));
  }

  //  GREGORIAN_TO_JD -- Determine Julian day number from Gregorian
  //  calendar date -- Except that we use 1/1-0 as day 0
  public static int gregorian_to_jd(int year, int month, int day) {
    return ((366 - 1) +
            (365 * (year - 1)) +
            (floorDiv(year - 1, 4)) +
            (-floorDiv(year - 1, 100)) +
            (floorDiv(year - 1, 400)) +
            ((((367 * month) - 362) / 12) +
             ((month <= 2) ? 0 : (leap_gregorian(year) ? -1 : -2)) + day));
  }

  //  JD_TO_GREGORIAN -- Calculate Gregorian calendar date from Julian
  //  day -- Except that we use 1/1-0 as day 0
  public static int jd_to_gregorian(int jd) {
    int wjd, depoch, quadricent, dqc, cent, dcent, quad, dquad,
        yindex, year, yearday, leapadj, month, day;

    wjd = jd;
    depoch = wjd - 366;
    quadricent = floorDiv(depoch, 146097);
    dqc = floorMod(depoch, 146097);
    cent = floorDiv(dqc, 36524);
    dcent = floorMod(dqc, 36524);
    quad = floorDiv(dcent, 1461);
    dquad = floorMod(dcent, 1461);
    yindex = floorDiv(dquad, 365);
    year = (quadricent * 400) + (cent * 100) + (quad * 4) + yindex;
    if (!((cent == 4) || (yindex == 4)))
      year++;
    yearday = wjd - gregorian_to_jd(year, 1, 1);
    leapadj = ((wjd < gregorian_to_jd(year, 3, 1)) ? 0
               : (leap_gregorian(year) ? 1 : 2));
    month = floorDiv(((yearday + leapadj) * 12) + 373, 367);
    day = (wjd - gregorian_to_jd(year, month, 1)) + 1;

    return (year*100+month)*100+day;
  }

  public void toDHMS() {
    // Convert from "hours" to "days, hours, minutes and seconds"
    // Actually, it converts to YYYYMMDDhh.mmss if input>=8784
    if (!(this.exponent >= 0 && this.mantissa != 0))
      return;
    boolean negative = (this.sign!=0);
    abs();

    int D,m;
    long h;
    h = toLong();
    frac();
    tmp1.assign(60);
    mul(tmp1);
    m = toInteger();
    frac();
    mul(tmp1);

    // MAGIC ROUNDING: Check if we are 2**-16 second short of a whole minute
    // i.e. "seconds" > 59.999985
    { tmp2.mantissa = ONE.mantissa; tmp2.exponent = ONE.exponent; tmp2.sign = ONE.sign; };
    tmp2.scalbn(-16);
    add(tmp2);
    if (this.compare(tmp1) >= 0) {
      // Yes. So set zero seconds instead and carry over to minutes and hours
      { this.mantissa = ZERO.mantissa; this.exponent = ZERO.exponent; this.sign = ZERO.sign; };
      m++;
      if (m >= 60) {
        m -= 60;
        h++;
      }
      // Phew! That was close. From now on it is integer arithmetic...
    } else {
      // Nope. So try to undo the damage...
      sub(tmp2);
    }

    D = (int)(h/24);
    h %= 24;
    if (D >= 366)
      D = jd_to_gregorian(D);

    add(m*100);
    div(10000);
    tmp1.assign(D*100L+h);
    add(tmp1);

    if (negative)
      neg();
  }

  public void fromDHMS() {
    // Convert from "days, hours, minutes and seconds" to "hours"
    // Actually, it converts from YYYYMMDDhh.mmss, if input>=1000000
    if (!(this.exponent >= 0 && this.mantissa != 0))
      return;
    boolean negative = (this.sign!=0);
    abs();

    int Y,M,D,m;
    long h;
    h = toLong();
    frac();
    tmp1.assign(100);
    mul(tmp1);
    m = toInteger();
    frac();
    mul(tmp1);

    // MAGIC ROUNDING: Check if we are 2**-10 second short of 100 seconds
    // i.e. "seconds" > 99.999
    { tmp2.mantissa = ONE.mantissa; tmp2.exponent = ONE.exponent; tmp2.sign = ONE.sign; };
    tmp2.scalbn(-10);
    add(tmp2);
    if (this.compare(tmp1) >= 0) {
      // Yes. So set zero seconds instead and carry over to minutes and hours
      { this.mantissa = ZERO.mantissa; this.exponent = ZERO.exponent; this.sign = ZERO.sign; };
      m++;
      if (m >= 100) {
        m -= 100;
        h++;
      }
      // Phew! That was close. From now on it is integer arithmetic...
    } else {
      // Nope. So try to undo the damage...
      sub(tmp2);
    }

    D = (int)(h/100);
    h %= 100;
    if (D>=10000) {
      M = D/100;
      D %= 100;
      if (D==0) D=1;
      Y = M/100;
      M %= 100;
      if (M==0) M=1;
      D = gregorian_to_jd(Y,M,D);
    }

    add(m*60);
    div(3600);
    tmp1.assign(D*24L+h);
    add(tmp1);

    if (negative)
      neg();
  }

  public void time() {
    long now = System.currentTimeMillis();
    int h,m,s;
    now /= 1000;
    s = (int)(now % 60);
    now /= 60;
    m = (int)(now % 60);
    now /= 60;
    h = (int)(now % 24);
    assign((h*100+m)*100+s);
    div(10000);
  }

  public void date() {
    long now = System.currentTimeMillis();
    now /= 86400000; // days
    now *= 24; // hours
    assign(now);
    add(719528*24); // 1970-01-01 era
    toDHMS();
  }
*/
//*****************************************************************************

/*  public static long randSeedA = 0x6487ed5110b4611aL;// Something to start with
  public static long randSeedB = 0x56fc2a2c515da54dL;// (mantissas of pi and e)

  // 64 Bit CRC Generators
  //
  // The generators used here are not cryptographically secure, but
  // two weak generators are combined into one strong generator by
  // skipping bits from one generator whenever the other generator
  // produces a 0-bit.
  private static void advanceBit() {
    randSeedA = (randSeedA<<1)^(randSeedA<0?0x1b:0);
    randSeedB = (randSeedB<<1)^(randSeedB<0?0xb000000000000001L:0);
  }

  // Get next bits from the pseudo-random sequence
  private static long nextBits(int bits) {
    long answer = 0;
    while (bits-- > 0) {
      while (randSeedA >= 0)
        advanceBit();
      answer = (answer<<1) + (randSeedB < 0 ? 1 : 0);
      advanceBit();
    }
    return answer;
  }

  // Accumulate randomness from seed, e.g. System.currentTimeMillis()
  public static void accumulateRandomness(long seed) {
    randSeedA ^= seed & 0x5555555555555555L;
    randSeedB ^= seed & 0xaaaaaaaaaaaaaaaaL;
    nextBits(63);
  }

  public void random() {
    sign = 0;
    exponent = 0x3fffffff;
    mantissa = nextBits(63);
    normalize();
  }*/

//*****************************************************************************

  private int digit(char a, int base, boolean twosComplement) {
    int digit = -1;
    if (a>='0' && a<='9')
      digit = (int)(a-'0');
    else if (a>='A' && a<='F')
      digit = (int)(a-'A')+10;
    if (digit >= base)
      return -1;
    if (twosComplement)
      digit ^= base-1;
    return digit;
  }

  private void shiftUp(int base) {
    if (base==2)
      scalbn(1);
    else if (base==8)
      scalbn(3);
    else if (base==16)
      scalbn(4);
    else
      mul10();
  }

  private void atof(String a, int base) {
    makeZero();
    int length = a.length();
    int index = 0;
    byte tmpSign = 0;
    boolean compl = false;
    while (index<length && a.charAt(index)==' ')
      index++;
    if (index<length && a.charAt(index)=='-') {
      tmpSign=1;
      index++;
    } else if (index<length && a.charAt(index)=='+') {
      index++;
    } else if (index<length && a.charAt(index)=='/') {
      // Input is twos complemented negative number
      compl=true;
      tmpSign=1;
      index++;
    }
    int d;
    while (index<length && (d=digit(a.charAt(index),base,compl))>=0) {
      shiftUp(base);
      add(d);
      index++;
    }
    if (index<length && (a.charAt(index)=='.' || a.charAt(index)==',')) {
      index++;
      { tmp2.mantissa = ONE.mantissa; tmp2.exponent = ONE.exponent; tmp2.sign = ONE.sign; };
      while (index<length && (d=digit(a.charAt(index),base,compl))>=0) {
        tmp2.shiftUp(base);
        shiftUp(base);
        add(d);
        index++;
      }
      if (compl)
        add(ONE);
      div(tmp2);
    } else if (compl) {
      add(ONE);
    }
    while (index<length && a.charAt(index)==' ')
      index++;
    if (index<length && (a.charAt(index)=='e' || a.charAt(index)=='E')) {
      index++;
      int exp = 0;
      boolean expNeg = false;
      if (index<length && a.charAt(index)=='-') {
        expNeg = true;
        index++;
      } else if (index<length && a.charAt(index)=='+') {
        index++;
      }
      while (index<length && a.charAt(index)>='0' && a.charAt(index)<='9') {
        if (exp < 400000000) // This takes care of overflows and makes inf or 0
          exp = (exp*10)+(int)(a.charAt(index)-'0');
        index++;
      }
      if (expNeg)
        exp = -exp;

      if (base==2)
        scalbn(exp);
      else if (base==8)
        scalbn(exp*3);
      else if (base==16)
        scalbn(exp*4);
      else {
        if (exponent > 300000000 || exponent < -300000000) {
          // Kludge to be able to enter very large and very small numbers
          // without causing over/underflows
          { tmp1.mantissa = TEN.mantissa; tmp1.exponent = TEN.exponent; tmp1.sign = TEN.sign; };
          tmp1.pow(exp/2);
          mul(tmp1);
          exp -= exp/2;
        }
        { tmp1.mantissa = TEN.mantissa; tmp1.exponent = TEN.exponent; tmp1.sign = TEN.sign; };
        tmp1.pow(exp);
        mul(tmp1);
      }
    }
    sign = tmpSign;
  }

//*****************************************************************************

  private void normalizeBCD() {
    if (mantissa == 0) {
      exponent = 0;
      return;
    }
    int carry = 0;
    for (int i=0; i<64; i+=4) {
      int d = (int)((mantissa>>>i)&0xf) + carry;
      carry = 0;
      if (d >= 10) {
        d -= 10;
        carry = 1;
      }
      mantissa &= ~(0xfL<<i);
      mantissa += (long)d<<i;
    }
    if (carry != 0) {
      if ((int)(mantissa&0xf)>=5)
        mantissa += 0x10; // Rounding, may be inaccurate
      mantissa >>>= 4;
      mantissa += 1L<<60;
      exponent++;
      if ((int)(mantissa&0xf)>=10) {
        // Oh, no, not again!
        normalizeBCD();
      }
    }
    while ((mantissa >>> 60) == 0) {
      mantissa <<= 4;
      exponent--;
    }
  }

  private void toBCD() {
    // Convert normal nonzero finite Real to BCD format, represented by 16
    // 4-bit decimal digits in the mantissa, and exponent as a power of 10
    { tmp1.mantissa = this.mantissa; tmp1.exponent = this.exponent; tmp1.sign = this.sign; };
    tmp1.abs();
    { tmp2.mantissa = tmp1.mantissa; tmp2.exponent = tmp1.exponent; tmp2.sign = tmp1.sign; };
    exponent = tmp1.lowPow10();
    if (exponent > 300000000 || exponent < -300000000) {
      // Kludge to be able to print very large and very small numbers
      // without causing over/underflows
      { tmp1.mantissa = TEN.mantissa; tmp1.exponent = TEN.exponent; tmp1.sign = TEN.sign; };
      tmp1.pow(exponent/2);
      tmp2.div(tmp1); // So, divide twice by not-so-extreme numbers
      { tmp1.mantissa = TEN.mantissa; tmp1.exponent = TEN.exponent; tmp1.sign = TEN.sign; };
      tmp1.pow(exponent-(exponent/2));
    }
    tmp2.div(tmp1);
    mantissa = 0;
    for (int i=60; i>=0; i-=4) {
      { tmp1.mantissa = tmp2.mantissa; tmp1.exponent = tmp2.exponent; tmp1.sign = tmp2.sign; };
      tmp1.floor();
      mantissa += (long)tmp1.toInteger()<<i;
      tmp2.sub(tmp1);
      tmp2.mul10();
    }
    if (tmp2.compare(FIVE) >= 0)
      mantissa++; // Rounding
    normalizeBCD();
  }

  private boolean testCarryWhenRounded(int base,int bitsPerDigit,int precision)
  {
    if (mantissa >= 0)
      return false; // msb not set -> carry cannot occur
    int shift = 64-precision*bitsPerDigit;
    if (shift<=0)
      return false; // too high precision -> no rounding necessary
    { tmp5.mantissa = this.mantissa; tmp5.exponent = this.exponent; tmp5.sign = this.sign; };
    if (base==10) {
      tmp5.mantissa += (5L<<(shift-4));
      tmp5.normalizeBCD();
    } else {
      tmp5.mantissa += (1L<<(shift-1));
    }
    if (tmp5.mantissa >= 0) {
      // msb has changed from 1 to 0 -> we have carry
      // now modify the original number
      mantissa = (1L<<(64-bitsPerDigit));
      exponent++;
      return true;
    }
    return false;
  }

  private void round(int base, int bitsPerDigit, int precision) {
    if ((this.exponent == 0 && this.mantissa == 0))
      return;
    int shift = 64-precision*bitsPerDigit;
    if (shift<=0)
      return; // too high precision -> no rounding necessary
    long orgMantissa = mantissa;
    if (base==10) {
      mantissa += (5L<<(shift-4));
      normalizeBCD();
    } else {
      mantissa += (1L<<(shift-1));
    }
    if (orgMantissa < 0 && mantissa >= 0) {
      // msb has changed from 1 to 0 -> we have carry
      mantissa = (1L<<(64-bitsPerDigit));
      exponent++;
    }
  }

  public static class NumberFormat
  {
    public int base; // 2 8 10 16
    public int maxwidth; // 30
    public int precision; // 0-16 (for decimal)
    public int fse; // NONE FIX SCI ENG
    public char point; // '.' ','
    public boolean removePoint; // true/false
    public char thousand; // '.' ',' ' ' 0
    public int align; // NONE LEFT RIGHT CENTER

    public static final int FSE_NONE = 0;
    public static final int FSE_FIX = 1;
    public static final int FSE_SCI = 2;
    public static final int FSE_ENG = 3;

    public static final int ALIGN_NONE = 0;
    public static final int ALIGN_LEFT = 1;
    public static final int ALIGN_RIGHT = 2;
    public static final int ALIGN_CENTER = 3;

    NumberFormat() {
      base = 10;
      maxwidth = 30;
      precision = 16;
      fse = FSE_NONE;
      point = '.';
      removePoint = true;
      thousand = 0;
      align = ALIGN_NONE;
    }
  }

  private String align(StringBuffer s, NumberFormat format) {
    if (format.align == NumberFormat.ALIGN_LEFT) {
      while (s.length()<format.maxwidth)
        s.append(' ');
    } else if (format.align == NumberFormat.ALIGN_RIGHT) {
      while (s.length()<format.maxwidth)
        s.insert(0,' ');
    } else if (format.align == NumberFormat.ALIGN_CENTER) {
      while (s.length()<format.maxwidth) {
        s.append(' ');
        if (s.length()<format.maxwidth)
          s.insert(0,' ');
      }
    }
    return s.toString();
  }

  private static StringBuffer ftoaBuf = new StringBuffer(40);
  private static StringBuffer ftoaExp = new StringBuffer(15);
  public static final String hexChar = "0123456789ABCDEF";

  private String ftoa(NumberFormat format) {
    ftoaBuf.setLength(0);

    if ((this.exponent < 0 && this.mantissa != 0)) {
      ftoaBuf.append("nan");
      return align(ftoaBuf,format);
    }
    if ((this.exponent < 0 && this.mantissa == 0)) {
      ftoaBuf.append((this.sign!=0) ? "-inf":"inf");
      return align(ftoaBuf,format);
    }

    int bitsPerDigit,digitsPerThousand;
    switch (format.base) {
      case 2:
        bitsPerDigit = 1;
        digitsPerThousand = 8;
        break;
      case 8:
        bitsPerDigit = 3;
        digitsPerThousand = 1000; // Disable thousands separator
        break;
      case 16:
        bitsPerDigit = 4;
        digitsPerThousand = 4;
        break;
      case 10:
      default:
        bitsPerDigit = 4;
        digitsPerThousand = 3;
        break;
    }
    if (format.thousand == 0)
      digitsPerThousand = 1000; // Disable thousands separator

    int accurateBits = 64;
    { tmp4.mantissa = this.mantissa; tmp4.exponent = this.exponent; tmp4.sign = this.sign; };
    if ((this.exponent == 0 && this.mantissa == 0)) {
      tmp4.exponent = 0;
      if (format.base != 10)
        tmp4.sign = 0; // Two's complement cannot display -0
    } else {
      if (format.base == 10) {
        tmp4.toBCD();
      } else {
        if ((tmp4.sign!=0)) {
          tmp4.mantissa = -tmp4.mantissa;
          if ((tmp4.mantissa >>> 62) == 3) {
            tmp4.mantissa <<= 1;
            tmp4.exponent--;
            accurateBits--; // ?
          }
        }
        tmp4.exponent -= 0x40000000-1;
        int shift = bitsPerDigit-1 - floorMod(tmp4.exponent,bitsPerDigit);
        tmp4.exponent = floorDiv(tmp4.exponent,bitsPerDigit);
        if (shift == bitsPerDigit-1) {
          // More accurate to shift up instead
          tmp4.mantissa <<= 1;
          tmp4.exponent--;
          accurateBits--;
        }
        else if (shift>0) {
          tmp4.mantissa = (tmp4.mantissa+(1L<<(shift-1)))>>>shift;
          if ((tmp4.sign!=0)) // Need to fill in some 1's at the top
            tmp4.mantissa |= 0x8000000000000000L>>(shift-1); // >> not >>>
        }
      }
    }
    int accurateDigits = (accurateBits+bitsPerDigit-1)/bitsPerDigit;

    int precision;
    int pointPos = 0;
    do
    {
      int width = format.maxwidth-1; // subtract 1 for decimal point
      int prefix = 0;
      if (format.base != 10)
        prefix = 1; // want room for at least one "0" or "f/7/1"
      else if ((tmp4.sign!=0))
        width--; // subtract 1 for sign

      boolean useExp = false;
      switch (format.fse) {
        case NumberFormat.FSE_SCI:
          precision = format.precision+1;
          useExp = true;
          break;
        case NumberFormat.FSE_ENG:
          pointPos = floorMod(tmp4.exponent,3);
          precision = format.precision+1+pointPos;
          useExp = true;
          break;
        case NumberFormat.FSE_FIX:
        case NumberFormat.FSE_NONE:
        default:
          precision = 1000;
          if (format.fse == NumberFormat.FSE_FIX)
            precision = format.precision+1;
          if (tmp4.exponent+1 >
                width-(tmp4.exponent+prefix)/digitsPerThousand-prefix+
                (format.removePoint ? 1:0) ||
              tmp4.exponent+1 > accurateDigits ||
              -tmp4.exponent >= width ||
              -tmp4.exponent >= precision)
          {
            useExp = true;
          } else {
            pointPos = tmp4.exponent;
            precision += tmp4.exponent;
            if (tmp4.exponent > 0)
              width -= (tmp4.exponent+prefix)/digitsPerThousand;
            if (format.removePoint && tmp4.exponent == width-prefix)
              width++; // Add  1 for the decimal point that will be removed
          }
          break;
      }

      if (prefix!=0 && pointPos>=0)
        width -= prefix;

      ftoaExp.setLength(0);
      if (useExp) {
        ftoaExp.append('e');
        ftoaExp.append(tmp4.exponent-pointPos);
        width -= ftoaExp.length();
      }
      if (precision > accurateDigits)
        precision = accurateDigits;
      if (precision > width)
        precision = width;
      if (precision > width+pointPos) // In case of negative pointPos
        precision = width+pointPos;
      if (precision <= 0)
        precision = 1;
    }
    while (tmp4.testCarryWhenRounded(format.base,bitsPerDigit,precision));

    tmp4.round(format.base,bitsPerDigit,precision);

    // Start generating the string. First the sign
    if ((tmp4.sign!=0) && format.base == 10)
      ftoaBuf.append('-');

    // Save pointPos for hex/oct/bin prefixing with thousands-sep
    int pointPos2 = pointPos;

    // Add leading zeros (or f/7/1)
    char prefixChar = (format.base==10 || (tmp4.sign==0)) ? '0' :
                      hexChar.charAt(format.base-1);
    if (pointPos < 0) {
      ftoaBuf.append(prefixChar);
      ftoaBuf.append(format.point);
      while (pointPos < -1) {
        ftoaBuf.append(prefixChar);
        pointPos++;
      }
    }

    // Add fractional part
    while (precision > 0) {
      ftoaBuf.append(hexChar.charAt((int)(tmp4.mantissa>>>(64-bitsPerDigit))));
      tmp4.mantissa <<= bitsPerDigit;
      if (pointPos>0 && pointPos%digitsPerThousand==0)
        ftoaBuf.append(format.thousand);
      if (pointPos == 0)
        ftoaBuf.append(format.point);
      precision--;
      pointPos--;
    }

    if (format.fse == NumberFormat.FSE_NONE) {
      // Remove trailing zeros
      while (ftoaBuf.charAt(ftoaBuf.length()-1) == '0')
        ftoaBuf.setLength(ftoaBuf.length()-1);
    }
    if (format.removePoint) {
      // Remove trailing point
      if (ftoaBuf.charAt(ftoaBuf.length()-1) == format.point)
        ftoaBuf.setLength(ftoaBuf.length()-1);
    }

    // Add exponent
    ftoaBuf.append(ftoaExp.toString());

    // In case hex/oct/bin number, prefix with 0's or f/7/1's
    if (format.base!=10) {
      while (ftoaBuf.length()<format.maxwidth) {
        pointPos2++;
        if (pointPos2>0 && pointPos2%digitsPerThousand==0)
          ftoaBuf.insert(0,format.thousand);
        if (ftoaBuf.length()<format.maxwidth)
          ftoaBuf.insert(0,prefixChar);
      }
      if (ftoaBuf.charAt(0) == format.thousand)
        ftoaBuf.deleteCharAt(0);
    }

    return align(ftoaBuf,format);
  }

  private static NumberFormat tmpFormat = new NumberFormat();

  public String toString() {
    tmpFormat.base = 10;
    return ftoa(tmpFormat);
  }

  public String toString(int base) {
    tmpFormat.base = base;
    return ftoa(tmpFormat);
  }

  public String toString(NumberFormat format) {
    return ftoa(format);
  }

///////////////////////////////////////////////////////////////////////////////
// To make library re-entrant, change private static tmp variables with this:
//
//   private static final int FREE = 2;
//
//   private boolean isFree() {
//     return (sign == FREE);
//   }
//   private void setFree() {
//     sign = FREE;
//   }
//
//   private static Real [] tmpArray = {
//     new Real(FREE,0,0), new Real(FREE,0,0),
//     new Real(FREE,0,0), new Real(FREE,0,0),
//     new Real(FREE,0,0), new Real(FREE,0,0),
//     new Real(FREE,0,0), new Real(FREE,0,0)
//   };
//
//   private static Real allocTmp() {
//     synchronized (tmpArray) {
//       int i;
//       for (i=0; i<tmpArray.length; i++)
//         if (tmpArray[i].isFree()) {
//           tmpArray[i].makeZero();
//           return tmpArray[i];
//         }
//       Real [] newTmpArray = new Real[tmpArray.length*2];
//       for (i=0; i<tmpArray.length; i++)
//         newTmpArray[i] = tmpArray[i];
//       for (i=tmpArray.length; i<newTmpArray.length; i++)
//         newTmpArray[i] = new Real(FREE,0,0);
//       tmpArray = newTmpArray;
//
//       i = tmpArray.length;
//       tmpArray[i].makeZero();
//       return tmpArray[i];
//     }
//   }
//
//   private static void freeTmp(Real a) {
//     synchronized (tmpArray) {
//       for (int i=0; i<tmpArray.length; i++)
//         if (a == tmpArray[i]) {
//           a.setFree();
//           return;
//         }
//     }
//   }

}
