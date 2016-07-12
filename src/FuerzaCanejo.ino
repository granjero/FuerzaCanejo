#include <Wire.h> //incluye la libreria WIRE que es una libreria para i2c
#include "RTClib.h" //libreria del reloj de tiemnpo real
#include "Adafruit_BMP085.h" //esta libreria es la libreria del sensor de presion y temp

#define MEMdir 0x50
#define reloj  0x68


//Defino paginas de la memoria
#define pagina0 0
#define pagina1 32
#define pagina2 64
#define pagina3 96
#define pagina4 128
#define pagina5 160
#define pagina6 192
#define pagina7 224
#define pagina8 256
#define pagina9 288

#define pagina10 320
#define pagina11 352
#define pagina12 384
#define pagina13 416
#define pagina14 448
#define pagina15 480
#define pagina16 512
#define pagina17 544
#define pagina18 576
#define pagina19 608
#define pagina20 640

#define pagina21 672
#define pagina22 704
#define pagina23 736
#define pagina24 768
#define pagina25 800
#define pagina26 832
#define pagina27 864
#define pagina28 896
#define pagina29 928
#define pagina30 960

//VARIABLES
String strSerial = ""; // va a recibir lo que venga por serie
int bandera = 0; //va a tener el dato que va a hacer que entre en un if u otro del loop
int banderaMenu = 0;
boolean strCompleto = false;  //bandera

int tono = 600; // para el ruido

//variables para setear hora
int hora;
String horaStr;
int minuto;
String minutoStr;
int segundo;
String segundoStr;
int dia;
String diaStr;
int mes;
String mesStr;
int ano;
String anoStr;

long presion;
long presionLeida[4]; //variable para almacenar la presion leida por el sensor
long presion0; //variable para almacenar la presion leida por el sensor
long presion1; //variable para almacenar la presion leida por el sensor
long presion2; //variable para almacenar la presion leida por el sensor
//long presion3; //variable para almacenar la presion leida por el sensor

int puesto = 0;

int pag = 0;

//INICIALIZA LIRERIAS
RTC_DS1307 RTC; // inicialioza en RTC la libreria del reloj
Adafruit_BMP085 BMP; // inicializa en BMP la libreria del sensor de presion

//SETUP
void setup()
{
  Wire.begin();
  RTC.begin();
  BMP.begin();

  Serial.begin(9600);
  limpiaPant();
  leeReloj();
  logo();
  menu();
}

void loop() {

  //muestra las instrucciones
  if(strCompleto && strSerial == "0\r" && banderaMenu == 0)
  {
    limpiaPant();
    instrucciones();
    delay(3000);
    limpiaPant();
    leeReloj();
    logo();
    menu();
    bandera = 0;
    strSerial = "";
    strCompleto = false;
  }

  //juega
  if(strCompleto && strSerial == "1\r" && banderaMenu == 0)
  {
    limpiaPant();
    Serial.println(F("\nChequear que el embolo este en la marca negra"));
    Serial.println(F("\n/ / / / / / / / / / / / / / / / / / / "));
    Serial.println(F("______________________________________"));
    Serial.println(F("\n\nIngrese su Nombre (hasta 32 caracteres)"));
    bandera = 1;
    strSerial = "";
    strCompleto = false;
  }

  if(strCompleto && bandera == 1)
  {
    limpiaPant();
    Serial.println(strSerial);
    Serial.println(F("Preparado en..."));
    delay(1800);

    //hace la cuenta regresiva 3 2 1
    for (int i = 3; i > 0; i--)
    {
      Serial.println(i);
      tone(11,1300, 200);
      delay(1000);
    }

    tone(11,2400, 200);

    Serial.println(F("FUEEERRRRRRRZAAAAAAAAAAAAAA CANEJO!!!!!!!!!!"));
    //toma las tres lecturas y hace el ruido de UUUUUUUUUuuuuuuiuiiiiiiiiii
    for (int i = 0; i < 3; i++)
    {
      for (int i = 0; i < 450; i++)
      {
        tono++;
        tone(11, tono, 15);
        delay(1);
      }
      presionLeida[i]= BMP.readPressure(); //a presion le asigna el valor de bmp.readPreassure que se fuerza a float para que ela division por 100 ande bien
    }
    noTone(11);

    //pasa a presion012 los valores de presion tomada pero con el rango achicado
    presion0 = map(presionLeida[0], 100000, 115000, 0, 1000);
    presion1 = map(presionLeida[1], 100000, 115000, 0, 1000);
    presion2 = map(presionLeida[2], 100000, 115000, 0, 1000);

    //asigna a presion el valor de la mayor medidatomada
    presion = decideMayor(presion0, presion1, presion2);

    //asigna a puesto la posicion en la que quedo el jugador
    puesto = decidePuesto( (float)presion, leeMEM(pagina1).toFloat(), leeMEM(pagina4).toFloat(), leeMEM(pagina7).toFloat(), leeMEM(pagina10).toFloat(), leeMEM(pagina13).toFloat()   );

    //ordena en la memoria los records segun el valor de record
    ordenaRecords(puesto, strSerial, (String)presion, leeRelojStr());

    limpiaPant();

    Serial.print(F("\nHiciste "));
    Serial.print(presion);
    Serial.println(F(" Puntos "));
    Serial.println();

    if (puesto < 6)
    {
      Serial.print(F("Quedaste en el puesto: "));
      Serial.println(puesto);
    }

    else
    {
      Serial.println(F("\nNo hay lugar para vos en el podio... Mas fuerza la próxima "));
    }
    Serial.println();

    bandera = 0;
    strSerial = "";
    strCompleto = false;

    menu();
  }

  //muestra los records
  if(strCompleto && strSerial == "2\r" && banderaMenu == 0)
  {
    limpiaPant();

    for(int i = 0; i < 5; i++)
    {
      Serial.print(i + 1);
      Serial.print(F("* "));
      getNombreRecord(leeMEM(pag));
      Serial.print(F("  Puntaje: "));
      pag = pag + 32;
      getPuntajeRecord(leeMEM(pag));
      Serial.print(F("  Fecha: "));
      pag = pag + 32;
      getFechaRecord(leeMEM(pag));
      Serial.println();
      pag = pag + 32;
    }

    Serial.println();
    menu();
    pag = 0;
    bandera = 0;
    strSerial = "";
    strCompleto = false;
  }

  //menu opciones
  if(strCompleto && strSerial == "3\r" && banderaMenu == 0)
  {
    limpiaPant();
    menuOpciones();
    banderaMenu = 1;
    strSerial = "";
    strCompleto = false;
  }


  if(strCompleto && strSerial == "1\r" && banderaMenu == 1)
  {

      limpiaPant();
      Serial.println(F("Borrando Records... (cobarde)"));

      for(int i = 0; i < 5; i++)
      {
        escribePagMEM(pag, "Sin Nombre");
        pag = pag + 32;
        escribePagMEM(pag, "0");
        pag = pag + 32;
        escribePagMEM(pag, leeRelojStr());
        pag = pag + 32;
      }
      pag = 0;

      limpiaPant();
      logo();
      menu();

      bandera = 0;
      banderaMenu = 0;
      strSerial = "";
      strCompleto = false;
  }

  if (strCompleto && strSerial == "2\r" && banderaMenu == 1)
  {
    limpiaPant();
    leeReloj();
    delay(2000);
    limpiaPant();
    logo();
    menu();
    bandera = 0;
    banderaMenu = 0;
    strSerial = "";
    strCompleto = false;
  }


  if(strCompleto && strSerial == "3\r" && banderaMenu == 1)
  {
    limpiaPant();
    Serial.print(F("\nSETEAR RELOJ\n\rIngrese la fecha y la hora con el siguiente formato:\n\rDD-MM-AAAA HH:MM:SS\n\rPor ejemplo:\n\r23-04-2016 13:45:55\n\r"));
    banderaMenu = 3;
    strSerial = "";
    strCompleto = false;
  }

  if (strCompleto && banderaMenu == 3)
  {
    diaStr = "";
    mesStr = "";
    anoStr = "";
    horaStr = "";
    minutoStr = "";
    segundoStr = "";
    diaStr += (String)strSerial[0];
    diaStr += (String)strSerial[1];
    dia = diaStr.toInt();

    mesStr += (String)strSerial[3];
    mesStr += (String)strSerial[4];
    mes = mesStr.toInt();

    anoStr += (String)strSerial[6];
    anoStr += (String)strSerial[7];
    anoStr += (String)strSerial[8];
    anoStr += (String)strSerial[9];
    ano = anoStr.toInt();

    horaStr += (String)strSerial[11];
    horaStr += (String)strSerial[12];
    hora = horaStr.toInt();

    minutoStr += (String)strSerial[14];
    minutoStr += (String)strSerial[15];
    minuto = minutoStr.toInt();

    segundoStr += (String)strSerial[17];
    segundoStr += (String)strSerial[18];
    segundo = segundoStr.toInt();

    setReloj(dia, mes, ano, hora, minuto, segundo);

    Serial.println(F("\nReloj seteado a:"));
    Serial.print(dia);
    Serial.print(F("-"));
    Serial.print(mes);
    Serial.print(F("-"));
    Serial.print(ano);
    Serial.print(F(" "));
    Serial.print(hora);
    Serial.print(F(":"));
    Serial.print(minuto);
    Serial.print(F(":"));
    Serial.println(segundo);

    delay(4000);
    limpiaPant();
    logo();
    menu();

    bandera = 0;
    banderaMenu = 0;
    strSerial = "";
    strCompleto = false;
  }

/*

  //borra todos los records
  if(strCompleto && strSerial == "3\r")
  {
    limpiaPant();
    Serial.println(F("Borrando Records... (cobarde)"));

    for(int i = 0; i < 5; i++)
    {
      escribePagMEM(pag, "Sin Nombre");
      pag = pag + 32;
      escribePagMEM(pag, "0");
      pag = pag + 32;
      escribePagMEM(pag, leeRelojStr());
      pag = pag + 32;
    }
    pag = 0;

    limpiaPant();
    logo();
    menu();

    bandera = 0;
    strSerial = "";
    strCompleto = false;
  }

  // setear reloj
  if(strCompleto && strSerial == "4\r")
  {
    limpiaPant();
    Serial.print(F("\nSETEAR RELOJ\n\rIngrese la fecha y la hora con el siguiente formato:\n\rDD-MM-AAAA HH:MM:SS\n\rPor ejemplo:\n\r23-04-2016 13:45:55\n\r"));
    bandera = 4;
    strSerial = "";
    strCompleto = false;
  }

  if (strCompleto && bandera == 4)
  {
    diaStr = "";
    mesStr = "";
    anoStr = "";
    horaStr = "";
    minutoStr = "";
    segundoStr = "";
    diaStr += (String)strSerial[0];
    diaStr += (String)strSerial[1];
    dia = diaStr.toInt();

    mesStr += (String)strSerial[3];
    mesStr += (String)strSerial[4];
    mes = mesStr.toInt();

    anoStr += (String)strSerial[6];
    anoStr += (String)strSerial[7];
    anoStr += (String)strSerial[8];
    anoStr += (String)strSerial[9];
    ano = anoStr.toInt();

    horaStr += (String)strSerial[11];
    horaStr += (String)strSerial[12];
    hora = horaStr.toInt();

    minutoStr += (String)strSerial[14];
    minutoStr += (String)strSerial[15];
    minuto = minutoStr.toInt();

    segundoStr += (String)strSerial[17];
    segundoStr += (String)strSerial[18];
    segundo = segundoStr.toInt();

    setReloj(dia, mes, ano, hora, minuto, segundo);

    Serial.println(F("\nReloj seteado a:"));
    Serial.print(dia);
    Serial.print(F("-"));
    Serial.print(mes);
    Serial.print(F("-"));
    Serial.print(ano);
    Serial.print(F(" "));
    Serial.print(hora);
    Serial.print(F(":"));
    Serial.print(minuto);
    Serial.print(F(":"));
    Serial.println(segundo);

    delay(4000);
    limpiaPant();
    logo();
    menu();

    bandera = 0;
    strSerial = "";
    strCompleto = false;
  }

  //lee reloj
  if(strCompleto && strSerial == "5\r")
  {
    limpiaPant();
    leeReloj();
    delay(2000);
    limpiaPant();
    logo();
    menu();
    bandera = 0;
    strSerial = "";
    strCompleto = false;
  }

  //comprueba sensor Presion
  if(strCompleto && strSerial == "6\r")
  {
    limpiaPant();
    if (!BMP.begin())
    {
      Serial.println(F("No se encuentra el modulo BMP180 chequear los cables"));
    }

    else
    {
      presion0 = (float)BMP.readPressure()/100; //a presion le asigna el valor de bmp.readPreassure que se fuerza a float para que ela division por 100 ande bien
      Serial.print(F("\nSensor OK\n\rPresion atmosferica = "));
      Serial.print(presion0);
      Serial.println(F(" hPa"));
    }
    delay(2000);
    limpiaPant();
    logo();
    menu();
    bandera = 0;
    strSerial = "";
    strCompleto = false;
  }
  */

  //escribe en la memoria
  if(strCompleto && strSerial != "l\r")
  {
    Serial.println(F("\nLas opciones validas son: 0, 1, 2, 3, 4, 5, y 6"));
    Serial.println(F("\nIngresaste:  "));
    Serial.println(strSerial);
    strSerial = "";
    strCompleto = false;
  }

  //lee paginas de la memoria
  if(strCompleto && strSerial == "l\r")
  {
    Serial.println(F("Lectura:"));

    for (int i = 0; i <= 8192; i = i +32 )
    {
      Serial.println(leeMEM(i));
    }
    strSerial = "";
    strCompleto = false;
  }
}

// FUNCIONES

/*
 * interrupcion serial
 * carga en charSerial lo que
 * entra por el serie
 */
void serialEvent()
{
  while (Serial.available())
  {
    // agarra el byte
    char charSerial = (char)Serial.read();
    // lo agrega a strSerial:
    //strSerial = "";
    strSerial.reserve(32);
    strSerial += charSerial;
    // cuando llega u RC setea la bandera
    // para que active algo en el loop
    if (charSerial == '\r')
    {
      strCompleto = true;
    }
  }
}

/*
 * escribe en la memoria 24c32
 * recibe la direccion de la memoria
 * y el byte de dato
 *
 */
void escribeMEM (int direccion, byte data)
{
  //transforma direccion en los dos address byte direccion
  byte BYTE_1 = direccion >> 8;
  byte BYTE_2 = direccion - (BYTE_1 << 8);

  Wire.beginTransmission(MEMdir);
  Wire.write(BYTE_1);
  Wire.write(BYTE_2);
  Wire.write(data);
  Wire.endTransmission();
  delay(10);
}

/*
 * escribe una pagina en la memoria 24c32
 * recibe la direccion de la memoria
 * y el string de  dato
 *
 */
void escribePagMEM (int direccion, String data)
{
  for (int i = 0; i < 32; i++)
    {
      escribeMEM(direccion,data[i]);
      direccion++;
    }
}

/*
 * lee la memoria 24c32
 * recibe la direccion de la memoria
 * y devuelve el String de la pagina de esa
 * direccion
 */
String leeMEM (int direccion)
{
  String paginaDeMemoriaR;
  paginaDeMemoriaR.reserve(32);
  paginaDeMemoriaR = "";
  //byte data;
  byte BYTE_1 = direccion >> 8;
  byte BYTE_2 = direccion - (BYTE_1 << 8);
  Wire.beginTransmission(MEMdir);
  Wire.write(BYTE_1);
  Wire.write(BYTE_2);
  Wire.endTransmission();
  delay(10);
  Wire.requestFrom(MEMdir, 32);
  delay(10);
  for(byte i=0; i < 32; i++)
  {
    paginaDeMemoriaR += (char)Wire.read();
  }
  delay(10);
  return paginaDeMemoriaR;
}

/*
 * setea el reloj
 * recibe los datos y setea
 */
void setReloj(int dia, int mes, int ano, int hora, int minutos, int segundos)
{
  Wire.beginTransmission(reloj);
  Wire.write((byte)0); // start at location 0
  Wire.write(bin2bcd(segundos)); //segundos
  Wire.write(bin2bcd(minutos)); //minutos
  Wire.write(bin2bcd(hora)); //hora
  Wire.write(bin2bcd(0)); // nros de dia
  Wire.write(bin2bcd(dia));
  Wire.write(bin2bcd(mes));
  Wire.write(bin2bcd(ano - 2000));
  Wire.endTransmission();
}

/*
 * lee el reloj
 * recibe los datos y setea
 */
void leeReloj()
{
  DateTime now = RTC.now();
  Serial.print(F("\nFECHA "));
  if(now.day() < 10)
  {
    Serial.print(F("0"));
  }
  Serial.print(now.day(), DEC);
  Serial.print('/');
  if(now.month() < 10)
  {
    Serial.print(F("0"));
  }
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(F(" "));
  Serial.print(F("HORA "));
  if(now.hour() < 10)
  {
    Serial.print(F("0"));
  }
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  if(now.minute() < 10)
  {
    Serial.print(F("0"));
  }
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  if(now.second() < 10)
  {
    Serial.print("0");
  }
  Serial.print(now.second(), DEC);
  Serial.println();
}

/*
 * lee el reloj y lo manda al String fechaHoraStr
 */
String leeRelojStr()
{
  String fechaHoraStrR;
  fechaHoraStrR.reserve(14);
  fechaHoraStrR = ""; // vacia el string
  DateTime now = RTC.now();

  //FECHA
  if(now.day() < 10)
  {
    fechaHoraStrR += "0";
  }
  fechaHoraStrR += (String)now.day();
  if(now.month() < 10)
  {
    fechaHoraStrR += "0";
  }
  fechaHoraStrR += (String)now.month();
  fechaHoraStrR += (String)now.year();
  //HORA
  if(now.hour() < 10)
  {
    fechaHoraStrR += "0";
  }
  fechaHoraStrR += (String)now.hour();
  if(now.minute() < 10)
  {
    fechaHoraStrR += "0";
  }
  fechaHoraStrR += (String)now.minute();
  if(now.second() < 10)
  {
    fechaHoraStrR += "0";
  }
  fechaHoraStrR += (String)now.second();
  //Serial.println(fechaHoraStr);
  return fechaHoraStrR;
}

/*
 * pasa de un numero en BCD al quivalente en BIN
 */
byte bcd2bin (byte val)
{
  return val - 6 * (val >> 4);
}

/*
 * pasa de un numero en BIN al equivalente en BCD
 */
byte bin2bcd (byte val)
{
  return val + 6 * (val / 10);
}

/*
 * logo de SolpeCanejo
 */
void logo()
{
  Serial.println(F("         _________\n\r         |        |\n\r     ____|________|____\n\r        //// _____)\n\r        _|   (o)(o)\n\r       (o        \\|\n\r         |     (..)              FUERZA CANEJO!!!\n\r         |    /||||\\ \n\r         | \\     /\n\r         |  -----  \n\r"));
}

/*
 * menu
 */
void menu()
{
  Serial.println(F("0. Instrucciones"));
  Serial.println(F("1. Jugar"));
  Serial.println(F("2. Ver Records"));
  Serial.println(F("3. Opciones"));
  /*
  Serial.println(F("4. Setear Reloj"));
  Serial.println(F("5. Leer Reloj"));
  Serial.println(F("6. Comprobar Sensor de Presion"));
  */
}

/*
 * menu Opciones
 */
void menuOpciones()
{
  Serial.println(F("1. Borrar Records"));
  Serial.println(F("2. Leer Reloj"));
  Serial.println(F("3. Setear Reloj"));
  Serial.println(F("4. Comprobar Sensor de Presion"));
}


/*
 * Instrucciones del juego
 */
 void instrucciones()
 {
   Serial.print(F("\nPosicionar el embolo sobre la linea de partida\n\rCuando sea el momento presionar el embolo fuerte y ganar\n\r"));
 }

/*
 * Limpia la pantalla de la terminal
 */
 void limpiaPant ()
 {
    Serial.write(27);
    Serial.print("[2J");
    Serial.write(27);
    Serial.print("[H");
 }

/*
 * Obtiene fecha y hora del record
 */
void getFechaRecord(String record)
{
  int i;
  String fechaR;
  fechaR.reserve(19);
  fechaR = "";
  for (i = 0; i < 19; i++)
  {
    fechaR += record[i];
    if (i == 1)
    {
      fechaR += "-";
    }
    if (i == 3)
    {
      fechaR += "-";
    }
    if (i == 7)
    {
      fechaR += " ";
    }
    if (i == 9)
    {
      fechaR += ":";
    }
    if (i == 11)
    {
      fechaR += ":";
    }
  }
  Serial.println(fechaR);
}

/*
 * Obtiene puntaje desde linea de Record
 */
void getPuntajeRecord(String record)
{
  int i;
  String puntajeR;
  puntajeR.reserve(32);
  puntajeR = "";

  for (i = 0; i < 32; i++)
  {
    puntajeR += record[i];
  }

  Serial.println(puntajeR);
}

/*
 * Obtiene nombre desde linea de Record
 */
void getNombreRecord(String record)
{
  int i;
  String nombreR;
  nombreR.reserve(32);
  nombreR = "";

  for (i = 0; i < 32; i++)
  {
    nombreR += record[i];
  }  Serial.println(nombreR);
}

/*
 * decide mayor valor
 */
 long decideMayor(long uno, long dos, long tres)
 {
  long mayor = uno;
  if (dos > mayor)
  {
    mayor = dos;
  }

  if (tres > mayor)
  {
    mayor = tres;
  }
  return mayor;
 }

/*
 * decide si es record
 */
 int decidePuesto(float presion, float uno, float dos, float tres, float cuatro, float cinco)
 {
  if (presion > uno)
  {
    return 1;
  }

  if (presion > dos)
  {
    return 2;
  }

  if (presion > tres)
  {
    return 3;
  }

  if (presion > cuatro)
  {
    return 4;
  }

  if (presion > cinco)
  {
    return 5;
  }

  else
  {
    return 99;
  }
 }

/*
 * ordena records
 */
 void ordenaRecords(int posicion, String nombre, String puntos, String fecha)
 {
  if (posicion == 1)
  {
    //Baja el 4 al 5
    escribePagMEM(pagina12, leeMEM(pagina9));
    escribePagMEM(pagina13, leeMEM(pagina10));
    escribePagMEM(pagina14, leeMEM(pagina11));

    // Baja el 3 al 4
    escribePagMEM(pagina9, leeMEM(pagina6));
    escribePagMEM(pagina10, leeMEM(pagina7));
    escribePagMEM(pagina11, leeMEM(pagina8));

    // Baja el 2 al 3
    escribePagMEM(pagina6, leeMEM(pagina3));
    escribePagMEM(pagina7, leeMEM(pagina4));
    escribePagMEM(pagina8, leeMEM(pagina5));

    //Baja el 1 al 2
    escribePagMEM(pagina3, leeMEM(pagina0));
    escribePagMEM(pagina4, leeMEM(pagina1));
    escribePagMEM(pagina5, leeMEM(pagina2));

    //Graba el 1
    escribePagMEM(pagina0, nombre);
    escribePagMEM(pagina1, puntos);
    escribePagMEM(pagina2, fecha);
  }

    if (posicion == 2)
  {
    //Baja el 4 al 5
    escribePagMEM(pagina12, leeMEM(pagina9));
    escribePagMEM(pagina13, leeMEM(pagina10));
    escribePagMEM(pagina14, leeMEM(pagina11));

    // Baja el 3 al 4
    escribePagMEM(pagina9, leeMEM(pagina6));
    escribePagMEM(pagina10, leeMEM(pagina7));
    escribePagMEM(pagina11, leeMEM(pagina8));

    // Baja el 2 al 3
    escribePagMEM(pagina6, leeMEM(pagina3));
    escribePagMEM(pagina7, leeMEM(pagina4));
    escribePagMEM(pagina8, leeMEM(pagina5));

    //Graba el 2
    escribePagMEM(pagina3, nombre);
    escribePagMEM(pagina4, puntos);
    escribePagMEM(pagina5, fecha);
  }

    if (posicion == 3)
  {
    //Baja el 4 al 5
    escribePagMEM(pagina12, leeMEM(pagina9));
    escribePagMEM(pagina13, leeMEM(pagina10));
    escribePagMEM(pagina14, leeMEM(pagina11));

    // Baja el 3 al 4
    escribePagMEM(pagina9, leeMEM(pagina6));
    escribePagMEM(pagina10, leeMEM(pagina7));
    escribePagMEM(pagina11, leeMEM(pagina8));

    //Graba el 3
    escribePagMEM(pagina6, nombre);
    escribePagMEM(pagina7, puntos);
    escribePagMEM(pagina8, fecha);
  }

    if (posicion == 4)
  {
    //Baja el 4 al 5
    escribePagMEM(pagina12, leeMEM(pagina9));
    escribePagMEM(pagina13, leeMEM(pagina10));
    escribePagMEM(pagina14, leeMEM(pagina11));

    //Graba el 4
    escribePagMEM(pagina9, nombre);
    escribePagMEM(pagina10, puntos);
    escribePagMEM(pagina11, fecha);
  }

    if (posicion == 5)
  {
    //Graba el 5
    escribePagMEM(pagina12, nombre);
    escribePagMEM(pagina13, puntos);
    escribePagMEM(pagina14, fecha);
  }
 }
