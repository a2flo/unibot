/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package ircbot;

/**
 * @author flo
 *
 */
public class Logger {
	public Logger() {
	}

	public enum LOG_TYPE {
		NONE,
		MSG,
		DEBUG,
		ERROR
	}

	public void print(LOG_TYPE type, String file, String msg) {
		String line = "";
		switch(type) {
			case MSG:
				line += "MSG: ";
				break;
			case DEBUG:
				line += "DEBUG: ";
				break;
			case ERROR:
				line += "ERROR: ";
				break;
		}

		line += file;
		line += ": ";
		line += msg;

		System.out.println(line);

		// TODO: log to file
	}
}
