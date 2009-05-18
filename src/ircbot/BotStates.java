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

import java.lang.String;
import java.util.ArrayList;
import java.util.Collection;

/**
 *
 * @author flo
 */
public class BotStates {
	private Logger log;
	private Config config;

	// bot states:
	private boolean connected;
	private boolean joined;
	private boolean op;
	private boolean quit;
	private boolean identified;
	private ArrayList<String> userList;

	public BotStates(Logger log, Config config) {
		this.log = log;
		this.config = config;

		userList = new ArrayList<String>();

		connected = false;
		joined = false;
		op = false;
		quit = false;
		identified = false;
	}

	public boolean isConnected() {
		return connected;
	}

	public void setConnected(boolean connected) {
		this.connected = connected;
	}

	public boolean isJoined() {
		return joined;
	}

	public void setJoined(boolean joined) {
		this.joined = joined;
	}

	public boolean isOp() {
		return op;
	}

	public void setOp(boolean op) {
		this.op = op;
	}

	public boolean isQuit() {
		return quit;
	}

	public void setQuit(boolean quit) {
		this.quit = quit;
	}

	public boolean isIdentified() {
		return identified;
	}

	public void setIdentified(boolean identified) {
		this.identified = identified;
	}

	public void addUser(String name) {
		userList.add(name);
	}

	public void deleteUser(String name) {
		if(userList.contains(name)) {
			userList.remove(name);
		}
	}

	public void updateUser(String from, String to) {
		if(userList.contains(from)) {
			userList.set(userList.indexOf(from), to);
		}
	}

	public void deleteAllUsers() {
		userList.clear();
	}

	public String[] getUsers() {
		return (String[])userList.toArray(new String[0]);
	}

}
