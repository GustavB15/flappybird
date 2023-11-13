/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING 
   
   This file was last modified 2023-03-02 by Gustav Björs and Samuel Bergsten Grip
   Any code that was written by us is prefaced by a comment saying so */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

/* Declare a helper function which is local to this file */
static void num32asc( char * s, int ); 

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

// All the code between this point down to the shiftup function was written by Gustav and Samuel

int wave = 0; // Används i waveProgress för att bestämma vilken position vår wave ska ha, ett värde av 0 betyder att ingen wave är igång
int score = 0; // Förvarar spelarens score vilket används för att bestämma storleken på speed och printas även ut vid game over
int obstacleFreq = 0; // Ändra denna mellan 0 till 70 för att bestämma när först hindret kommer
int waveCount = 4; // Antalet gånger man kan använda wave-funktionerna
int gameover = 0; // 0 är falskt, 1 är sannt
int birdpos = 15; // Ändra denna för att bestämma vart fågeln börjar på x-axeln
int speed = 0; // Speed bestämmer hur ofta nya obstacles kommer att skapas
int i, j;

void runGame (){ 
	if (gameover == 0){
		rewrite();
		if (obstacleFreq == speed){ // Detta värde bestämmer frekvensen för hinderna
			gen(obstaclePos(), obstacleGap()); // Vi använder funktionerna obstaclePos och obstacleGap för att skapa ett värde för positionen av gapet på nästa hinder och dess storlek
			obstacleFreq = 0; // Vi sätter värdet på obstacleFreq till 0 så att vi kan räkna om det
			obstacleSpeed(); // Vi kallar på obstacleSpeed för att skapa ett nytt värde på speed efter i printat ut ett hinder
		}
		display_image(0, display);
		shiftdown(); // Skiftar ner fågeln på y-axeln med ett steg
		obstacleFreq++; // Vi inkrementerar obstacleFreq för att ta det närmre till värdet av speed
		score++; // Vi inkrementerar även värdet av score varenda gång vi kallar runGame när spelet är igång
	}
	else
		finish(); // Om variabeln gameover inte är 0, alltså om vi spelaren förlorat så fortsätter vi till funktionen finish
}

void gameInit() {
	if (score == 0){ // Vi använder denna funktion för att fylla skärmen och starta vår countdown
			rewrite(); // Vi börjar med att kalla rewrite för att skriva in fågeln och vår box in i vår display array
			display_image(0, display); // Visar upp display på den fysiska skärmen så att man kan se fågeln medan vi räknar ner
			countdown(); // Kallar på countdown-funktionen
		}
}

void rewrite (){ // Denna funktion kallar på alla de andra funktionerna vi behöver för att uppdatera spelet
	obstacleLeft(); // Vi flyttar våra obstacles till vänster
	border(127); // Vi skickar värdet 127 till border för att fylla i bara de pixlar längst till höger av vår box
	boxToDisplay(); // Vi kopierar innehållet av box till display
	if (wave > 0) // Om vi i funktionen waveUse givit wave värdet 1 så kommer denna kallas varenda gång vi uppdaterar
		waveProgress();
	writeBird(); // Vi skriver in fågeln på displayen
	sidePoints(); // Vi uppdaterar de punkter vi satt till höger om displayen
}

void finish (){ // Denna funktion kallas när man förlorat, den skriver bara text på skärmen
	display_string(0, "Game Over");
	display_string(1, "Score");
	display_string(2, itoaconv(score));
	display_update();
}

void restart (){ // Denna funktion används för att resetta spelet
	obstacleFreq = 70; // Återställer värdet av obstacleFreq till 70
	score = 0; // Återställer score till 0
	for (i = 0; i < 128; i++) // Vi loopar funktionen border för att tömma arrayen box från hinder
		border(i);
	for (i = 0; i < 4; i++) // Vi återställer sedan fågelns position till sin startposition
		for (j = 0; j < 8; j++)
			bird[i][j] = birdOriginal[i][j];
	gameover = 0; // Sätter integern gameover till 0 så att spelet kan börja igen
	wave = 0; // Sätter värdet wave till 0 för att radera alla waves på skärmen
	waveCount = 4; // Sätter integern waveCount till 4 för att återställa antalet mäjliga användningar av wave-funktionen
	speed = 70; // Återställer vår speed till 70
}

void boxToDisplay (){ // Funktion för att skriva över vårat register med hinder till displayen
	for (i = 0; i < 4; i++){ 
		for (j = 1; j < 119; j++){ // Vi använder värdet 1 som första och 119 så att vi får linjer på båda ändarna av displayen, den högra sidan har extra mycket plats för att visa vår waveCount
			display[i][j] = box[i][j];
		}
	}
}

void writeBird (){ // Funktion för att skriva in vår fågel på displayen
	for (i = 0; i < 4; i++){ 
		for (j = 0; j < 8; j++){
            if ( (bird[i][j] | box[i][birdpos+j]) != 255){ // Vi använder en if-statement för att se om fågeln kolliderar med något upplyst område, alltså några av våra obstacles eller border
                gameover = 1; // Om fågeln kolliderar med något så sätter vi variabeln gameover till 1, för att stoppa spelet
            }
			display[i][birdpos+j] &= bird[i][j]; // När vi skriver in fågeln på displayen så använder vi oss av en and operand så att fågeln inte skriver över värden över och under den
		}
	}
}

void border (int i){ // Funktion för att skriva in nya värden på vår box-array efter vi flyttat dess position, används även för att tömma vår box från hinder med restart-funktionen
	box[0][i] = 254; // Här fyller vi i toppenpixeln av vår box med vitt och lämnar resten svart
	box[1][i] = 255; // Vi låter båda mittenplanerna av vår box vara svart
	box[2][i] = 255;
	box[3][i] = 127; // Vi fyller i bottenpixeln med vitt och lämnar resten svart
}

void obstacleLeft (){ //Funktion för att flytta vårat register med hinder till vänster
	for (i = 0; i < 4; i++){
		for (j = 1; j < 129; j++){
			box[i][j-1] = box[i][j]; // Vi använder två for-loops för att flytta hela vår box-array ett steg till vänster
		}
	}
}

void sidePoints (){ // Skriver in 0 till 4 punkter på displayens högra sida, dessa används för vår countdown och våra waves
	for (i = 0; i < 4; i++)
		for (j = 120; j < 128; j++) // Vi tömmer den högra sidan av displayen så att det blir svart
			display[i][j] = 255;
		
	for (i = 0; i < waveCount; i++)
		for (j = 123; j < 127; j++) // Vi målar nu i ett antal punkter som är lika med wavecount, detta kan vara mellan 0 till 4 punkter
			display[i][j] = 195;
}

void countdown (){ // Räknar ner innan spelet börjar med hjälp av våra punkter till höger om displayen
	for (i = 3; i >= 0; i--){
		delay(1000); // Delay är inte väldigt effektivt men vi valde att använda det ändå
		for (j = 123; j < 127; j++){ 
			display[i][j] = 255; // Vi överskriver hela displayen mellan punkterna mellan x-värdet 122 och 125 med 255, så att punkterna försvinner
			display_image(0, display); // Vi uppdaterar displayen för att visa antalet resterande punkter
		}
	}
}

void waveUse (){ // Kallas när vi trycker på BTN3, sätter integern wave till 1 för att starta en wave och sänker antalet möjliga användingar av wave-funktionen med 1
	if (waveCount > 0 && wave == 0){ // Om en wave redan är igång så kommer denna funtion inte göra något
		wave = 1; // Genom att sätta wave till ett värde över 0 så kommer vi sedan kalla waveProgress i vår rewrite-funktion
		waveCount--;
	}
}

void waveProgress (){ // Denna funktion skriver in vår wave i displayen
	int temp;
	for (i = 0; i < 2; i++) // Arrayen för wave är bara 16 bits hög 
		for (j = 0; j < 16; j++){
			temp = wave+j+birdpos+4; // Birdpos + 4 används för att starta vår wave strax framför fågeln, wave används så att wave kan flytta sig mellan kallningar av funktionen
			display[i+1][temp] &= waveArray[i][j]; // Här skriver vi in vågen på displayen, eftersom att vågen alltid kommer ut i mitten av y-axeln så använder vi i+1 på displayen
            if ((waveArray[i][j] | box[i+1][temp]) != 255) // Om waven i någon punkt kolliderar med något hinder
                box[i+1][temp] = 255; // Vi skriver om bitsen som vår wave kolliderar med så att de släcks
		}
	wave++; // Vi inkrementerar wave så att den skrivs in längre till höger vid nästa kallning av funktionen
	if (wave == (79 + birdpos)) // Vid värdet 79 + birdpos så kommer vår wave nå slutet av displayen
		wave = 0; // Vi sätter värdet av wave till 0 för att stänga av vår wave
}

int obstaclePos (){ // Denna funktion bestämmer positionen av gapen på vårat nästa hinder
	return 3 * (score & 0x3) + 1; // Vi tar ut de 2 minsta bitsen på variablen score, detta kan vara mellan 0 till 3, vi multiplicerar sedan detta med 3 och adderar med 1 för att få större variation på olika positioner
}

int obstacleGap () { // Denna funktion bestämmer storleken på gapet på vårat nästa hinder
	return 14 + ((score & 0x1C)>>2); // Vi tar ut bits 2 till 4 på variabeln score, och shiftar dess position till höger med 2, detta värde kan vara mellan 0 till 7, vi adderar med 14 för att garantera att gapet är stort nog
}

int obstacleSpeed (){ // Denna funktion bestämmer hastigheten som nya hinder skapas
	speed = 70 - (score/50); // Vi använder vår score variabel för att höja hastigheten
	if (speed < 30)
		speed = 30; // Om speed skulle bli mindre än 30 så sätter vi den till 30, så att hinderna inte kommer för snabbt eller konstant skrivs in
}

uint8_t generateTop(int bits){ //Returnerar en byte där de "bits" minsta signifikanta bitarna är 0. Resten är ettor. 
	if(bits <= 0) //Om antalet bits är mindre än 0 så kan vi bara returnera alla ettor som är decimala värdet 255
		return 255;
	uint8_t one = 1;
	uint8_t res = 0;
	for(i = bits; i < 8; i++)  //Sätter de bitarna som ska vara 1. (8-bits) mest signifikanta bitar.
		res = (one << i) | res; //Sätter en bit till 1 på bitplats i 
	return res; 
}

uint8_t generateBottom(int bits){//Returnerar en byte där de "bits" mest signifikanta bitarna är 0. Resten är ettor.
	if(bits <= 0) //Om antalet bits är mindre än 0 så kan vi bara returnera alla ettor som är decimala värdet 255
		return 255; 
	uint8_t one = 1;
	uint8_t res = 0;
	for(i = 0; i < 8 - bits; i++) //Sätter de bitarna som ska vara 1. (8-bits) minst signifikanta bitar
		res = (one << i) | res; //Sätter en bit till 1 på bitplats i 
	return res; 
}

void gen(int topBits, int gap){ // topBits bestämmer antalet bitar på hindret ovanför gapet, gap bestämmer hur stort gapet är
	int i, j;
	uint8_t one = 1;
	uint8_t val = 0; // Vi initialiserar variabln val till 0, denna kommer att ändras senare
	for(i = 0; i < 4; i++){  //Iterar fyra gånger. En gång för varje 8 bitar (displayen är 32 pixlar hög)
		if(topBits > 0){ //Kollar om vi fortfarande har bitar på hindret ovanför gapet som ska printas ut.
			if(topBits < 8){ //Om antalet bitar är mindre än 8 då ska en del av byten vara hinder och en del gap
				val = generateTop(topBits); //Sätter de minst signifikanta bitarna topBits till 0 resten 1 som är gapet. 
				gap = gap - (8 - topBits); //Minskar gapet med antalet bitar var med i gapet
				topBits = 0; //Inga mer bitar för hindret längst up
			}
			else{
				val = generateTop(8); //Om antalet bitar är 8 kommer det inte finnas något gap
				topBits = topBits - 8; //Minskar antalet bitar i hindret med 8
			}
		}
		else if(gap >= 8){ //Om antalet bitar i gapet är större eller lika med 8 kommer vi ha en hel byte som representerar gapet
			gap = gap - 8;
			val = 255; //255 är värdet för en byte med alla ettor
		}
		else{ //Om vi inte har bitar kvar i hindret längst upp och gapet inte är 8 bitar betyder det att nästa byte kommer innehålla nollor som representerar hindret längst ner
			if(gap > 0){ //Om gapet är större än 0 ska vi fortfarande ha med ettor(gap) i byten
				val = generateBottom(8-gap); //Sätter de mest signifikanta bitarna 8-gap till 0 resten 1 som är gapet. 
				gap = 0; //Inga gap bitar kvar
            }
            else{ //Inget gap kvar
                //val = generateBottom(8);
				val = 0; //Resten kommer vara hindret längst ner som ska vara nollor
			}
		}
		
		for(j = 0; j < 4; j++){ //Ett hinder har fyra pixlars bredd så vi lägger till längden på hindret/gapet till alla element på bredden.
			obstacle[i][j] = val;
		}
	}
	for (i = 0; i < 4; i++){ //Sätter in hindret längst till höger på skärmen
		for (j=0; j<4; j++){
			box[i][j+124] = obstacle[i][j];
		}
	}
}

void shiftdown (){ // Denna funktion låter fågeln falla
	for (i = 0; i < 8; i++){ // Vår fågel är 8 bits lång så vi går mellan 0 till 7
		int temp0 = bird[0][i] & 0x80; // Vi sparar den mest signifikanta biten från det första lagret från toppen
		
		bird[0][i] = (bird[0][i] << 1) | ((bird[3][i] & 0x80) >> 7); 
		// Vi skiftar alla bits i det första lagret ett steg till vänster och lägger till den mest signifikanta biten i det sista lagret till den minst signifikanta av den första
		
		int temp1 = bird[1][i] & 0x80; // Vi sparar den mest signifikanta biten från det andra lagret från toppen
		
		bird[1][i] = (bird[1][i] << 1) | (temp0 >> 7);
		// Vi skiftar alla bits i det andra lagret ett steg till vänster och lägger till den mest signifikanta biten i det första lagret till den minst signifikanta av den andra
		
		int temp2 = bird[2][i] & 0x80; // Vi sparar den mest signifikanta biten från det tredje lagret från toppen
		
		bird[2][i] = (bird[2][i] << 1) | (temp1 >> 7);
		// Vi skiftar alla bits i det tredje lagret ett steg till vänster och lägger till den mest signifikanta biten i det andra lagret till den minst signifikanta av den tredje
		
		bird[3][i] = (bird[3][i] << 1) | (temp2 >> 7);
		// Vi skiftar alla bits i det fjärde lagret ett steg till vänster och lägger till den mest signifikanta biten i det tredje lagret till den minst signifikanta av den fjärde
	}
}

void shiftup (){ // Denna funktion låter fågeln hoppa, fungerar på samma sätt som shiftdown fast tvärtom
	for (i = 0; i < 8; i++){
		int temp0 = bird[3][i] & 0x3f;
		bird[3][i] = (bird[3][i] >> 6) | ((bird[0][i] & 0x3f) << 2);
		int temp1 = bird[2][i] & 0x3f;
		bird[2][i] = (bird[2][i] >> 6) | (temp0 << 2);
		int temp2 = bird[1][i] & 0x3f;
		bird[1][i] = (bird[1][i] >> 6) | (temp1 << 2);
		bird[0][i] = (bird[0][i] >> 6) | (temp2 << 2);
	}
}

// THE FOLLOWING CODE WAS NOT WRITTEN BY GUSTAV AND SAMUEL


/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

/* tick:
   Add 1 to time in memory, at location pointed to by parameter.
   Time is stored as 4 pairs of 2 NBCD-digits.
   1st pair (most significant byte) counts days.
   2nd pair counts hours.
   3rd pair counts minutes.
   4th pair (least significant byte) counts seconds.
   In most labs, only the 3rd and 4th pairs are used. */
void tick( unsigned int * timep )
{
  /* Get current value, store locally */
  register unsigned int t = * timep;
  t += 1; /* Increment local copy */
  
  /* If result was not a valid BCD-coded time, adjust now */

  if( (t & 0x0000000f) >= 0x0000000a ) t += 0x00000006;
  if( (t & 0x000000f0) >= 0x00000060 ) t += 0x000000a0;
  /* Seconds are now OK */

  if( (t & 0x00000f00) >= 0x00000a00 ) t += 0x00000600;
  if( (t & 0x0000f000) >= 0x00006000 ) t += 0x0000a000;
  /* Minutes are now OK */

  if( (t & 0x000f0000) >= 0x000a0000 ) t += 0x00060000;
  if( (t & 0x00ff0000) >= 0x00240000 ) t += 0x00dc0000;
  /* Hours are now OK */

  if( (t & 0x0f000000) >= 0x0a000000 ) t += 0x06000000;
  if( (t & 0xf0000000) >= 0xa0000000 ) t = 0;
  /* Days are now OK */

  * timep = t; /* Store new value */
}

/* display_debug
   A function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/   
void display_debug( volatile int * const addr )
{
  display_string( 1, "Addr" );
  display_string( 2, "Data" );
  num32asc( &textbuffer[1][6], (int) addr );
  num32asc( &textbuffer[2][6], *addr );
  display_update();
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init(void) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}

void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

void display_image(int x, const uint8_t *data) { 
	int i, j;
	
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(x & 0xF);
		spi_send_recv(0x10 | ((x >> 4) & 0xF));
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 128; j++) /* Changed the value of max j to 128 instead of 32 */
			spi_send_recv(~data[i*128 + j]);
	}
}

void display_update(void) {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;
			
			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc( char * s, int n ) 
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}

/*
 * nextprime
 * 
 * Return the first prime number larger than the integer
 * given as a parameter. The integer must be positive.
 */
#define PRIME_FALSE   0     /* Constant to help readability. */
#define PRIME_TRUE    1     /* Constant to help readability. */
int nextprime( int inval )
{
   register int perhapsprime = 0; /* Holds a tentative prime while we check it. */
   register int testfactor; /* Holds various factors for which we test perhapsprime. */
   register int found;      /* Flag, false until we find a prime. */

   if (inval < 3 )          /* Initial sanity check of parameter. */
   {
     if(inval <= 0) return(1);  /* Return 1 for zero or negative input. */
     if(inval == 1) return(2);  /* Easy special case. */
     if(inval == 2) return(3);  /* Easy special case. */
   }
   else
   {
     /* Testing an even number for primeness is pointless, since
      * all even numbers are divisible by 2. Therefore, we make sure
      * that perhapsprime is larger than the parameter, and odd. */
     perhapsprime = ( inval + 1 ) | 1 ;
   }
   /* While prime not found, loop. */
   for( found = PRIME_FALSE; found != PRIME_TRUE; perhapsprime += 2 )
   {
     /* Check factors from 3 up to perhapsprime/2. */
     for( testfactor = 3; testfactor <= (perhapsprime >> 1) + 1; testfactor += 1 )
     {
       found = PRIME_TRUE;      /* Assume we will find a prime. */
       if( (perhapsprime % testfactor) == 0 ) /* If testfactor divides perhapsprime... */
       {
         found = PRIME_FALSE;   /* ...then, perhapsprime was non-prime. */
         goto check_next_prime; /* Break the inner loop, go test a new perhapsprime. */
       }
     }
     check_next_prime:;         /* This label is used to break the inner loop. */
     if( found == PRIME_TRUE )  /* If the loop ended normally, we found a prime. */
     {
       return( perhapsprime );  /* Return the prime we found. */
     } 
   }
   return( perhapsprime );      /* When the loop ends, perhapsprime is a real prime. */
} 

/*
 * itoa
 * 
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 * 
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 * 
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 * 
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 * 
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 * 
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 * 
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 * 
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";
  
  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}

