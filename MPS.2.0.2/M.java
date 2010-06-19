/*
 * M.java - the Java representation of an empty M class
 * THe M class is generated from the MIDletPascal source code.
 */

import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;
import java.lang.Object;
import java.lang.*;

public class M extends Canvas implements Runnable
{
	public static Image I;
	public static Graphics G;
	public static M T;
	public static int KC;
	
	public void paint(Graphics g)
	{
		g.drawImage(I, 0, 0, Graphics.TOP|Graphics.LEFT);	
	}

	public void run()
	{
		R();
	}

	public static void R()
	{
		int a[] = new int[5];
		String b = new String();
		while (true)
		{
			a[2] = 3;
			b = b + "Lala";
		}
	}

}
