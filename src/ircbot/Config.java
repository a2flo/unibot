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

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.prefs.BackingStoreException;
import java.util.prefs.InvalidPreferencesFormatException;
import java.util.prefs.Preferences;

/**
 *
 * @author flo
 */
public class Config {
	private Logger log;

	private static String hostname;
	private static int port;
	private static String bot_name;
	private static String bot_realname;
	private static String bot_password;
	private static String channel;
	private static String owner_name;
	
	public Config(Logger log) {
		this.log = log;

		Preferences prefs = Preferences.systemNodeForPackage(Config.class);
		try {
			prefs.clear();
		}
		catch(BackingStoreException ex) {}
		
		try {
			java.util.prefs.Preferences.importPreferences(new BufferedInputStream(new FileInputStream("/etc/unibot.conf")));
		}
		catch(IOException ex) { }
		catch(InvalidPreferencesFormatException ex) { }
		
		hostname = prefs.get("hostname", "irc.freenode.net");
		port = prefs.getInt("port", 6667);
		bot_name = prefs.get("bot_name", "[unibot]");
		bot_realname = prefs.get("bot_realname", "UniBot");
		bot_password = prefs.get("bot_password", "<PASSWORD>");
		channel = prefs.get("channel", "#unichannel");
		owner_name = prefs.get("owner_name", "[flo]");
	}

	public String getBotName() {
		return bot_name;
	}

	public String getBotPassword() {
		return bot_password;
	}

	public String getBotRealname() {
		return bot_realname;
	}

	public String getChannel() {
		return channel;
	}

	public String getHostname() {
		return hostname;
	}

	public String getOwnerName() {
		return owner_name;
	}

	public int getPort() {
		return port;
	}

}
