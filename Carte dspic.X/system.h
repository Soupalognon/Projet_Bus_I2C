/* 
 * File:   system.h
 * Author: Quentin
 *
 * Created on 23 octobre 2014, 12:00
 */

#ifndef SYSTEM_H
#define	SYSTEM_H

#ifdef	__cplusplus
extern "C" {
#endif

/******************************************************************************/
/******************************** INCLUDES ************************************/
/******************************************************************************/
    #include <libpic30.h>
    #include <p33FJ128MC804.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include <math.h>
    #include "Uart.h"
    #include "interruptions.h"
    #include "I2C.h"
#include "codeurs.h"
#include "asserv.h"
#include "fonctions_deplacements.h"
    #include "gestion_AX12.h"
    #include "autom.h"
 #include "pwm.h"
#include "Config_robots.h"

/******************************************************************************/
/***************************** Defines ****************************************/
/******************************************************************************/

#define CARTE_V2            //Carte Bleu 
//#define CARTE_V1          //Carte Verte 
    
    
    //Macro pour les tempos
#define FOSC            80000000LL     //80017142LL      //80000000
#define FCY             (FOSC/2)
#define delay_us(x) __delay32(((x*FCY)/1000000L))   // delays x us
#define delay_ms(x) __delay32(((x*FCY)/1000L))      // delays x ms


/******************************************************************************/
/********************************* Timers *************************************/
/******************************************************************************/

#define ACTIVE                  0b1
#define DESACTIVE               0b0

#define DIGITAL                 1
#define ANALOGIQUE              0

#define TIMER_5ms               T1CONbits.TON
#define TIMER_10ms              T4CONbits.TON
#define TIMER_90s               T2CONbits.TON
#define TIMER_DEBUG             T5CONbits.TON

#define FLAG_TIMER_5ms          IFS0bits.T1IF
#define FLAG_TIMER_10ms         IFS1bits.T4IF
#define FLAG_TIMER_90s          IFS0bits.T3IF
#define FLAG_TIMER_DEBUG        IFS1bits.T5IF

/******************************************************************************/
/********************************  PORT CARTE   *******************************/
/******************************************************************************/

#ifdef CARTE_V1
    #define SENS_MOTEUR_DROIT   PORTBbits.RB11
    #define SENS_MOTEUR_GAUCHE  PORTBbits.RB13

    #define XBEE_RESET          PORTAbits.RA7
    #define INHIBIT_AX12        PORTAbits.RA10
    #define DIR_UART_AX12       PORTBbits.RB7


    #define SYS_JACK            PORTBbits.RB4

    //Du JACK  vers les codeurs
    #define CAPTEUR1            PORTAbits.RA4
    #define CAPTEUR2            PORTAbits.RA9
    #define CAPTEUR3            PORTCbits.RC3
    #define CAPTEUR4            PORTCbits.RC4

    //Carte d'extenssion
    //De la gauche vers la droite (nappe en bas)
    #define CAPTEUR5            PORTAbits.RA8
    #define CAPTEUR6            PORTAbits.RA3
    #define CAPTEUR7            PORTAbits.RA2
    #define CAPTEUR8            PORTCbits.RC1
    #define CAPTEUR9            PORTCbits.RC2
    #define CAPTEUR10           PORTCbits.RC0
#endif
#ifdef CARTE_V2
    #define SENS_MOTEUR_DROIT   PORTBbits.RB12
    #define SENS_MOTEUR_GAUCHE  PORTBbits.RB10
    #define SENS_MOTEUR_X       PORTBbits.RB14
    #define SENS_MOTEUR_Y       PORTAbits.RA10
    #define ALIM_MOTEUR_Y       PORTAbits.RA7

    #define XBEE_RESET          PORTBbits.RB7
    #define INHIBIT_AX12        PORTAbits.RA0
    #define DIR_UART_AX12       PORTAbits.RA1

    #define SYS_JACK            PORTAbits.RA9
    #define SYS_COULEUR         PORTCbits.RC3
    #define SYS_STRAT           PORTCbits.RC4

    #define CAPTEUR1            PORTCbits.RC0
    #define CAPTEUR2            PORTCbits.RC1
    #define CAPTEUR3            PORTCbits.RC2
    #define CAPTEUR4            PORTAbits.RA2
    #define CAPTEUR5            PORTAbits.RA3 
    #define CAPTEUR6            PORTAbits.RA8   
    #define CAPTEUR7            PORTBbits.RB4   
    #define CAPTEUR8            PORTAbits.RA4


#endif

/******************************************************************************/
/******************************* Interruptions  *******************************/
/******************************************************************************/

#define PRIO_INTER_TIMER1               0   // Timer Assev
#define PRIO_INTER_TIMER2               0   // Timer 2 en mode 32 bits (couplé à T3 donc osef)
#define PRIO_INTER_TIMER3               0   // Timer Fin de match
#define PRIO_INTER_TIMER4               0   // Autom
#define PRIO_INTER_TIMER5               0   //Prio Timer debug
#define PRIO_INTER_UART1_RX             0   // Prio XBEE
#define PRIO_INTER_UART2_TX             0   // AX12
#define PRIO_INTER_UART2_RX             0   // AX12
#define PRIO_INTER_I2C_MAITRE           0   // I²C désactivé pour le moment
#define PRIO_INTER_I2C_ESCLAVE          0   // I²C désactivé pour le moment
#define PRIO_INTER_QEI1                 0   // Codeurs : prio la plus haute
#define PRIO_INTER_QEI2                 0   // Codeurs : prio la plus haute

#define ACTIV_INTER_TIMER1              0   // Timer asserv : 5 ms
#define ACTIV_INTER_TIMER2              0   // Osef : TIMER 2 et 3 sur 32 bits
#define ACTIV_INTER_TIMER3              0   // Timer 90 secondes : fin de match
#define ACTIV_INTER_TIMER4              0   // Timer Autom : 10 ms
#define ACTIV_INTER_TIMER5              0   // Timer debug : 200 ms
#define ACTIV_INTER_UART1_RX            0   // Uart XBEE
#define ACTIV_INTER_UART2_TX            0   // UART AX12 : Acquittement trame envoyée
#define ACTIV_INTER_UART2_RX            0   // UART AX12
#define ACTIV_INTER_I2C_MAITRE          0   // Pas implémenté pour le moment
#define ACTIV_INTER_I2C_ESCLAVE         0   // Pas implémenté pour le moment
#define ACTIV_INTER_QEI1                0   // Codeurs
#define ACTIV_INTER_QEI2                0   // Codeurs


/******************************************************************************/
/*************************** Variables Globales *******************************/
/******************************************************************************/

    


/******************************************************************************/
/****************************** Prototypes ************************************/
/******************************************************************************/

    void ConfigADC (void);

    /**
     * Fonction qui initialise tous les systèmes de la carte
     * Uart, Timer, PWM, mapping ....
     */
    void init_system (void);


    /**
     * Fonction qui rèfle l'horloge à 80Mhz
     */
    void init_clock(void);

    /**
     * Fonction qui init le Timer asserv
     */
    void config_timer_5ms();


    /**
     * Fonction qui init le Timer d'autom
     */
    void config_timer_10ms();


    /**
     * Fonction qui init le timer de fin de match
     */
    void config_timer_90s(void);


    /**
     * Fonction qui configure toutes les interruptions
     * Ainsi que les priorités
     */
    void ConfigInterrupt (void);


    /**
     * Fonction qui configure toures les broches entrées sorties
     */
    void ConfigPorts (void);


    /**
     * Configure le mappage des modules spéciaux : UART, QEI
     */
    void ConfigMapping (void);

    



#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_H */

