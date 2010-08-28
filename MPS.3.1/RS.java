// this is the class to implement record store functionality
// for MIDletPascal
import javax.microedition.rms.*;

public class RS
{
	// open record store
	public static RecordStore j(String name) 
	{
		try
		{
			return RecordStore.openRecordStore(name, true);
		}
		catch(Exception e)
		{
			return null;
		}
	}

	
	// closes record store
	public static void L(RecordStore rs)
	{
 		try
		{
			rs.closeRecordStore();
		}
		catch(Exception e)
		{
		}
  	}

	// deletes a record store
	public static void L(String name)
	{
		try
		{
			RecordStore.deleteRecordStore(name);
		}
		catch(Exception e)
		{
		}
	}

	// deletes a record from a record store
	public static void L(RecordStore rs, int id)
	{
		try
		{
			rs.deleteRecord(id);
		}
		catch(Exception e)
		{
		}
	}

	// adds a record
	public static int L(RecordStore rs, String data)
	{
		try
		{
			return rs.addRecord(data.getBytes(), 0, data.length());
		}
		catch(Exception e)
		{
			return -1;
		}
	}

	// sets a data in the entry
	public static void L(RecordStore rs, String data, int id)
	{
	  try
		{
		  rs.setRecord(id, data.getBytes(), 0, data.getBytes().length);
		}
		catch (Exception e)
		{}
	}

	// returns the next record ID
	public static int Lja(RecordStore rs)
	{
		try
		{
			return rs.getNextRecordID();
		}
		catch (Exception e)
		{
			return -1;
		}
	}

	// read the data from the record store
	public static String j(RecordStore rs, int id)
	{
		try
		{
			byte[] res;
			res = rs.getRecord(id);

			if (res == null)
				return "";

			return new String(res);
		}
		catch(Exception e)
		{
			return "";
		}
	}

	// return the number of records in the record store
	public static int j(RecordStore rs)
	{
		try
		{
			return rs.getNumRecords();
		}
		catch(Exception e)
		{
			return 0;
		}
	}
};
