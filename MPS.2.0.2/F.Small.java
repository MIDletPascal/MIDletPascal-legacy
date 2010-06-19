
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
  /** Zero constant */
  final static public F ZERO=new F();
  /** One constant */
  final static public F ONE=new F(1L);
  
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
    a=new F(2L);
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

  // x=35 diff=1.46%
  // precise y=3.5
  // x=15 diff=0.06%
  // x=20 diff=0.40%
  // x=25 diff=1.31%
  // x=30 diff=2.95%
  // if x negative y must be integer value
    
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
}
