#include "system.h"

/*
 * config_timer_debug()
 * ERREUR! UTILISATION du TIMER 4 au lieu du TIMER 5
 */
#define TIMER_I2C T5CONbits.TON     //Dans system.h
#define FLAG_TIMER_I2C IFS1bits.T5IF    //Dans system.h
#define ERREUR_I2C_TIMEOUT -1       //Correspond � l'erreur timeout g�rer dans le code 
//IPC1bits.T2IP = 0;  //Dans ConfigInterrupt (system.c)


void config_timer_reception_I2C(void)   //Dans system.c     REMPLACER LA FONCTION config_timer_debug())
{
    //TIMER 5
    TIMER_I2C = DESACTIVE;
    T5CONbits.TCS = 0;          //clock sur FCY
    T5CONbits.TGATE = 0;        //Mode Gate d�sactiv�
    T5CONbits.TCKPS = 0b11;     //Divise la fr�quence par 256 (prescaler 1:256)
    TMR5 = 0x00;                //Reset du timer

    PR5 = 31256;                 //d�compte de 200ms

    //IPC6bits.T4IP = 0x03;       //priorit� � 3 --> d�finit dans la table des interrupt
    FLAG_TIMER_I2C = 0;        //Clear flag interrupt timer
    IEC1bits.T5IE = 1;          //Enable Timer5 interrupt
}

//Permet de lancer un timer de 5ms qui g�re le timeout de la r�ception I2C
unsigned int compteurDelayI2C = 0;  
unsigned int flagCompteurDelayI2C = 0;   

unsigned int get_flagCompteurDelayI2C()
{
    return flagCompteurDelayI2C;
}

void add_compteurDelayI2C()
{
    compteurDelayI2C++;
}

/*
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void) //A rajouter dans Timer1 !!
{
    FLAG_TIMER_5ms = 0;
    
    if(get_flagCompteurDelayI2C())
        add_compteurDelayI2C();
}


void __attribute__((__interrupt__, no_auto_psv)) _T5Interrupt(void) //Remplacer La fonction d�j� l�!!!
{
    FLAG_TIMER_I2C = 0;
    
    //Impl�menter un syst�me de r�cup�ration des donn�es capteurs
    
    //lectureInformationI2C(ID, data);
}
*/

void InitI2C(unsigned long Fscl, unsigned int delay) //Peut �tre mettre Fscl en param�tre??...
{   
    I2C1BRG = ((1/Fscl - delay) * FCY) - 2;
    //I2C1BRG = 392;    //Pour Fscl = 100Khz
    //I2C1BRG = 92; //Pour Fscl = 400KHz
    
    I2C1CONbits.ACKDT = 0;  //Send ACK during Acknowledge 
    I2C1CONbits.I2CEN = 1;
}

int envoiInformationI2C(unsigned int ID, unsigned int data) //En tant que MAITRE
{       
    //ATTENTION!! Le dernier bit de l'ID doit �tre 0 pour demander l'�criture
    
    //if(I2C1STATbits.P  == 1)   //Pour checker si le bus est bien en idle mode
    {
        I2C1CONbits.SEN = 1;    //Envoi un bit de start
        while(I2C1CONbits.SEN == 0);        //Attend qu'il soit fini
        
        ID = ID << 1;   //Le dernier bit sera � z�ro (mode �criture)
        
        I2C1TRN = ID;    //ID de l'esclave.
        //while (I2C1STATbits.TBF);
        while(I2C1STATbits.TRSTAT);       //Attend que l'envoi soit fini TRN + ACK.    Passe � 0 quand fini      
        
        I2C1TRN = data; //Envoi alors la donn�e
        //while (I2C1STATbits.TBF);
        while(I2C1STATbits.TRSTAT);       //Attend que l'envoi soit fini TRN + ACK.    Passe � 0 quand fini
        
        //I2C1CONbits.ACKDT = 1;  //Se pr�pare � un envoyer un NACK qui signifie la fin de la com.
        //I2C1CONbits.ACKEN = 1;  //Envoi le NACK
        //while(IFS1bits.MI2C1IF != 1);   //Attend que le NACK soit envoy�
        I2C1CONbits.PEN = 1;    //Envoi un bit de stop  
        while(I2C1CONbits.PEN == 0);
    }
    //else
        //return 0; //Signifie que la communication � �chou�
    
    return 1;   //Signifie que la communication � r�ussi
}

int lectureInformationI2C(unsigned int ID, unsigned int data) //En tant que MAITRE  
{
    //ATTENTION!! Le dernier bit de l'ID doit �tre 1 pour demander la lecture
    
    int value;
    
    //if(I2C1STATbits.P  == 1)   //Pour checker si le bus est bien en idle mode
    {
        I2C1CONbits.SEN = 1;    //Envoi un bit de start
        while(I2C1CONbits.SEN == 0);        //Attend qu'il soit fini
        
        ID = ID << 1;
        ID |= 1;    //met le dernier bit � 1 (mode lecture)
        
        I2C1TRN = ID;    //ID de l'esclave.
        //while (I2C1STATbits.TBF); //Attend l'envoi complet
        while(I2C1STATbits.TRSTAT);       //Attend que l'envoi soit fini TRN + ACK.    Passe � 0 quand fini     
        
        I2C1TRN = data; //Envoi alors la donn�e
        //while (I2C1STATbits.TBF);
        while(I2C1STATbits.TRSTAT);       //Attend que l'envoi soit fini TRN + ACK.    Passe � 0 quand fini
        
        I2C1CONbits.RSEN = 1;		//Generate Restart
        while(I2C1CONbits.RSEN == 0);	//Wait for restart	
        
        I2C1TRN = ID | 0x01;    //ID de l'esclave. Rajoute le bit de lecture
        while(I2C1STATbits.TRSTAT);       //Attend que l'envoi soit fini TRN + ACK.    Passe � 0 quand fini
       
        I2C1CONbits.RCEN = 1;   //Active la r�ception    
        while(I2C1CONbits.RCEN);
        
        value = I2C1RCV;        //On r�cup�re la donn�e
        
        //Permet de gerer le timeout r�ception I2C
        compteurDelayI2C = 0;   //Reset le flag
        flagCompteurDelayI2C = 1;    //Lance le flag
        while(I2C1STATbits.RBF == 0)   //On attend que la donn�e est �t� lu en entier
        {
            if(compteurDelayI2C >= 4) //Attend ENVIRON 20ms pour le timeout reception
            {
                flagCompteurDelayI2C = 0;    //Stop le flag
                I2C1CONbits.PEN = 1;    //Envoi un bit de stop
                while(I2C1CONbits.PEN == 0);
                return ERREUR_I2C_TIMEOUT;  //-1 correspond � une erreur timeout
            }
        }
        
        I2C1CONbits.ACKDT = 1;  //Se pr�pare � un envoyer un NACK qui signifie la fin de la com.
        I2C1CONbits.ACKEN = 1;  //Envoi le NACK
        
        I2C1CONbits.PEN = 1;    //Envoi un bit de stop
        while(I2C1CONbits.PEN == 0);
    }
    
    return value;
}


void receptionInformation()
{
    
}