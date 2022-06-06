# LIBRERIA PARA CONTROLAR SERVOS

Autor: Martin Andersen

Compañia: @ideaalab ( http://www.ideaalab.com )


## QUE HACE ESTA LIBRERIA?

- Permite controlar entre 1 y 4 servos
- Permite configurar posicion min y max para cada servo
- Permite configurar una velocidad para cada servo
- Permite habilitar o desahibiliar cada servo para ahorrar energia


## INTRODUCCION
- Para controlar un servo tenemos que enviar constantemente un pulso con un periodo de 20mS/20000uS. En el instante 0uS del PWM se debe poner en HIGH. La posicion del servo viene determinada por el momento en que pongamos el PWM en LOW. Normalmente 500uS posiciona la servo en un extremo, y 2500uS lo posiciona en el extremo opuesto.
- Se pueden controlar hasta 4 servos. Para ello configuramos el timer1 para que interrumpa cada 5000uS. Cada servo tiene su propio "marco de tiempo" dentro del cual se pone a HIGH y luego a LOW su PWM. Es el CCP1 el encargado de poner a LOW el PWM. Al haber 4 marcos de tiempo, los PWM de los servos siempre se actualizan correctamente a los 20mS necesarios. No importa si usamos 1 o mas servos, los 4 marcos de tiempo se mantienen.
- Las posiciones de los servos siempre se establecen en micro segundos.
- La libreria se puede usar de dos maneras:
  1. **Control directo:** permite controlar la posicion del servo instantaneamente. Se le da la posicion y la libreria mueve el servo a dicha direccion.
  2. **Control indirecto:** se le dice la posicion del servo, pero la libreria gestiona las posiciones intermedias, desde la posicion actual hasta la posicion final. De esta manera permite variar la velocidad del movimiento del servo.
- El ahorro de energia (EnergySave) se consigue dejando de enviar pulsos de PWM al servo. Esto reduce el consumo en la mayoria de los servos ya que este deja de intentar posicionarse constantemente.
- Podemos trabajar con el rango de posiciones comprendidas entre min y max, o podemos usar un rango diferente que se adapte a otras necesidades.


## COMO CONFIGURAR LA LIBRERIA?

### Servos usados
Hay que crear un define con la cantidad de servos a usar:
```
#define NUM_SERVOS	4
```

### Pines de señal
Cada servo que usemos hay que crearle un define con su pin correspondiente:
```
#define SERVO1_PIN	PIN_A1
#define SERVO2_PIN	PIN_A2
#define SERVO3_PIN	PIN_A3
#define SERVO4_PIN	PIN_A4
```

### Modo de control de los servos
Si queremos el control directo sobre los servos podemos crea un define SERVO_DIRECT_POSITION, esto ahorra RAM ya que **Servo[].fin**, **Servo[].enabled**, **Servo[].vel**, **Servo[].contES**, **Servo[].dirPos**, **Servo[].stop** no se usan.  
Si no se crea este define, la libreria funciona por control indirecto.
```
#define SERVO_DIRECT_POSITION
```

### Señal invertida
Si nuestro circuito tiene algun tipo de adaptador de señal, conversor de niveles, etc. que invierte la señal del servo, podemos usar la constante:
```
#define SERVO_INVERT_SIGNAL
```
Esto hara que la señal salga invertida, y al pasar por el conversor de niveles la señal se desinvierta

### MIN y MAX del ancho de pulso
Se puede configurar un minimo y un maximo GLOBAL en uS para el movimiento de los servos. Esto limita el movimiento de los servos dentro de ese rango.
Los limites individuales de cada servo no podran ser mayores que los limites globales. Por defecto los min/max son 500 y 2500 respectivamente, si se quieren usar otros se pueden crear los siguientes define:
```
#define SERVO_POS_MIN		1000
#define SERVO_POS_MAX		2000
```

### Posiciones por defecto (control indirecto)
En control indirecto normalmente querremos que el servo se mueva entre dos posiciones.  
Por defecto las posiciones 1 y 2 de los servos son 700uS y 1700uS.
Si se quiere usar otras hay que crear los siguiente define con valores comprendidos entre SERVO_POS_MIN y SERVO_POS_MAX
```
#define SERVO_POS1		1200	//posicion 1 por defecto
#define SERVO_POS2		1900	//posicion 2 por defecto
```

### Velocidad de los servos (control indirecto)
La velocidad de los servos por defecto es la mas rapida, si se quiere usar otra crear el siguiente define con un valor entre 0 y (SERVO_NUM_VEL - 1).  
Esta es la velocidad *por defecto*, por software luego se puede modificar a otra.
```
#define SERVO_VEL_DEFAULT	3
```

### Energy save (control indirecto)
Cuando EnergySave esta activado, cuando se mueve el servo, y este llega a su posicion final, espera TIEMPO_ENERGY_SAVE (mS) y luego desactiva el PWM. Asi el servo pasa a consumir menos energia, y tambien libera la fuerza del brazo, por lo que es posible moverlo se su posicion aplicando fuerza sobre el.

EnergySave por defecto esta activado, pero se puede desactivar creando el siguiente define:
```
#define ENERGY_SAVE_DEFAULT	FALSE
```

Si EnergySave esta activado y una vez que el servo llego a la posicion final comienza un temporizador que una vez finalizado deja de enviar señal al servo.
El tiempo por defecto es de 1000mS, pero se puede cambiar creando el siguiente define:
```
#define TIEMPO_ENERGY_SAVE	500
```

## VARIABLES

### Control directo
*Servo[]** (6 bytes)
```
Servo[num]
	.pos: posicion actual del servo					(2 bytes)
	.min: posicion minima del servo					(2 bytes)
	.max: posicion maxima del servo					(2 bytes)
```

### Control indirecto
**Servo[]** (10 bytes)
```
Servo[num]
	.pos: posicion actual del servo					(2 bytes)
	.fin: posicion a la que queremos llegar				(2 bytes)
	.min: posicion minima del servo					(2 bytes)
	.max: posicion maxima del servo					(2 bytes)
	.contES: variable para contar el energy save			(1 bytes)

	.enabled: flag para saber si el servo se esta moviendo		(1 bit)
	.ES: flag para saber si el servo funciona con EnergySave	(1 bit)
	.vel: velocidad del servo, del 1 al 8				(3 bits)
	.stop: indica si el servo ha llegado al fin			(1 bit)
	.dirPos: indica hacia donde se mueve (S_MIN / S_MAX)		(1 bit)
	.nul: este bit no se usa					(1 bits)
```

**ServoVel[0-7]** es un array constante que contiene los valores de las 8 velocidades disponibles (posiciones 0 a 7)

**flagServoRefresh** indica si hay que modificar el pulso de los servos.

**ServoFrame** nos indica que servo estamos controlando.


## FUNCIONES

**Servo_init()**: inicializa la libreria
		
**Servo_Config(int num, long min, long max, int vel, short es)**: Configura el servo  
		num: numero del servo que estamos configurando (entre 0 y NUM_SERVOS-1)  
		min: valor minimo para el servo (en uS)  
		max: valor maximo para el servo (en uS)  
		vel: (control indirecto) velocidad a la que se mueve (de 0 a 7)  
		es: (control indirecto) EnergySave activo o no

**Servo_Active(int num, short en)**: (control indirecto) activa o desactiva el envio de pulsos al servo  
		num: dice a que servo va dirigido  
		en: dice si lo activamos (true) o desactivamos (false)

**Servo_Mover(int num, long pos)**: cambia el servo de posicion. Asumimos que los valores que nos dan estan dentro de los rangos admitidos.  
		num: dice a que servo va dirigido  
		pos: dice la posicion del servo a la que queremos ir

**Servo_Refresh_Pos()**: (control indirecto) actualiza la posicion de los servos. Esta rutina hay que ponerla en el main para llamarla constantemente y que actualice los valores de  los servos. Usar solo con SERVO_INDIRECT_POSITION

## POSICIONAR SERVO LEYENDO UN POTENCIOMETRO
Los calculos EXACTOS para obtener la posicion del servo segun un valor del ADC:
```
RANGO_SERVOS = SERVO_POS_MAX - SERVO_POS_MIN
Pos = SERVO_POS_MIN + (RANGO_SERVOS / ADC_MAX_VALUE) * ADC)
```
Este calculo implica utilizar FLOAT, que es muy ineficiente y consume mucha ROM.

Tambien podriamos alterar el orden y obtener el mismo resultado usando int32:
```
Pos = SERVO_POS_MIN + ((RANGO_SERVOS * ADC) / ADC_MAX_VALUE)
```
Asi conservamos la precision. Pero la division es un calculo que hay que hacer durante la ejecucion, y consume mas tiempo y ROM (aunque menos que con FLOAT).  
Podemos hacer una aproximacion que es mas eficiente, y con poco error:
```
Pos = SERVO_POS_MIN + ((RANGO_SERVOS * ADC) >> 8)
```
Esto es equivalente a dividir entre 256, en vez de dividir entre 255 como deberiamos. Esto da un error maximo de 8, proporcional al valor del ADC.  
Para distribuir el error a lo largo de todo el rango incrementaremos el valor de SERVO_POS_MAX. Al hacerlo asi obtenemos una desviacion maxima de 1 sobre el valor "real" y queda distribuido alternamente en todo el rango del ADC.  
Para calcular el valor de SERVO_POS_MAX podemos usar la hoja de calculo o la siguiente formula:
```
m = Valor deseado para SERVO_POS_MIN
M = Valor deseado para SERVO_POS_MAX
A = Valor maximo del ADC
X = Valor que tenemos que usar en SERVO_POS_MAX, para que los calculos de multiplicar el ADC maximo nos de M como resultado. Usar el siguiente entero del resultado.

X = (M - m) * 256 / A + m

Ej:
m = 500
M = 2500
A = 255
X = (2500 - 500) * 256 / 255 + 500 = 2507.84 -> 2508
```

## RECURSOS USADOS

Necesita el **Timer1** y el modulo **CCP1** para funcionar


## EJEMPLO CONTROL DIRECTO
```
#define SERVO1_PIN	PIN_A2			//pin servo

#define SERVO_DIRECT_POSITION		//control directo
#define NUM_SERVOS	1				//solo usamos un servo

#define SERVO_POS_MIN		500		//valor minimo del pulso en uS
#define SERVO_POS_MAX		2500	//valor maximo del pulso en uS

#include "..\servo.c"

void main (void){
	Servo_Config(0, SERVO_POS_MIN, SERVO_POS_MAX);
	Servo_init();					//inicializo libreria
	
	do{
		int val = read_adc();		//lee ADC (valor invertido)
		
		Servo_Mover_Pot(0, val);	//mueve el servo a la posicion correspondiente del potenciometro
		delay_ms(100);
		
	}while(true);
}
```

## EJEMPLO CONTROL INDIRECTO
```
#define SERVO1_PIN	PIN_A1			//pin servo 1
#define SERVO2_PIN	PIN_A2			//pin servo 2
#define SERVO3_PIN	PIN_A4			//pin servo 3
#define SERVO4_PIN	PIN_A5			//pin servo 4

#define SERVO_INDIRECT_POSITION		//control indirecto
#define NUM_SERVOS	4				//usamos 4 servos

#define SERVO_POS_MIN		500		//valor minimo del pulso en uS
#define SERVO_POS_MAX		2500	//valor maximo del pulso en uS

#include "..\servo.c"

int x = 0;							//variable para seleccionar servo

void main (void){
	Servo_Config(0, 500, 1500, 1, TRUE);
	Servo_Config(1, 1000, 2000, 4, TRUE);
	Servo_Config(2, 1500, 2500, 6, TRUE);
	Servo_Config(3, 2000, 1000, 7, TRUE);
	
	Servo_init();							//inicializo servo
	
	//alterna cada 2 segundos la posicion de cada servo (siempre que este haya llegado al extremo)
	do{
		if(time++ > 2000){
			time = 0;
			
			if(Servo[x].dirPos == S_MIN){
				Servo_Mover(x, Servo[x].max);
			}
			else{
				Servo_Mover(x, Servo[x].min);
			}
					
			x++;
		
			if(x == NUM_SERVOS)
				x = 0;
		}
		
		Servo_Refresh_Pos();
		delay_ms(1);
		
	}while(TRUE);
}
```