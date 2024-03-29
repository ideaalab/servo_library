#include "servo.h"

/*
 * TIMER 1
 * Interrumpe cada 5mS
 * En cada interrupcion se pone en high el pin del servo correspondiente y se
 * actualiza el CCP con el valor de dicho servo.
 * Cuando trabajamos con menos de 4 servos los servos que no se usan no tienen
 * CCP al que actualizar, por lo que se usa un valor cualquiera dentro del rango
 * 
 * Usa 56 de ROM
 */
#int_TIMER1 //timer control de servo
void TIMER1_isr(void) {
//Configurado para 1 tick cada 1 uS
short ServoOutOfRange = FALSE;
	
	set_timer1(CERO_TIMER1);	//reinicia el timer
	
	switch(ServoFrame){
		case(0):
#ifdef SERVO1_PIN
	#ifdef SERVO_DIRECT_POSITION
			SERVO_SIGNAL_HIGH(SERVO1_PIN);
	#else
			output_bit(SERVO1_PIN, SERVO_ENABLED(0));
	#endif
#endif
			break;
		case(1):
#ifdef SERVO2_PIN
	#ifdef SERVO_DIRECT_POSITION
			SERVO_SIGNAL_HIGH(SERVO2_PIN);
	#else
			output_bit(SERVO2_PIN, SERVO_ENABLED(1));
	#endif
#else
			ServoOutOfRange = TRUE;
#endif
			break;
		case(2):
#ifdef SERVO3_PIN
	#ifdef SERVO_DIRECT_POSITION
			SERVO_SIGNAL_HIGH(SERVO3_PIN);
	#else
			output_bit(SERVO3_PIN, SERVO_ENABLED(2));
	#endif
#else
			ServoOutOfRange = TRUE;
#endif
			break;
		case(3):
#ifdef SERVO4_PIN
	#ifdef SERVO_DIRECT_POSITION
			SERVO_SIGNAL_HIGH(SERVO4_PIN);
	#else
			output_bit(SERVO4_PIN, SERVO_ENABLED(3));
	#endif
#else
			ServoOutOfRange = TRUE;
#endif
			flagServoRefresh = TRUE;
			break;
	}
	
	if(ServoOutOfRange == FALSE){
		CCP_1 = CERO_TIMER1 + Servo[ServoFrame].pos;   //actualiza el CCP con el valor de posicion actual
	}
	else{
		CCP_1 = CERO_TIMER1 + SERVO_POS_MAX;   //actualiza el CCP con un valor "cualquiera" de servo
	}
}

/*
 * CCP 1
 * Pone en LOW el PWM del servo correspondiente
 * 
 * Usa 22 de ROM
 */
#int_CCP1
void CCP1_isr(void){
	//apaga el servo correspondiente
	switch(ServoFrame){
		case(0):
#ifdef SERVO1_PIN
			SERVO_SIGNAL_LOW(SERVO1_PIN);
#endif
			ServoFrame = 1;
			break;
		case(1):
#ifdef SERVO2_PIN
			SERVO_SIGNAL_LOW(SERVO2_PIN);
#endif
			ServoFrame = 2;
			break;
		case(2):
#ifdef SERVO3_PIN
			SERVO_SIGNAL_LOW(SERVO3_PIN);
#endif
			ServoFrame = 3;
			break;
		case(3):
#ifdef SERVO4_PIN
			SERVO_SIGNAL_LOW(SERVO4_PIN);
#endif
			ServoFrame = 0;
			break;
	}
}

/* RUTINAS */

/* 
 * Configura el timer1 para que tenga ticks de 1uS
 * Habilita las interrupciones
 * 
 * Usa 33 de ROM
 */ 
void Servo_init(void){
//timer1 config
#if getenv("CLOCK") == 4000000
	setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);		
#elif getenv("CLOCK") == 8000000
	setup_timer_1(T1_INTERNAL|T1_DIV_BY_2);
#elif getenv("CLOCK") == 16000000
	setup_timer_1(T1_INTERNAL|T1_DIV_BY_4);
#elif getenv("CLOCK") == 32000000
	setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);
#else
	#ERROR La velocidad del PIC debe ser de 4, 8, 16 o 32Mhz
#endif
	
	CCP_1 = Servo[0].min;			//le da el primer valor a CCP1 para que interrumpa cuando debe
	setup_ccp1(CCP_COMPARE_INT);	//configura CCP
	
	enable_interrupts(INT_TIMER1);	//activa timer 1
	enable_interrupts(INT_CCP1);	//activa CCP1
	enable_interrupts(GLOBAL);		//enable global interrupt
}

/*
 * Configura el servo
 * -num: numero del servo que estamos configurando (entre 0 y NUM_SERVOS-1)
 * -min: valor minimo para el servo (en uS) tambien llamado POSICION1
 * -max: valor maximo para el servo (en uS)	tambien llamado POSICION2
 * -vel: velocidad a la que se mueve (de 0 a 7) [para SERVO_INDIRECT_POSITION]
 * -es: EnergySave activo o no [para SERVO_INDIRECT_POSITION]
 */
#ifdef SERVO_DIRECT_POSITION
void Servo_Config(int num, long min, long max){
	//de momento asumimos que el usuario ha configurado el TRIS de cada pin
	Servo[num].pos = min;	//establece posicion actual
	Servo[num].min = min;	//establece minimo
	Servo[num].max = max;	//establece maximo
	//Servo[num].enabled = TRUE;
}
#else
void Servo_Config(int num, long min, long max, int vel, short es){
	//de momento asumimos que el usuario ha configurado el TRIS de cada pin
	Servo[num].vel = vel;	//establece velocidad
	Servo[num].min = min;	//establece minimo
	Servo[num].max = max;	//establece maximo

	Servo[num].pos = min;	//establece posicion actual
	Servo[num].fin = min;
	Servo[num].ES = es;
	Servo[num].contES = 0;
	Servo[num].enabled = FALSE;
	Servo[num].stop = TRUE;
	//Servo[num].enabled = TRUE;
}
#endif

#ifdef SERVO_INDIRECT_POSITION
/*
 * ACTIVA O DESACTIVA EL ENVIO DE PULSOS AL SERVO
 *	"num" dice a que servo va dirigido
 *	"en" dice si lo activamos (true) o desactivamos (false)
 */
void Servo_Active(int num, short en){
	Servo[num].contES = 0;
	Servo[num].enabled = en;
}
#endif

/*
 * POSICIONAMIENTO DE SERVOS
 * "num" dice a que servo va dirigido
 * "pos" dice la posicion del servo a la que queremos ir
 * -Para SERVO_DIRECT_POSITION asumimos que el valor que nos pasan esta dentro
 * de los limites del servo
 * -Para SERVO_INDIRECT_POSITION no hace falta comprobar que estamos dentro de
 * los limites de min y max ya que esto se hace en Servo_Refresh_Pos()
 * 
 * Usa 23 de ROM
 */
void Servo_Mover(int num, long pos){	
#ifdef SERVO_DIRECT_POSITION
	Servo[num].pos = pos;
#else
	Servo[num].fin = pos;
	Servo[num].contES = 0;
	Servo[num].enabled = TRUE;
	
	Servo[num].stop = FALSE;
	
	if(pos == Servo[num].min){
		Servo[num].dirPos = S_MIN;
	}
	else if(pos == Servo[num].max){
		Servo[num].dirPos = S_MAX;
	}
#endif
}

#ifdef SERVO_INDIRECT_POSITION
/*
 * ACTUALIZA POSICIONES DE SERVOS
 * esta rutina hay que llamarla constantemente desde el main
 * para que actualice los valores de los servos
 * 
 * Usa 
 */
void Servo_Refresh_Pos(void){
//rutina optimizada para 1 servo (consume menos memoria)
#if NUM_SERVOS == 1
	if(flagServoRefresh == TRUE){
		flagServoRefresh = FALSE;
		
		//aumenta la posicion si el servo que esta activo
		if(Servo[0].enabled == TRUE){
			/* estamos incrementando pos */
			if(Servo[0].pos < Servo[0].fin){
				Servo[0].pos = Servo[0].pos + ServoVel[Servo[0].vel];
				//si nos pasamos
				if(Servo[0].pos > Servo[0].fin){
					Servo[0].pos = Servo[0].fin;
				}
			}
			/* estamos disminuyendo pos */
			else if(Servo[0].pos > Servo[0].fin){
				Servo[0].pos = Servo[0].pos - ServoVel[Servo[0].vel];
				//si nos pasamos
				if(Servo[0].pos < Servo[0].fin){
					Servo[0].pos = Servo[0].fin;
				}
			}
			/* hemos llegado al final*/
			else{	//servo[0].pos == servo[0].fin
				Servo[0].stop = TRUE;
				//solo usamos el contador si EnergySave esta activado
				if(Servo[0].ES == TRUE){
					if(++Servo[0].contES == VUELTAS_ENERGY_SAVE){
						Servo[0].enabled = FALSE;
					}
				}
			}
		}
	}
//rutina para 4 servos (consume mas memoria)
#else
	int x;
	
	if(flagServoRefresh == TRUE){
		flagServoRefresh = FALSE;
		
		//aumenta la posicion en los servos que estan activos
		for(x=0;x<MAX_SERVOS;x++){
			if(Servo[x].enabled == TRUE){
				/* estamos incrementando pos */
				if(Servo[x].pos < Servo[x].fin){
					Servo[x].pos = Servo[x].pos + ServoVel[Servo[x].vel];
					//si nos pasamos
					if(Servo[x].pos > Servo[x].fin){
						Servo[x].pos = Servo[x].fin;
					}
				}
				/* estamos disminuyendo pos */
				else if(Servo[x].pos > Servo[x].fin){
					Servo[x].pos = Servo[x].pos - ServoVel[Servo[x].vel];
					//si nos pasamos
					if(Servo[x].pos < Servo[x].fin){
						Servo[x].pos = Servo[x].fin;
					}
				}
				/* hemos llegado al final*/
				else{	//servo[x].pos == servo[x].fin
					Servo[x].stop = TRUE;
					//solo usamos el contador si EnergySave esta activado
					if(Servo[x].ES == TRUE){
						if(++Servo[x].contES == VUELTAS_ENERGY_SAVE){
							Servo[x].enabled = FALSE;
						}
					}
				}
			}
		}
	}
#endif
}
#endif

/*
 * Mueve el servo a una posicion acorde al valor de un potenciometro
 * Consume mas ROM pero es mas precisa
 * 
 * Consume 202 de ROM ¿?
 */
void Servo_Mover_Pot(int potVal){
signed int32 tmp;

	tmp = potVal * RANGO_SERVOS;
	tmp = tmp / POT_MAX_VAL;

	Servo_Mover(0, Servo[0].min + tmp);
}

void Servo_Mover_Pot(int num, int potVal){
signed int32 tmp;

	tmp = potVal * RANGO_SERVOS;
	tmp = tmp / POT_MAX_VAL;
	
	Servo_Mover(num, Servo[num].min + tmp);
}

/*
 * Mueve el servo a una posicion acorde al valor de un potenciometro
 * Para usar esta funcion hay que calcular SERVO_POS_MIN y SERVO_POS_MAX
 * de acuerdo con el archivo de Excel adjunto para que los resultados sean
 * mas precisos
 * 
 * Consume 76 de ROM ¿?
 */

void Servo_Mover_Pot_Small(int potVal){
signed int32 tmp;

	tmp = potVal * RANGO_SERVOS;
	tmp = DIV_BY_256(tmp);


	Servo_Mover(0, Servo[0].min + tmp);
}

void Servo_Mover_Pot_Small(int num, int potVal){
signed int32 tmp;

	tmp = potVal * RANGO_SERVOS;
	tmp = DIV_BY_256(tmp);

	Servo_Mover(num, Servo[num].min + tmp);
}