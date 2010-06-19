
import java.lang.*;
/**
 * <p>Title: Class for float-point calculations in J2ME applications (MIDP 1.0/CLDC 1.0 where float or double types are not available)</p>
 * <p>Description: It makes float-point calculations via integer numbers</p>
 * <p>Copyright: Nikolay Klimchuk Copyright (c) 2002-2004</p>
 * <p>Company: UNTEH</p>
 * <p>License: Free for use only for non-commercial purpose</p>
 * <p>If you want to use all or part of this class for commercial applications then take into account these conditions:</p>
 * <p>1. I need a one copy of your product which includes my class with license key and so on</p>
 * <p>2. Please append my copyright information henson.midp.F (C) by Nikolay Klimchuk on ‘About’ screen of your product</p>
 * <p>3. If you have web site please append link <a href=”http://henson.newmail.ru”>Nikolay Klimchuk</a> on the page with description of your product</p>
 * <p>That's all, thank you!</p>
 * @author Nikolay Klimchuk http://henson.newmail.ru
 * @version 0.99
 */

public class F
{
  /** ERROR constant */
  final static private F ERROR=new F(Long.MAX_VALUE, Long.MAX_VALUE);
  /** Number of itterations in sqrt method, if you want to make calculations
   * more precise set ITNUM=6,7,... or ITMUN=4,3,... to make it faster */
  final static private int ITNUM=5;
  /** Square root from 3 */
  final static public F SQRT3=new F(1732050807568877294L, -18L);
  /** The F value that is closer than any other to pi, the ratio of the circumference of a circle to its diameter */
  final static public F PI=new F(3141592653589793238L, -18L);
  /** Zero constant */
  final static public F ZERO=new F();
  /** One constant */
  final static public F ONE=new F(1L);
  /** The F value that is closer than any other to e, the base of the natural logarithms */
  final static public F E = new F(271828182845904512L, -17);
  /** Log10 constant */
  final static public F LOG10 = new F(2302585092994045684L, -18);
  //
  /** Pi/2 constant */
  final static public F PIdiv2=PI.Div(new F(2L));
  /** Pi/4 constant */
  final static public F PIdiv4=PIdiv2.Div(new F(2L));
  /** Pi/6 constant */
  final static public F PIdiv6=PIdiv2.Div(new F(3L));
  /** Pi/12 constant */
  final static public F PIdiv12=PIdiv6.Div(new F(2L));
  /** Pi*2 constant */
  final static public F PImul2=PI.Mul(new F(2L));
  /** Pi*4 constant */
  final static public F PImul4=PI.Mul(new F(4L));
  /** ln(0.5) constant */
  final static public F LOGdiv2 = new F(-6931471805599453094L, -19);
  /** Mantissa */
  public long m_Val;
  /** Exponent */
  public long m_E;
  /** Limit of value */
  private long maxLimit=Long.MAX_VALUE/100;
  //
  /**
   * Create object with zero inside
   */
  public F()
  {
    m_Val=m_E=0;
  }
  /**
   * Create object and makes initialization with value
   * @param value long - value
   */
  public F(long value)
  {
    m_Val=value;
    m_E=0;
  }
  /**
   * Create object and makes initialization both mantissa and exponent
   * @param value long - mantissa
   * @param e long - exponent
   */
  public F(long value, long e)
  {
    m_Val=value;
    if(m_Val==0)
      m_E=0;
    else
      m_E=e;
  }

  public F(int value, int e)
  {
    m_Val=value;
    if(m_Val==0)
      m_E=0;
    else
      m_E=e;  	
  }
  /**
   * Create object and makes initialization by other F object
   * @param value F - source object
   */
  public F(F value)
  {
    m_Val=value.m_Val;
    if(m_Val==0)
      m_E=0;
    else
      m_E=value.m_E;
  }
  /**
   * Convert F object to long number
   * @return long - number
   */
  public long toLong()
  {
    long tmpE=m_E;
    long tmpVal=m_Val;
    //
    while(tmpE!=0)
    {
      if(tmpE<0)
      {
        tmpVal/=10;
        tmpE++;
      }
      else
      {
        tmpVal*=10;
        tmpE--;
      }
    }
    return tmpVal;
  }

  public int toInt()
  {
  	return (int)(toLong());
  }
  
  /**
   * Check value of current F object is NaN
   * @return boolean - true-if NaN, false-if not
   */
  public boolean isError()
  {
    return (this.m_Val==ERROR.m_Val && this.m_E==ERROR.m_E);
  }
  /** Convert F object to string */
  public String toString()
  {
    if(isError())
      return "NaN";
    //
    RemoveZero();
    //
    Long l=new Long(m_Val);
    String str=l.toString();
    int len=str.length();
    boolean neg=false;
    if(m_Val<0L)
    {
      neg=true;
      str=str.substring(1, len);
      len--;
    }
    //
    StringBuffer sb=new StringBuffer();
    //
    if(m_E<0L)
    {
      int absE=(int)Math.abs(m_E);
      if(absE<len)
      {
        sb.append(str.substring(0, len-absE));
        sb.append(".");
        sb.append(str.substring(len-absE));
      }
      else
      {
        sb.append(str);
        for(int  i=0; i<(absE-len); i++)
          sb.insert(0, "0");
        sb.insert(0, "0.");
      }
    }
    else
    {
      if(len+m_E>6)
      {
        sb.append(str.charAt(0));
        if(str.length()>1)
        {
          sb.append(".");
          sb.append(str.substring(1));
        }
        else
          sb.append(".0");
        sb.append("E"+(len-1+m_E));
      }
      else
      {
        sb.append(str);
        for(int i=0; i<m_E; i++)
          sb.append("0");
      }
    }
    //
    str=sb.toString();
    sb=null;
    if(neg)
      str="-"+str;
    //
    return str;
  }

  public StringBuffer toStringBuffer()
  {
  	StringBuffer bf = new StringBuffer(toString());
	return bf;
  }
  /**
   * Append value of argument to value of current F object and return as new F object
   * @param value F - argument
   * @return F - current+value
   */
  public F Add(F value)
  {
    if(value.Equal(ZERO))
      return new F(this);
    //
    long e1=m_E;
    long e2=value.m_E;
    long v1=m_Val;
    long v2=value.m_Val;
    // E must be equal in both operators
    while (e1 != e2)
    {
      if(e1 > e2)
      {
        if(Math.abs(v1)<maxLimit)
        {
          v1*=10;
          e1--;
        }
        else
        {
          v2/=10;
          e2++;
        }
      }
      else
      if(e1 < e2)
      {
        if(Math.abs(v2)<maxLimit)
        {
          v2*=10;
          e2--;
        }
        else
        {
          v1/=10;
          e1++;
        }
      }
    }
    //
    if( (v1>0 && v2>Long.MAX_VALUE-v1) || (v1<0 && v2<Long.MIN_VALUE-v1) )
    {
      v1/=10; e1++;
      v2/=10; e2++;
    }
    //
    if(v1>0 && v2>Long.MAX_VALUE-v1)
      return new F(ERROR);
    else
    if(v1<0 && v2<Long.MIN_VALUE-v1)
      return new F(ERROR);
    //
    return new F(v1+v2, e1);
  }
  /**
   * Subtract value of argument from value of current F object and return as new F object
   * @param value F - argument
   * @return F - current-value
   */
  public F Sub(F value)
  {
    if(value.Equal(ZERO))
      return new F(m_Val, m_E);
    return Add(new F(-value.m_Val, value.m_E));
  }
  /**
   * Multiply value of current F object on argument and return as new F object
   * @param value F - argument
   * @return F - current*value
   */
  public F Mul(F value)
  {
    if(value.Equal(ZERO) || this.Equal(ZERO))
      return new F(ZERO);
    if(value.Equal(ONE))
      return new F(this);
    //
    boolean negative1=(m_Val<0);
    if(negative1) m_Val=-m_Val;
    boolean negative2=(value.m_Val<0);
    if(negative2) value.m_Val=-value.m_Val;
    // Check overflow and underflow
    do
    {
      if(value.m_Val>m_Val)
      {
        if(Long.MAX_VALUE/m_Val<value.m_Val)
        {
          value.m_Val/=10;
          value.m_E++;
        }
        else
          break;
      }
      else
      {
        if(Long.MAX_VALUE/value.m_Val<m_Val)
        {
          m_Val/=10;
          m_E++;
        }
        else
          break;
      }
    } while(true);
    //
    if(negative1) m_Val=-m_Val;
    if(negative2) value.m_Val=-value.m_Val;
    //
    long e=m_E+value.m_E;
    long v=m_Val*value.m_Val;
    return new F(v, e);
  }
  /**
   * Divide value of current F object on argument and return as new F object
   * @param value F - argument
   * @return F - current/value
   */
  public F Div(F value)
  {
    if(value.Equal(ONE))
      return new F(this);
    //
    long e1=m_E;
    long e2=value.m_E;
    long v2=value.m_Val;
    if(v2==0L)
      return new F(ERROR);
    long v1=m_Val;
    if(v1==0L)
      return new F(ZERO);
    //
    long val=0L;
    while(true)
    {
      val+=(v1/v2);
      v1%=v2;
      if(v1==0L || Math.abs(val)>(Long.MAX_VALUE/10L))
        break;
      if(Math.abs(v1)>(Long.MAX_VALUE/10L))
      {
        v2/=10L;
        e2++;
      }
      else
      {
        v1*=10L;
        e1--;
      }
      val*=10L;
    }
    //
    F f=new F(val, e1-e2);
    f.RemoveZero();
    return f;
  }
  public void RemoveZero()
  {
    if(m_Val==0)
      return;
    while ( m_Val%10 == 0 )
    {
     m_Val/=10;
     m_E++;
    }
  }
  /**
   * Is value of current F object greater?
   * @param x F - argument
   * @return boolean - true-if current value is greater x, false-if not
   */
  public boolean Great(F x)
  {
    long e1=m_E;
    long e2=x.m_E;
    long v1=m_Val;
    long v2=x.m_Val;
    //
    while (e1 != e2)
    {
      if(e1 > e2)
      {
        if(Math.abs(v1)<maxLimit)
        {
          v1*=10;
          e1--;
        }
        else
        {
          v2/=10;
          e2++;
        }
      }
      else
      if(e1 < e2)
      {
        if(Math.abs(v2)<maxLimit)
        {
          v2*=10;
          e2--;
        }
        else
        {
          v1/=10;
          e1++;
        }
      }
    }
    //
    return v1>v2;
  }
  /**
   * Is value of current F object less?
   * @param x F - argument
   * @return boolean - true-if current value is less x, false-if not
   */
  public boolean Less(F x)
  {
    long e1=m_E;
    long e2=x.m_E;
    long v1=m_Val;
    long v2=x.m_Val;
    //
    while (e1 != e2)
  {
    if(e1 > e2)
    {
      if(Math.abs(v1)<maxLimit)
      {
        v1*=10;
        e1--;
      }
      else
      {
        v2/=10;
        e2++;
      }
    }
    else
    if(e1 < e2)
    {
      if(Math.abs(v2)<maxLimit)
      {
        v2*=10;
        e2--;
      }
      else
      {
        v1/=10;
        e1++;
      }
    }
  }
  //
  return v1<v2;
  }
  /**
   * Equal with current F object?
   * @param x F - argument
   * @return boolean - true-if equal, false-if not
   */
  public boolean Equal(F x)
  {
    long e1=m_E;
    long e2=x.m_E;
    long v1=m_Val;
    long v2=x.m_Val;
    //
    if((v1==0 && v2==0) || (v1==v2 && e1==e2))
      return true;
    // Values with exponent differences more than 20 times never could be equal
    /*
    if(Math.abs(e1-e2)>20)
      return false;
    */
    long diff=e1-e2;
    if(diff<-20 || diff>20)
      return false;
    //
    while (e1 != e2)
    {
      if(e1 > e2)
      {
        if(Math.abs(v1)<maxLimit)
        {
          v1*=10;
          e1--;
        }
        else
        {
          v2/=10;
          e2++;
        }
      }
      else
      if(e1 < e2)
      {
        if(Math.abs(v2)<maxLimit)
        {
          v2*=10;
          e2--;
        }
        else
        {
          v1/=10;
          e1++;
        }
      }
    }
    //
    return (v1==v2);
  }
  /**
   * Reverse sign of value in current F object and return as new F object
   * @return F - new F object
   */
  public F Neg()
  {
    return new F(-m_Val, m_E);
  }
  /**
   * Returns the trigonometric sine of an angle. Special cases: If the argument is NaN or an infinity, then the result is NaN. If the argument is zero, then the result is a zero with the same sign as the argument. A result must be within 1 ulp of the correctly rounded result. Results must be semi-monotonic
   * @param x F - an angle, in radians
   * @return F - the sine of the argument
   */
  static public F sin(F x)
  {
    while( x.Great(PI) )
      x=x.Sub(PImul2);
    while( x.Less(PI.Neg()) )
      x=x.Add(PImul2);
    // x*x*x
    F m1=x.Mul(x.Mul(x));
    F q1=m1.Div(new F(6L));
    // x*x*x*x*x
    F m2=x.Mul(x.Mul(m1));
    F q2=m2.Div(new F(120L));
    // x*x*x*x*x*x*x
    F m3=x.Mul(x.Mul(m2));
    F q3=m3.Div(new F(5040L));
    // x*x*x*x*x*x*x*x*x
    F m4=x.Mul(x.Mul(m3));
    F q4=m4.Div(new F(362880L));
    // x*x*x*x*x*x*x*x*x*x*x
    F m5=x.Mul(x.Mul(m4));
    F q5=m5.Div(new F(39916800L));
    //
    F result=x.Sub(q1).Add(q2).Sub(q3).Add(q4).Sub(q5);
    // 1e-6
    if(result.Less(new F(-999999, -6)))
      return new F(-1L);
    // 1e-6
    if(result.Great(new F(999999, -6)))
      return new F(1L);
    // 5e-4
    if(result.Great(new F(-5, -4)) && result.Less(new F(5, -4)))
      return new F(0L);
    //
    return result;
  }
  /**
   * Returns the trigonometric cosine of an angle. Special cases: If the argument is NaN or an infinity, then the result is NaN. A result must be within 1 ulp of the correctly rounded result. Results must be semi-monotonic
   * @param x F - an angle, in radians
   * @return F - the cosine of the argument
   */
  static public F cos(F x)
  {
    while( x.Great(PI) )
      x=x.Sub(PImul2);
    while( x.Less(PI.Neg()) )
      x=x.Add(PImul2);
    // x*x
    F m1=x.Mul(x);
    F q1=m1.Div(new F(2L));
    // x*x*x*x
    F m2=m1.Mul(m1);
    F q2=m2.Div(new F(24L));
    // x*x*x*x*x*x
    F m3=m1.Mul(m2);
    F q3=m3.Div(new F(720L));
    // x*x*x*x*x*x*x*x
    F m4=m2.Mul(m2);
    F q4=m4.Div(new F(40320L));
    // x*x*x*x*x*x*x*x*x*x
    F m5=m4.Mul(m1);
    F q5=m5.Div(new F(3628800L));
    F result=ONE.Sub(q1).Add(q2).Sub(q3).Add(q4).Sub(q5);
    // 1e-6
    if(result.Less(new F(-999999, -6)))
      return new F(-1L);
    // 1e-6
    if(result.Great(new F(999999, -6)))
      return new F(1L);
    // 5e-4
    if(result.Great(new F(-5, -4)) && result.Less(new F(5, -4)))
      return new F(0L);
    //
    return result;
  }
  /**
   * Returns the correctly rounded positive square root of a double value. Special cases: If the argument is NaN or less than zero, then the result is NaN. If the argument is positive infinity, then the result is positive infinity. If the argument is positive zero or negative zero, then the result is the same as the argument. Otherwise, the result is the double value closest to the true mathematical square root of the argument value
   * @param x F - a value
   * @return F - the positive square root of a. If the argument is NaN or less than zero, the result is NaN
   */
  static public F sqrt(F x)
  {
    int sp=0;
    boolean inv=false;
    F a,b;
    //
    if(x.Less(ZERO))
      return new F(ERROR);
    if(x.Equal(ZERO))
      return new F(ZERO);
    if(x.Equal(ONE))
      return new F(ONE);
    // argument less than 1 : invert it
    if(x.Less(ONE))
    {
      x=ONE.Div(x);
      inv=true;
    }
    //
    long e=x.m_E/2;
    // exponent compensation
    F tmp=new F(x.m_Val, x.m_E-e*2);
    // process series of division by 16 until argument is <16
    while(tmp.Great(new F(16L)))
    {
      sp++;
      tmp=tmp.Div(new F(16L));
    }
    // initial approximation
    a=new F(new F(2L));
    // Newtonian algorithm
    for(int i=ITNUM; i>0; i--)
    {
      b=tmp.Div(a);
      a=a.Add(b);
      a=a.Div(new F(2L));
    }
    // multiply result by 4 : as much times as divisions by 16 took place
    while(sp>0)
    {
      sp--;
      a=a.Mul(new F(4L));
    }
    // exponent compensation
    a.m_E+=e;
    // invert result for inverted argument
    if(inv)
      a=ONE.Div(a);
    return a;
  }
  /**
   * Returns the trigonometric tangent of an angle. Special cases: If the argument is NaN or an infinity, then the result is NaN. If the argument is zero, then the result is a zero with the same sign as the argument. A result must be within 1 ulp of the correctly rounded result. Results must be semi-monotonic
   * @param x F - an angle, in radians
   * @return F - the tangent of the argument
   */
  static public F tan(F x)
  {
    F c=cos(x);
    if(c.Equal(ZERO)) return new F(ERROR);
    return (sin(x).Div(c));
  }
  /**
   * Returns a new F object initialized to the value represented by the specified String
   * @param str String - the string to be parsed
   * @param radix int - basement of number
   * @return F - the F object represented by the string argument
   */
  static public F parse(String str, int radix)
  {
    // Abs
    boolean neg=false;
    if(str.charAt(0)=='-')
    {
      str=str.substring(1);
      neg=true;
    }
    //
    int pos=str.indexOf(".");
    long exp=0;
    // Find exponent position
    int pos2=str.indexOf('E');
    if(pos2==-1) pos2=str.indexOf('e');
    //
    if(pos2!=-1)
    {
      String tmp=new String(str.substring(pos2+1));
      exp=Long.parseLong(tmp);
      str=str.substring(0, pos2);
    }
    //
    if(pos!=-1)
    {
      for(int m=pos+1; m<str.length(); m++)
      {
        if(Character.isDigit(str.charAt(m)))
          exp--;
        else
          break;
      }
      str=str.substring(0, pos)+str.substring(pos+1);
      while(str.length()>1 && str.charAt(0)=='0' && str.charAt(1)!='.')
        str=str.substring(1);
    }
    //
    long result=0L;
    int len=str.length();
    //
    StringBuffer sb=new StringBuffer(str);
    while(true)
    {
      // Long value can't have length more than 20
      while(len>20)
      {
        // Very large number for Long
        sb=sb.deleteCharAt(len-1);
        // Compensation of removed zeros
        if(len<pos || pos==-1)
          exp++;
        //
        len--;
      }
      //
      try
      {
        result=Long.parseLong(sb.toString(), radix);
        if(neg)
          result=-result;
        break;
      }
      catch(Exception e)
      {
        // Very large number for Long
        sb=sb.deleteCharAt(len-1);
        // Compensation of removed zeros
        if(len<pos || pos==-1)
          exp++;
        //
        len--;
      }
    }
    sb=null;
    //
    F newValue=new F(result, exp);
    newValue.RemoveZero();
    return newValue;
  }
  /**
   * Returns the arc cosine of an angle, in the range of 0.0 through pi. Special case: If the argument is NaN or its absolute value is greater than 1, then the result is NaN. A result must be within 1 ulp of the correctly rounded result. Results must be semi-monotonic
   * @param x F - the value whose arc cosine is to be returned
   * @return F - the arc cosine of the argument
   */
  static public F acos(F x)
  {
    F f=asin(x);
    if(f.isError())
      return f;
    return PIdiv2.Sub(f);
  }
  /**
   * Returns the arc sine of an angle, in the range of -pi/2 through pi/2. Special cases: If the argument is NaN or its absolute value is greater than 1, then the result is NaN. If the argument is zero, then the result is a zero with the same sign as the argument. A result must be within 1 ulp of the correctly rounded result. Results must be semi-monotonic
   * @param x F - the value whose arc sine is to be returned
   * @return F - the arc sine of the argument
   */
  static public F asin(F x)
  {
    if( x.Less(ONE.Neg()) || x.Great(ONE) ) return new F(ERROR);
    if( x.Equal(ONE.Neg()) ) return PIdiv2.Neg();
    if( x.Equal(ONE) ) return PIdiv2;
    return atan(x.Div(sqrt(ONE.Sub(x.Mul(x)))));
  }
  /**
   * Returns the arc tangent of an angle, in the range of -pi/2 through pi/2. Special cases: If the argument is NaN, then the result is NaN. If the argument is zero, then the result is a zero with the same sign as the argument. A result must be within 1 ulp of the correctly rounded result. Results must be semi-monotonic
   * @param x F - the value whose arc tangent is to be returned
   * @return F - the arc tangent of the argument
   */
  static public F atan(F x)
  {
      boolean signChange=false;
      boolean Invert=false;
      int sp=0;
      F x2, a;
      // check up the sign change
      if(x.Less(ZERO))
      {
          x=x.Neg();
          signChange=true;
      }
      // check up the invertation
      if(x.Great(ONE))
      {
          x=ONE.Div(x);
          Invert=true;
      }
      // process shrinking the domain until x<PI/12
      while(x.Great(PIdiv12))
      {
          sp++;
          a=x.Add(SQRT3);
          a=ONE.Div(a);
          x=x.Mul(SQRT3);
          x=x.Sub(ONE);
          x=x.Mul(a);
      }
      // calculation core
      x2=x.Mul(x);
      a=x2.Add(new F(14087812, -7));
      a=new F(55913709, -8).Div(a);
      a=a.Add(new F(60310579, -8));
      a=a.Sub(x2.Mul(new F(5160454, -8)));
      a=a.Mul(x);
      // process until sp=0
      while(sp>0)
      {
          a=a.Add(PIdiv6);
          sp--;
      }
      // invertation took place
      if(Invert) a=PIdiv2.Sub(a);
      // sign change took place
      if(signChange) a=a.Neg();
      //
      return a;
  }
  /**
   * Converts rectangular coordinates (x, y) to polar (r, theta). This method computes the phase theta by computing an arc tangent of y/x in the range of -pi to pi. Special cases: If either argument is NaN, then the result is NaN. If the first argument is positive zero and the second argument is positive, or the first argument is positive and finite and the second argument is positive infinity, then the result is positive zero. If the first argument is negative zero and the second argument is positive, or the first argument is negative and finite and the second argument is positive infinity, then the result is negative zero. If the first argument is positive zero and the second argument is negative, or the first argument is positive and finite and the second argument is negative infinity, then the result is the double value closest to pi. If the first argument is negative zero and the second argument is negative, or the first argument is negative and finite and the second argument is negative infinity, then the result is the double value closest to -pi. If the first argument is positive and the second argument is positive zero or negative zero, or the first argument is positive infinity and the second argument is finite, then the result is the double value closest to pi/2. If the first argument is negative and the second argument is positive zero or negative zero, or the first argument is negative infinity and the second argument is finite, then the result is the double value closest to -pi/2. If both arguments are positive infinity, then the result is the double value closest to pi/4. If the first argument is positive infinity and the second argument is negative infinity, then the result is the double value closest to 3*pi/4. If the first argument is negative infinity and the second argument is positive infinity, then the result is the double value closest to -pi/4. If both arguments are negative infinity, then the result is the double value closest to -3*pi/4. A result must be within 2 ulps of the correctly rounded result. Results must be semi-monotonic
   * @param y F - the ordinate coordinate
   * @param x F - the abscissa coordinate
   * @return F - the theta component of the point (r, theta) in polar coordinates that corresponds to the point (x, y) in Cartesian coordinates
   */
  static public F atan2(F y, F x)
  {
    // if x=y=0
    if(y.Equal(ZERO) && x.Equal(ZERO))
      return new F(ZERO);
    // if x>0 atan(y/x)
    if(x.Great(ZERO))
      return atan(y.Div(x));
    // if x<0 sign(y)*(pi - atan(|y/x|))
    if(x.Less(ZERO))
    {
      if(y.Less(ZERO))
        return F.PI.Sub(atan(y.Div(x))).Neg();
      else
        return F.PI.Sub(atan(y.Div(x).Neg()));
    }
    // if x=0 y!=0 sign(y)*pi/2
    if(y.Less(ZERO))
      return PIdiv2.Neg();
    else
      return new F(PIdiv2);
  }

  // precise
  // x=-35 diff=1.48%
  // x=-30 diff=0.09%
  // x=30 diff=0.09%
  // x=31 diff=0.17%
  // x=32 diff=0.31%
  // x=33 diff=0.54%
  // x=34 diff=0.91%
  // x=35 diff=1.46%
  /**
   * Returns Euler's number e raised to the power of a double value. Special cases: If the argument is NaN, the result is NaN. If the argument is positive infinity, then the result is positive infinity. If the argument is negative infinity, then the result is positive zero. A result must be within 1 ulp of the correctly rounded result. Results must be semi-monotonic
   * @param x F - the exponent to raise e to
   * @return F - the value e^x, where e is the base of the natural logarithms
   */
  static public F exp(F x)
  {
    if(x.Equal(ZERO))
      return new F(ONE);
    //
    F f=new F(ONE);
    long d=1;
    F k=null;
    boolean isless=x.Less(ZERO);
    if(isless)
      x=x.Neg();
    k=new F(x).Div(new F(d));
    //
    for(long i=2; i<50; i++)
    {
      f=f.Add(k);
      k=k.Mul(x).Div(new F(i));
    }
    //
    if(isless)
      return ONE.Div(f);
    else
      return f;
  }

  // precise
  // x=25 diff=0.12%
  // x=30 diff=0.25%
  // x=35 diff=0.44%
  // x=40 diff=0.67%
  /**
   * Internal log subroutine
   * @param x F
   * @return F
   */
  static private F _log(F x)
  {
    if(!x.Great(ZERO))
      return new F(ERROR);
    //
    F f=new F(ZERO);
    // Make x to close at 1
    int appendix=0;
    while(x.Great(ZERO) && x.Less(ONE))
    {
      x=x.Mul(new F(2));
      appendix++;
    }
    //
    x=x.Div(new F(2));
    appendix--;
    //
    F y1=x.Sub(ONE);
    F y2=x.Add(ONE);
    F y=y1.Div(y2);
    //
    F k=new F(y);
    y2=k.Mul(y);
    //
    for(long i=1; i<50; i+=2)
    {
      f=f.Add(k.Div(new F(i)));
      k=k.Mul(y2);
    }
    //
    f=f.Mul(new F(2));
    for(int i=0; i<appendix; i++)
      f=f.Add(LOGdiv2);
    //
    return f;
  }
  /**
   * Returns the natural logarithm (base e) of a double value. Special cases: If the argument is NaN or less than zero, then the result is NaN. If the argument is positive infinity, then the result is positive infinity. If the argument is positive zero or negative zero, then the result is negative infinity. A result must be within 1 ulp of the correctly rounded result. Results must be semi-monotonic
   * @param x F - a number greater than 0.0
   * @return F - the value ln(x), the natural logarithm of x
   */
  static public F log(F x)
  {
    if(!x.Great(ZERO))
      return new F(ERROR);
    //
    if(x.Equal(ONE))
      return new F(ZERO);
    //
    boolean neg=false;
    if(x.m_Val<0)
    {
      neg=true;
      x.m_Val=-x.m_Val;
    }
    int index=0;
    while(x.Great(F.ONE))
    {
      x=x.Div(new F(2));
      index++;
    }
    F res=_log(x);
    for(int i=0; i<index; i++)
      res=res.Sub(LOGdiv2);
    if(neg)
      return F.ONE.Div(res);
    else
      return res;
  }

  static public F log10(F x)
  {
    if(!x.Great(ZERO))
      return new F(ERROR);
    //
    boolean neg=false;
    if(x.m_Val<0)
    {
      neg=true;
      x.m_Val=-x.m_Val;
    }
    //
    int index=0;
    if(x.Great(F.ONE))
    {
      // Áîëüøå 1
      while(x.Great(F.ONE))
      {
        x=x.Div(new F(10));
        index++;
      }
    }
    else
    {
      // Ìåíüøå èëè ðàâíî 1
      while(x.Less(F.ONE))
      {
        x=x.Mul(new F(10));
        index--;
      }
    }
    //
    F res=new F(index);
    if(!x.Equal(ONE))
      res=res.Add(log(x).Div(LOG10));
    //
    if(neg)
      return F.ONE.Div(res);
    else
      return res;
  }
  // precise y=3.5
  // x=15 diff=0.06%
  // x=20 diff=0.40%
  // x=25 diff=1.31%
  // x=30 diff=2.95%
  // if x negative y must be integer value
  /**
   * Returns the value of the first argument raised to the power of the second argument. Special cases: If the second argument is positive or negative zero, then the result is 1.0. If the second argument is 1.0, then the result is the same as the first argument. If the second argument is NaN, then the result is NaN. If the first argument is NaN and the second argument is nonzero, then the result is NaN. If the absolute value of the first argument is greater than 1 and the second argument is positive infinity, or the absolute value of the first argument is less than 1 and the second argument is negative infinity, then the result is positive infinity. If the absolute value of the first argument is greater than 1 and the second argument is negative infinity, or the absolute value of the first argument is less than 1 and the second argument is positive infinity, then the result is positive zero. If the absolute value of the first argument equals 1 and the second argument is infinite, then the result is NaN. If the first argument is positive zero and the second argument is greater than zero, or the first argument is positive infinity and the second argument is less than zero, then the result is positive zero. If the first argument is positive zero and the second argument is less than zero, or the first argument is positive infinity and the second argument is greater than zero, then the result is positive infinity. If the first argument is negative zero and the second argument is greater than zero but not a finite odd integer, or the first argument is negative infinity and the second argument is less than zero but not a finite odd integer, then the result is positive zero. If the first argument is negative zero and the second argument is a positive finite odd integer, or the first argument is negative infinity and the second argument is a negative finite odd integer, then the result is negative zero. If the first argument is negative zero and the second argument is less than zero but not a finite odd integer, or the first argument is negative infinity and the second argument is greater than zero but not a finite odd integer, then the result is positive infinity. If the first argument is negative zero and the second argument is a negative finite odd integer, or the first argument is negative infinity and the second argument is a positive finite odd integer, then the result is negative infinity. If the first argument is finite and less than zero if the second argument is a finite even integer, the result is equal to the result of raising the absolute value of the first argument to the power of the second argument if the second argument is a finite odd integer, the result is equal to the negative of the result of raising the absolute value of the first argument to the power of the second argument if the second argument is finite and not an integer, then the result is NaN. If both arguments are integers, then the result is exactly equal to the mathematical result of raising the first argument to the power of the second argument if that result can in fact be represented exactly as a double value. (In the foregoing descriptions, a floating-point value is considered to be an integer if and only if it is finite and a fixed point of the method ceil or, equivalently, a fixed point of the method floor. A value is a fixed point of a one-argument method if and only if the result of applying the method to the value is equal to the value.) A result must be within 1 ulp of the correctly rounded result. Results must be semi-monotonic.
   * @param x F - the base
   * @param y F - the exponent
   * @return F - the value a^b
   */
  static public F pow(F x, F y)
  {
    if(x.Equal(ZERO))
      return new F(ZERO);
    if(x.Equal(ONE))
      return new F(ONE);
    if(y.Equal(ZERO))
      return new F(ONE);
    if(y.Equal(ONE))
      return new F(x);
    //
    long l=y.toLong();
    boolean integerValue=y.Equal(new F(l));
    //
    if(integerValue)
    {
      boolean neg=false;
      if(y.Less(new F(0)))
        neg=true;
      //
      F result=new F(x);
      for(long i=1; i<(neg?-l:l); i++)
        result=result.Mul(x);
      //
      if(neg)
        return ONE.Div(result);
      else
        return result;
    }
    else
    {
      if(x.Great(ZERO))
        return exp(y.Mul(log(x)));
      else
        return new F(ERROR);
    }
  }
  /**
   * Returns the smallest (closest to negative infinity) double value that is not less than the argument and is equal to a mathematical integer. Special cases: If the argument value is already equal to a mathematical integer, then the result is the same as the argument. If the argument is NaN or an infinity or positive zero or negative zero, then the result is the same as the argument. If the argument value is less than zero but greater than -1.0, then the result is negative zero
   * @param x F - a value
   * @return F - the smallest (closest to negative infinity) floating-point value that is not less than the argument and is equal to a mathematical integer
   */
  static public F ceil(F x)
  {
    long tmpVal=x.m_Val;
    //
    if(x.m_E<0)
    {
      long coeff=1;
      //
      if(x.m_E>-19)
      {
        for(long i=0; i<-x.m_E; i++)
          coeff*=10;
        tmpVal/=coeff;
        tmpVal*=coeff;
        if(x.m_Val-tmpVal>0)
          tmpVal+=coeff;
      }
      else
      if(tmpVal>0)
        return ONE;
      else
        return ZERO;
    }
    //
    return new F(tmpVal, x.m_E);
  }
  /**
   * Returns the largest (closest to positive infinity) double value that is not greater than the argument and is equal to a mathematical integer. Special cases: If the argument value is already equal to a mathematical integer, then the result is the same as the argument. If the argument is NaN or an infinity or positive zero or negative zero, then the result is the same as the argument
   * @param x F - a value
   * @return F - the largest (closest to positive infinity) floating-point value that is not greater than the argument and is equal to a mathematical integer
   */
  static public F floor(F x)
  {
    long tmpVal=x.m_Val;
    //
    if(x.m_E<0)
    {
      long coeff=1;
      //
      if(x.m_E>-19)
      {
        for(long i=0; i<-x.m_E; i++)
          coeff*=10;
        tmpVal/=coeff;
        tmpVal*=coeff;
        if(x.m_Val-tmpVal<0)
          tmpVal-=coeff;
      }
      else
      if(tmpVal<0)
        return ONE.Neg();
      else
        return ZERO;
    }
    //
    return new F(tmpVal, x.m_E);
  }
  /**
   * Returns the absolute value of a F object. If the argument is not negative, the argument is returned. If the argument is negative, the negation of the argument is returned. Special cases: If the argument is positive zero or negative zero, the result is positive zero. If the argument is infinite, the result is positive infinity. If the argument is NaN, the result is NaN
   * @param x F - the argument whose absolute value is to be determined
   * @return F - the absolute value of the argument
   */
  static public F abs(F x)
  {
    if(x.m_Val<0)
      return x.Neg();
    return new F(x);
  }
  /**
   * Integer part of F object
   * @param x F - source F object
   * @return F - result F object
   */
  static public F Int(F x)
  {
    long tmpVal=x.m_Val;
    //
    if(x.m_E<0)
    {
      long coeff=1;
      //
      if(x.m_E>-19)
      {
        for(long i=0; i<-x.m_E; i++)
          coeff*=10;
        tmpVal/=coeff;
        tmpVal*=coeff;
      }
      else
        return F.ZERO;
    }
    //
    return new F(tmpVal, x.m_E);
  }
  /**
   * Fractional part of F object
   * @param x F - source F object
   * @return F - result F object
   */
  static public F Frac(F x)
  {
    return x.Sub(Int(x));
  }
  /**
   * Converts an angle measured in degrees to an approximately equivalent angle measured in radians. The conversion from degrees to radians is generally inexact
   * @param x F - an angle, in degrees
   * @return F - the measurement of the angle x in radians
   */
  static public F toRadians(F x)
  {
    return x.Mul(PI).Div(new F(180L));
  }
  /**
   * Converts an angle measured in radians to an approximately equivalent angle measured in degrees. The conversion from radians to degrees is generally inexact; users should not expect cos(toRadians(90.0)) to exactly equal 0.0
   * @param x F - an angle, in radians
   * @return F - the measurement of the angle angrad in degrees
   */
  static public F toDegrees(F x)
  {
    return x.Mul(new F(180L)).Div(PI);
  }
}
