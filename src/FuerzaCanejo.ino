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
//String paginaDeMemoria = ""; // va a recibir lo que se lee de la memoria
int bandera = 0; //va a tener el dato que va a hacer que entre en un if u otro del loop
//String lineaRecordMEM = ""; //va a tener el contenido de una RECORD para guardar lo en memoria
//String fechaHoraStr = ""; // va a tener un strng con la fecha y la hora DDMMYYYYhhmmss
//String nombre = "";
//String fecha = "";
//String puntaje = "";

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
long presion0; //variable para almacenar la presion leida por el sensor
long presion1; //variable para almacenar la presion leida por el sensor
long presion2; //variable para almacenar la presion leida por el sensor
long presion3; //variable para almacenar la presion leida por el sensor

int record = 0;

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
  if(strCompleto && strSerial == "0\r")
  {
    limpiaPant();
    instrucciones();
    delay(2000);
    limpiaPant();
    logo();
    menu();
    bandera = 0;
    strSerial = "";
    strCompleto = false;
  }

  //juega
  if(strCompleto && strSerial == "1\r")
  {
    limpiaPant();
    Serial.print(F("\nIngrese su Nombre (hasta 32 caracteres)"));
    bandera = 1;
    strSerial = "";
    strCompleto = false;
  }

  if(strCompleto && bandera == 1)
  {
    limpiaPant();
    Serial.print(F("\n\n\n\rAijuuuna Don "));
    Serial.println(strSerial);
    Serial.println(F("Preparado en..."));
    delay(1800);
    Serial.println(F("3"));
    tone(11,1300, 200);
    delay(1000);
    Serial.println(F("2"));
    tone(11,1300, 200);
    delay(1000);
    Serial.println(F("1"));
    tone(11,1300, 200);
    delay(1000);
    Serial.println(F("SOOOOOOOPLE CANEJO!!!!!!!!!!"));
    tone(11,2400, 200);

    for (int i = 0; i < 450; i++)
    {
      tono++;
      tone(11, tono, 15);
      delay(1);
    }
    presion0 = BMP.readPressure(); //a presion le asigna el valor de bmp.readPreassure que se fuerza a float para que ela division por 100 ande bien

    for (int i = 0; i < 450; i++)
    {
      tono++;
      tone(11, tono, 15);
      delay(1);
    }
    presion1 = BMP.readPressure(); //a presion le asigna el valor de bmp.readPreassure que se fuerza a float para que ela division por 100 ande bien

    for (int i = 0; i < 450; i++)
    {
      tono++;
      tone(11, tono, 15);
      delay(1);
    }
    presion2 = BMP.readPressure(); //a presion le asigna el valor de bmp.readPreassure que se fuerza a float para que ela division por 100 ande bien

    noTone(11);
    /*
    Serial.println(leeMEM(pagina1));
    Serial.println(leeMEM(pagina4));
    Serial.println(leeMEM(pagina7));
    Serial.println(leeMEM(pagina10));
    Serial.println(leeMEM(pagina13));
    */
    presion = decideMayor(presion0, presion1, presion2);

    //int decideRecord(long presion,long uno, long dos, long tres, long cuatro, long cinco)
    record = decideRecord( (float)presion, leeMEM(pagina1).toFloat(), leeMEM(pagina4).toFloat(), leeMEM(pagina7).toFloat(), leeMEM(pagina10).toFloat(), leeMEM(pagina13).toFloat()   );

    limpiaPant();

    Serial.print(F("\nHiciste "));
    Serial.print(presion);
    Serial.println(F(" Puntos "));
    Serial.println();

    if (record < 6)
    {
      Serial.print(F("Quedaste en el puesto: "));
      Serial.println(record);
    }

    else
    {
      Serial.println(F("\nNo hay lugar para vos en el podio... Mas fuerza la próxima "));
    }
    Serial.println();

    ordenaRecords(record, strSerial, (String)presion, leeRelojStr());

    //escribePagMEM(pagina0, strSerial);
    //escribePagMEM(pagina1, (String)presion);
    //escribePagMEM(pagina2, leeRelojStr());

    //Serial.println(lineaRecordMEM);

    bandera = 0;
    //strSerial == "2\r";
    strSerial = "";
    strCompleto = false;

    menu();
  }

  //muestra los records
  if(strCompleto && strSerial == "2\r")
  {
    limpiaPant();
    //lee pagina 0
    //Serial.println(leeMEM(pagina0));
    //Serial.println();
    Serial.print(F("1* "));
    getNombreRecord(leeMEM(pagina0));
    Serial.print(F("  Puntaje: "));
    getPuntajeRecord(leeMEM(pagina1));
    Serial.print(F("  Fecha: "));
    getFechaRecord(leeMEM(pagina2));

    //lee pagina 1
    //Serial.println(leeMEM(pagina1));
    Serial.println();
    Serial.print(F("2* "));
    getNombreRecord(leeMEM(pagina3));
    Serial.print(F("  Puntaje: "));
    getPuntajeRecord(leeMEM(pagina4));
    Serial.print(F("  Fecha: "));
    getFechaRecord(leeMEM(pagina5));

    //lee pagina 2
    //Serial.println(leeMEM(pagina2));
    Serial.println();
    Serial.print(F("3* "));
    getNombreRecord(leeMEM(pagina6));
    Serial.print(F("  Puntaje: "));
    getPuntajeRecord(leeMEM(pagina7));
    Serial.print(F("  Fecha: "));
    getFechaRecord(leeMEM(pagina8));

    //lee pagina 3
    //Serial.println(leeMEM(pagina3));
    Serial.println();
    Serial.print(F("4* "));
    getNombreRecord(leeMEM(pagina9));
    Serial.print(F("  Puntaje: "));
    getPuntajeRecord(leeMEM(pagina10));
    Serial.print(F("  Fecha: "));
    getFechaRecord(leeMEM(pagina11));

    //lee pagina 4
    //Serial.println(leeMEM(pagina4));
    Serial.println();
    Serial.print(F("5* "));
    getNombreRecord(leeMEM(pagina12));
    Serial.print(F("  Puntaje: "));
    getPuntajeRecord(leeMEM(pagina13));
    Serial.print(F("  Fecha: "));
    getFechaRecord(leeMEM(pagina14));

    Serial.println();
    Serial.println();
    menu();

    bandera = 0;
    strSerial = "";
    strCompleto = false;
  }

  //borra todos los records
  if(strCompleto && strSerial == "3\r")
  {
    limpiaPant();
    Serial.println(F("Borrando Records... (cobarde)"));

    escribePagMEM(pagina0, "Sin Nombre");
    escribePagMEM(pagina1, "0");
    escribePagMEM(pagina2, leeRelojStr());

    escribePagMEM(pagina3, "Sin Nombre");
    escribePagMEM(pagina4, "0");
    escribePagMEM(pagina5, leeRelojStr());

    escribePagMEM(pagina6, "Sin Nombre");
    escribePagMEM(pagina7, "0");
    escribePagMEM(pagina8, leeRelojStr());

    escribePagMEM(pagina9, "Sin Nombre");
    escribePagMEM(pagina10, "0");
    escribePagMEM(pagina11, leeRelojStr());

    escribePagMEM(pagina12, "Sin Nombre");
    escribePagMEM(pagina13, "0");
    escribePagMEM(pagina14, leeRelojStr());

    //delay(1000);
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

  //escribe en la memoria
  if(strCompleto && strSerial != "l\r")
  {
    Serial.println(F("\nEscritura... "));
    Serial.println(strSerial);
    escribePagMEM(pagina15, "300000");
    escribePagMEM(pagina16, "200000");
    escribePagMEM(pagina17, "100000");
    escribePagMEM(pagina18, "90000");
    escribePagMEM(pagina19, "80000");
    strSerial = "";
    strCompleto = false;
  }

  //lee paginas de la memoria
  if(strCompleto && strSerial == "l\r")
  {
    Serial.println(F("Lectura:"));
/*
    Serial.println(leeMEM(pagina0));
    Serial.println(leeMEM(pagina1));
    Serial.println(leeMEM(pagina2));
    Serial.println(leeMEM(pagina3));
    Serial.println(leeMEM(pagina4));
    Serial.println(leeMEM(pagina5));
    Serial.println(leeMEM(pagina6));
    Serial.println(leeMEM(pagina7));
    Serial.println(leeMEM(pagina8));
    Serial.println(leeMEM(pagina9));
    Serial.println(leeMEM(pagina10));
    Serial.println(leeMEM(pagina11));
    Serial.println(leeMEM(pagina12));
    Serial.println(leeMEM(pagina13));
    Serial.println(leeMEM(pagina14));
    Serial.println(leeMEM(pagina15));
    Serial.println(leeMEM(pagina16));
    Serial.println(leeMEM(pagina17));
    Serial.println(leeMEM(pagina18));
    Serial.println(leeMEM(pagina19));
    Serial.println(leeMEM(pagina20));
  */
    for (i = 0; i <= 8192; i = i +32 )
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
  byte data;
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
  Serial.println(F("         _________\n\r         |        |\n\r     ____|________|____\n\r        //// _____)\n\r        _|   (o)(o)\n\r       (o        \\|\n\r         |     (..)\n\r         |    /||||\\ \n\r         | \\     \\\\ \n\r         |  ---- _\\\\_ \n\r                /    \\\n\r   ____________|      |_______   SOPLE CANEJO!!!\n\r                \\    /\n\r                 \\__/\n\r"));
}

/*
 * menu
 */
void menu()
{
  Serial.println(F("0. Instrucciones"));
  Serial.println(F("1. Jugar"));
  Serial.println(F("2. Records"));
  Serial.println(F("3. Borrar Records"));
  Serial.println(F("4. Setear Reloj"));
  Serial.println(F("5. Leer Reloj"));
  Serial.println(F("6. Comprobar Sensor de Presion"));
}

/*
 * Instrucciones del juego
 */
 void instrucciones()
 {
   Serial.print(F("\nSoplar fuerte y ganar\n\r"));
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
  String fechaR;
  fechaR.reserve(19);
  fechaR = "";
  fechaR += record[0];
  fechaR += record[1];
  fechaR += "-";
  fechaR += record[2];
  fechaR += record[3];
  fechaR += "-";
  fechaR += record[4];
  fechaR += record[5];
  fechaR += record[6];
  fechaR += record[7];
  fechaR += " ";
  fechaR += record[8];
  fechaR += record[9];
  fechaR += ":";
  fechaR += record[10];
  fechaR += record[11];
  fechaR += ":";
  fechaR += record[12];
  fechaR += record[13];

  Serial.println(fechaR);
}

/*
 * Obtiene puntaje desde linea de Record
 */
void getPuntajeRecord(String record)
{
  String puntajeR;
  puntajeR.reserve(32);
  puntajeR = "";
  puntajeR += record[0];
  puntajeR += record[1];
  puntajeR += record[2];
  puntajeR += record[3];
  puntajeR += record[4];
  puntajeR += record[5];
  puntajeR += record[6];
  puntajeR += record[7];
  puntajeR += record[8];
  puntajeR += record[9];
  puntajeR += record[10];
  puntajeR += record[11];
  puntajeR += record[12];
  puntajeR += record[13];
  puntajeR += record[14];
  puntajeR += record[15];
  puntajeR += record[16];
  puntajeR += record[17];
  puntajeR += record[18];
  puntajeR += record[19];
  puntajeR += record[20];
  puntajeR += record[21];
  puntajeR += record[22];
  puntajeR += record[23];
  puntajeR += record[24];
  puntajeR += record[25];
  puntajeR += record[26];
  puntajeR += record[27];
  puntajeR += record[28];
  puntajeR += record[29];
  puntajeR += record[30];
  puntajeR += record[31];

  Serial.println(puntajeR);
}

/*
 * Obtiene nombre desde linea de Record
 */
void getNombreRecord(String record)
{
  String nombreR;
  nombreR.reserve(32);
  nombreR = "";
  nombreR += record[0];
  nombreR += record[1];
  nombreR += record[2];
  nombreR += record[3];
  nombreR += record[4];
  nombreR += record[5];
  nombreR += record[6];
  nombreR += record[7];
  nombreR += record[8];
  nombreR += record[9];
  nombreR += record[10];
  nombreR += record[11];
  nombreR += record[12];
  nombreR += record[13];
  nombreR += record[14];
  nombreR += record[15];
  nombreR += record[16];
  nombreR += record[17];
  nombreR += record[18];
  nombreR += record[19];
  nombreR += record[20];
  nombreR += record[21];
  nombreR += record[22];
  nombreR += record[23];
  nombreR += record[24];
  nombreR += record[25];
  nombreR += record[26];
  nombreR += record[27];
  nombreR += record[28];
  nombreR += record[29];
  nombreR += record[30];
  nombreR += record[31];

  Serial.println(nombreR);
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
 int decideRecord(float presion, float uno, float dos, float tres, float cuatro, float cinco)
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

  if (presion <= cinco)
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
