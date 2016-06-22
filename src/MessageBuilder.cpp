/*
 * MessageBuilder.cpp
 *
 *  Created on: Apr 26, 2016
 *      Author: luciano
 */

#include "MessageBuilder.h"

MessageBuilder::MessageBuilder() {
}


clientMsj MessageBuilder::createSuccessfullyConnectedMessageForClient(Client *client, bool colaboration) {
	clientMsj message;
	string planeID = std::to_string(client->plane->getId());
	strncpy(message.id, planeID.c_str(), kLongChar);
	strncpy(message.type, "connection_ok", kLongChar);
	if(colaboration)
		strncpy(message.value, "colaboration", kLongChar);
	message.isFirstTimeLogin = true;

	return message;
}

clientMsj MessageBuilder::createSuccessfullyReconnectedMessageForClient(Client *client, bool colaboration) {
	clientMsj message = this->createSuccessfullyConnectedMessageForClient(client, colaboration);
	message.isFirstTimeLogin = false;

	return message;
}

clientMsj MessageBuilder::createServerFullMessage() {
	clientMsj message;
	strncpy(message.id, "0", 20);
	strncpy(message.type, "server_full", 20);
	strncpy(message.value, "Try again later", 20);
	return message;
}

clientMsj MessageBuilder::createUserNameAlreadyInUseMessage() {
	clientMsj message;
	strncpy(message.id, "0", 20);
	strncpy(message.type, "error", 20);
	strncpy(message.value, "Name already used", 20);
	return message;
}


mensaje MessageBuilder::createInitialMessageForClient(Client *client) {
	mensaje message;

	strcpy(message.action, "create");
	strcpy(message.imagePath, client->getPlane()->getPath().c_str());
	message.id = client->getPlane()->getId();
	message.photograms = client->getPlane()->getPhotograms();
	message.actualPhotogram = client->getPlane()->getActualPhotogram();
	message.height = client->getPlane()->getHeigth();
	message.width = client->getPlane()->getWidth();
	message.posX = client->getPlane()->getPosX();
	message.posY = client->getPlane()->getPosY();
	message.activeState = true;

	return message;
}

mensaje MessageBuilder::createInitialScoreMessage(Score* playerScore, int posX, int posY,clientMsj mess){
	mensaje message;

	strcpy(message.action, "createScore");
	message.id = playerScore->getId();
	message.posX = posX;
	message.posY = posY;
	strcpy(message.imagePath, mess.value);
	message.height = playerScore->getClientTeamId();

	return message;
}

mensaje MessageBuilder::createPlaneMovementMessageForClient(Client *client) {
	mensaje message;
	message.id = client->plane->getId();
	strncpy(message.action, "draw", kLongChar);
	message.posX = client->plane->getPosX();
	message.posY = client->plane->getPosY();
	message.actualPhotogram = client->plane->getActualPhotogram();
	return message;
}

mensaje MessageBuilder::createInitBackgroundMessage(Escenario *escenario){
	mensaje message;
	message.id = escenario->getId();
	strncpy(message.action,"create", kLongChar);
	strncpy(message.imagePath, escenario->getPath().c_str(), kLongChar);
	message.photograms = escenario->getPhotograms();
	message.actualPhotogram = escenario->getActualPhotogram();
	message.height = escenario->getHeigth();
	message.width = escenario->getWidth();
	message.posX = escenario->getPosX();
	message.posY = escenario->getPosY();
	message.activeState = true;
	return message;
}

mensaje MessageBuilder::createBulletMessage(Object* bullet){
	mensaje message;
	message.id = bullet->getId();
	strcpy(message.action,"create");
	strcpy(message.imagePath, bullet->getPath().c_str());
	message.height = bullet->getHeigth();
	message.width = bullet->getWidth();
	message.posX = bullet->getPosX();
	message.posY = bullet->getPosY();
	message.activeState = true;
	message.actualPhotogram = bullet->getActualPhotogram();
	return message;
}

mensaje MessageBuilder::createBulletSoundMessage() {
	mensaje message;
	strcpy(message.action,"bulletSound");
	return message;
}

mensaje MessageBuilder::createBackgroundUpdateMessage(Escenario* escenario){
	mensaje msg;
	strncpy(msg.action, "draw", 20);
	msg.id = escenario->getId();
	msg.posY = escenario->getPosY();
	msg.posX = escenario->getPosX();
	msg.actualPhotogram = escenario->getActualPhotogram();
	return msg;
}

mensaje MessageBuilder::createBackgroundElementUpdateMessageForElement(DrawableObject *element){
	mensaje msg = this->createBackgroundElementBaseMessageForElement(element);
	strncpy(msg.action, "draw", 20);

	return msg;
}

mensaje MessageBuilder::createBackgroundElementCreationMessageForElement(DrawableObject *element){
	mensaje msg;
	memset(&msg, 0, sizeof(mensaje));

	msg = this->createBackgroundElementBaseMessageForElement(element);
	strncpy(msg.action, "create", 20);

	return msg;
}

mensaje MessageBuilder::createBackgroundElementBaseMessageForElement(DrawableObject *element){
	mensaje msg;
	msg.id = element->getId();
	msg.posX = element->getPosX();
	msg.posY = element->getPosY();
	strncpy(msg.imagePath, element->getPath().c_str(), 20);
	msg.height = element->getHeigth();
	msg.width = element->getWidth();
	msg.actualPhotogram = element->getActualPhotogram();
	msg.activeState = true;

	return msg;
}

mensaje MessageBuilder::createReconnectionMessageForClient(Client* client) {
	mensaje reconnection;
	strcpy(reconnection.action, "path");
	reconnection.id = client->plane->getId();
	strcpy(reconnection.imagePath, client->plane->getPath().c_str());
	reconnection.width = client->plane->getWidth();
	reconnection.height = client->plane->getHeigth();

	return reconnection;
}

mensaje MessageBuilder::createDisconnectionMessageForClient(Client* client) {
	mensaje disconnection;

	strcpy(disconnection.action, "path");
	disconnection.id = client->plane->getId();
	strcpy(disconnection.imagePath, "disconnected.png");
	disconnection.height = 68;
	disconnection.width = 68;

	return disconnection;
}

mensaje MessageBuilder::createUpdatePhotogramMessageForPlane(Avion* plane) {
	mensaje photogramMsg;

	strncpy(photogramMsg.action,"draw", kLongChar);
	photogramMsg.actualPhotogram = plane->getActualPhotogram();
	photogramMsg.id = plane->getId();
	photogramMsg.posX = plane->getPosX();
	photogramMsg.posY = plane->getPosY();

	return photogramMsg;
}

mensaje MessageBuilder::createExplosionMessage(Explosion* explosion){
	mensaje message;
	message.id = explosion->getId();
	strcpy(message.action,"create");
	strcpy(message.imagePath, explosion->getPath().c_str());
	message.height = explosion->getHeigth();
	message.width = explosion->getWidth();
	message.posX = explosion->getPosX();
	message.posY = explosion->getPosY();
	message.activeState = true;
	message.actualPhotogram = explosion->getActualPhotogram();

	return message;
}

mensaje MessageBuilder::createExplosionSoundMessage(){
	mensaje message;
	strcpy(message.action,"explosionSound");
	return message;
}

mensaje MessageBuilder::createEnemyPlaneCreationMessage(EnemyPlane* enemyPlane){
	mensaje message;
	message.id = enemyPlane->getId();
	strcpy(message.action,"create");
	strcpy(message.imagePath, enemyPlane->getPath().c_str());
	message.height = enemyPlane->getHeigth();
	message.width = enemyPlane->getWidth();
	message.posX = enemyPlane->getPosX();
	message.posY = enemyPlane->getPosY();
	message.activeState = true;
	message.actualPhotogram = enemyPlane->getActualPhotogram();

	return message;
}

mensaje MessageBuilder::createEnemyBulletCreationMessage(EnemyPlane* enemyPlane, int id) {
	mensaje message;
	message.id = id;
	strcpy(message.action,"create");
	strcpy(message.imagePath, "enemybullet.png");
	message.height = 12;
	message.width = 12;
	message.posX = enemyPlane->getPosX() + enemyPlane->getWidth()/2;
	message.posY = enemyPlane->getPosY() + enemyPlane->getHeigth();
	message.activeState = true;
	message.actualPhotogram = 1;

	return message;
}

mensaje MessageBuilder::createLifeMessage(int id, int height, int width){
	mensaje message;
	message.id = id;
	strcpy(message.action,"create");
	strcpy(message.imagePath, "heart.png");
	message.height = 42;
	message.width = 39;
	if(id == 5001 || id == 10001 || id == 15001){
		message.posX = 0 + (id/5000)*50;
		message.posY = 25;
	}else if(id == 5002 || id == 10002 || id == 15002){
		message.posX = 170 + (id/5000)*50;
		message.posY = 25;
	}else if(id == 5003 || id == 10003 || id == 15003){
		message.posX = 340 + (id/5000)*50;
		message.posY = 25;
	}else if(id == 5004 || id == 10004 || id == 15004){
		message.posX = 510 + (id/5000)*50;
		message.posY = 25;
	}
	message.activeState = true;
	message.actualPhotogram = 1;

	return message;
}

menuResponseMessage MessageBuilder::createMenuMessage(vector<Team *> *teams) {
	menuResponseMessage message;

	message.id = 0;
	message.firstTeamIsAvailableToJoin = false;
	message.secondTeamIsAvailableToJoin = false;
	strncpy(message.firstTeamName , "", kLongChar);
	strncpy(message.secondTeamName , "", kLongChar);
	message.userCanCreateATeam = true;

	if (teams->size() >= 1) {
		message.firstTeamIsAvailableToJoin = !(*teams)[0]->isFull();
		strncpy(message.firstTeamName , (*teams)[0]->teamName.c_str(), kLongChar);
	}
	if (teams->size() == 2) {
		message.userCanCreateATeam = false;
		message.secondTeamIsAvailableToJoin = !(*teams)[1]->isFull();
		strncpy(message.secondTeamName , (*teams)[1]->teamName.c_str(), kLongChar);
	}
	return message;
}

StatsTypeMessage MessageBuilder::createStatsTypeMessageCollaborationType(bool isCollaborative) {
	StatsTypeMessage message;
	message.id = 0;
	if (isCollaborative) {
		strncpy(message.statType , "collaboration", kLongChar);
	} else {
		strncpy(message.statType , "teams", kLongChar);
	}
	return message;
}

CollaborationStatsMessage MessageBuilder::createCollaborationStatsMessage(ScoreManager *scoreManager, ClientList *clientList) {
	CollaborationStatsMessage message;
	message.id = 0;
	Score *bestScore = scoreManager->getBestScore();
	Client *clientWithBestScore;
	for (Client *client : clientList->clients) {
		if (bestScore->getClientSocket() == client->getSocketMessages()) {
			clientWithBestScore = client;
		}
	}

	strncpy(message.bestPlayerName , clientWithBestScore->getName().c_str(), kLongChar);
	message.bestPlayerScore = bestScore->getScore();

	return message;
}

TeamsStatsMessage MessageBuilder::createTeamsStatsMessage(ScoreManager *scoreManager, vector<Team *> *teams, ClientList* clients) {
	TeamsStatsMessage message;
	message.id = 0;
	Team *winnerTeam;
	Team *losserTeam;
	//Agrego el teamIsAlive para saber si el equipo 1, aparte de tener mas puntos, tiene aunque sea 1 jugador vivo.
	if (teams->at(0)->getPoints() <= teams->at(1)->getPoints() && clients->teamIsAlive(1)) {
		winnerTeam = teams->at(1);
		losserTeam = teams->at(0);
	} else {
		winnerTeam = teams->at(0);
		losserTeam = teams->at(1);
	}

	strncpy(message.winnerTeamName , winnerTeam->teamName.c_str(), kLongChar);
	message.winnerTeamScore = winnerTeam->getPoints();
	strncpy(message.firstPlayerNameOfWinnerTeam , winnerTeam->clients[0]->getName().c_str(), kLongChar);
	message.firstPlayerScoreOfWinnerTeam = scoreManager->getScoreForClientId(((Client *)winnerTeam->clients[0])->getSocketMessages())->getScore();
	if (winnerTeam->clients.size() >= 2) {
		strncpy(message.secondPlayerNameOfWinnerTeam , ((Client *)winnerTeam->clients[1])->getName().c_str(), kLongChar);
		message.secondPlayerScoreOfWinnerTeam = scoreManager->getScoreForClientId(((Client *)winnerTeam->clients[1])->getSocketMessages())->getScore();
		if (winnerTeam->clients.size() == 3) {
			strncpy(message.thirdPlayerNameOfWinnerTeam , ((Client *)winnerTeam->clients[2])->getName().c_str(), kLongChar);
			message.thirdPlayerScoreOfWinnerTeam = scoreManager->getScoreForClientId(((Client *)winnerTeam->clients[2])->getSocketMessages())->getScore();
		} else {
			strncpy(message.thirdPlayerNameOfWinnerTeam , kNoTeamPlaceholder, kLongChar);
		}
	} else {
		strncpy(message.secondPlayerNameOfWinnerTeam , kNoTeamPlaceholder, kLongChar);
		strncpy(message.thirdPlayerNameOfWinnerTeam , kNoTeamPlaceholder, kLongChar);
	}

	strncpy(message.losserTeamName , losserTeam->teamName.c_str(), kLongChar);
	message.losserTeamScore = losserTeam->getPoints();
	strncpy(message.firstPlayerNameOfLosserTeam , ((Client *)losserTeam->clients[0])->getName().c_str(), kLongChar);
	message.firstPlayerScoreOfLosserTeam = scoreManager->getScoreForClientId(((Client *)losserTeam->clients[0])->getSocketMessages())->getScore();
	if (losserTeam->clients.size() >= 2) {
		strncpy(message.secondPlayerNameOfLosserTeam , ((Client *)losserTeam->clients[1])->getName().c_str(), kLongChar);
		message.secondPlayerScoreOfLosserTeam = scoreManager->getScoreForClientId(((Client *)losserTeam->clients[1])->getSocketMessages())->getScore();
		if (losserTeam->clients.size() == 3) {
			strncpy(message.thirdPlayerNameOfLosserTeam , ((Client *)losserTeam->clients[2])->getName().c_str(), kLongChar);
			message.thirdPlayerScoreOfLosserTeam = scoreManager->getScoreForClientId(((Client *)losserTeam->clients[2])->getSocketMessages())->getScore();
		} else {
			strncpy(message.thirdPlayerNameOfLosserTeam , kNoTeamPlaceholder, kLongChar);
		}
	} else {
		strncpy(message.secondPlayerNameOfLosserTeam , kNoTeamPlaceholder, kLongChar);
		strncpy(message.thirdPlayerNameOfLosserTeam , kNoTeamPlaceholder, kLongChar);
	}

	return message;
}

MessageBuilder::~MessageBuilder() {
}

