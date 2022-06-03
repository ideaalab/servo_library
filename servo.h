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

/*#if getenv("ADC_RESOLUTION") == 10
//si la resolucion es de 10 bits
#define ADC_MAX_VAL		(1023.0)
#else
//si la resolucion es otra asumimos que es de 8 bits
#define ADC_MAX_VAL		(255.0)
#endif*/

#define MAX_SERVOS			4	//cuantos servos podemos manejar

#if NUM_SERVOS > MAX_SERVOS
#error "No se pueden controlar mas de 4 servos con esta libreria"
#endif

#define T_T1				20	//cada cuanto interrumpe Timer1 en mS

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

#ifndef SERVO_POS1
#define SERVO_POS1			700		//posicion1 por defecto
#endif

#ifndef SERVO_POS2
#define SERVO_POS2			1700	//posicion2 por defecto
#endif

//#define S_RANGE				(SERVO_POS_MAX - SERVO_POS_MIN)	//rango de posiciones del servo
//#define S_CENTER			(S_RANGE / 2)					//centro del PWM

/*#ifndef RANGE_MAX_VAL
#define RANGE_MAX_VAL		ADC_MAX_VAL
#endif*/
//#define RANGE_UNI			(S_RANGE / RANGE_MAX_VAL)		//cuantas unidades de posicion del servo incrementa cada unidad del rango

#define CERO_TIMER1			(65535 - (SERVO_PERIOD / MAX_SERVOS))	//para que reinicie cada 5mS

#define SERVO_MIN			(CERO_TIMER1 + SERVO_POS_MIN)	//posicion minima del servo con respecto al timer 1
#define SERVO_MAX			(CERO_TIMER1 + SERVO_POS_MAX)	//posicion maxima del servo con respecto al timer 1

#define SERVO_DEF1			(CERO_TIMER1 + SERVO_POS1)	//posicion 1 por defecto con respecto al timer 1
#define SERVO_DEF2			(CERO_TIMER1 + SERVO_POS2)	//posicion 2 por defecto con respecto al timer 1

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
void Servo_init(void);
void Servo_Mover(int num, long pos);
#ifdef SERVO_DIRECT_POSITION
void Servo_Config(int num, long min, long max);
#else
void Servo_Config(int num, int vel, short es, long min, long max);
void Servo_Active(int num, short en);
void Servo_Refresh_Pos(void);
#endif

#endif	/* SERVO_H */
