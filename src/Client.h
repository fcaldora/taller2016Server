/*
 * Client.h
 *
 *  Created on: Apr 24, 2016
 *      Author: facundo
 */

#ifndef CLIENT_H_
#define CLIENT_H_
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>

using namespace std;

class Client {
public:
	Client(string name, int socketM, int socketKA);
	virtual ~Client();

	string getName(){
		return name;
	}

	bool getConnnectionState(){
		return connected;
	}

	void setConnected(bool connected) {
		this->connected = connected;
	}

	void setName(const string& name) {
		this->name = name;
	}

	int getPosX() const {
		return posX;
	}

	void setPosX(int posX) {
		this->posX = posX;
	}

	int getPosY() const {
		return posY;
	}

	void setPosY(int posY) {
		this->posY = posY;
	}

	int getSocketKeepAlive() const {
		return socketKeepAlive;
	}

	void setSocketKeepAlive(int socketKeepAlive) {
		this->socketKeepAlive = socketKeepAlive;
	}

	int getSocketMessages() const {
		return socketMessages;
	}

	void setSocketMessages(int socketMessages) {
		this->socketMessages = socketMessages;
	}

private:
	string name;
	int posX;
	int posY;
	bool connected;
	int socketMessages;
	int socketKeepAlive;
};

#endif /* CLIENT_H_ */
