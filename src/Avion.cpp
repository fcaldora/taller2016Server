/*
 * Avion.cpp
 *
 *  Created on: 24 de abr. de 2016
 *      Author: gusta
 */

#include "Avion.h"

Avion::Avion() :DrawableObject() {
	this->numberOfPhotograms = 1;
	this->actualPhotogramNumber = 1;
	velDisparo = 0;
	velDesplazamiento = 0;
}

void Avion::setVelDesplazamiento(int velDesplazamiento){
	this->velDesplazamiento = velDesplazamiento;
}

int Avion::getVelDesplazamiento(){
	return velDesplazamiento;
}

void Avion::setVelDisparo(int velDisparo){
	this->velDisparo = velDisparo;
}

int Avion::getVelDisparo(){
	return velDisparo;
}

void Avion::moveHorizontal(int movement){
	posX += movement;
}

void Avion::moveVerical(int movement){
	posY += movement;
}

void Avion::moveOneStepRight() {
	moveHorizontal(this->velDesplazamiento);
}

void Avion::moveOneStepLeft() {
	moveHorizontal(-this->velDesplazamiento);
}

void Avion::moveOneStepUp() {
	moveVerical(-this->velDesplazamiento);
}

void Avion::moveOneStepDown() {
	moveVerical(this->velDesplazamiento);
}

void Avion::incrementPhotogramNumber() {
	if (this->getActualPhotogramNumber() < this->getNumberOfPhotograms())
		this->actualPhotogramNumber++;
	else
		this->actualPhotogramNumber = 1;
}

Avion::~Avion() {
}

