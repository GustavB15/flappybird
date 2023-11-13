/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog

   For copyright and licensing, see file COPYING 
   
   This file was last modified 2023-03-02 by Gustav Björs and Samuel Bergsten Grip
   Any code that was written by us is prefaced by a comment saying so */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

/* Interrupt Service Routine */
void user_isr( void )
{
	if(IFS(0) & 0x100){ // Vi kollar om timern har triggat en interrupt och fortsätter om den har det
		IFSCLR(0) = 0x100; // Vi sätter flaggan för interrupt till 0 för att återställa den
		gameInit(); // Funktionen gameInit kommer endast att göra något om värdet på integern score är 0, alltså om vi startat om spelet
		runGame(); // Funktionen runGame kallar på alla de funktioner vi behöver använda varenda gång vi uppdaterar spelet
	}
}


/* Lab-specific initialization goes here */
void labinit( void )
{
	restart(); // Vi kallar restart-funktionen i börjar av programmet för att förbereda spelplanet
	TRISD = 0xff0;
	
	T2CON = 0x0;
	T2CONSET = 0x70;
	TMR2 = 0x0;
	PR2 = 80000000 / (256 * 20); // Vår standard är 256 * 20, ändra denna för att modifiera hastigheten av spelet, eftersom att även fågeln går på denna hastighet så kommer detta även påverka hur snabbt fågeln faller
	T2CONSET = 0x8000;
	
	IPC(2) = 0x10;
	IEC(0) = 0x100;
	enable_interrupt();
  return;
}

/* This function is called repetitively from the main program */
void labwork( void )
{
	int btns = getbtns();
	
	if (btns & 0x8){
		shiftup(); // Om man trycker på BTN4 så kallas funktionen shiftup, så att fågeln hoppar
		while (btns & 0x8){ // För att undvika att shiftup kallas flera gånger i rad så använder vi en while-loop som väntar tills knappen släpps innan den fortsätter
			btns = getbtns();
		}
		delay(20); // Vi använder en kort delay efteråt för att minska risken att system tror att vi tryckt knappen igen när vi släpper den
	}
	
	if (btns & 0x4){ // Om man trycket på BTN3 så kallas funktionen waveUse
		waveUse(); // Till denna funktion så använder vi inte en while-loop eftersom att funktionen waveUse har en intervall då den inte gör något efter man kallat den en gång
	}
	
	if (btns & 0x2){ // Om man trycker på BTN2 så startar vi om programmet med funktionen restart
		restart(); 
		while (btns & 0x2){ // Här använder vi en while-loop igen så att programmet inte konstant försöker omstartar om man hade hållt in knappen
			btns = getbtns();
		}
	}
}
