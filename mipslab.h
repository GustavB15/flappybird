/* mipslab.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING 
   
   This file was last modified 2023-03-02 by Gustav Björs and Samuel Bergsten Grip
   Any code that was written by us is prefaced by a comment saying so*/

/* Declare display-related functions from mipslabfunc.c */
void display_image(int x, const uint8_t *data);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
uint8_t spi_send_recv(uint8_t data);

/* Declare lab-related functions from mipslabfunc.c */
char * itoaconv( int num );
void labwork(void);
int nextprime( int inval );
void quicksleep(int cyc);
void tick( unsigned int * timep );

/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr );

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];
/* Declare bitmap array containing icon */
extern uint8_t box[4][128];
extern uint8_t bird[4][8];
extern uint8_t obstacle[4][4];
extern uint8_t birdOriginal[4][8];
extern uint8_t display[4][128];

uint8_t waveArray[2][16];

/* Declare text buffer for display output */
extern char textbuffer[4][16];

/* Declare functions written by students.
   Note: Since we declare these functions here,
   students must define their functions with the exact types
   specified in the laboratory instructions. */
   
   /* The following are the functions written by Gustav and Samuel and used in the project */
void runGame(void);
void gameInit(void);
void rewrite (void);
void finish (void);
void restart (void);
void boxToDisplay (void);
void writeBird (void);
void border (int);
void obstacleLeft (void);
void sidePoints (void);
void countdown (void);
void waveUse (void);
void waveProgress (void);
int obstaclePos (void);
int obstacleGap (void);
int obstacleSpeed (void);
uint8_t generateTop(int);
uint8_t generateBottom(int);
void gen(int, int);
  

/* Written as part of asm lab: delay, time2string */
void delay(int);
void time2string( char *, int );
/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
int getbtns(void);
int getsw(void);
void enable_interrupt(void);
