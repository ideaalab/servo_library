/*******************************************************************************
 * File:   servo.h
 * Author: Martin Andersen
 * Company: IDEAA Lab ( http://www.ideaalab.com )
 * 
 * Libreria que permite controlar entre 1 y 4 servos:
 * -Permite configurar posicion min y max para cada servo
 * -Permite configurar una velocidad para cada servo
 * 
 *******************************************************************************
 * INSTRUCCIONES
 * 
 * -Hay que crear un define con la cantidad de servos a usar:
 * #define NUM_SERVOS	4
 * 
 * -A cada servo (que este en uso) hay que crearle un define con su pin
 * correspondiente:
 * #define SERVO1_PIN	PIN_A1
 * #define SERVO2_PIN	PIN_A2
 * #define SERVO3_PIN	PIN_A3
 * #define SERVO4_PIN	PIN_A4
 * 
 * -Se puede configurar un minimo y un maximo en uS para el movimiento de los
 * servos. Por defecto son 500 y 2500 respectivamente. Usar los siguientes
 * defines:
 * #define SERVO_POS_MIN		1000
 * #define SERVO_POS_MAX		2000
 * 
 * -Se puede configurar una velocidad por defecto para el movimiento del servo,
 * por defecto la velocidad usada es la mas rapida, si se quiere usar otra crear
 * el siguiente define con un valor entre 0 y (SERVO_NUM_VEL - 1)
 * #define SERVO_VEL_DEFAULT	3
 * 
 * -Se puede configurar la posicion 1 y 2 por defecto para el movimiento del
 * servo, por defecto las posiciones son 700uS y 1700uS, si se quiere usar otra
 * crear los siguiente define con valores entre SERVO_POS_MIN y SERVO_POS_MAX
 * #define SERVO_POS1			1200	//posicion1 por defecto
 * #define SERVO_POS2			1900	//posicion2 por defecto
 * 
 * -Cuando EnergySave = TRUE se deja de enviar la posicion al servo transcurrido
 * cierto tiempo. En la mayoria de los servos esto se traduce en que el servo
 * deja de posicionarse y se queda "libre" en su posicion actual. Esto ahorra
 * bateria, pero permite que el servo pierda su posicion por accion de una
 * fuerza externa. Por defecto esta activada, pero se puede desactivar creando
 * el siguiente define:
 * #define ENERGY_SAVE_DEFAULT	FALSE
 * 
 * -Cuando EnergySave = TRUE y una vez que el servo llego a la posicion final
 * comienza un temporizador que una vez finalizado deja de enviar señal al servo
 * El tiempo por defecto es de 1000mS, pero se puede cambiar creando el
 * siguiente define:
 * #define TIEMPO_ENERGY_SAVE	500
 * 
 * -Cuando posicionamos un servo segun el valor de un potenciometro tenemos la
 * constante RANGE_UNI que multiplicada por el valor del potenciometro nos dara
 * una posicion del servo, dentro del rango MIN y MAX establecidos.
 * Ej:
 *		//leo adc y lo convierto en un valor entre min y max
 *		long pos1 = SERVO_POS_MIN + (read_adc() * RANGE_UNI);
 *		//muevo el servo con este valor
 *		Servo_Mover(0, pos1);
 * 
 * -Si queremos usar un rango diferente podemos declarar RANGE_UNI con el valor
 * que necesitemos. Usar "NUM.0" para que el compilador entienda que queremos
 * guardar los decimales, sino perderemos resolucion en el movimiento:
 * #define RANGE_UNI			(S_RANGE / 500.0)
 * 
 * -Se crea un array con los valores de cada servo. Cada servo necesita 10 bytes
 * y los valores son los siguientes:
 * Servo[num]
 *			.pos: posicion actual del servo							(2 bytes)
 *			.fin: posicion a la que queremos llegar					(2 bytes)
 *			.min: posicion minima del servo							(2 bytes)
 *			.max: posicion maxima del servo							(2 bytes)
 *			.enabled: flag para saber si el servo se esta moviendo	(1 bit)
 *			.vel: velocidad del servo, del 1 al 8					(3 bits)
 *			.nul: estos bits no se usan								(4 bits)
 *			.contES: variable para contar el energy save			(1 byte)
 * 
 * -Si se crea un define SERVO_DIRECT_POSITION entonces los servos se posicionan
 * directamente, por lo que fin, enabled, vel, contES no se usan y ahorramos RAM
 * ya que cada servo solo necesita 6 bytes:
 * #define SERVO_DIRECT_POSITION
 * 
 * Servo[num]
 *			.pos: posicion actual del servo							(2 bytes)
 *			.min: posicion minima del servo							(2 bytes)
 *			.max: posicion maxima del servo							(2 bytes)
 * 
 *******************************************************************************
 *FUNCIONES
 * 
 * -void Servo_init(void)
 *	Inicializa la libreria
 * 
 * -void Servo_Config(int num, int vel, long min, long max)
 *	Configura el servo
 *		num: numero del servo que estamos configurando (entre 0 y NUM_SERVOS-1)
 *		vel: velocidad a la que se mueve (de 0 a 7)
 *		min: valor minimo para el servo (en uS)
 *		max: valor maximo para el servo (en uS)
 * 
 * -void Servo_Active(int num, short en)
 *	Activa o desactiva el envio de pulsos al servo
 *		num: dice a que servo va dirigido
 *		en: dice si lo activamos (true) o desactivamos (false)
 * 
 * -void Servo_Mover(int num, long pos)
 *	Cambia el servo de posicion
 *		num: dice a que servo va dirigido
 *		pos: dice la posicion del servo a la que queremos ir
 * 
 * -void Servo_Refresh_Pos(void)
 *	Actualiza la posicion de los servos. Esta rutina hay que ponerla en el main
 *	para llamarla constantemente y que actualice los valores de  los servos.
 *  Usar solo en caso de que NO estemos usando SERVO_DIRECT_POSITION
 * 
 * -Si estamos usando SERVO_DIRECT_POSITION y queremos un control mas directo de
 *  los servos podemos escribir y leer directamente en Servo[x].pos/min/max en
 *  vez de usar las funciones Servo_Config() y Servo_Mover().
 *  
 ******************************************************************************/

#ifndef SERVO_H
#define	SERVO_H

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

#if getenv("ADC_RESOLUTION") == 8
#define ADC_MAX_VAL		255
#elif getenv("ADC_RESOLUTION") == 10
#define ADC_MAX_VAL		1023
#else
#ERROR "Resolucion del ADC no soportada."
#endif

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

#define S_RANGE				(SERVO_POS_MAX - SERVO_POS_MIN)	//rango de posiciones
#define S_CENTER			(S_RANGE / 2)					//centro del PWM
#ifndef RANGE_UNI
#define RANGE_UNI			(S_RANGE / 255.0)				//cuantas unidades de posicion incrementa cada unidad de adc
#endif

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
	int enabled:1;	//flag para saber si el servo se esta moviendo (1 bit)
	int vel:3;		//velocidad del servo, 8 velocidades (3 bits)
	int nul:4;		//de momento estos 4 bits no se usan
	int contES;		//variable para contar el energy save (1 byte)
}servo_t;
#endif

/* VARIABLES GOBALES */
#ifndef SERVO_DIRECT_POSITION
short EnergySave = ENERGY_SAVE_DEFAULT;		//indica si los servos funcionan constantemente o solo durante un tiempo
const int ServoVel[SERVO_NUM_VEL] = {SERVO_VEL1, SERVO_VEL2, SERVO_VEL3, SERVO_VEL4, SERVO_VEL5, SERVO_VEL6, SERVO_VEL7, SERVO_VEL8};
#endif

short flagServoRefresh = FALSE;	//indica si hay que modificar el pulso de los servos
int ServoFrame = 0;				//variable para saber que servo estamos controlando
servo_t Servo[NUM_SERVOS];		//crea la variable de servos

/* PROTOTIPOS */
void Servo_init(void);
void Servo_Mover(int num, long pos);
#ifdef SERVO_DIRECT_POSITION
void Servo_Config(int num, long min, long max);
#else
void Servo_Config(int num, int vel, long min, long max);
void Servo_Active(int num, short en);
void Servo_Refresh_Pos(void);
#endif

#endif	/* SERVO_H */
