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
- Se pueden controlar hasta 4 servos. Para ello configuramos el timer1 para que interrumpa cada 5000uS. Cada servo tiene su propio (siempre el mismo) "marco de tiempo" dentro del cual se pone a HIGH y luego a LOW su PWM. Es el CCP1 el encargado de poner a LOW el PWM. Al haber 4 marcos de tiempo, los PWM de los servos siempre se actualizan correctamente a los 20mS necesarios. No importa si usamos 1 o mas servos, los 4 marcos de tiempo se mantienen.
- Las posiciones de los servos siempre se establecen en micro segundos.
- La libreria se puede usar de dos maneras:
  1. **Control directo:** permite controlar la posicion del servo instantaneamente. Se le da la posicion y la libreria mueve el servo a dicha direccion.
  2. **Control indirecto:** cuado se manda a mover a un servo a una posicion, la libreria gestiona el movimiento de manera que se adapte a una velocidad configurable. No se controla la velocidad realmente, sino que se posiciona al servo en diferentes posiciones internmedias entre el inicio y el final y esto
produce una modificacion de la velocidad.
- El ahorro de energia (EnergySave) se consigue dejando de enviar pulsos de PWM al servo. Esto reduce el consumo en la mayoria de los servos ya que este deja de intentar posicionarse constantemente.
- Podemos trabajar con el rango de posiciones comprendidas entre min y max, o podemos usar un rango diferente que se adapte a otras necesidades, por ejemplo los valores de un potenciometro.


## COMO CONFIGURAR LA LIBRERIA?

### Servos usados
Hay que crear un define con la cantidad de servos a usar:
```
define NUM_SERVOS	4
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
Si queremos el control directo sobre los servos podemos crea un define SERVO_DIRECT_POSITION, esto ahorra RAM ya que Servo[].fin, .enabled, .vel, .contES, .dirPos, .stop no se usan
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

### Posiciones por defecto
Por defecto las posiciones 1 y 2 de los servos son 700uS y 1700uS.
Si se quiere usar otras hay que crear los siguiente define con valores comprendidos entre SERVO_POS_MIN y SERVO_POS_MAX
```
#define SERVO_POS1		1200	//posicion1 por defecto
#define SERVO_POS2		1900	//posicion2 por defecto
```

### Velocidad de los servos
La velocidad de los servos por defecto es la mas rapida, si se quiere usar otra crear el siguiente define con un valor entre 0 y (SERVO_NUM_VEL - 1)
```
#define SERVO_VEL_DEFAULT	3
```

### Energy save
EnergySave por defecto esta activado, pero se puede desactivar creando el siguiente define:
```
#define ENERGY_SAVE_DEFAULT	FALSE
```

Si EnergySave esta activado y una vez que el servo llego a la posicion final comienza un temporizador que una vez finalizado deja de enviar señal al servo.
El tiempo por defecto es de 1000mS, pero se puede cambiar creando el siguiente define:
```
#define TIEMPO_ENERGY_SAVE	500
```

### Rango
Si vamos a trabajar con un rango de posiciones diferente al de min/max podemos declarar RANGE_MAX_VAL. Por defecto este es igual al maximo valor del ADC interno (255/1023). Si queremos usar un rango diferente hay que declarar RANGE_MAX_VAL con el valor que necesitemos. Usar "NUM".0 para que el compilador entienda que queremos guardar los decimales, sino perderemos resolucion en el movimiento:
```
#define RANGE_MAX_VAL		(500.0)
```


## VARIABLES

Con SERVO_DIRECT_POSITION la variable Servo[] necesita 6 bytes
```
Servo[num]
	.pos: posicion actual del servo					(2 bytes)
	.min: posicion minima del servo					(2 bytes)
	.max: posicion maxima del servo					(2 bytes)
```

Con SERVO_INDIRECT_POSITION la variable Servo[] necesita 10 bytes
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

**Servo_Config(int num, int vel, long min, long max)**: Configura el servo
		num: numero del servo que estamos configurando (entre 0 y NUM_SERVOS-1)
		*vel: velocidad a la que se mueve (de 0 a 7)
		min: valor minimo para el servo (en uS)
		max: valor maximo para el servo (en uS)

**Servo_Active(int num, short en)**: ctiva o desactiva el envio de pulsos al servo
		num: dice a que servo va dirigido
		en: dice si lo activamos (true) o desactivamos (false)

**Servo_Mover(int num, long pos)**: cambia el servo de posicion. Asumimos que los valores que nos dan estan dentro de los rangos admitidos.
		num: dice a que servo va dirigido
		pos: dice la posicion del servo a la que queremos ir

**Servo_Refresh_Pos()**: actualiza la posicion de los servos. Esta rutina hay que ponerla en el main para llamarla constantemente y que actualice los valores de  los servos. Usar solo con SERVO_INDIRECT_POSITION


## OTRAS FUNCIONALIDADES

Si estamos usando SERVO_DIRECT_POSITION y queremos un control mas directo de los servos podemos escribir y leer directamente en Servo[x].pos/min/max en vez de usar las funciones Servo_Config() y Servo_Mover().

Cuando posicionamos un servo segun el valor de un potenciometro tenemos la constante RANGE_UNI que multiplicada por el valor del potenciometro nos dara una posicion del servo, dentro del rango MIN y MAX establecidos.
Ej:
```
long pos1 = SERVO_POS_MIN + (read_adc() * RANGE_UNI);	//leo adc y lo convierto en un valor entre min y max
Servo_Mover(0, pos1);									//muevo el servo con este valor
```


## RECURSOS USADOS

Necesita el **Timer1** y el modulo **CCP1** para funcionar