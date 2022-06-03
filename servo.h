/* =============================================================================
 *                   LIBRERIA PARA CONTROLAR SERVOS
 * 
 * Autor: Martin Andersen
 * Compania: IDEAA Lab ( http://www.ideaalab.com )
 * 
 * =============================================================================
 * QUE HACE ESTA LIBRERIA?
 * 
 * >Permite controlar entre 1 y 4 servos
 * 
 * >Permite configurar posicion min y max para cada servo
 * 
 * >Permite configurar una velocidad para cada servo
 * 
 * >Permite habilitar o desahibiliar cada servo para ahorrar energia
 * 
 * =============================================================================
 * INTRODUCCION
 * >Para controlar un servo el periodo del PWM es de 20mS/20000uS. En el
 * instante 0uS del PWM se debe poner en HIGH. La posicion del servo viene 
 * determinada por el momento en que pongamos el PWM en LOW. Normalmente 500uS
 * posiciona la servo en un extremo, y 2500uS lo posiciona en el extremo opuesto.
 * 
 * >Se pueden controlar hasta 4 servos. Para ello configuramos el timer1 para
 * que interrumpa cada 5000uS. Cada servo tiene su propio (siempre el mismo)
 * "marco de tiempo" dentro del cual se pone a HIGH y luego a LOW su PWM. Es el
 * CCP1 el encargado de poner a LOW el PWM. Al haber 4 marcos de tiempo, los PWM
 * de los servos siempre se actualizan correctamente a los 20mS necesarios. No
 * importa si usamos 1 o mas servos, los 4 marcos de tiempo se mantienen.
 * 
 * >Las posiciones de los servos siempre se establecen en micro segundos.
 * 
 * >La libreria se puede usar de dos maneras:
 * -Control directo: permite controlar la posicion del servo instantaneamente.
 * Se le da la posicion y la libreria mueve el servo a dicha direccion.
 * -Control indirecto: cuado se manda a mover a un servo a una posicion, la
 * libreria gestiona el movimiento de manera que se adapte a una velocidad
 * configurable. No se controla la velocidad realmente, sino que se posiciona al
 * servo en diferentes posiciones internmedias entre el inicio y el final y esto
 * produce una modificacion de la velocidad.
 * 
 * >El ahorro de energia (EnergySave) se consigue dejando de enviar pulsos de
 * PWM al servo. Esto reduce el consumo en la mayoria de los servos ya que este
 * deja de intentar posicionarse constantemente.
 * 
 * >Podemos trabajar con el rango de posiciones comprendidas entre min y max, o
 * podemos usar un rango diferente que se adapte a otras necesidades, por ejemplo
 * los valores de un potenciometro.
 * 
 * =============================================================================
 * COMO CONFIGURAR LA LIBRERIA?
 * 
 * >Hay que crear un define con la cantidad de servos a usar:
 * >define NUM_SERVOS	4
 * 
 * >Cada servo que usemos hay que crearle un define con su pin correspondiente:
 * #define SERVO1_PIN	PIN_A1
 * #define SERVO2_PIN	PIN_A2
 * #define SERVO3_PIN	PIN_A3
 * #define SERVO4_PIN	PIN_A4
 * 
 * >Si queremos el control directo sobre los servos podemos crea un define
 * SERVO_DIRECT_POSITION, esto ahorra RAM ya que Servo[].fin, .enabled, .vel,
 * .contES, .dirPos, .stop no se usan
 * #define SERVO_DIRECT_POSITION
 * 
 * >Si nuestro circuito tiene algun tipo de adaptador de señal, conversor de
 * niveles, etc. que invierte la señal del servo, podemos usar la constante:
 * #define SERVO_INVERT_SIGNAL
 * Esto hara que la señal salga invertida, y al pasar por el conversor de
 * niveles la señal se desinvierta
 * 
 * >Se puede configurar un minimo y un maximo GLOBAL en uS para el movimiento de
 * los servos. Esto limita el movimiento de los servos dentro de ese rango.
 * Los limites individuales de cada servo no podran ser mayores que los limites
 * globales. Por defecto los min/max son 500 y 2500 respectivamente, si se
 * quieren usar otros se pueden crear los siguientes define:
 * #define SERVO_POS_MIN		1000
 * #define SERVO_POS_MAX		2000
 * 
 * >Por defecto las posiciones 1 y 2 de los servos son 700uS y 1700uS.
 * Si se quiere usar otras hay que crear los siguiente define con valores
 * comprendidos entre SERVO_POS_MIN y SERVO_POS_MAX
 * #define SERVO_POS1			1200	//posicion1 por defecto
 * #define SERVO_POS2			1900	//posicion2 por defecto
 * 
 * >La velocidad de los servos por defecto es la mas rapida, si se quiere usar
 * otra crear el siguiente define con un valor entre 0 y (SERVO_NUM_VEL - 1)
 * #define SERVO_VEL_DEFAULT	3
 * 
 * >EnergySave por defecto esta activado, pero se puede desactivar creando el
 * siguiente define:
 * #define ENERGY_SAVE_DEFAULT	FALSE
 * 
 * >Si EnergySave esta activado y una vez que el servo llego a la posicion final
 * comienza un temporizador que una vez finalizado deja de enviar señal al servo
 * El tiempo por defecto es de 1000mS, pero se puede cambiar creando el
 * siguiente define:
 * #define TIEMPO_ENERGY_SAVE	500
 * 
 * >Si vamos a trabajar con un rango de posiciones diferente al de min/max
 * podemos declarar RANGE_MAX_VAL. Por defecto este es igual al maximo valor del
 * ADC interno (255/1023). Si queremos usar un rango diferente hay que declarar
 * RANGE_MAX_VAL con el valor que necesitemos. Usar "NUM".0 para que el
 * compilador entienda que queremos guardar los decimales, sino perderemos
 * resolucion en el movimiento:
 * #define RANGE_MAX_VAL		(500.0)
 * =============================================================================
 * VARIABLES
 * 
 * Con SERVO_DIRECT_POSITION la variable Servo[] necesita 6 bytes
 * >Servo[num]
 *			.pos: posicion actual del servo							(2 bytes)
 *			.min: posicion minima del servo							(2 bytes)
 *			.max: posicion maxima del servo							(2 bytes)
 * 
 * Con SERVO_INDIRECT_POSITION la variable Servo[] necesita 10 bytes
 * >Servo[num]
 *			.pos: posicion actual del servo							(2 bytes)
 *			.fin: posicion a la que queremos llegar					(2 bytes)
 *			.min: posicion minima del servo							(2 bytes)
 *			.max: posicion maxima del servo							(2 bytes)
 *			.contES: variable para contar el energy save			(1 bytes)
 * 
 *			.enabled: flag para saber si el servo se esta moviendo	(1 bit)
 *			.ES: flag para saber si el servo funciona con EnergySave(1 bit)
 *			.vel: velocidad del servo, del 1 al 8					(3 bits)
 *			.stop: indica si el servo ha llegado al fin				(1 bit)
 *			.dirPos: indica hacia donde se mueve (S_MIN / S_MAX)	(1 bit)
 *			.nul: este bit no se usa								(1 bits)
 * 
 * >ServoVel[0-7]: es un array constante que contiene los valores de las 8
 * velocidades disponibles (posiciones 0 a 7)
 * 
 * >flagServoRefresh: indica si hay que modificar el pulso de los servos.
 * 
 * >ServoFrame: nos indica que servo estamos controlando.
 * =============================================================================

 * =============================================================================
 *FUNCIONES
 * 
 * >Servo_init(): inicializa la libreria
 * 
 * >Servo_Config(int num, int vel, long min, long max): Configura el servo
 *		num: numero del servo que estamos configurando (entre 0 y NUM_SERVOS-1)
 *		*vel: velocidad a la que se mueve (de 0 a 7)
 *		min: valor minimo para el servo (en uS)
 *		max: valor maximo para el servo (en uS)
 * 
 * >Servo_Active(int num, short en): ctiva o desactiva el envio de pulsos al servo
 *		num: dice a que servo va dirigido
 *		en: dice si lo activamos (true) o desactivamos (false)
 * 
 * >Servo_Mover(int num, long pos): cambia el servo de posicion. Asumimos que los
 * valores que nos dan estan dentro de los rangos admitidos.
 *		num: dice a que servo va dirigido
 *		pos: dice la posicion del servo a la que queremos ir
 * 
 * >Servo_Refresh_Pos(): actualiza la posicion de los servos. Esta rutina hay
 * que ponerla en el main para llamarla constantemente y que actualice los
 * valores de  los servos. Usar solo con SERVO_INDIRECT_POSITION
 * 
 * =============================================================================
 * OTRAS FUNCIONALIDADES
 * 
 * >Si estamos usando SERVO_DIRECT_POSITION y queremos un control mas directo de
 * los servos podemos escribir y leer directamente en Servo[x].pos/min/max en
 * vez de usar las funciones Servo_Config() y Servo_Mover().
 * 
 * >Cuando posicionamos un servo segun el valor de un potenciometro tenemos la
 * constante RANGE_UNI que multiplicada por el valor del potenciometro nos dara
 * una posicion del servo, dentro del rango MIN y MAX establecidos.
 * Ej:
 *		//leo adc y lo convierto en un valor entre min y max
 *		long pos1 = SERVO_POS_MIN + (read_adc() * RANGE_UNI);
 *		//muevo el servo con este valor
 *		Servo_Mover(0, pos1);
 * =============================================================================
 * RECURSOS USADOS
 * 
 * >Necesita el Timer1 y el modulo CCP1 para funcionar
 * ========================================================================== */

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
