/* 
 * File:   I2C.h
 * Author: user
 *
 * Created on 12 novembre 2015, 22:32
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif

void InitI2C(); //Peut être mettre Fscl en paramètre??...
int envoiInformationI2C(unsigned int ID, unsigned int data);
int lectureInformationI2C(unsigned int ID, unsigned int data);
unsigned int get_flagCompteurDelayI2C();
void add_compteurDelayI2C();



#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

