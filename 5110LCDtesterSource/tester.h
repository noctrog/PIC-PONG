#include <16F628A.h>

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES PUT                      //Power Up Timer
#FUSES INTRC_IO                 //Internal RC Osc, no CLKOUT
#FUSES NOMCLR                   //Master Clear pin used for I/O
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O

#use delay(int=4000000)




