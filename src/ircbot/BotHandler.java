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
import java.io.*;
import java.text.SimpleDateFormat;
import java.util.*;
import java.net.*;

public class BotHandler {

	private Logger log;
	private Connector connection;
	private BotStates bot_states;
	private String data;
	private IRC_COMMAND current_cmd;
	private String servername = "";
	private long start_time;

	private enum IRC_COMMAND {

		NONE,
		ADMIN,
		AWAY,
		CONNECT,
		DIE,
		ERROR,
		INFO,
		NVITE,
		SON,
		JOIN,
		KICK,
		KILL,
		LINKS,
		LIST,
		LUSERS,
		MODE,
		MOTD,
		NAMES,
		NICK,
		NOTICE,
		OPER,
		PART,
		PASS,
		PING,
		PONG,
		PRIVMSG,
		QUIT,
		REHASH,
		RESTART,
		SERVICE,
		SERVLIST,
		SERVER,
		SQUERY,
		SQUIT,
		STATS,
		SUMMON,
		TIME,
		TOPIC,
		TRACE,
		USER,
		USERHOST,
		USERS,
		VERSION,
		WALLOPS,
		WHO,
		WHOIS,
		WHOWAS,
		CMD_001,
		CMD_002,
		CMD_003,
		CMD_004,
		CMD_005,
		CMD_372
	}
	private static String[] IRC_COMMAND_STR = {
		"NONE",
		"ADMIN",
		"AWAY",
		"CONNECT",
		"DIE",
		"ERROR",
		"INFO",
		"NVITE",
		"SON",
		"JOIN",
		"KICK",
		"KILL",
		"LINKS",
		"LIST",
		"LUSERS",
		"MODE",
		"MOTD",
		"NAMES",
		"NICK",
		"NOTICE",
		"OPER",
		"PART",
		"PASS",
		"PING",
		"PONG",
		"PRIVMSG",
		"QUIT",
		"REHASH",
		"RESTART",
		"SERVICE",
		"SERVLIST",
		"SERVER",
		"SQUERY",
		"SQUIT",
		"STATS",
		"SUMMON",
		"TIME",
		"TOPIC",
		"TRACE",
		"USER",
		"USERHOST",
		"USERS",
		"VERSION",
		"WALLOPS",
		"WHO",
		"WHOIS",
		"WHOWAS",
		"001",
		"002",
		"003",
		"004",
		"005",
		"372"
	};

	public BotHandler(Logger log, Connector connection, BotStates bot_states) {
		this.log = log;
		this.connection = connection;
		this.bot_states = bot_states;

		start_time = System.currentTimeMillis();
	}

	public void handle() throws IOException {
		// read data, if a null pointer is returned, quit the bot
		if((data = connection.readData()) == null) {
			bot_states.setQuit(true);
			return;
		}

		// handle the data
		current_cmd = parseIRCCmd(data);
		String[] cmd_tokens = data.split(" ");
		switch(current_cmd) {
			case NONE:
				// ignore
				break;
			case CMD_001:
				bot_states.setConnected(true);
				log.print(LOG_TYPE.DEBUG, "BotHandler.java", "successfully connected to the server!");
				connection.identify();
				break;
			case CMD_004:
				servername = cmd_tokens[3];
				log.print(LOG_TYPE.DEBUG, "BotHandler.java", "server name is: " + servername);
				break;
			case PING:
				connection.send("PONG " + servername);
				log.print(LOG_TYPE.DEBUG, "BotHandler.java", "Pong!");
				break;
			case JOIN:
				// TODO: this is currently executed each time a user joins the channel,
				// add a user check (if bot then setjoined(true) else "hi there, $user")
				log.print(LOG_TYPE.DEBUG, "BotHandler.java", "joined the channel");
				bot_states.setJoined(true);
				connection.sendChannelMsg("hi there ;)");
				break;
			case NOTICE:
				String notice = cmd_tokens[3].substring(1);
				for(int i = 4; i < cmd_tokens.length; i++) notice += " " + cmd_tokens[i]; // TODO: maybe there is a better java function?

				if(notice.contains("You are now identified for") && notice.contains(connection.getBotName())) {
					bot_states.setIdentified(true);
				}
				break;
			case PRIVMSG:
				// handle the message (also trim the leading ':')
				String msg = cmd_tokens[3].substring(1);
				for(int i = 4; i < cmd_tokens.length; i++) msg += " " + cmd_tokens[i]; // TODO: maybe there is a better java function?
				handleMessage(cmd_tokens[0], cmd_tokens[2], msg);
				break;
			case MODE:
				// TODO: check for +o and set op
				break;
			case TOPIC:
				break;
		}

		log.print(LOG_TYPE.MSG, ">>>", data);
	}

	private IRC_COMMAND parseIRCCmd(String cmd) {
		String[] cmd_tokens = cmd.split(" ");
		IRC_COMMAND irc_cmd = IRC_COMMAND.NONE;

		// check for normal irc command, some commands are in the first token ...
		try {
			irc_cmd = IRC_COMMAND.valueOf(cmd_tokens[0]);
		}
		catch(IllegalArgumentException e) {
			// do nothing, no normal irc command found
		}

		if(irc_cmd == IRC_COMMAND.NONE) {
			// ... others are in the second token
			try {
				irc_cmd = IRC_COMMAND.valueOf(cmd_tokens[1]);
			}
			catch(IllegalArgumentException e) {
				// do nothing, no normal irc command found
			}
		}

		if(irc_cmd == IRC_COMMAND.NONE) {
			// check for numbered irc command
			if(cmd_tokens[1].length() == 3 && cmd_tokens[1].matches("[0-9]{3,3}")) {
				try {
					irc_cmd = IRC_COMMAND.valueOf("CMD_" + cmd_tokens[1]);
				}
				catch(IllegalArgumentException e) {
					// do nothing, no numbered irc command found
				}
			}
		}

		return irc_cmd;
	}

	private void handleMessage(String sender, String location, String msg) throws IOException {
		// all bot commands have to start with '!'
		if(location.equals(connection.getChannel()) && msg.charAt(0) == '!') {
			msg = msg.substring(1);
			
			// official commands
			if(msg.equals("help")) {
				connection.sendChannelMsg("help:");
				connection.sendChannelMsg("\t !system: the bot's host system");
				connection.sendChannelMsg("\t !time: local bot time");
				connection.sendChannelMsg("\t !uptime: time since bot start");
				connection.sendChannelMsg("\t !who's your daddy?: that would be me!");
				connection.sendChannelMsg("\t !quit: quits the bot (op only)");
			}
			else if(msg.equals("who's your daddy?")) {
				// TODO: add support for non-unix (aka windows) platforms?
				try {
					InetAddress addr = InetAddress.getLocalHost();
					connection.sendChannelMsg(System.getenv("LOGNAME") + " @" + addr.toString());
				}
				catch(UnknownHostException e) {
					System.err.println(e);
					connection.sendChannelMsg(System.getenv("LOGNAME"));
				}
			}
			else if(msg.equals("system")) {
				try {
					String system = "";
					Process proc = Runtime.getRuntime().exec("uname -a");
					BufferedReader in_stream = new BufferedReader(new InputStreamReader(proc.getInputStream()));
					system = in_stream.readLine();

					connection.sendChannelMsg(system);
				}
				catch(IOException e) {
					System.err.println(e);
				}
			}
			else if(msg.equals("time")) {
				Calendar cal = Calendar.getInstance();
				SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss z dd.MM.yyyy");
				connection.sendChannelMsg(sdf.format(cal.getTime()));
			}
			else if(msg.equals("uptime")) {
				long uptime = System.currentTimeMillis() - start_time;
				String uptime_str = "";

				long t_day = 1000*60*60*24;
				long t_hour = 1000*60*60;
				long t_minute = 1000*60;
				long t_second = 1000;

				uptime_str += (uptime / t_day) + "d ";
				uptime %= t_day;
				uptime_str += (uptime / t_hour) + "h ";
				uptime %= t_hour;
				uptime_str += (uptime / t_minute) + "m ";
				uptime %= t_minute;
				uptime_str += (uptime / t_second) + "s";
				uptime %= t_second;

				connection.sendChannelMsg(uptime_str);
			}
			else if(msg.equals("quit")) {
				if(sender.substring(1, sender.indexOf("!n=")).equals(connection.getOwnerName())) {
					connection.sendQuit();
					bot_states.setQuit(true);
				}
				else {
					// TODO: check for bot_states.isOp()
					connection.sendKick(sender.substring(1, sender.indexOf("!n=")), "you're not " + connection.getOwnerName() + "!");
				}
			}
			// ... and the rest ;)
			else if(msg.equals("spec")) {
				connection.sendChannelMsg("http://www.ietf.org/rfc/rfc2812.txt");
			}
			else if((msg.contains("what is the answer to") || msg.contains("what's the answer to")) && msg.contains("?")) {
				connection.sendChannelMsg("42");
			}
		}
	}
}
