#include <tester.h>
//#define VERS "v1.0a"//version del firm

#define LCD_rst   PIN_A6
//#define nok_cs       PIN_A0//CE
#define LCD_dc    PIN_A7//DC
#define LCD_sda   PIN_A0//DIN
#define LCD_sclk  PIN_A1//B5//CLK

#define J1_UP PIN_B3 //pulsadores INIT
#define J1_DOWN  PIN_B2 //modo 
#define J2_UP  PIN_B1 //contraste -
#define J2_DOWN PIN_B0 //contraste +

//#DEFINE MODE_NORMAL 0
#DEFINE MODE_INVERSO 1
#DEFINE MODE_ALL_ON 2
#DEFINE MODE_ALL_OFF 3
#DEFINE VERSION "v1.1"

#define VELOCIDAD_PALAS 3

#define LARGO_PALAS_FACIL 16
#define LARGO_PALAS_MEDIO 10
#define LARGO_PALAS_DIFICIL 5

#define ANCHO_PALAS_FACIL 4
#define ANCHO_PALAS_MEDIO 3
#define ANCHO_PALAS_DIFICIL 3

#define PERIMETRO_BOLA_FACIL 6
#define PERIMETRO_BOLA_MEDIO 4
#define PERIMETRO_BOLA_DIFICIL 3

#define VELOCIDAD_BOLA_FACIL 1
#define VELOCIDAD_BOLA_MEDIO 2
#define VELOCIDAD_BOLA_DIFICIL 3

#define VELOCIDAD_IA_FACIL 1
#define VELOCIDAD_IA_MEDIO 2
#define VELOCIDAD_IA_DIFICIL 4

unsigned int8 contraste=70;
#include "PCD8544_LCDdriverV1_1.c"

struct bola
{
   int8 velX;
   int8 velY;
   int8 posY;
   int8 posX;
   int8 perimetro;
};

struct pala
{
   int8 velocidad;
   unsigned int8 posY;
   unsigned int8 posX;
   int8 ancho;
   int8 largo;
   int8 velIA;
};



unsigned int8 dificultad = 1;
int1 IA = 0;

unsigned int8 puntJ1 = 0;

int8 golpes = 0;
int8 velocidadAumentada1;
int8 velocidadAumentada2;
int8 velocidadInicial;

unsigned int8 puntJ2 = 0;

void seteaContraste();
void Actualizar(struct pala &J1, struct pala &J2, struct bola &pelota, unsigned int8 &puntJ1, unsigned int8 &puntJ2);
//void Gol(int1 izquierda, struct bola &pelota);


void main()
{
   input(J1_UP); 
   input(J1_DOWN); 
   input(J2_UP); 
   input(J2_DOWN); 

   //Pines no usados como salidas
   output_low (PIN_A2);
   output_low (PIN_A3);
   output_low (PIN_A4);
   output_low (PIN_B7);
   output_low (PIN_B6);
   output_low (PIN_B5);
   output_low (PIN_B4);

   port_b_pullups(true);
   delay_ms(50);//estabilizar tensiones 
  
   //----------Primera imagen--------/
   //--------------------------------/
   
   LCD_init();
   seteaContraste();            //pone el contraste al 70, valor puesto mas arriba en la variable contraste
   LCD_clear_screen();         // borrar DDRAM para que no inicie con basura en la pantalla
   LCD_write_command(0b00001100);   // modo funcionamiento normal (puede ser allOn,normal,alloff,inverso)
   LCD_gotoxy(0,0);
   printf(LCD_printchar_inv, "     PONG     ");
   LCD_gotoxy(0,2);
   printf(LCD_printchar, "By Ramon Calvo   Gonzalez");
   delay_ms(2000);
   
   //---------Menu de inicio---------/
   //--------------------------------/
   
   inicio:
   puntJ1 = 0;
   puntJ2 = 0;
   golpes = 0;
   dificultad = 1;
   LCD_clear_screen();
   LCD_gotoxy(0, 0);
   
   while (input_state(J1_UP)){   //mientras que el boton arriba no este presionado...
   
     /* LCD_write_data_inv("PONG");
      LCD_gotoxy(0,1);
      LCD_write_data_inv("Facil");
      LCD_gotoxy(0,2);
      LCD_write_data_inv("Medio");
      LCD_gotoxy(0,3);
      LCD_write_data_inv("Dificil");*/
      
         LCD_gotoxy(0,0);
         printf(LCD_printchar_inv, "     PONG     ");
         
         LCD_gotoxy(0,2); 
         if (dificultad == 1)
            printf(LCD_printchar_inv, "     Facil     ");
         else
            printf(LCD_printchar, "     Facil     ");
            
         LCD_gotoxy(0,3);
         if (dificultad == 2)
            printf(LCD_printchar_inv, "     Medio     ");
         else
            printf(LCD_printchar, "     Medio     ");
            
         LCD_gotoxy(0,4);
         if (dificultad == 3)
            printf(LCD_printchar_inv, "    Dificil   ");
         else
            printf(LCD_printchar, "    Dificil   ");

      
         
      //Lee los botones para cambiar la dificultad   
      if (!input_state(J1_DOWN) && (dificultad > 0))
         dificultad++;
      if (!input_state(J1_DOWN) && (dificultad == 4))
         dificultad = 1;
         
      //delay_ms(1); //un respiro
      
   }
   
   //----------------Efecto to chungo de transicion tt-------------------------/
   //--------------------------------------------------------------------------/
   
   LCD_write_command(0b00001101);  //modo inverso
   delay_ms(500); //dejar tiempo para el boton
   LCD_write_command(0b00001100);  //vuelta a la normalidad
   LCD_clear_screen();
   
   //--------------Seleccion IA/J2--------------/
   //-------------------------------------------/
   
   while (input_state(J1_UP)){   //menu de seleccion IA/J2
   
      LCD_gotoxy(0,0);
      printf(LCD_printchar_inv, "     PONG     ");
      
      LCD_gotoxy(0,2);
      if (IA == 0)
         printf(LCD_printchar_inv, "      J2      ");
      else 
         printf(LCD_printchar, "      J2      ");
      
      LCD_gotoxy(0,3);
      if (IA == 1)
         printf(LCD_printchar_inv, "      IA      ");
      else 
         printf(LCD_printchar, "      IA      ");
         
      //Lee los botones para cambiar el modo   
      if (!input_state(J1_DOWN))
         IA = 1 - IA;
   }
   
   //--------------------------------/
   //---Inicializar los jugadores----/
   
   struct pala J1, J2;
   struct bola pelota;
   
   switch(dificultad){
      case 1:
         J1.largo = LARGO_PALAS_FACIL;
         J2.largo = LARGO_PALAS_FACIL;
         J1.ancho = ANCHO_PALAS_FACIL;
         J2.ancho = ANCHO_PALAS_FACIL; 
         J2.velIA = VELOCIDAD_IA_FACIL;
         pelota.perimetro = PERIMETRO_BOLA_FACIL;
         pelota.velX = VELOCIDAD_BOLA_FACIL;
         pelota.velY = VELOCIDAD_BOLA_FACIL;
         velocidadInicial = VELOCIDAD_BOLA_FACIL;
         break;
      case 2:
         J1.largo = LARGO_PALAS_MEDIO;
         J2.largo = LARGO_PALAS_MEDIO;
         J1.ancho = ANCHO_PALAS_MEDIO;
         J2.ancho = ANCHO_PALAS_MEDIO;
         J2.velIA = VELOCIDAD_IA_MEDIO;      
         pelota.perimetro = PERIMETRO_BOLA_MEDIO;
         pelota.velX = VELOCIDAD_BOLA_MEDIO;
         pelota.velY = VELOCIDAD_BOLA_MEDIO;
         velocidadInicial = VELOCIDAD_BOLA_MEDIO;
         break;
      case 3:
         J1.largo = LARGO_PALAS_DIFICIL;
         J2.largo = LARGO_PALAS_DIFICIL;
         J1.ancho = ANCHO_PALAS_DIFICIL;
         J2.ancho = ANCHO_PALAS_DIFICIL;
         J2.velIA = VELOCIDAD_IA_DIFICIL;
         pelota.perimetro = PERIMETRO_BOLA_DIFICIL;
         pelota.velX = VELOCIDAD_BOLA_DIFICIL;
         pelota.velY = VELOCIDAD_BOLA_DIFICIL;
         velocidadInicial = VELOCIDAD_BOLA_DIFICIL;
         break;
   }
   
   //Crear las siferentes velocidades
   velocidadAumentada1 = pelota.velX + 1;
   velocidadAumentada2 = pelota.velX + 2;
   
   //J1.ancho = 4;  // definida mas arriba
   J1.velocidad = VELOCIDAD_PALAS;
   J1.posX = 0;
   J1.posY = 23 - (J1.largo/2);
   //J2.ancho = 4; // definida mas arriba
   J2.velocidad = VELOCIDAD_PALAS;
   J2.posX = 83 - J2.ancho;
   J2.posY = 23 - (J2.largo/2);
   pelota.posX = 42 - (pelota.perimetro/2);
   pelota.posY = 24 - (pelota.perimetro/2);
   
   
   LCD_write_command(0b00001101);  //modo inverso
   delay_ms(500); //dejar tiempo para el boton
   LCD_write_command(0b00001100);  //vuelta a la normalidad
   LCD_clear_screen();
   
  // unsigned int8 puntJugUno = 0;
  // unsigned int8 puntJugUno = 0;
   
   //Dar a la pelota una velocidad inicial
   pelota.velX = 1;
   pelota.velY = 1;

   while (true)
   {  
      //---------------------------------------------------------------------------------------------------------------------------//
      //---------------------------------------------------------------------------------------------------------------------------//
      //-------------------------------------En este punto el juego ya ha empezado-------------------------------------------------//
      //---------------------------------------------------------------------------------------------------------------------------//
      //---------------------------------------------------------------------------------------------------------------------------//      
      
      
      //---------------------------------------------------------------------//
      Actualizar(J1, J2, pelota, puntJ1, puntJ2);  //actualiza el estado del juego
      
      //Comprobar si alguien ha ganado
      if ((puntJ1 > 6) || (puntJ2 > 6))
         goto inicio;
      
         
      //---------------------------------------------------------------------//
      //----------------------Refrescar pantalla-----------------------------//
      
      LCD_gotoxy(0,0);
 
      int y;
      int x;
 
      for (y = 0; y < 6; y++){
         for (x = 0; x < 84; x++){
            int8 pixel = 0x00;
            int realY = 8 * y;
            
            //---------------------------PELOTA--------------------------------/
            //-----------------------------------------------------------------/
            if (x >= pelota.posX && x <= pelota.posX + pelota.perimetro -1 && pelota.posY + pelota.perimetro > realY && pelota.posY < realY + 8){
               int8 ballMask = 0x00;
               int i;
               for (i = 0; i < realY + 8 - pelota.posY; i++){
                  ballMask = ballMask >> 1;
                  if (i < pelota.perimetro){
                     ballMask |= 0x80;
                  }
               }
            
               pixel |= ballMask;
            }
            
            //------------------------------J1---------------------------------/
            //-----------------------------------------------------------------/
            if (x >= J1.posX && x <= J1.posX + J1.ancho -1 && J1.posY + J1.largo > realY && J1.posY < realY +8){
               int8 J1Mask = 0x00;
               int i;
               for (i = 0; i < realY + 8 - J1.posY; i++){
                  J1Mask = J1Mask >> 1;
                  if (i < J1.largo){
                     J1Mask |= 0x80;
                  }
               }
               pixel |= J1Mask;
            }
            
            if (x >= J2.posX && x <= J2.posX + J2.ancho -1 && J2.posY + J2.largo > realY && J2.posY < realY +8){
               int8 J2Mask = 0x00;
               int i;
               for (i = 0; i < realY + 8 - J2.posY; i++){
                  J2Mask = J2Mask >> 1;
                  if (i < J2.largo){
                     J2Mask |= 0x80;
                  }
               }
               pixel |= J2Mask;
            }            
            
            LCD_write_data(pixel);
         }
      }
      
      
   }
}

void seteaContraste()
{
   LCD_setContrast(contraste);
}


void Actualizar(struct pala &J1, struct pala &J2, struct bola &pelota, unsigned int8 &puntJ1, unsigned int8 &puntJ2)
{
   //--------------------------------------------------------------------//
   //--------------------Controles del jugador---------------------------//
   //--------------------------------------------------------------------//
   
   if ((!input_state(J1_UP))&(J1.posY > 0)){
      J1.posY -= J1.velocidad;
   }
   if ((!input_state(J1_DOWN))&(J1.posY < 47 - J1.largo)){
      J1.posY += J1.velocidad;
   }
   if (!IA & (!input_state(J2_UP))&(J2.posY > 0)){
      J2.posY -= J2.velocidad;
   }
   if (!IA & (!input_state(J2_DOWN))&(J2.posY < 47 - J2.largo)){
      J2.posY += J2.velocidad;
   }
   
   //--------------------------------------------------------------------//
   //-----------------------------IA-------------------------------------//
   //--------------------------------------------------------------------//
   
   if (IA){
      if (pelota.velX > 0){   //si la pelota viene hacia el...
         if (pelota.posY < J2.posY){    //si la pelota esta por encima
            J2.posY -= J2.velIA;
         }
         
         if (pelota.posY > J2.posY + J2.largo){      //si la pelota esta por debajo
            J2.posY += J2.velIA;
         }   
      }
   }
   
   //--------------------------------------------------------------------//
   //-----------Actualizar la posicion de la pelota----------------------//
   //--------------------------------------------------------------------//
   
   pelota.posX += pelota.velX;
   pelota.posY += pelota.velY;
   
   
   //--------------------------------------------------------------------//
   //--------------------Mirar las colisiones----------------------------//
   //--------------------------------------------------------------------//   
   
   //Colisiones con los bordes
   
   //Bordes arriba/abajo
   if (pelota.posY <= 1){
      pelota.velY = pelota.velY * -1;
   }
   else if (pelota.posY >= 48 - pelota.perimetro){
      pelota.velY = pelota.velY * -1;
   }   
   
   //Bordes  laterales
   if (pelota.posX > 83/* && (pelota.velX > 0)*/){
      
      //Resetear golpes
      golpes = 0;
      puntJ1 += 1;
      
      pelota.velX = velocidadInicial;
      pelota.velY = velocidadInicial;
      
      LCD_gotoxy (30, 0);
      printf(LCD_printchar, "%u", puntJ1);
      LCD_gotoxy (48, 0);
      printf(LCD_printchar, "%u", puntJ2);
   
      LCD_write_command(0b00001100);  //vuelta a la normalidad
      delay_ms(500);
      LCD_write_command(0b00001101);  //modo inverso
      delay_ms(500); //dejar tiempo para el boton
      LCD_write_command(0b00001100);  //vuelta a la normalidad
      delay_ms(500);
      LCD_write_command(0b00001101);  //modo inverso
      delay_ms(500); //dejar tiempo para el boton
      LCD_write_command(0b00001100);  //vuelta a la normalidad
      
      //Devuelve la pelota el centro del campo
      pelota.posX = 42 - (pelota.perimetro/2);
      pelota.posY = 24 - (pelota.perimetro/2); 
   }
   
   
   if ((pelota.posX <= 0)/* && ((-pelota.velX) > 0)*/){
   
      //Resetear golpes
      golpes = 0;
      puntJ2 += 1;
      
      pelota.velX = velocidadInicial;
      pelota.velY = velocidadInicial;
      
      LCD_gotoxy (30, 0);
      printf(LCD_printchar, "%u", puntJ1);
      LCD_gotoxy (54, 0);
      printf(LCD_printchar, "%u", puntJ2);
   
      LCD_write_command(0b00001100);  //vuelta a la normalidad
      delay_ms(500);
      LCD_write_command(0b00001101);  //modo inverso
      delay_ms(500); //dejar tiempo para el boton
      LCD_write_command(0b00001100);  //vuelta a la normalidad
      delay_ms(500);
      LCD_write_command(0b00001101);  //modo inverso
      delay_ms(500); //dejar tiempo para el boton
      LCD_write_command(0b00001100);  //vuelta a la normalidad
      
      //Devuelve la pelota el centro del campo
      pelota.posX = 42 - (pelota.perimetro/2);
      pelota.posY = 24 - (pelota.perimetro/2);     
   }
   

   //Colisiones con las barras
   
   //J1                          //Comprobar eje Y                                                                //Comprobar eje X
   if (pelota.posY >= J1.posY - pelota.perimetro && pelota.posY <= J1.posY + J1.largo + pelota.perimetro && pelota.posX <= J1.posX + J1.ancho){ 
      pelota.velX = -pelota.velX;
      golpes += 1;

      if (golpes == 5){
               pelota.velX = velocidadAumentada1;
      }
      
      if (golpes == 10){
               pelota.velY = velocidadAumentada1;
      }
      
      if (golpes == 15){
         pelota.velX = velocidadAumentada2;
         pelota.velY = velocidadAumentada2; 
      }
   }
   
   //J2
   if (pelota.posY >= J2.posY && pelota.posY <= J2.posY + J2.largo + pelota.perimetro && pelota.posX >= J2.posX - pelota.perimetro){
      if (pelota.velX > 0)
         pelota.velX = -pelota.velX;
      golpes += 1;
      
      if (golpes == 5){
         pelota.velX = -velocidadAumentada1;
      }
      
      if (golpes == 10){
         pelota.velY = -velocidadAumentada1;
      }
      
      if (golpes == 15){
         pelota.velX = -velocidadAumentada2;
         pelota.velY = -velocidadAumentada2; 
      }
      
   }
   
   //--------------------------------------------------------------------//
   //-------------Aumentar la velocidad de la pelota---------------------//
   //--------------------------------------------------------------------//   
   

   
  /* if (golpes > 4 && golpes < 10){
      if (pelota.velX > 0)
         pelota.velX = velocidadAumentada1;
      else 
         pelota.velX = -velocidadAumentada1;
   }
   
   if (golpes > 9 && golpes < 15){
      if (pelota.velY > 0)
         pelota.velY = velocidadAumentada1; 
      else 
         pelota.velY = -velocidadAumentada1;
   }

   if (golpes > 14 && golpes < 20){
      if (pelota.velX > 0)
         pelota.velX = velocidadAumentada2; 
      else 
         pelota.velX = -velocidadAumentada2;      
   }
   
   if (golpes > 19){
      if (pelota.velY > 0)
         pelota.velY = velocidadAumentada2; 
      else 
         pelota.velY = -velocidadAumentada2;     
   }*/

}

/*void Gol (int1 izquierda, struct bola &pelota)
{
   //Hacer un efecto para que se note el punto
   
   LCD_write_command(0b00001100);  //vuelta a la normalidad
   delay_ms(500);
   LCD_write_command(0b00001101);  //modo inverso
   delay_ms(500); //dejar tiempo para el boton
   LCD_write_command(0b00001100);  //vuelta a la normalidad
   delay_ms(500);
   LCD_write_command(0b00001101);  //modo inverso
   delay_ms(500); //dejar tiempo para el boton
   LCD_write_command(0b00001100);  //vuelta a la normalidad
   
   //Resetear golpes
   golpes = 0;
   
   //Aumenta la puntuación
   if (!izquierda)
      puntJ1++;
   else
      puntJ2++;
      
   
   //Devuelve la pelota el centro del campo
   pelota.posX = 42 - (pelota.perimetro/2);
   pelota.posY = 24 - (pelota.perimetro/2);
}*/
