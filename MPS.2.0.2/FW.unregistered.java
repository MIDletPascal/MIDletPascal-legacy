/*
 * This is the framework file for the MIDLetPascal generated MIDlets
 * The Pascal code is compiled into java class called M.
 * */
import javax.microedition.midlet.*;  
import javax.microedition.lcdui.*;
  
public class FW extends MIDlet implements CommandListener
{
	public boolean threadStarted = false;
	public M m = null;
	public static	FW fw = null;
	public static int MP; // MidletPaused
	public Display display;
	public Command cmd = null;

	// command & forms support
	public static Command LC = null;
	public static Displayable CD;
	public static Form F;
	public static TextBox TB;
	public static Alert A;
	public static List L;

	public void startApp()
	{
		MP = 0;
		
		display = Display.getDisplay(this);
		
		fw = this;

		if (m == null)
		{
			int z = 103 + "MIDlet".length();
			// create the continue command
			cmd = new Command("Run", Command.OK, 1);
			// create the new form
			String s1 = new String("MIDlet");
			String s2 = new String("un#%gist%2%d v%zsion of ");
			s2 = s2.toLowerCase();
			s2 = s2.replace('r', 'b');
			s2 = s2.replace('e', 'a');
			s2 = s2.replace('%', 'e');
			s2 = s2.replace('#', 'r');
			s2 = s2.replace('2', 'r');
			s2 = s2.replace('z', 'r');
			Form f = new Form(s1 + " information");

			String s4 = new String("");
			String as = new String("as");
			s4 = "MIDlet";
			s4 = s4 + "P" + as;
			f.append("This " + s1 + " w" + as + " created with " + s2 + s4 + "cal.");
			String s3 = new String("www.$i69!tp%scal.com");
			s3 = s3.toLowerCase();
			s3 = s3.replace('!', '@');
			s3 = s3.replace('$', 'm');
			s3 = s3.replace('@', 'e');
			s3 = s3.replace('%', 'a');
			s3 = s3.replace('6', 'd');
			s3 = s3.replace('9', 'l');
			s3 = s3.replace('n', '!');
			f.append(s3);
			f.addCommand(cmd);
			f.setCommandListener(this);

			TB = new TextBox("", "", 2, TextField.ANY);
			A = new Alert("", "", null, AlertType.INFO);
			L = new List("", List.IMPLICIT);

			display.setCurrent(f);
		}
		else
		{
			m.repaint();
			m.serviceRepaints();
		}	
   	 }

    public void commandAction(Command c, Displayable item) {
        if (c == cmd)
	{	
		m = new M();	
	
	    	M.T = m;
		M.I = Image.createImage(m.getWidth(), m.getHeight());
		M.G = M.I.getGraphics();
		M.KC = 0;
		display.setCurrent(m);
		CD = m;
		try
		{
		m.setCommandListener(this);
		}
		catch (Exception e)
		{
			// do nothing, this handles NokiaAPI exceptions
		}
		
		F = new Form("");
		F.setCommandListener(this);

		if (! threadStarted)
		{
			 new Thread(m).start();
			 threadStarted = true;
		}
	}
	else
	{
		LC = c;
	}
    }
     
    public void pauseApp()
    {      
         MP = -1;
    }      
      
    
    public void destroyApp(boolean unconditional)
    {      
            m = null;
			M.I = null;
			M.G = null;
			CD = null;
			F = null;
			TB = null;
			A = null;
			L = null;
			fw = null;
			LC = null; 
			notifyDestroyed();
    }
}     

