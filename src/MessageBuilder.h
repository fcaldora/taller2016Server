/*
 * MessageBuilder.h
 *
 *  Created on: Apr 26, 2016
 *      Author: luciano
 */

#ifndef MESSAGEBUILDER_H_
#define MESSAGEBUILDER_H_

#include "Constants.h"
#include "Client.h"
#include "Object.h"
#include "Escenario.h"
#include "Team.h"

class MessageBuilder {
public:
	MessageBuilder();
	virtual ~MessageBuilder();

	clientMsj createSuccessfullyConnectedMessageForClient(Client *client);
	clientMsj createSuccessfullyReconnectedMessageForClient(Client *client);
	clientMsj createServerFullMessage();
	clientMsj createUserNameAlreadyInUseMessage();
	mensaje createInitialMessageForClient(Client *client);
	mensaje createPlaneMovementMessageForClient(Client *client);
	mensaje createInitBackgroundMessage(Escenario *escenario);
	mensaje createBulletMessage(Object*);
	mensaje createBackgroundUpdateMessage(Escenario *escenario);
	mensaje createBackgroundElementUpdateMessageForElement(DrawableObject *element);
	mensaje createBackgroundElementCreationMessageForElement(DrawableObject *element);
	mensaje createWindowInitMessage(int screenHeight, int screenWidth);
	mensaje createResetGameMessage();
	mensaje createReconnectionMessageForClient(Client* client);
	mensaje createDisconnectionMessageForClient(Client* client);
	mensaje createUpdatePhotogramMessageForPlane(Avion* plane);
	menuResponseMessage createMenuMessage(vector<Team *> *teams);

private:
	mensaje createBackgroundElementBaseMessageForElement(DrawableObject *element);
};

#endif /* MESSAGEBUILDER_H_ */
