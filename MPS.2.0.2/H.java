// The HTTP class for the MIDletPascal
import javax.microedition.io.*;
import java.io.*;

public class H
{
	public HttpConnection c = null;
	public OutputStream o = null;
	public InputStream i = null;

	// open a http connection
	public int L(String url)
	{
		try
		{
			c = (HttpConnection)Connector.open(url);
			o = c.openOutputStream();
			return -1;
		}
		catch(Exception e)
		{
			c = null;
			return 0;
		}
	}

	// return true if the connection is open
	public int L()
	{
		if (c != null)
			return -1;
		return 0;
	}

	// close the connection
	public void c()
	{
		try
		{
			if (i != null)
			{
				i.close();
				i = null;
			}
			if (o != null)
			{
				o.close();
				o = null;
			}
			c.close();
			c = null;
		}
		catch(Exception e)
		{
			c = null;
		}
	}

	// add http header
	public void L(String header, String value)
	{
		try
		{
			c.setRequestProperty(header, value);
		}
		catch(Exception e)
		{}
	}

	// setRequestMethod :
	// HEAD, POST, GET bi trebalo rijesiti kao string konstante
	public void c(String method)
	{
		try
		{
			c.setRequestMethod(method);
		}
		catch(Exception e)
		{}
	}

	// get header field
	public String i(String header)
	{
		try
		{
			String value;
			value = c.getHeaderField(header);

			if (value == null)
			  value = "";

			return value;
		}
		catch(Exception e)
		{
			return "";
		}
	}

	// set message text
	public int o(String message)
	{
		try
		{
			o.write(message.getBytes());

			return -1;
		}
		catch(Exception e)
		{
			o = null;
			return 0;
		}
	}

	// getresponse code
	// ovaj takodjer otvara konekciju i ceka dok se ne odgovori
	public int o()
	{
		try
		{
			int code;
			code = c.getResponseCode();
			i = c.openInputStream();
			return code;
		}
		catch(Exception e)
		{
			return -1;
		}
	}

	// read the input data from the connection
	public String j()
	{
		try
		{
			StringBuffer retval = new StringBuffer();
			int c;

			while ((c=i.read()) != -1)
			{
				retval.append((char)(char)c);
			}
			
			return retval.toString();
		}
		catch(Exception e)
		{
			return "";
		}
	}

};
