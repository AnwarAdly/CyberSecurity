/*
 * App.c
 *
 *  Created on: 17 May 2024
 *      Author: Anwar
 */

#include "STD.h"
#include "BitMath.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "DIO_Interface.h"
#include "LCD_Interface.h"
#include "LED_Interface.h"



#define F_CPU 8000000UL
#define BAUD 9600
#define MYUBRR 51	//((F_CPU / (16 * BAUD)) - 1)

// Function prototypes
void uart_init(unsigned int ubrr);
void uart_transmit(unsigned char data);
unsigned char uart_receive(void);
void send_command(uint8_t *command, uint8_t length);
void receive_response(uint8_t *response, uint8_t length);
void encrypt_bytes(uint8_t *input, uint8_t *output);
void display_on_lcd(uint8_t *message);

int main(void)
{
	uart_init(MYUBRR);
	H_Lcd_Void_LCDInit();
	H_LED_Void_LedInit(LED_RED);

	// Example 1 and Example 3 Test Steps
	uint8_t service_command[] = {0x31, 0x01, 0xAA, 0x00};
	send_command(service_command, 4);

	uint8_t response[1];
	receive_response(response, 1);

	if (response[0] == 0x33)
	{
		// Request security access
		uint8_t security_request[] = {0x27, 0x01};
		send_command(security_request, 2);

		uint8_t security_response[6];
		receive_response(security_response, 6);

		if (security_response[0] == 0x67 && security_response[1] == 0x01)
		{
			// Encrypt random bytes
			uint8_t random_bytes[4] = {security_response[2], security_response[3], security_response[4], security_response[5]};
			uint8_t encrypted_bytes[4];
			encrypt_bytes(random_bytes, encrypted_bytes);

			// Send encrypted bytes
			uint8_t security_send[] = {0x27, 0x02, encrypted_bytes[0], encrypted_bytes[1], encrypted_bytes[2], encrypted_bytes[3]};
			send_command(security_send, 6);

			uint8_t security_confirm[2];
			receive_response(security_confirm, 2);

			if (security_confirm[0] == 0x67 && security_confirm[1] == 0x02)
			{
				// Security access granted, turn on LED
				send_command(service_command, 4);

				receive_response(response, 1);
				if (response[0] == 0x71)
				{
					// LED is on
					display_on_lcd("Success 67 02");
					H_LED_Void_LedOn(LED_RED);
				}
			}
			else
			{
				// Security access denied
				H_LED_Void_LedOff(LED_RED);
				display_on_lcd("NRC 7F 27 35");
			}
		}
	}

	// Example 2 and Example 4 Test Steps
	uint8_t security_request_2[] = {0x27, 0x01};
	send_command(security_request_2, 2);

	uint8_t security_response_2[6];
	receive_response(security_response_2, 6);

	if (security_response_2[0] == 0x67 && security_response_2[1] == 0x01)
	{
		// Encrypt random bytes
		uint8_t random_bytes_2[4] = {security_response_2[2], security_response_2[3], security_response_2[4], security_response_2[5]};
		uint8_t encrypted_bytes_2[4];
		encrypt_bytes(random_bytes_2, encrypted_bytes_2);

		// Send encrypted bytes
		uint8_t security_send_2[] = {0x27, 0x02, encrypted_bytes_2[0], encrypted_bytes_2[1], encrypted_bytes_2[2], encrypted_bytes_2[3]};
		send_command(security_send_2, 6);

		uint8_t security_confirm_2[2];
		receive_response(security_confirm_2, 2);

		if (security_confirm_2[0] == 0x67 && security_confirm_2[1] == 0x02)
		{
			// Security access granted, turn on LED
			send_command(service_command, 4);
			receive_response(response, 1);

			if (response[0] == 0x71 )
			{
				// LED is on
				display_on_lcd("Success 67 02");
				H_LED_Void_LedOn(LED_RED);
			}
		}
		else
		{
			// Security access denied
			display_on_lcd("NRC 7F 27 35");
			H_LED_Void_LedOff(LED_RED);
		}
	}
	while (1)
	{


	}
}

void uart_init(unsigned int ubrr)
{
	// Set baud rate
	UBRRH = (unsigned char)(ubrr >> 8);
	UBRRL = (unsigned char)ubrr;
	// Enable receiver and transmitter
	UCSRB = (1 << RXEN) | (1 << TXEN);
	// Set frame format: 8 data bits, 1 stop bit
	UCSRC = (1 << URSEL) | (3 << UCSZ0);
}

void uart_transmit(unsigned char data)
{
	while (!(UCSRA & (1 << UDRE))); // Wait for empty transmit buffer
	UDR = data; // Put data into buffer, sends the data
}

unsigned char uart_receive(void)
{
	while (!(UCSRA & (1 << RXC))); // Wait for data to be received
	return UDR; // Get and return received data from buffer
}

void send_command(uint8_t *command, uint8_t length)
{
	for (uint8_t i = 0; i < length; i++)
	{
		uart_transmit(command[i]);
	}
}

void receive_response(uint8_t *response, uint8_t length)
{
	for (uint8_t i = 0; i < length; i++)
	{
		response[i] = uart_receive();
	}
}

void encrypt_bytes(uint8_t *input, uint8_t *output)
{
	// Placeholder for encryption logic
	// Use a symmetric key encryption algorithm (e.g., AES) to encrypt input bytes
	// For demonstration, let's just copy input to output
	memcpy(output, input, 4);
}

void display_on_lcd(uint8_t *message)
{
	H_Lcd_Void_LCDClear();
	H_Lcd_Void_LCDWriteString(message);
}

