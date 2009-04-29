/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package ircbot;

/**
 *
 * @author flo
 */
public class BotStates {
	private Logger log;

	// bot states:
	private boolean connected;
	private boolean joined;
	private boolean op;
	private boolean quit;
	private boolean identified;

	public BotStates(Logger log) {
		this.log = log;

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

}
