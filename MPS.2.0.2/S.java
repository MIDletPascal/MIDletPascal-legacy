import java.io.*;
import javax.microedition.lcdui.*;

public class S
{
  public static int parseInt(String s)
  {
    try
    {
      return Integer.parseInt(s);
    }
    catch (Exception e)
    {
      return 0;
    }
  };

  // getChar
  public static int gc(String s, int pos)
  {
  	try
	{
		return s.charAt(pos);
	}
	catch (Exception e)
	{
		return '\000';
	}
  }

  // setChar
  public static String gc(String s, int c, int pos)
  {
  	try
	{
		StringBuffer sb = new StringBuffer(s);
		sb.setCharAt(pos, (char)c);
		return sb.toString();
	}
	catch (Exception e)
	{
		return s;
	}
  }

  // openResource
  public static InputStream r(String resourceName)
  {
  	try
	{
		return FW.fw.getClass().getResourceAsStream(resourceName);
	} catch(Exception e)
	{
		return null;
	}
  }

  // closeResource
  public static void r(InputStream is)
  {
  	try
	{
		is.close();
	} catch (Exception e)
	{
		return;
	}
  }

  // readNextByte
  public static int rb(InputStream is)
  {
  	try
	{
		byte []b = new byte[2];
		is.read(b, 0, 1);

		return b[0];
	} catch (Exception e)
	{
		return 1000; //ERROR konstanta
	}
  }

  // readNextLine
  public static String nl(InputStream is)
  {
	StringBuffer retVal = new StringBuffer();
  	try
	{
		byte []b = new byte[2];
		boolean lineStarted = false;

		do
		{
			is.read(b, 0, 1);

			if ((b[0] != '\n') && (b[0] != '\r'))
			{
				lineStarted = true;
				retVal.append((char)b[0]);
			}
			else
			{
				if (lineStarted)
					break;
			}
		} while(true);
		
	}
	catch(Exception e)
	{
		return retVal.toString();
	}

	return retVal.toString();
  }

  /* imageFromImage */
  public static Image ii(Image img, int x, int y, int cx, int cy)
  {
  	try
	{
		Image newImage = Image.createImage(cx, cy);
		newImage.getGraphics().drawImage(img, -x, -y, Graphics.TOP|Graphics.LEFT);
		return newImage;
	}
	catch (Exception e)
	{
		return Image.createImage(1, 1);
	}
  }

  /* imageFromCanvas */
  public static Image ii(int x, int y, int cx, int cy)
  {
  	return ii(M.I, x, y, cx, cy);
  }

  /* imageFromBuffer*/
  public static Image ii(String buf, int length)
  {
  	try
	{
		return Image.createImage(buf.getBytes(), 0, length);
	}
	catch (Exception e)
	{
		return Image.createImage(1,1);
	}
  }
  
};
