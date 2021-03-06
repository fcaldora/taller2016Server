/*
 * Avion.h
 *
 *  Created on: 24 de abr. de 2016
 *      Author: gusta
 */

#include <string>
#ifndef AVION_H_
#define AVION_H_

#include "DrawableObject.h"

using namespace std;

class Avion: public DrawableObject {
public:
	Avion();
	virtual ~Avion();

	void setVelDesplazamiento(int velDesplazamiento);
	int getVelDesplazamiento();
	void setVelDisparo(int velDisparo);
	int getVelDisparo();
	void moveVerical(int movement);
	void moveHorizontal(int movement);

	void moveOneStepRight();
	void moveOneStepLeft();
	void moveOneStepUp();
	void moveOneStepDown();

	void incrementPhotogramNumber();
private:
	int velDesplazamiento;
	int velDisparo;
};

#endif /* AVION_H_ */
