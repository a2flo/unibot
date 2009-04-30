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

import java.io.IOException;

/**
 *
 * @author flo
 */
public class Main {
	private static Logger log;
	private static Config config;
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

		config = new Config(log);

		connection = new Connector(log, config);
		connection.connect();

		bot_states = new BotStates(log, config);
		bot = new BotHandler(log, config, connection, bot_states);

		while(true) {
			if(bot_states.isQuit()) break;

			bot.handle();
		}

		log.print(LOG_TYPE.DEBUG, "ircbot.java", "> IRC-Bot Quit!");
	}

}
