/*
 * GameManager.cpp

 *
 *  Created on: Apr 26, 2016
 *      Author: luciano
 */

#include "GameManager.h"

#include <strings.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <thread>
#include "LogWriter.h"
#include "Avion.h"
#include "Escenario.h"
#include <mutex>
#include "Client.h"
#include "Object.h"
#include "Constants.h"
#include "MenuPresenter.h"
#include <sys/socket.h>
#include "MessageBuilder.h"
#include <chrono>
#include "DrawableObject.h"
#include <ctime>
#include <unistd.h>

#define kServerTestFile "serverTest.txt"

std::mutex mutexColaMensajes;
std::mutex writingInLogFileMutex;
map<unsigned int, thread> clientEntranceMessages;
map<unsigned int, thread> keepAliveThreads;
BulletList objects;
bool appShouldTerminate, gameInitiated, userWasConnected;
LogWriter *logWriter;
int numberOfCurrentAcceptedClients;
bool gameHasBeenReseted;
list<mensaje> drawableList; //lista que guarda todos los mensajes iniciales para levantar el juego :
							//datos de la ventana, escenario, aviones, elementos del escenario.


GameManager::GameManager() {
	this->appShouldTerminate = false;
	this->menuPresenter = NULL;
	this->parser = NULL;
	this->socketManager = NULL;
	this->xmlLoader = NULL;
	this->escenario = NULL;
}


int sendMsj(int socket, int bytesAEnviar, clientMsj* mensaje) {
	int enviados = 0;
	int res = 0;

	while (enviados < bytesAEnviar) {
		res = send(socket, &(mensaje)[enviados], bytesAEnviar - enviados,
		MSG_WAITALL);
		if (res == 0) { //Se cerró la conexion. Escribir en log de errores de conexion.
			return 0;
		} else if (res < 0) { //Error en el envio del mensaje. Escribir en el log.
			return -1;
		} else {
			enviados += res;
		}
	}

	return enviados;
}

int sendMsjInfo(int socket, int bytesAEnviar, mensaje* mensaje) {
	int enviados = 0;
	int res = 0;

	while (enviados < bytesAEnviar) {
		res = send(socket, &(mensaje)[enviados], bytesAEnviar - enviados,
		MSG_WAITALL);
		if (res == 0) { //Se cerró la conexion. Escribir en log de errores de conexion.
			return 0;
		} else if (res < 0) { //Error en el envio del mensaje. Escribir en el log.
			return -1;
		} else {
			enviados += res;
		}
	}
	return enviados;
}

int readMsj(int socket, int bytesARecibir, clientMsj* mensaje) {
	int recibidos = 0;
	int totalBytesRecibidos = 0;
	while (totalBytesRecibidos < bytesARecibir) {
		recibidos = recv(socket, &mensaje[totalBytesRecibidos],
				bytesARecibir - totalBytesRecibidos, MSG_WAITALL);
		if (recibidos < 0) {
			shutdown(socket, SHUT_RDWR);
			return -1;
		} else if (recibidos == 0) { //se corto la conexion desde el lado del servidor.
			shutdown(socket, SHUT_RDWR);
			return -1;
		} else {
			totalBytesRecibidos += recibidos;
		}
	}
	return recibidos;
}


void broadcast(mensaje msg, ClientList* clientList){
	std::list<Client*>::iterator it;
	for (it = clientList->clients.begin(); it != clientList->clients.end(); ++it) {
		if((*it)->getConnnectionState()){
			sendMsjInfo((*it)->getSocketMessages(), sizeof(msg), &msg);
		}
	}
}

void* broadcastMsj( ClientList *clientList, Procesador* processor, Escenario* escenario) {
	std::list<Client*>::iterator it;
	std::list<Object>::iterator objectIt;
	int contador = 0;
	while(!appShouldTerminate){
		usleep(1000);
		if (!gameHasBeenReseted) {
			objects.moveBullets();
			for(int i = 0; i < objects.bulletQuantity(); i++){
				mensaje msg;
				objects.bulletMessage(i, msg, processor->getScreenWidth(), processor->getScreenHeight());
				if(strcmp(msg.action, "Bullet deleted") != 0)
					broadcast(msg, clientList);
			}
		}

		if(gameInitiated){
			if (!gameHasBeenReseted) {
				usleep(1000);
				escenario->update();
				mensaje msj = MessageBuilder().createBackgroundUpdateMessage(escenario);
				broadcast(msj, clientList);
				for (int i = 0; i < escenario->getNumberElements(); i++){
					DrawableObject* element = escenario->getElement(i);
					msj = MessageBuilder().createBackgroundElementUpdateMessageForElement(element);
					broadcast(msj, clientList);
				}
			}
		}

		contador++;
		if (contador == 30){
			contador = 0;
			if (!gameHasBeenReseted) {
				for(it = clientList->clients.begin(); it != clientList->clients.end(); it++){
					if((*it)->plane->updatePhotogram()){
						mensaje photogramMsg = MessageBuilder().createUpdatePhotogramMessageForPlane((*it)->plane);
						broadcast(photogramMsg, clientList);
					}
				}
			}
		}
	}
	pthread_exit(NULL);
}


void sendGameInfo(ClientList* clientList){
	list<mensaje>::iterator it;
	for (it = drawableList.begin(); it != drawableList.end(); it++){
		broadcast((*it), clientList);
	}
}

void disconnectClientForSocketConnection(unsigned int socketConnection, ClientList *clientList) {
	Client* clientDisconnect = clientList->getClientForSocket(socketConnection);
	clientDisconnect -> setConnected(false);
	map<unsigned int, thread>::iterator threadItr;

	for(threadItr = clientEntranceMessages.begin(); threadItr != clientEntranceMessages.end(); ++threadItr){
		if(threadItr->first == socketConnection)
			threadItr->second.detach();
	}

	for(threadItr = keepAliveThreads.begin(); threadItr != keepAliveThreads.end(); ++threadItr){
		if(threadItr->first == socketConnection)
			threadItr->second.detach();
	}
}

void *clientReader(int socketConnection, ClientList *clientList, Procesador *procesor, Escenario* escenario) {
	int res = 0;
	bool clientHasDisconnected = false;
	while (!appShouldTerminate && !clientHasDisconnected) {
		clientMsj message;
		res = readMsj(socketConnection, sizeof(message), &message);
		if (res < 0) {

			shutdown(socketConnection, SHUT_RDWR);
			clientHasDisconnected = true;
			disconnectClientForSocketConnection(socketConnection, clientList);
			Client *client = clientList->getClientForSocket(socketConnection);
			mensaje disconnection = MessageBuilder().createDisconnectionMessageForClient(client);
			broadcast(disconnection, clientList);
		} else {
			procesor->processMessage(message);
		}
	}
	pthread_exit(NULL);
}

void* waitForClientConnection(int maxNumberOfClients, int socketHandle, XmlParser *parser, ClientList *clientList, Procesador *procesor, Escenario* escenario) {
	while (!appShouldTerminate) {
		logWriter->writeWaitingForClientConnection();
		int socketConnection = accept(socketHandle, NULL, NULL);
		logWriter->writeClientConnectionReceived();

		struct timeval timeOut;
		timeOut.tv_sec = 100;
		timeOut.tv_usec = 0;
		setsockopt(socketHandle, SOL_SOCKET, SO_RCVTIMEO, &timeOut, sizeof(struct timeval));

		clientMsj message;
		mensaje escenarioMsj;
		//Leo el mensaje de conexion
		readMsj(socketConnection, sizeof(message), &message);
		userWasConnected = false;
		if (!clientList->checkIfUserNameIsAlreadyInUse(message.value)) {

			if (numberOfCurrentAcceptedClients >= maxNumberOfClients) {
				message = MessageBuilder().createServerFullMessage();
			} else {
				numberOfCurrentAcceptedClients++;
				//Creo el cliente con el nombre del mensaje y lo agrego a la lista
				string name(message.value);

				Avion *clientPlane = parser->getAvion(numberOfCurrentAcceptedClients);
				Client* client = new Client(name, socketConnection, 0, clientPlane);

				clientList->addClient(client);

				message = MessageBuilder().createSuccessfullyConnectedMessage();

				clientEntranceMessages[socketConnection] = std::thread(
						clientReader, socketConnection, clientList, procesor, escenario);
				mensaje mensajeAvion = MessageBuilder().createInitialMessageForClient(client);
				drawableList.push_back(mensajeAvion);
			}
		} else {
			Client* client = clientList->getClientForName(message.value);
			if (client->getConnnectionState()) {
				logWriter->writeUserNameAlreadyInUse(message.value);
				message = MessageBuilder().createUserNameAlreadyInUseMessage();
			} else {
				mensaje reconnection = MessageBuilder().createReconnectionMessageForClient(client);
				broadcast(reconnection, clientList);

				logWriter->writeResumeGameForUserName(message.value);
				client->setSocketMessages(socketConnection);
				client->setConnected(true);

				message = MessageBuilder().createSuccessfullyConnectedMessage();
				escenarioMsj = MessageBuilder().createInitBackgroundMessage(escenario);
				clientEntranceMessages[socketConnection] = std::thread(clientReader, socketConnection, clientList, procesor, escenario);
				userWasConnected = true;
			}
		}

		sendMsj(socketConnection, sizeof(message), &(message));
		//Se envia la info para levantar el juego recien cuando todos se conectaron.
		if(numberOfCurrentAcceptedClients == maxNumberOfClients && !gameInitiated){
			sendGameInfo(clientList);
			gameInitiated = true;
		}
		if(userWasConnected){
			mensaje ventanaMsj;
			ventanaMsj.height = parser->getAltoVentana();
			ventanaMsj.width = parser->getAnchoVentana();
			sendMsjInfo(socketConnection, sizeof(mensaje), &ventanaMsj);
			sendMsjInfo(socketConnection, sizeof(mensaje), &escenarioMsj);
			std::list<Client*>::iterator it;
			for (it = clientList->clients.begin(); it != clientList->clients.end(); ++it) {
				mensaje mensajeObjeto = MessageBuilder().createInitialMessageForClient((*it));
				sendMsjInfo(socketConnection, sizeof(mensaje), &mensajeObjeto);
			}
			for(int i = 0; i < parser->getNumberOfElements(); i++){
				mensaje elementMsg;
				DrawableObject* element = parser->getElementAtIndex(i);
				elementMsg = MessageBuilder().createBackgroundElementCreationMessageForElement(element);
				sendMsjInfo(socketConnection, sizeof(mensaje), &elementMsg);
			}
		}
	}
	pthread_exit(NULL);
}

int GameManager::initGameWithArguments(int argc, char* argv[]) {
	gameHasBeenReseted = false;
	this->menuPresenter = new MenuPresenter(this->appShouldTerminate, this);
	this->clientList = new ClientList();

	const char* fileName;
	logWriter = new LogWriter;
	this->xmlLoader = new XMLLoader(logWriter);
	numberOfCurrentAcceptedClients = 0;
	if (argc != 2) {
		fileName = kServerTestFile;
		logWriter->writeUserDidnotEnterFileName();
	} else {
		fileName = argv[1];
		if (!this->xmlLoader->serverXMLIsValid(fileName)) {
			fileName = kServerTestFile;
		}
	}

	this->parser = new XmlParser(fileName);
	logWriter->setLogLevel(this->parser->getLogLevel());
	this->socketManager = new SocketManager(logWriter, this->parser);
	int success = this->socketManager->createSocketConnection();
	if (success == EXIT_FAILURE)
		return EXIT_FAILURE;

	this->setUpGame();

	std::thread broadcastThread(broadcastMsj,this->clientList, this->procesor, this->escenario);
	std::thread clientConnectionWaiter(waitForClientConnection,
			this->maxNumberOfClients, this->socketManager->socketHandle, this->parser, this->clientList, this->procesor, this->escenario);

	this->menuPresenter->presentMenu();

	clientConnectionWaiter.detach();
	broadcastThread.detach();
	return EXIT_SUCCESS;
}

void GameManager::setUpGame() {
	this->maxNumberOfClients = this->parser->getMaxNumberOfClients();
	int screenHeight = this->parser->getAltoVentana();
	int screenWidth = this->parser->getAnchoVentana();

	this->procesor = new Procesador(this->clientList, screenWidth, screenHeight, this);
	gameInitiated = false;

	mensaje ventanaMsj = MessageBuilder().createWindowInitMessage(screenHeight, screenWidth);
	drawableList.push_back(ventanaMsj);

	this->escenario = this->parser->getFondoEscenario();
	mensaje escenarioMsj = MessageBuilder().createInitBackgroundMessage(this->escenario);
	drawableList.push_back(escenarioMsj);

	for(int i = 0; i < this->parser->getNumberOfElements(); i++){
		DrawableObject* element = this->parser->getElementAtIndex(i);
		this->escenario->addElement(element);
		mensaje elementMsg = MessageBuilder().createBackgroundElementCreationMessageForElement(element);
		drawableList.push_back(elementMsg);
	}

	this->escenario->transformPositions();
	objects.setIdOfFirstBullet(this->maxNumberOfClients + this->escenario->getNumberElements());
}

void GameManager::userDidChooseExitoption() {
	logWriter->writeUserDidFinishTheApp();
	this->appShouldTerminate = true;
	appShouldTerminate = true;
}

void GameManager::broadcastMessage(mensaje message) {
	broadcast(message, clientList);
}

void GameManager::restartGame(mensaje message) {
	//escenario->restart();
	gameHasBeenReseted = !gameHasBeenReseted;
	broadcast(message, clientList);
	this->escenario->removeAllElements();
	objects.removeAllBullets();
	drawableList.clear();

	this->escenario = this->parser->getFondoEscenario();
	mensaje escenarioMsj = MessageBuilder().createInitBackgroundMessage(this->escenario);
	drawableList.push_back(escenarioMsj);
	for(int i = 0; i < this->parser->getNumberOfElements(); i++){
		DrawableObject* element = this->parser->getElementAtIndex(i);
		this->escenario->addElement(element);
		mensaje elementMsg = MessageBuilder().createBackgroundElementCreationMessageForElement(element);
		drawableList.push_back(elementMsg);
	}

	this->escenario->transformPositions();
	objects.setIdOfFirstBullet(this->maxNumberOfClients + this->escenario->getNumberElements());

	sendGameInfo(clientList);

	gameHasBeenReseted = false;
}

Object* GameManager::createBulletForClient(Client* client){
	Object bullet;
	bullet.setId(objects.getLastId() + 1);
	bullet.setPath("bullet.png");
	bullet.setPosX(client->plane->getPosX() + client->plane->getWidth()/2 - 15);
	bullet.setPosY(client->plane->getPosY() + 1);
	bullet.setWidth(30);
	bullet.setHeigth(30);
	bullet.setStatus(true);
	//La velocidad de disparo es relativa a la velocidad del avion.
	bullet.setStep(client->plane->getVelDisparo() + client->plane->getVelDesplazamiento());
	objects.addElement(bullet);
	return &bullet;
}

void GameManager::detachClientMessagesThreads() {
	//Creo un iterador para hacer detach en los threads abiertos antes de cerrar el server
	map<unsigned int, thread>::iterator threadItr;

	//Recorro ambos mapas y hago detach.
	for (threadItr = clientEntranceMessages.begin();
			threadItr != clientEntranceMessages.end(); ++threadItr) {
		threadItr->second.detach();
	}
	for (threadItr = keepAliveThreads.begin();
			threadItr != keepAliveThreads.end(); ++threadItr) {
		threadItr->second.detach();
	}
}

GameManager::~GameManager() {
	delete this->procesor;
	delete this->clientList;
	detachClientMessagesThreads();
	delete this->menuPresenter;
	delete this->parser;
	delete this->xmlLoader;
	delete this->socketManager;
	delete this->escenario;
	delete logWriter;
}

