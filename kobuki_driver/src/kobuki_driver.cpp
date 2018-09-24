/*====================================================================================================*/
/* Kobuki driver                                                                                      */
/* Using:                                                                                             */
/* Serial Port Programming in C - Non Cannonical mode                                                 */
/*----------------------------------------------------------------------------------------------------*/
/* Program ... <brief description>                                                                    */
/*----------------------------------------------------------------------------------------------------*/
/* Compiler/IDE  : gcc 4.6.3                                                                          */
/* Library       :                                                                                    */
/* Commands      : gcc -o kobuki_driver kobuki_driver.c                                               */
/* OS            : Linux(x86) (Linux Mint 13 Maya)(Linux Kernel 3.x.x)                                */                              
/* Programmer    : Anderson Domingues & Darlan Alves Jurak                                            */
/* Date	         : 24 September 2018                                                                  */
/*====================================================================================================*/

/*====================================================================================================*/
/* Basic serial communication from:  					                                              */
/* www.xanthium.in										                                              */
/* Copyright (C) 2014 Rahul.S                                                                         */
/*====================================================================================================*/

/*====================================================================================================*/
/* Running the executable                                                                             */
/* ---------------------------------------------------------------------------------------------------*/ 
/* 1) Compile the  kobuki_driver.c  file using gcc on the terminal (without quotes)                   */
/*                                                                                                    */
/*	" gcc -o kobuki_driver kobuki_driver.c "                                                          */
/*                                                                                                    */
/* 2) Linux will not allow you to access the serial port from user space,you have to be root.So use   */
/*    "sudo" command to execute the compiled binary as super user.                                    */
/*                                                                                                    */
/*       "sudo ./kobuki_driver"                                                                       */
/*                                                                                                    */
/*====================================================================================================*/


	/* PACOTE QUE TOCA MUSIQUINHA */
    //unsigned int packet_size = 7;
    // packet[0] = 0xAA;
    // packet[1] = 0x55;
    // packet[2] = 0x03;
    // packet[3] = 0x04;
    // packet[4] = 0x01;
    // packet[5] = 0x04;


/*====================================================================================================*/
/* Sellecting the Serial port Number on Linux                                                         */
/* ---------------------------------------------------------------------------------------------------*/ 
/* /dev/ttyUSBx - when using USB to Serial Converter, where x can be 0,1,2...etc                      */
/* /dev/ttySx   - for PC hardware based Serial ports, where x can be 0,1,2...etc                      */
/*====================================================================================================*/

/*-------------------------------------------------------------*/
/* termios structure -  /usr/include/asm-generic/termbits.h    */ 
/* use "man termios" to get more info about  termios structure */
/*-------------------------------------------------------------*/

	#include <cstring>
	#include <iostream>
    #include <stdio.h>
    #include <fcntl.h>   /* File Control Definitions           */
    #include <termios.h> /* POSIX Terminal Control Definitions */
    #include <unistd.h>  /* UNIX Standard Definitions 	   */ 
    #include <errno.h>   /* ERROR Number Definitions           */
    #include <stdint.h>

using namespace std;

void SerialConfig(int*);
void MountPacket_Twist(unsigned char*, unsigned int, int);

int main(void)
{
    int             fd;
    int             bytes_written; 
    unsigned int    packet_size = 13;       // PT
	unsigned char   packet[packet_size];
    
    SerialConfig(&fd);                      // Serial configuration
    MountPacket_Twist(packet, packet_size, 5);        //
	        
    for (int i = 0; i < 100; i++){   
 
        bytes_written = write(fd, packet, packet_size);
        // printf("written %d bytes\n", bytes_written);
        
        sleep(3);
    }

    close(fd); /* Close the serial port */

	return 0;

}

void SerialConfig(int* fd){

    printf("\n +----------------------------------+");
    printf("\n |        Kobuki Driver             |");
    printf("\n +----------------------------------+");

    *fd = open("/dev/ttyUSB0",O_WRONLY | O_NOCTTY | O_NONBLOCK | O_NDELAY);	
                            
	if (fd < 0)
		cout << "Error " << errno << " opening " << "/dev/ttyUSB0" << ": " << strerror (errno) << endl;

    struct termios tty;	                        /* Create the structure                          */
    tcgetattr(*fd, &tty);	                    /* Get the current attributes of the Serial port */

    /* Setting the Baud rate */         
    cfsetispeed(&tty, B115200);                 /* Set Read  Speed as 9600                       */
    cfsetospeed(&tty, B115200);                 /* Set Write Speed as 9600                       */

    /* 8N1 Mode */          
    tty.c_cflag &= ~PARENB;                     /* Disables the Parity Enable bit(PARENB),So No Parity   */
    tty.c_cflag &= ~CSTOPB;                     /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
    tty.c_cflag |=  CS8;                        /* Set the data bits = 8                                 */

    if((tcsetattr(*fd, TCSANOW, &tty)) != 0)    /* Set the attributes to the termios structure*/
        printf("\n  ERROR ! in Setting attributes");
    else
        printf("\n  BaudRate = 115200 \n  StopBits = 1 \n  Parity   = none");
        
    tcflush(*fd, TCIFLUSH);                     /* Discards old data in the rx buffer            */
}

void MountPacket_Twist(unsigned char* packet, unsigned int packet_size, int velocity){

    unsigned char packet_checksum = 0;

    /* PACOTE QUE FAZ MOVER */
    packet[0] = 0xAA; // Header 0
    packet[1] = 0x55; // Header 1
    packet[2] = 0x09; // Lenght
    
    //payload move
    packet[3] = 0x01; // Sub-Payload 0 - Header
    packet[4] = 0x04; // Sub-Payload 1 - Lenght

    //payload musiquinha
    packet[9] = 0x04;
    packet[10] = 0x01;
    packet[11] = 0x02;

    //packet[5] = 0x90; // Sub-Payload 2 - Speed
    //packet[6] = 0xFF; // Sub-Payload 3 - Speed
    uint16_t* vel = (uint16_t*)&packet[5];
    *vel = velocity * 50;
    
    //packet[7] = 0x70; // Sub-Payload 4 - Radius
    //packet[8] = 0x20; // Sub-Payload 5 - Radius
    uint16_t* rad = (uint16_t*)&packet[7];
    // *rad = i * 50;
    *rad = 5 * 0;

    packet_checksum = 0;
    for (unsigned int i = 2; i < packet_size; i++)
        packet_checksum ^= packet[i];

    packet[packet_size -1] = packet_checksum;

}