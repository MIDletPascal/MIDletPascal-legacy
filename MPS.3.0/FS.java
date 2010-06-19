/*
 * This class is used when Forms support is needeed
 * */
import javax.microedition.lcdui.*;

public class FS
{
	/* add a textfiled into the form */
	public static int Lj(String label, String text, 
			int maxSize, int constraints)
	{
		try
		{
			return FW.F.append(new TextField(label, text, maxSize, constraints));
		}
		catch (Exception E)
		{
			return -1;
		}
	}

	/* add gauge */
	public static int Lj(String label, int interactive, 
			int maxValue, int initialValue)
	{
		try
		{
			boolean inter = true;
			if (interactive == 0)
				inter = false;
			// j-a-s-d: avoid AV false alarm
			return FW.F.append(new Gauge(label, inter, maxValue, initialValue));
		}
		catch (Exception E)
		{
			return -1;
		}
	}

	/* add choice group */
	public static int Lj(String label, int type)
	{
		try
		{
			ChoiceGroup cg = new ChoiceGroup(label, type);
			return FW.F.append(cg);
		}
		catch (Exception E)
		{
			return -1;
		}
	}

	/* get int value from gauge */
	public static int Lja(int itemNum)
	{
		try
		{
			Item it = FW.F.get(itemNum);
			Gauge g = (Gauge)it;
			return g.getValue();			
		}
		catch(Exception e)
		{
			return -1;
		}
	}

	/* set value to gauge */
	public static void Lja(int itemNum, int value)
	{
		try
		{
			Item it = FW.F.get(itemNum);
			Gauge g = (Gauge)it;
			g.setValue(value);
		}
		catch(Exception e)
		{
			return;
		}
	}

	/* get text from textfield */
	public static String ja(int itemNum)
	{
		try
		{
			Item it = FW.F.get(itemNum);
			TextField tf = (TextField)it;
			return tf.getString();			
		}
		catch(Exception e)
		{
			return "";
		}
	}

	/* set text of field */
	public static void ja(int itemNum, String text)
	{
		try
		{
			Item it = FW.F.get(itemNum);
			TextField tf = (TextField)it;
			tf.setString(text);
		}
		catch(Exception e)
		{
			return;
		}
	}

	/* choice group append item (string, null) */
	public static int I(int itemNum, String text)
	{
		try
		{
			ChoiceGroup cg = (ChoiceGroup)FW.F.get(itemNum);
			return cg.append(text, null);
		}
		catch(Exception e)
		{
			return -1;
		}
	}

	/* choice group append item (string, image) */
	public static int I(int itemNum, String text, Image image)
	{
		try
		{
			ChoiceGroup cg = (ChoiceGroup)FW.F.get(itemNum);
			return cg.append(text, image);
		}
		catch(Exception e)
		{
			return -1;
		}
	}

	/* choice is selected item */
	public static int L(int itemNum, int entryNum)
	{
		try
		{
			ChoiceGroup cg = (ChoiceGroup)FW.F.get(itemNum);
			if (cg.isSelected(entryNum))
					  return -1;
			else
					  return 0;
		}
		catch(Exception e)
		{
			return -1;
		}
	}
	
	/* choice get selected item */
	public static int L(int itemNum)
	{
		try
		{
			ChoiceGroup cg = (ChoiceGroup)FW.F.get(itemNum);
			return cg.getSelectedIndex();
		}
		catch(Exception e)
		{
			return -1;
		}
	}

	/* formAddDateField */
	public static int dd(String title, int type)
	{
		try
		{
			DateField df = new DateField(title, type);
			return FW.F.append(df);
		}
		catch (Exception e)
		{
			return -1;
		}
	}

	/* formSetDate */
	public static void dd(int index, int time)
	{
		try
		{
			DateField df = (DateField)FW.F.get(index);
			df.setDate(new java.util.Date(((long)time) * 1000));
		}
		catch (Exception e)
		{
			return;
		}
	}

	/* formGetDate*/
	public static int dd(int index)
	{
		try
		{
			DateField df = (DateField)FW.F.get(index);
			return (int) (df.getDate().getTime() / 1000);
		}
		catch (Exception e)
		{
			return -1;
		}
	}

	/* getFormTitle*/
	public static String gft()
	{
		try
		{
			String title = FW.F.getTitle();
			if (title == null)
				title = "";
			return title;
		}
		catch (Exception e)
		{
			return "";
		}
	}
};
