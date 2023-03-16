#ifndef SERVO_H
#define	SERVO_H

/* COMPROBACION DE MODO DEFUNCIONAMIENTO */
#ifndef SERVO_DIRECT_POSITION
#define SERVO_INDIRECT_POSITION
#endif

/* COMPROBACION DE SERVOS DECLARADOS */
//1 servo
#if NUM_SERVOS == 1
#ifndef SERVO1_PIN
#ERROR No se ha declarado el servo 1
#endif
#endif

//2 servos
#if NUM_SERVOS == 2
#ifndef SERVO1_PIN
#ERROR No se ha declarado el servo 1
#endif

#ifndef SERVO2_PIN
#ERROR No se ha declarado el servo 2
#endif
#endif

//3 servos
#if NUM_SERVOS == 3
#ifndef SERVO1_PIN
#ERROR No se ha declarado el servo 1
#endif

#ifndef SERVO2_PIN
#ERROR No se ha declarado el servo 2
#endif

#ifndef SERVO3_PIN
#ERROR No se ha declarado el servo 3
#endif
#endif

//4 servos
#if NUM_SERVOS == 4
#ifndef SERVO1_PIN
#ERROR No se ha declarado el servo 1
#endif

#ifndef SERVO2_PIN
#ERROR No se ha declarado el servo 2
#endif

#ifndef SERVO3_PIN
#ERROR No se ha declarado el servo 3
#endif

#ifndef SERVO4_PIN
#ERROR No se ha declarado el servo 4
#endif
#endif

/* --- CONSTANTES --- */
//genericas
#ifndef TRUE
#define TRUE				1
#endif

#ifndef FALSE
#define FALSE				0
#endif

#define MAX_SERVOS			4	//cuantos servos podemos manejar

#if NUM_SERVOS > MAX_SERVOS
#error "No se pueden controlar mas de 4 servos con esta libreria"
#endif

#define T_T1				5	//cada cuanto interrumpe Timer1 en mS
#warning "Era 20, y se cambio por 5. COMPROBAR QUE NO GENERE PROBLEMAS. Se puede usar SERVO_REFRESH"
#define SERVO_REFRESH		(T_T1 * 4)

//velocidades
#define SERVO_NUM_VEL		8

#define SERVO_VEL1			1
#define SERVO_VEL2			2
#define SERVO_VEL3			3
#define SERVO_VEL4			4
#define SERVO_VEL5			5
#define SERVO_VEL6			10
#define SERVO_VEL7			50
#define SERVO_VEL8			100

#ifndef SERVO_VEL_DEFAULT
#define SERVO_VEL_DEFAULT	(SERVO_NUM_VEL - 1)
#endif

#define S_MIN	0
#define S_MAX	1

//posiciones
#define SERVO_PERIOD		20000	//en uS

#ifndef SERVO_POS_MIN
#define SERVO_POS_MIN		500		//posicion minima del servo
#endif

#ifndef SERVO_POS_MAX
#define SERVO_POS_MAX		2500	//posicion maxima del servo
#endif

#define SERVO_POS_MID		((SERVO_POS_MIN + SERVO_POS_MAX) / 2)

#ifndef SERVO_POS1
#define SERVO_POS1			700		//posicion1 por defecto
#endif

#ifndef SERVO_POS2
#define SERVO_POS2			1700	//posicion2 por defecto
#endif

#define CERO_TIMER1			(65535 - (SERVO_PERIOD / MAX_SERVOS))	//para que reinicie cada 5mS

#define SERVO_MIN			(CERO_TIMER1 + SERVO_POS_MIN)	//posicion minima del servo con respecto al timer 1
#define SERVO_MAX			(CERO_TIMER1 + SERVO_POS_MAX)	//posicion maxima del servo con respecto al timer 1

#define SERVO_DEF1			(CERO_TIMER1 + SERVO_POS1)	//posicion 1 por defecto con respecto al timer 1
#define SERVO_DEF2			(CERO_TIMER1 + SERVO_POS2)	//posicion 2 por defecto con respecto al timer 1

//Calculos ADC/POT -> Posicion servo
#define RANGO_SERVOS		(signed int32)(SERVO_POS_MAX - SERVO_POS_MIN)


#ifndef POT_MAX_VAL
#define POT_MAX_VAL		255
#endif

#define DIV_BY_256(x)	(x>>8)	//macro para dividir un valor entre 256

//energy save
#ifndef ENERGY_SAVE_DEFAULT
#define ENERGY_SAVE_DEFAULT	TRUE
#endif

#ifndef TIEMPO_ENERGY_SAVE
#define TIEMPO_ENERGY_SAVE	1000	//mS que mantiene el servo en posicion despues de haber llegado
#endif								//tiene que ser multiplo de 20, y max 5100mS

#define VUELTAS_ENERGY_SAVE	(1000 * TIEMPO_ENERGY_SAVE / SERVO_PERIOD)
/* ------------------ */

#ifdef SERVO_DIRECT_POSITION
typedef struct{
	long pos;		//posicion actual del servo (2 bytes)
	long min;		//posicion minima del servo (2 bytes)
	long max;		//posicion maxima del servo (2 bytes)
}servo_t;
#else	
typedef struct{
	long pos;		//posicion actual del servo (2 bytes)
	long fin;		//posicion a la que queremos llegar (2 bytes)
	long min;		//posicion minima del servo (2 bytes)
	long max;		//posicion maxima del servo (2 bytes)
	int contES;		//variable para contar el energy save (1 byte)
	
	int enabled:1;	//flag para saber si el servo se esta moviendo (1 bit)
	int ES:1;		//flag para saber si este servo tiene EnergySave activado (1 bit)
	int vel:3;		//velocidad del servo, 8 velocidades (3 bits)
	int stop:1;		//flag para saber si llego al final del movimiento
	int dirPos:1;	//indica si se encuentra de camino a S_MIN o S_MAX
	int nul:1;		//de momento este bit no se usa
}servo_t;
#endif

/* VARIABLES GOBALES */
#ifdef SERVO_INDIRECT_POSITION
const int ServoVel[SERVO_NUM_VEL] = {SERVO_VEL1, SERVO_VEL2, SERVO_VEL3, SERVO_VEL4, SERVO_VEL5, SERVO_VEL6, SERVO_VEL7, SERVO_VEL8};
#endif

short flagServoRefresh = FALSE;	//indica si hay que modificar el pulso de los servos
int ServoFrame = 0;				//variable para saber que servo estamos controlando
servo_t Servo[NUM_SERVOS];		//crea la variable de servos

/* MACROS */
#ifdef SERVO_INVERT_SIGNAL
#define SERVO_SIGNAL_HIGH(x)	output_low(x)
#define SERVO_SIGNAL_LOW(x)		output_high(x)
#define SERVO_ENABLED(x)		!Servo[x].enabled
#else
#define SERVO_SIGNAL_HIGH(x)	output_high(x)
#define SERVO_SIGNAL_LOW(x)		output_low(x)
#define SERVO_ENABLED(x)		Servo[x].enabled
#endif

/* PROTOTIPOS */
void TIMER1_isr(void);
void CCP1_isr(void);

void Servo_init(void);
void Servo_Mover(int num, long pos);

void Servo_Mover_Pot(int potVal);
void Servo_Mover_Pot(int num, int potVal);

void Servo_Mover_Pot_Small(int potVal);
void Servo_Mover_Pot_Small(int num, int potVal);

#ifdef SERVO_DIRECT_POSITION
void Servo_Config(int num, long min, long max);
#else
#warning "Se cambio el orden de las variables de Servo_Config en la libreria, COMPROBAR!!"

void Servo_Config(int num, long min, long max, int vel, short es);
void Servo_Active(int num, short en);
void Servo_Refresh_Pos(void);
#endif

#endif	/* SERVO_H */
