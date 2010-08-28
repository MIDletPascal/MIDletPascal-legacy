/*
 * The player class used for playing the music.
 * */

import javax.microedition.media.*;

public class P
{
		public static Player p = null;

		/* load music player */
		public static int a(String resource, String type)
		{
			try
			{
   		  if (p != null)
			  {
			  	p.close();
			  }
				p = Manager.createPlayer(FW.fw.getClass().getResourceAsStream(resource), type);
				p.realize();
				p.prefetch();
			}
			catch (Exception e)
			{
				return 0;
			}

			return -1;
		}

		/* play */
		public static int a()
		{
			try
			{
				if (p == null)
					return 0;

				p.start();
			}
			catch (Exception e)
			{
				return 0;
			}

			return -1;
		}

		/* stops playing the music */
		public static void b()
		{
			try
			{
				if (p != null)
				{
					p.stop();
				}
			}
			catch (Exception e)
			{}
		}

		/* set loop count, -1 indefinitely */
		public static int a(int x)
		{
			try
			{
				if (p != null)
				{
					if (p.getState() == Player.STARTED)
							  return 0;
					p.setLoopCount(x);
					return -1;
				}
				
			} catch (Exception e)
			{
				return 0;
			}
			return 0;
		}

		/* get duration time in milliseconds */
		public static int c()
		{
			try
			{
				if (p != null)
				{
					return (int)(p.getDuration() / 1000);
				}
				else return -1;
			}
			catch (Exception e)
			{
				return -1;
			}
		}
		
}
