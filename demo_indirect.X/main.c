#include <16F1825.h>
#device adc=8
#use delay(clock=32000000)

#FUSES INTRC_IO, NOWDT, PUT, NOMCLR, NOPROTECT, NOCPD, BROWNOUT, NOCLKOUT, NOIESO, NOFCMEN
#FUSES NOWRT, PLL_SW, NOSTVREN, BORV19, NODEBUG, NOLVP

#use fast_io(a)               //se accede al puerto a como memoria
#use fast_io(c)               //se accede al puerto c como memoria

#byte PORTA	= getenv("SFR:PORTA")
#byte PORTC	= getenv("SFR:PORTC")

/* PORT A */
//pins
#bit A0			= PORTA.0			//
#bit SERVO1		= PORTA.1			//servo 1
#bit SERVO2		= PORTA.2			//servo 2
#bit A3			= PORTA.3			//
#bit SERVO3		= PORTA.4			//servo 3
#bit SERVO4		= PORTA.5			//servo 4

//defines
#define P_A0		PIN_A0			//I
#define SERVO1_PIN	PIN_A1			//O
#define SERVO2_PIN	PIN_A2			//O
#define P_A3		PIN_A3			//I
#define SERVO3_PIN	PIN_A4			//O
#define SERVO4_PIN	PIN_A5			//O

//Bits			    543210
#define TRIS_A	0b00001001			//define cuales son entradas y cuales salidas
#define WPU_A	0b00000000			//define los weak pull up

/* PORT C */
//pins
#bit C0			= PORTC.0			//
#bit C1			= PORTC.1			//
#bit C2			= PORTC.2			//
#bit C3			= PORTC.3			//
#bit C4			= PORTC.4			//
#bit C5			= PORTC.5			//

//defines
#define P_C0		PIN_C0			//I
#define P_C1		PIN_C1			//I
#define P_C2		PIN_C2			//I
#define P_C3		PIN_C3			//I
#define P_C4		PIN_C4			//I
#define P_C5		PIN_C5			//I

//Bits			    543210
#define TRIS_C	0b00111111			//define cuales son entradas y cuales salidas
#define WPU_C	0b00000000			//define los weak pull up

/* VARIABLES */
int x = 0;							//variable para seleccionar servo
long time = 0;

/* CONSTANTES GENERALES */
#define DEBUG

/* CONSTANTES PARA LIBRERIA SERVOS */
#define SERVO_INDIRECT_POSITION
#define NUM_SERVOS	4

#define SERVO_POS_MIN		500		//valor minimo del pulso en uS
#define SERVO_POS_MAX		2500	//valor maximo del pulso en uS (se utiliza este valor para compensar la division entre 256)

#include "..\servo.c"

void main(void){
	setup_oscillator(OSC_8MHZ|OSC_PLL_ON);	//configura oscilador interno
	setup_wdt(WDT_OFF);						//configuracion wdt
	//setup_timer_0(T0_INTERNAL|T0_DIV_1);	//
	//setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);	//
	//setup_timer_2(T2_DIV_BY_1,255,1);		//
	setup_dac(DAC_OFF);						//configura DAC
	setup_adc(ADC_CLOCK_INTERNAL);			//configura ADC
	setup_adc_ports(NO_ANALOGS);			//canal ADC
	//setup_ccp1(CCP_COMPARE_INT);			//
	setup_spi(SPI_DISABLED);				//configura SPI
	//setup_uart(FALSE);					//configura UART
	setup_vref(VREF_OFF);					//no se usa voltaje de referencia
	setup_comparator(NC_NC_NC_NC);			//comparador apagado
	
	//enable_interrupts(GLOBAL);			//
	
	set_tris_a(TRIS_A);						//configura pines
	port_a_pullups(WPU_A);					//configura weak pull ups
	
	set_tris_c(TRIS_C);						//configura pines
	port_c_pullups(WPU_C);					//configura weak pull ups
	
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