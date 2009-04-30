/*
 *  UniBot
 *  Copyright (C) 2009 Florian Ziesche
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

package ircbot;

import ircbot.Logger.LOG_TYPE;

import java.net.*;
import java.io.*;

public class Connector {
	private Logger log;
	private Config config;

	private Socket socket;
	private BufferedReader in_stream;
	private PrintStream out_stream;

	public Connector(Logger log, Config config) {
		this.log = log;
		this.config = config;
	}

	public void connect() {
		try {
			log.print(LOG_TYPE.DEBUG, "Connector.java", "connecting to " + config.getHostname() + "on port #" + config.getPort());
			socket = new Socket(config.getHostname(), config.getPort());
			socket.setTcpNoDelay(true);
			out_stream = new PrintStream(socket.getOutputStream());
			in_stream = new BufferedReader(new InputStreamReader(socket.getInputStream()));

			send("NICK " + config.getBotName());
			send("USER " + config.getBotName() + " 0 * " + config.getBotRealname());
			joinChannel();
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
		send("PRIVMSG " + config.getChannel() + " :" + msg);
	}

	public void sendPrivMsg(String where, String msg) {
		send("PRIVMSG " + where + " :" + msg);
	}

	public void sendKick(String who, String reason) {
		send("KICK " + config.getChannel() + " " + who + " :" + reason);
	}

	public void sendQuit() {
		send("PART :bye bye!");
		send("QUIT");
	}

	public void identify() {
		sendPrivMsg("NickServ", "identify " + config.getBotPassword());
	}

	public void joinChannel() {
		send("JOIN " + config.getChannel());
	}

}
