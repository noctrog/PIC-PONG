//////MODIFOCADA PARA ESTE CASOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
//////////////////PCD8544 LCD driver. v1.1 by villamany////////////////////
//Libreria para LCDs basados en controladores PCD8544 de 84 x 48 pixeles 
//monocromaticos, que son los utilizados por los Nokia 5110, 3110...

/*
//El pin del backlight se conecta a masa. (Tambien se prodria conrolar via PWM)

Definir los siguientes salidas en el programa principal, si alguno no se va a usar,
basta con no definirlo.
#define LCD_sda   PIN_A3//DIN. Necesario
#define LCD_sclk  PIN_A4//CLK. Necesario
#define LCD_dc    PIN_A2//DC. Necesario
#define LCD_rst   PIN_A5//Necesario. Si solo se va a inicializar una vez
//                         se podria conectar a algun autorreset existente (tipo 
//                         reistencia elevada-condensador), el
//                         requisito es estar a nivel bajo unos 30ms tras dar 
//                         alimentacion, luego debe pasar a nivel alto
#define LCD_cs    PIN_A1//CE //Opcional. Se puede dejar conectado a masa si no 
//                           se va a usar otro dispositivo que comparta los
//                           pines de comunicacion.
*/

/*
//Si se quiere ajustar el contraste, utilizar la funcion LCD_setContrast (valor)
, valor debe ser de 0 a 127, durante la inicializacion se setea a "LCD_DEFAULT_CONTRAST",
62 que es lo habitual cuando se alimenta a 3.3v.
*/


/*Si se quiere usar el LCD en modo direccionamiento vertical, descomentar la linea 
"#define LCD_USE_VERTICAL_ADDR 0b00000010"*/

//#define LCD_USE_VERTICAL_ADDR 0b00000010

/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
#define  high_LCD_dc    output_high(LCD_dc)
#define  high_LCD_cs    output_high(LCD_cs)
#define  high_LCD_sda   output_high(LCD_sda);
#define  high_LCD_sclk  output_high(LCD_sclk);
#define  high_LCD_rst  output_high(LCD_rst);

#define  low_LCD_dc     output_low(LCD_dc)
#define  low_LCD_cs     output_low(LCD_cs)
#define  low_LCD_sda    output_low(LCD_sda);
#define  low_LCD_sclk   output_low(LCD_sclk);
#define  low_LCD_rst   output_low(LCD_rst);

#define LCD_DISPLAY_ALL_ON 0b00001001 //Independientemente del contenido de la DDRAM activa todos los pixeles
#define LCD_DISPLAY_ALL_OFF 0b00001000 //Independientemente del contenido de la DDRAM desactiva todos los pixeles
#define LCD_DISPLAY_NORMAL 0b00001100 //Muestra el contenido de la DDRAM en la pantalla
#define LCD_DISPLAY_INVERSE 0b00001101 //Muestra el contenido de la DDRAM en la pantalla en contraste inverso

char charsel,charpos,chardata;
char lcdx, lcdy;     // usado por la escritura de caracteres de mas de 8 pixel de altura (no usado si se utiliza solo la fuente contenida en esta libreria)
int8 byteCount,nbytesToWrite;

//fuente standard de 6x8 (5x8 mas un espacio=6x8)
int8 const font5x8t1[240]=
  {
  0x00,0x00,0x00,0x00,0x00,   // 20 space          ASCII table for LCD: 96 rows * 5 bytes= 480 bytes
  0x00,0x00,0x5f,0x00,0x00,   // 21 !
  0x00,0x07,0x00,0x07,0x00,   // 22 "
  0x14,0x7f,0x14,0x7f,0x14,   // 23 #
  0x24,0x2a,0x7f,0x2a,0x12,   // 24 $
  0x23,0x13,0x08,0x64,0x62,   // 25 %
  0x36,0x49,0x55,0x22,0x50,   // 26 &
  0x00,0x05,0x03,0x00,0x00,   // 27 '
  0x00,0x1c,0x22,0x41,0x00,   // 28 (
  0x00,0x41,0x22,0x1c,0x00,   // 29 )
  0x14,0x08,0x3e,0x08,0x14,   // 2a *
  0x08,0x08,0x3e,0x08,0x08,   // 2b +
  0x00,0x50,0x30,0x00,0x00,   // 2c ,
  0x08,0x08,0x08,0x08,0x08,   // 2d -
  0x00,0x60,0x60,0x00,0x00,   // 2e .
  0x20,0x10,0x08,0x04,0x02,   // 2f /
  0x3e,0x51,0x49,0x45,0x3e,   // 30 0
  0x00,0x42,0x7f,0x40,0x00,   // 31 1
  0x42,0x61,0x51,0x49,0x46,   // 32 2
  0x21,0x41,0x45,0x4b,0x31,   // 33 3
  0x18,0x14,0x12,0x7f,0x10,   // 34 4
  0x27,0x45,0x45,0x45,0x39,   // 35 5
  0x3c,0x4a,0x49,0x49,0x30,   // 36 6
  0x01,0x71,0x09,0x05,0x03,   // 37 7
  0x36,0x49,0x49,0x49,0x36,   // 38 8
  0x06,0x49,0x49,0x29,0x1e,   // 39 9
  0x00,0x36,0x36,0x00,0x00,   // 3a :
  0x00,0x56,0x36,0x00,0x00,   // 3b ;
  0x08,0x14,0x22,0x41,0x00,   // 3c <
  0x14,0x14,0x14,0x14,0x14,   // 3d =
  0x00,0x41,0x22,0x14,0x08,   // 3e >
  0x02,0x01,0x51,0x09,0x06,   // 3f ?
  0x32,0x49,0x79,0x41,0x3e,   // 40 @
  0x7e,0x11,0x11,0x11,0x7e,   // 41 A
  0x7f,0x49,0x49,0x49,0x36,   // 42 B
  0x3e,0x41,0x41,0x41,0x22,   // 43 C
  0x7f,0x41,0x41,0x22,0x1c,   // 44 D
  0x7f,0x49,0x49,0x49,0x41,   // 45 E
  0x7f,0x09,0x09,0x09,0x01,   // 46 F
  0x3e,0x41,0x49,0x49,0x7a,   // 47 G
  0x7f,0x08,0x08,0x08,0x7f,   // 48 H
  0x00,0x41,0x7f,0x41,0x00,   // 49 I
  0x20,0x40,0x41,0x3f,0x01,   // 4a J
  0x7f,0x08,0x14,0x22,0x41,   // 4b K
  0x7f,0x40,0x40,0x40,0x40,   // 4c L
  0x7f,0x02,0x0c,0x02,0x7f,   // 4d M
  0x7f,0x04,0x08,0x10,0x7f,   // 4e N
  0x3e,0x41,0x41,0x41,0x3e    // 4f O
};
 
int8 const font5x8t2[240]=            {
  0x7f,0x09,0x09,0x09,0x06,   // 50 P
  0x3e,0x41,0x51,0x21,0x5e,   // 51 Q
  0x7f,0x09,0x19,0x29,0x46,   // 52 R
  0x46,0x49,0x49,0x49,0x31,   // 53 S
  0x01,0x01,0x7f,0x01,0x01,   // 54 T
  0x3f,0x40,0x40,0x40,0x3f,   // 55 U
  0x1f,0x20,0x40,0x20,0x1f,   // 56 V
  0x3f,0x40,0x38,0x40,0x3f,   // 57 W
  0x63,0x14,0x08,0x14,0x63,   // 58 X
  0x07,0x08,0x70,0x08,0x07,   // 59 Y
  0x61,0x51,0x49,0x45,0x43,   // 5a Z
  0x00,0x7f,0x41,0x41,0x00,   // 5b [
  0x02,0x04,0x08,0x10,0x20,   // 5c
  0x00,0x41,0x41,0x7f,0x00,   // 5d
  0x04,0x02,0x01,0x02,0x04,   // 5e
  0x40,0x40,0x40,0x40,0x40,   // 5f
  0x00,0x01,0x02,0x04,0x00,   // 60
  0x20,0x54,0x54,0x54,0x78,   // 61 a
  0x7f,0x48,0x44,0x44,0x38,   // 62 b
  0x38,0x44,0x44,0x44,0x20,   // 63 c
  0x38,0x44,0x44,0x48,0x7f,   // 64 d
  0x38,0x54,0x54,0x54,0x18,   // 65 e
  0x08,0x7e,0x09,0x01,0x02,   // 66 f
  0x0c,0x52,0x52,0x52,0x3e,   // 67 g
  0x7f,0x08,0x04,0x04,0x78,   // 68 h
  0x00,0x44,0x7d,0x40,0x00,   // 69 i
  0x20,0x40,0x44,0x3d,0x00,   // 6a j
  0x7f,0x10,0x28,0x44,0x00,   // 6b k
  0x00,0x41,0x7f,0x40,0x00,   // 6c l
  0x7c,0x04,0x18,0x04,0x78,   // 6d m
  0x7c,0x08,0x04,0x04,0x78,   // 6e n
  0x38,0x44,0x44,0x44,0x38,   // 6f o
  0x7c,0x14,0x14,0x14,0x08,   // 70 p
  0x08,0x14,0x14,0x18,0x7c,   // 71 q
  0x7c,0x08,0x04,0x04,0x08,   // 72 r
  0x48,0x54,0x54,0x54,0x20,   // 73 s
  0x04,0x3f,0x44,0x40,0x20,   // 74 t
  0x3c,0x40,0x40,0x20,0x7c,   // 75 u
  0x1c,0x20,0x40,0x20,0x1c,   // 76 v
  0x3c,0x40,0x30,0x40,0x3c,   // 77 w
  0x44,0x28,0x10,0x28,0x44,   // 78 x
  0x0c,0x50,0x50,0x50,0x3c,   // 79 y
  0x44,0x64,0x54,0x4c,0x44,   // 7a z
  0x00,0x08,0x36,0x41,0x00,   // 7b
  0x00,0x00,0x7f,0x00,0x00,   // 7c
  0x00,0x41,0x36,0x08,0x00,   // 7d
  0x10,0x08,0x08,0x10,0x08,   // 7e
  0x78,0x46,0x41,0x46,0x78    // 7f
};
/*-----------------------------------------------------------------------------*/
/*Envia un byte de datos al LCD para dibujarlo en la pantalla, inviertiendo
el color de cada uno de los pixeles*/
void LCD_write_data_inv(char byteforLCD_data_inv)
{
  char caa;
   high_LCD_dc;
   #ifDef LCD_cs low_LCD_cs; #endIf  // chip enabled
   byteforLCD_data_inv =~ byteforLCD_data_inv;
   for (caa=8;caa>0;caa--)
   {
      low_LCD_sclk;
    if ((byteforLCD_data_inv & 0x80)==0)
    {
       low_LCD_sda;
    }
    else
       high_LCD_sda;
    high_LCD_sclk;
    byteforLCD_data_inv = byteforLCD_data_inv<<1;
   }
}
/*-----------------------------------------------------------------------------*/
/* Subrutina de escritura serie. Utilizada internamente para enviar comandos o 
datos al LCD*/
void LCD_write_dorc(char byteforLCD)         
{
  char caa;
  for (caa=8;caa>0;caa--)
  {
    low_LCD_sclk;
    if ((byteforLCD & 0x80)==0)
    {
         low_LCD_sda;
    }
    else
         high_LCD_sda;
 
    high_LCD_sclk;
    byteforLCD=byteforLCD<<1;
  }
}
/*-----------------------------------------------------------------------------*/
/*Envia un byte de comandos al LCD. Utilizada Internamente*/
void LCD_write_data(char byteforLCD_data)
{
  high_LCD_dc;
  #ifDef LCD_cs low_LCD_cs; #endIf  // chip enabled
  LCD_write_dorc(byteforLCD_data);
  #ifDef LCD_cs high_LCD_cs; #endIf  // chip disabled
}
/*-----------------------------------------------------------------------------*/
/*Envia un byte de comandos al LCD. Utilizada Internamente*/
void LCD_write_command(char byteforLCD_command)
{
  low_LCD_dc;
  #ifDef LCD_cs low_LCD_cs;#endIf
  LCD_write_dorc(byteforLCD_command);
  #ifDef LCD_cs high_LCD_cs; #endIf  // chip disabled
}
/*-----------------------------------------------------------------------------*/
/*Setea el cursor en la coordenada X,Y indicada*/
void LCD_gotoxy(int8 xLCD, int8 yLCD)   
{
  LCD_write_command(0x40|(yLCD & 0x07));   // Y axe initialisation: 0100 0yyy
  LCD_write_command(0x80|(xLCD & 0x7f));   // X axe initialisation: 1xxx xxxx
  //igualar cursores
  lcdx=xLCD;
  lcdy=yLCD;
}
/*-----------------------------------------------------------------------------*/
void table_to_LCD(int1 mode)   // extract ascii from tables & write to LCD
{
  if (charsel<0x20)return;
  if (charsel>0x7f)return;
  for (byteCount=0;byteCount<5;byteCount++)
  {
    if (charsel<0x50)
    {      
      charpos=((charsel-0x20)*5);
      chardata=font5x8t1[(charpos+byteCount)];
    }            // use font5x8t1
    else
       if (charsel>0x4f)
       {
          charpos=((charsel-0x50)*5);
          chardata=font5x8t2[(charpos+byteCount)];
       }            // use font5x8t2
    if (!mode)
    {
      LCD_write_data(chardata);      // send data to LCD
    }
    else
    {
      LCD_write_data_inv(chardata);      // send data to LCD
    }
  }
  if (!mode)//a�adir un pixel de espacio despues del caracter
      LCD_write_data(0x00);
  else
      LCD_write_data(0xff);
}
/*-----------------------------------------------------------------------------*/
/*Dibuja un caracter estandard en el LCD*/
void LCD_printchar(int8 cvar)  
{
   charsel=cvar;
   table_to_LCD(0);
}
/*-----------------------------------------------------------------------------*/
/*Dibuja un caracter estandard en el LCD invirtiendo su color*/
void LCD_printchar_inv(int8 cvar)
{
   charsel=cvar;
   table_to_LCD(1);
}
/*-----------------------------------------------------------------------------*/
/*Borra una linea del LCD*/
void LCD_erase_y(int8 yLCD)
{
   LCD_gotoxy(0,yLCD);
   printf(LCD_printchar,"              ");
}
/*-----------------------------------------------------------------------------*/
/*Borra una columna del LCD*/
void LCD_erase_x(int8 xLCD)
{
  char column;
  for (column=0;column!=6;column++)
  {
    LCD_gotoxy(xLCD,column);
    LCD_write_data(0x00);
    LCD_write_data(0x00);
    LCD_write_data(0x00);
    LCD_write_data(0x00);
    LCD_write_data(0x00);
    LCD_write_data(0x00);
  }
}
/*-----------------------------------------------------------------------------*/
/*Borra la pantalla del LCD*/
void LCD_clear_screen()
{
   int16 n;
   LCD_gotoxy(0,0);
   for (n=0;n<504;n++) LCD_write_data (0x00);//84 pixel x 6 lineas=504 
}
/*-----------------------------------------------------------------------------*/
/*
Setear el contraste
Contraste seteable por medio de Vop (tension de operacion del LCD), que es ajustable
de 3.06v a 10.68v (0 a 127 en valor decimal)(0=3.06 y 127=10.68) (valor de 61 esta bien para
inicializar funcionando a 3.3v que equivalen a 6,72v.
ATENCION: El datasheet recomienda no siperar el valor de 8.5v para Vop, el cual
equivale a un valor de 90*/
void LCD_setContrast(int8 valor)
{
   int8 aux=0b00100001;
   #ifDef LCD_USE_VERTICAL_ADDR aux=aux| LCD_USE_VERTICAL_ADDR; #endIf
   LCD_write_command(aux);//entrar en modo instrucciones extendidas
   LCD_write_command(0b10000000|valor);//ajustar contraste
   aux=0b00100000;
   #ifDef  LCD_USE_VERTICAL_ADDR aux=aux| LCD_USE_VERTICAL_ADDR; #endIf
   LCD_write_command(aux);//volver al modo instrucciones normales
}
/*-----------------------------------------------------------------------------*/
/*Cambia entre los siguientes modos de funcionamiento:
LCD_LCD_DISPLAY_ALL_ON 0b00001001 //Independientemente del contenido de la DDRAM activa todos los pixeles
DISPLAY_ALL_OFF 0b00001000 //Independientemente del contenido de la DDRAM desactiva todos los pixeles
LCD_DISPLAY_NORMAL 0b00001100 //Muestra el contenido de la DDRAM en la pantalla
LCD_DISPLAY_INVERSE 0b00001101 //Muestra el contenido de la DDRAM en la pantalla en contraste inverso
LCD_Para restaurarlo hay que llamar a "LCD_ScreenNormal()"*/
void LCD_SetDisplay(INT8 DISPLAYMODE)
{
   LCD_write_command(DISPLAYMODE);    
}
/*-----------------------------------------------------------------------------*/
/*Inicializa el LCD. Debe llamarse al iniciar el programa principal*/
void LCD_init()
{
   
   #ifDef LCD_rst low_LCD_rst;#endIf
   delay_us (1);// en datasheet pone mantener durante 100ns (=0.01us), no se por que ponia reset chip during >= 250ms en la libreria original   
   #ifDef LCD_rst high_LCD_rst;#endIf
   
   high_LCD_dc;
   #ifDef LCD_cs high_LCD_cs;#endIf
   
   int8 aux=0b000100001;// Function set. Chip On,modo direccionamiento horizontal/vertical, entrar en modo instrucciones extendidas
   #ifDef  LCD_USE_VERTICAL_ADDR aux=aux| LCD_USE_VERTICAL_ADDR; #endIf
   LCD_write_command(aux);
   
   LCD_write_command(0b10000000|contraste);//Setear Vop (contraste)//contraste del programa modificado

   LCD_write_command(0b00010011);   // bias

   aux=0b000100000;// Function set. Chip On,modo horizontal/vertical, salir del modo instrucciones extendidas
   #ifDef  LCD_USE_VERTICAL_ADDR aux=aux| LCD_USE_VERTICAL_ADDR; #endIf
   LCD_write_command(aux);

   //LCD_write_command(0b00001100);   // modo funcionamiento normal (puede ser allOn,normal,alloff,inverso)
   //LCD_clear_screen();         // borrar DDRAM para que no inicie con basura en la pantalla
}


