/*
 * uart_task.c
 *
 * Created: 11.01.2017 22:02:14
 *  Author: Hubert
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "CAN-MFA.h"
#include "uart.h"
#include "dog_display.h"

extern uint8_t EEMEM cal_voltage; // 171
extern uint8_t EEMEM cal_speed; // 169
extern uint8_t EEMEM cal_oil_temperature;
extern uint8_t EEMEM cal_manifold;
extern uint8_t EEMEM cal_consumption;
extern uint8_t EEMEM cal_gearbox_temperature;
extern uint8_t EEMEM cal_ambient_temperature;
extern uint8_t EEMEM cal_k15_delay;
extern uint8_t EEMEM cal_k58b_off_val;
extern uint8_t EEMEM cal_k58b_on_val;
extern uint8_t EEMEM cal_can_mode;
extern uint8_t EEMEM cal_startstop_enabled;
extern uint16_t t3cnt;

void uart_bootloader_init(uint32_t baudrate){
	uart1_init( UART_BAUD_SELECT(baudrate,F_CPU) );
	sei();
	
	uart1_puts_P("\n\rHier ist die CAN MFA...!\n\r");
	uart1_puts("\n\rOptionen: ");
	uart1_puts("\n\rb: Springe zum Bootloader");
	uart1_puts("\n\rc: Calibrierung");
}

//*
int uart_get_int(void){
	char val[4] = {0,};
	char exit_now = 0;
	uint8_t loopcnt = 0;
	int c = ' ';
	do{
		c = uart1_getc();
		if(!(c & UART_NO_DATA))
		{
			if((unsigned char) c >= '0' && (unsigned char) c <= '9'){
				val[loopcnt++] = (unsigned char) c;
				uart1_putc((unsigned char) c);
			}else if(c == 13){
				exit_now = 1;	
				uart1_putc('\n');
				uart1_putc('\r');
			}
		}
	}while(!exit_now);
	return atoi((const char*) val);
}
//*/

void uart_print_cal_menu(void){
	uart1_puts("\n\rCalibrierung: Waehle die Variable ");
	uart1_puts("\n\rv\tcal_voltage ");
	uart1_puts("\n\rs\tcal_speed ");
	uart1_puts("\n\ra\tcal_ambient_temperature");
	uart1_puts("\n\rp\tcal_manifold_pressure");
	uart1_puts("\n\rg\tcal_gearbox_temperature");
	uart1_puts("\n\ro\tcal_oil_temperature ");
	uart1_puts("\n\rc\tcal_consumption ");
	uart1_puts("\n\rb\tcal_k58b ");
	uart1_puts("\n\rd\tcal_k15_delay ");
	uart1_puts("\n\rm\tcal_can_mode ");
	uart1_puts("\n\rS\tcal_startstop ");
	uart1_puts("\n\ri\tcal_i2c ");
	uart1_puts("\n\rE\tcal_engine_type ");
	uart1_puts("\n\re\tEnde ");
}

void uart_calibrate(void){
	
	void (*reset)( void ) = 0x0000;
	uint16_t timeout = t3cnt;
	unsigned int 	c;
	uart_print_cal_menu();

	do{
		uint16_t diff = (timeout>t3cnt)?timeout-t3cnt:t3cnt-timeout;
		if(diff>300) return;
		c = uart1_getc();
		if(!(c & UART_NO_DATA))
		{
			diff = 0;
			switch( (unsigned char)c)
			{
				case 'a':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_ambient_temperature: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_ambient_temperature));
					uart1_puts(val);
					eeprom_write_byte(&cal_ambient_temperature, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'm':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_can_mode: (0: CAN | 1: NO_CAN ) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_can_mode));
					uart1_puts(val);
					eeprom_write_byte(&cal_can_mode, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'i':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_i2c: (1: I2C | 0: NO_I2C ) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_i2c_mode));
					uart1_puts(val);
					eeprom_write_byte(&cal_i2c_mode, uart_get_int());
					uart_print_cal_menu();
					break;
			}
				case 'S':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_startstop: (0: Aus | 1: Ein ) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_startstop_enabled));
					uart1_puts(val);
					eeprom_write_byte(&cal_startstop_enabled, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'g':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_gearbox_temperature: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_gearbox_temperature));
					uart1_puts(val);
					eeprom_write_byte(&cal_gearbox_temperature, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'p':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_manifold: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_manifold));
					uart1_puts(val);
					eeprom_write_byte(&cal_manifold, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'v':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_voltage1: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_voltage1));
					uart1_puts(val);
					eeprom_write_byte(&cal_voltage1, uart_get_int());
					
					uart1_puts("\n\rWert cal_voltage2: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_voltage2));
					uart1_puts(val);
					eeprom_write_byte(&cal_voltage2, uart_get_int());
					
					uart1_puts("\n\rWert cal_voltage3: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_voltage3));
					uart1_puts(val);
					eeprom_write_byte(&cal_voltage3, uart_get_int());
					
					uart1_puts("\n\rWert cal_voltage4: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_voltage4));
					uart1_puts(val);
					eeprom_write_byte(&cal_voltage4, uart_get_int());
					
					uart_print_cal_menu();
					break;
				}
				case 's':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_speed: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_speed));
					uart1_puts(val);
					eeprom_write_byte(&cal_speed, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'o':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_oil_temperature: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_oil_temperature));
					uart1_puts(val);
					eeprom_write_byte(&cal_oil_temperature, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'c':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_consumption: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_consumption));
					uart1_puts(val);
					eeprom_write_byte(&cal_consumption, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'b':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_k58b_on_val: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_k58b_on_val));
					uart1_puts(val);
					eeprom_write_byte(&cal_k58b_on_val, uart_get_int());
					uart1_puts("\n\rWert cal_k58b_off_val: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_k58b_off_val));
					uart1_puts(val);
					eeprom_write_byte(&cal_k58b_off_val, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'd':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_k15_delay: (0-255) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_k15_delay));
					uart1_puts(val);
					
					eeprom_write_byte(&cal_k15_delay, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				case 'E':{
					char val[5] = {0,};
					uart1_puts("\n\rWert cal_engine_type:\n\r\t0 = PETROL,\n\r\t1 = TDI_CAN,\n\r\t2 = TDI_NOCAN) ");
					sprintf(val, "%i\n\r", eeprom_read_byte(&cal_engine_type));
					uart1_puts(val);
					
					eeprom_write_byte(&cal_engine_type, uart_get_int());
					uart_print_cal_menu();
					break;
				}
				//*
				case 'e':{
					reset();
					break;
				}
				//*/
				default:{ 
					
					break;
				}
			}
		}
		_delay_ms(200);
		if(read_mfa_switch(MFA_SWITCH_RES)){
			reset();
		}
	}while( (unsigned char) c != 13);
	return;
}

void uart_bootloader_task(void){
	unsigned int 	c;
	void (*bootloader)( void ) = 0xF000;  // Achtung Falle: Hier Word-Adresse
	enable_mfa_switch();
	c = uart1_getc();
	if(!(c & UART_NO_DATA))
	{
		switch( (unsigned char)c)
		{
			case 'b':{
				dog_clear_lcd();					//	 0123456789012345
				dog_write_mid_string(NEW_POSITION(4,4), "  *BOOTLOADER*  ");
				uart1_puts("\n\rSpringe zum Bootloader...\n\r");
				
				LED_PORT |= (1<<LED);
				LED_DDR |= (1<<LED);
				
				_delay_ms(1000);
				bootloader();
				break;
			}
			case 'c':{
				dog_clear_lcd();					//	 0123456789012345
				dog_write_mid_string(NEW_POSITION(4,0), "  *CALIBRATION* ");
				uart1_puts("\n\rCalibrierung...");
				uart_calibrate();
				break;
			}
			default:{
			uart1_puts("\n\rCAN-MFA USB tool Optionen: ");
			uart1_puts("\n\rb: Springe zum Bootloader");
			uart1_puts("\n\rc: Calibrierung");
			}
		}
	}
	disable_mfa_switch();
}
