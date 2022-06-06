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
#bit SERIAL_TX	= PORTA.0			//serial out
#bit POT		= PORTA.1			//pot
#bit SERVO1		= PORTA.2			//servo
#bit A3			= PORTA.3			//
#bit A4			= PORTA.4			//
#bit A5			= PORTA.5			//

//defines
#define P_SERIAL_TX	PIN_A0			//O
#define P_POT		PIN_A1			//I
#define SERVO1_PIN	PIN_A2			//O
#define P_A3		PIN_A3			//I
#define P_A4		PIN_A4			//I
#define P_A5		PIN_A5			//I

#define POT_CH	1					//pot channel

//Bits			    543210
#define TRIS_A	0b00111010			//define cuales son entradas y cuales salidas
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

/* CONSTANTES GENERALES */
#define DEBUG

/* CONSTANTES PARA LIBRERIA SERVOS */
#define SERVO_DIRECT_POSITION
#define NUM_SERVOS	1

#define SERVO_POS_MIN		500		//valor minimo del pulso en uS
#define SERVO_POS_MAX		2508	//valor maximo del pulso en uS (se utiliza este valor para compensar la division entre 256)

#ifdef DEBUG
#use rs232(baud=250000, xmit=P_SERIAL_TX, ERRORS)
#endif

#include "..\servo.c"

void main(void){
	setup_oscillator(OSC_8MHZ|OSC_PLL_ON);	//configura oscilador interno
	setup_wdt(WDT_OFF);						//configuracion wdt
	//setup_timer_0(T0_INTERNAL|T0_DIV_1);	//
	//setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);	//
	//setup_timer_2(T2_DIV_BY_1,255,1);		//
	setup_dac(DAC_OFF);						//configura DAC
	setup_adc(ADC_CLOCK_INTERNAL);			//configura ADC
	setup_adc_ports(sAN1|VSS_VDD);			//canal ADC
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
	
	set_adc_channel(POT_CH);
	
	Servo_Config(0, SERVO_POS_MIN, SERVO_POS_MAX);
	Servo_init();							//inicializo servo
	
	do{		
		int val = read_adc();		//lee ADC
		
		Servo_Mover_Pot(0, val);	//mueve el servo a la posicion correspondiente del potenciometro
		Servo_Mover_Pot_Small(0, val);	//mueve el servo a la posicion correspondiente del potenciometro
		Servo_Mover_Pot(0, val);	//mueve el servo a la posicion correspondiente del potenciometro
		Servo_Mover_Pot_Small(0, val);	//mueve el servo a la posicion correspondiente del potenciometro
		
#ifdef DEBUG
		printf("Pot: %3u | Srv: %4Lu\r\n", val, Servo[0].pos);
#endif
		
		delay_ms(100);
		
	}while(TRUE);
}