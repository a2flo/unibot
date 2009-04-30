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
	private Config config;
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

	public BotHandler(Logger log, Config config, Connector connection, BotStates bot_states) {
		this.log = log;
		this.config = config;
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
		// cmd_tokens:
		// [0]: sender
		// [1]: command
		// [2]: location
		// [3+]: data
		current_cmd = parseIRCCmd(data);
		String[] cmd_tokens = data.split(" ");

		String cmd_sender = "", cmd_command = "", cmd_location = "", cmd_data = "", cmd_data2 = "", cmd_joined_data = "";
		if(cmd_tokens.length > 0) cmd_sender = cmd_tokens[0];
		if(cmd_tokens.length > 1) cmd_command = cmd_tokens[1];
		if(cmd_tokens.length > 2) cmd_location = cmd_tokens[2];
		if(cmd_tokens.length > 3) {
			cmd_data = cmd_tokens[3];
			cmd_joined_data = cmd_data;
		}
		if(cmd_tokens.length > 4) cmd_data2 = cmd_tokens[4];
		if(cmd_tokens.length > 4) {
			for(int i = 4; i < cmd_tokens.length; i++) {
				cmd_joined_data += " " + cmd_tokens[i]; // TODO: is there a better way in java to do this?
			}
		}

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
				servername = cmd_data;
				log.print(LOG_TYPE.DEBUG, "BotHandler.java", "server name is: " + servername);
				break;
			case PING:
				connection.send("PONG " + servername);
				log.print(LOG_TYPE.DEBUG, "BotHandler.java", "PONG!");
				break;
			case JOIN:
				// if the bot joined the channel, set the flag and send a "hi there ;)" message
				if(stripUser(cmd_sender).equals(config.getBotName())) {
					log.print(LOG_TYPE.DEBUG, "BotHandler.java", "joined the channel");
					bot_states.setJoined(true);
					connection.sendChannelMsg("hi there ;)");
				}
				// if another user joined the channel, greet him
				else {
					connection.sendChannelMsg("hi there, " + stripUser(cmd_sender));
				}
				break;
			case NOTICE:
				if(cmd_joined_data.contains("You are now identified for") && cmd_joined_data.contains(config.getBotName())) {
					bot_states.setIdentified(true);
				}
				break;
			case PRIVMSG:
				// handle the message (also trim the leading ':')
				handleMessage(cmd_sender, cmd_location, cmd_joined_data.substring(1));
				break;
			case MODE:
				// only handle mode stuff in the bots host channel
				if(cmd_location.equals(config.getChannel())) {
					// mode for the bot was set
					if(cmd_data2.equals(config.getBotName())) {
						// bot was given op
						if(cmd_data.equals("+o")) {
							bot_states.setOp(true);
						}
						// op was taken from bot
						if(cmd_data.equals("-o")) {
							bot_states.setOp(false);
						}
					}
				}
				break;
			case KICK:
				// check if bot was kicked
				if(cmd_data.equals(config.getBotName())) {
					// check if the user who kicked the bot is the owner
					if(!cmd_data2.substring(1).equals(config.getOwnerName())) {
						// if not, rejoin the channel and kick the user who kicked the bot
						connection.joinChannel();
						
						// random kick message
						String[] kick_messages = { "try me!", "booyah!", "bot pwnage!", "don't mess with me!" };
						try {
							// wait for 3 seconds before kicking the user
							Thread.sleep(3000);
						}
						catch(InterruptedException e) { /* nothing */ }
						connection.sendKick(cmd_data2.substring(1), kick_messages[(int)(Math.random() * kick_messages.length)]);
					}
				}
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
		if(location.equals(config.getChannel()) && msg.charAt(0) == '!') {
			msg = msg.substring(1);
			
			// official commands
			if(msg.equals("help")) {
				connection.sendChannelMsg("help:");
				connection.sendChannelMsg("    !src: link to unibot source code");
				connection.sendChannelMsg("    !system: the bot's host system");
				connection.sendChannelMsg("    !time: local bot time");
				connection.sendChannelMsg("    !uptime: time since bot start");
				connection.sendChannelMsg("    !who's your daddy?: that would be me!");
				connection.sendChannelMsg("    !quit: quits the bot (op only)");
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
				if(stripUser(sender).equals(config.getOwnerName())) {
					connection.sendQuit();
					bot_states.setQuit(true);
				}
				else {
					if(bot_states.isOp()) {
						connection.sendKick(stripUser(sender), "you're not " + config.getOwnerName() + "!");
					}
				}
			}
			else if(msg.equals("src")) {
				connection.sendChannelMsg("git : git clone git://git.assembla.com/unibot.git");
				connection.sendChannelMsg("trac: http://trac-git.assembla.com/unibot");
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

	private String stripUser(String str) {
		return str.substring(1, str.indexOf("!n="));
	}
}
