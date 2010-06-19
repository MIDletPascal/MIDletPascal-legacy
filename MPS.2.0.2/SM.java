/*
 * class used for sending SMS
 * */

import javax.microedition.io.*;
import javax.wireless.messaging.*;

public class SM implements Runnable
{
	public static boolean isSending = false;
	public static int success = 0;

	private String destination;
	private String message;

	public static int send(String destination,
			String message)
	{
		if (isSending)
			return 0;

		new SM(destination, message);

		return -1;
	}
	
	/*
	 * Send SMS message to the specified destination. Return true 
	 * if succedded, false otherwise.
	 *
	 * Destination is in format: sms://+number
	 * */
	public SM(String destination,
			String message)
	{		
		success = 0;
		isSending = true;
		this.destination = destination + ":276";
		this.message = message;

		try{
		Thread t = new Thread(this);
		t.start();
		} catch (Exception e)
		{
			isSending = false;
		}
	}

	public void run()
	{           
        	try {       
			// try sending using wireless api
        		MessageConnection smsconn = (MessageConnection)Connector.open(destination);
	            	TextMessage txtmessage = (TextMessage)smsconn.newMessage(
        	        	MessageConnection.TEXT_MESSAGE);
            		txtmessage.setAddress(destination);
	            	txtmessage.setPayloadText(message);
        	    	smsconn.send(txtmessage);
			smsconn.close();
			isSending = false;
			success = -1;
			return;
        	} catch (Throwable t1) {
			// try sending using old siemensAPI, will not work on SL45i
			try
			{
				DatagramConnection smsconn = (DatagramConnection)Connector.open(destination);
	            		Datagram dgram = smsconn.newDatagram(message.getBytes(), message.getBytes().length, destination);
        		    	smsconn.send(dgram);
				smsconn.close();
				isSending = false;
				success = -1;
			} catch (Throwable t2)
			{
				isSending = false;
				success = 0;
				return;
			}

	        }        	
	
		// never come here
		isSending = false;
		success = -1;
	}

	/*
	 * isSendingSMS
	 * */
	public static int IS()
	{
		if (isSending)
			return -1;
		else
			return 0;
	}

	/*
	 * get success, returns true if message sending has succedded
	 * */
	public static int GS()
	{
		return success;
	}
}
