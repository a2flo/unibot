/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package ircbot;

import ircbot.Logger.LOG_TYPE;

import java.util.*;
import java.net.*;
import java.io.*;

public class Connector {
	private Logger log;

	private Socket socket;
	private BufferedReader in_stream;
	private PrintStream out_stream;

	private final static String hostname = "irc.freenode.net";
	private final static int port = 6667;
	private final static String bot_name = "[flos_bot]";
	private final static String bot_realname = "Flo's Bot";
	private final static String bot_password = "<PASSWORD>";
	private final static String channel = "#unichannel";
	private final static String owner_name = "[flo]";

	public Connector(Logger log) {
		this.log = log;
	}

	public void connect() {
		try {
			log.print(LOG_TYPE.DEBUG, "Connector.java", "connecting to " + hostname + "on port #" + port);
			socket = new Socket(hostname, port);
			socket.setTcpNoDelay(true);
			out_stream = new PrintStream(socket.getOutputStream());
			in_stream = new BufferedReader(new InputStreamReader(socket.getInputStream()));

			send("NICK " + bot_name);
			send("USER " + bot_name + " 0 * " + bot_realname);
			send("JOIN " + channel);
		} catch (IOException e) {
			System.err.println(e);

			try {
				socket.close();
			} catch (IOException ee) {
				System.err.println(ee);
			}
		}
	}

	public void disconnect() {
		try {
			socket.close();
		} catch (IOException e) {
			System.err.println(e);
		}
	}

	public String readData() {
		try {
			return in_stream.readLine();
		} catch (IOException e) {
			System.err.println(e);
		}
		return null;
	}

	public void send(String cmd) {
		log.print(LOG_TYPE.DEBUG, "Connector.java", "sending: " + cmd);
		out_stream.println(cmd);
		out_stream.flush();
	}

	public void sendChannelMsg(String msg) {
		send("PRIVMSG " + channel + " :" + msg);
	}

	public void sendPrivMsg(String where, String msg) {
		send("PRIVMSG " + where + " :" + msg);
	}

	public void sendKick(String who, String reason) {
		send("KICK " + channel + " " + who + " :" + reason);
	}

	public void sendQuit() {
		send("PART :bye bye!");
		send("QUIT");
	}

	public String getHostname() {
		return hostname;
	}

	public String getChannel() {
		return channel;
	}

	public String getOwnerName() {
		return owner_name;
	}

	public void identify() {
		sendPrivMsg("NickServ", "identify " + bot_password);
	}

	public String getBotName() {
		return bot_name;
	}

}
