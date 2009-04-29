/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package ircbot;

import ircbot.Logger.LOG_TYPE;

import java.io.IOException;
import java.util.*;
import java.net.*;

/**
 *
 * @author flo
 */
public class Main {
	private static Logger log;
	private static Connector connection;
	private static BotHandler bot;
	private static BotStates bot_states;

    /**
     * @param args the command line arguments
     */
	public static void main(String[] args) throws IOException {
        System.setProperty("file.encoding", "UTF-8");
		log = new Logger();
		log.print(LOG_TYPE.DEBUG, "ircbot.java", "> IRC-Bot Start!");

		connection = new Connector(log);
		connection.connect();

		bot_states = new BotStates(log);
		bot = new BotHandler(log, connection, bot_states);

		while(true) {
			if(bot_states.isQuit()) break;

			bot.handle();
		}

		log.print(LOG_TYPE.DEBUG, "ircbot.java", "> IRC-Bot Quit!");
	}

}
