//die ist ein Test

/*#########################neue Version mit Mega 2560: ab 30.06.2022 #######################################################
  Dachrechner, neue Version mit Mega 2560: ab 30.06.2022
  Luftdruck, Temperaturen und Lichtverhaeltnisse werden gemessen.

  Relaisplatine 1 ersetzt durch Helbig Platine 1008
  Unterschied: 
   digitalWrite(i, LOW);  //Wenn auf Low -> dann nicht angezogen
   bei Chinesenplatine :  
   digitalWrite(i, HIGH);  //Wenn auf High -> dann nicht angezogen
   (gilt nur für die ersten 8 Relais), die ersten 4 Rolläden


################################################################################ */

#define SERIAL_TX_BUFFER_SIZE 256
#define SERIAL_RX_BUFFER_SIZE 256

#include <Wire.h> 
#include <SPI.h>
//#include <SD.h>
#include <EEPROM.h>
#include <SFE_BMP180.h>
#include <DHT.h>
#include <BH1750.h>
#include <TimeLib.h>
#include <SoftwareSerial.h>
#include <Ethernet.h>
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t
#include <Dusk2Dawn.h> //zur Berechnung von Sonnenauf und Untergang
#include <VirtuinoEthernet_WebServer.h>    // Neccesary virtuino library for ethernet shield

#include <avr/wdt.h>  //Bibliothek des Watchdogs

//Pin 12 wahrscheinlich defekt!!
//#define DHTPIN1 12   // modify to the pin we connected
//#define DHTPIN2 11   // modify to the pin we connected

//#define DHTTYPE1 Feuchte11   // AM2301 
//#define DHTTYPE2 DHT22   // AM2302 
//#define DHTTYPE2 DHT11   

//DHT Feuchte1(DHTPIN1, DHTTYPE1);
DHT Feuchte2(11, DHT22);

SFE_BMP180 Luftdruck;
BH1750 Lichtmesser;

//Wind
int sensRichtung = A4;
int sensGeschw = A5;

//byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte mac [ ] = {0x50 , 0xCC , 0xF8 , 0x49 , 0x0A , 0x57 } ;
IPAddress ip ( 192 , 168 , 178, 30 );
//IPAddress Raspberry(192, 168, 178, 15);
IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);
//unsigned int localPort = 8888;      // local port to listen onUdp.begin(localPort);
//unsigned int DachPort = 7777;      //Zum Übertragen von Dachdaten zum Raspberry,

 char Sunrise[6];
 char Sunset[] = "00:00";
 bool ZeitOK = false;
 

 //char server[] = "www.heisopo.de";
//char timeServer[] = "ptbtime1.ptb.de"; // time.nist.gov NTP server
//char Raspberry[] = "heisopi";
//int Sendepause = 30;  //Sendepause in Sekunden zur Datenuebertragung ins Internet
const int Sendepause = 3000;  //Sendepause in Sekunden zur Datenuebertragungin msec
const int PauseDHT = 10000;  //Sendepause in mSekunden zur Messung des DHT22
unsigned long Sendezeit = millis();
unsigned long SendeDHT = millis();

//bool Sommerzeit = false;    //Winterzeit = false;

String SyncZeit;
time_t letzteZeit = 0; // when the digital clock was displayed
Dusk2Dawn Heimat(49.3252, 6.78, 1);  //zur Berechnung von Sonnenauf und Untergang
int SA = 0;
int SU = 0;
bool RaspDatenOK = false; //Meldung für PC
bool Dach2OK= false; //Meldung Dach2
//EthernetClient Heisopo;
//EthernetUDP Udp;
//EthernetUDP DachUdp;

//Parameter für Simulation
bool Simulation = false;
unsigned long  StartLicht = 0; //Zeit zur Erhöhung/Verminderung des Lichts
int IntervLicht = 0;  //Zeitintervall, in dem die Lichtstärke erhöht/vermindert wird
int SimLicht = 0;
bool SimAb = false;
int SimZeit;
int DeltaUhrzeit = 0; //Delta zur tatsächlichen Uhrzeit

#define ALTITUDE 210.0 //Aktuelle Hoehe

const int MAX_PAGE_NAME_LEN = 8;  // max characters in a page name 
char buffer[MAX_PAGE_NAME_LEN+1];  // page name + terminating null
//const int chipSelect = 4;

//Rolladensteuerung
const int RelStart = 22;
const int WieOft = 50;   //die Bedingung zum Öffnen muss "WieOft" mal erfüllt sein, um zu öffnen
unsigned long RelaisEinZeit[22]; //Zeit, zu der das Relais angezogen hat
int Anzugszeit[22]; //Oeffnen- und Schliesszeit, diese Zeit benötigt der Rolladen zum kiompletten Schliessen oder Oeffnen
int AnzZeit[22]; //berechnete Dauer der Anzugszeit des Relais bezogen auf den Dimmgrad
int AlteHelligkeit[11];
int AlteZeit[11];
int Anzahl[11];
bool VonHand[22];
bool SchonGemessen = false; //Vermeidet falsche Reaktion der Rolladen, wenn noch nicht gemessen wurde nach Rechnerstart
int aus;   //notwendig zur Verwendung unterschiedlicher Relaisplatinen, Helbig-Platine: LOW=Relais Nicht angezogen, China-Platine LOW = Relais angezogen

int ZustandLaden[11]; //aktueller Zustand der Laden, Oeffnungsgrad des Ladens in Prozent
int AlterZustandLaden[11]; //aktueller Zustand der Laden, Oeffnungsgrad des Ladens in Prozent
unsigned long Relaiszustand; //binaere Zustaende der Relais, 1 = angezogen (22 Bit)
String Ladendaten[11];

bool Sendelerlaubnis = true;
bool Zeitsimulation = false;  //Dient zum Simulieren der Zeit in der Testphase

int Testnummer = 1;

const unsigned long Faktor = 1800000;  //2000Imp/Wh * 3600s/h * 1000ms/s


 int Licht[4] ;
 int Lichtquelle;

bool DatenZumPC = true;

//const int SendepauseUDP = 5;  //Sendepause in Sekunden zur Datenuebertragung per UDP zum Raspberry
//unsigned long SendezeitUDP = millis();

const int AnzDachbyts = 90;
byte Dachwert[AnzDachbyts+1];  //Dachbyts, die zum RaspArd gesendet werden
const int RaspArdByts = 60;
byte RaspArd[RaspArdByts+1];   //RaspByts, die vom RaspArd kommen
const int AnzDach2Byts = 60;
byte VomDach2[AnzDach2Byts + 1];
byte ZumDach2[AnzDach2Byts + 1];  //Dachbyts, die zum RaspArd gesendet werden

const int AdresseNeustart = 512;
int AnzNeustarts;
bool Neustart = true;
const int AdresseEepromRolladen = 0;
const int AnzSoll = 250;
byte Rolladenwert[AnzSoll]; 
unsigned long TimerBetriebsmeldung;
const int Watchdog =  60;   //wenn nach dieser Zeit keine Sendeanforderung vom PC gekommen ist, keine Meldungen mehr zum PC senden,


EthernetServer VirtuinoServer(8000);                              // default port=8000
VirtuinoEthernet_WebServer virtuino(&VirtuinoServer);

void(* resetFunc) (void) = 0;  // declare reset fuction at address 0

void setup()
{

  virtuino.DEBUG = false;
  Ethernet.begin(mac, ip);
  Serial.begin(57600);  //Kommunikation mit PC
  Serial2.begin(19200);  //Kommunikation mit RaspArduino
 // Serial3.begin(57600);  //Kommunikation mit Dachrechner2

  PCFehlerMeldung("Neustart des Dach-Arduino erfolgt (mit Watchdog)");

  //######################################################################

  //PCStatusMeldung("\nInitialisiere SD Karte...");
  //if (!card.init(SPI_HALF_SPEED, chipSelect)) {
	 // Serial.println("nicht erfolgreich");
	 //
  //}
  //else {
	 // PCStatusMeldung("Verbindung zur SD it ok ");
  //}

 
  //######################################################################
 
  //Rolladenrelais auf Ausgang
 
   for (int i = RelStart; i < RelStart +22; i++)
    {
	   pinMode(i, OUTPUT);
	   if (i < RelStart + 8) aus = LOW; else aus = HIGH;
	   digitalWrite(i, aus);  //Wenn auf High -> dann nicht angezogen
	}
   //Rolladenrelais auf Ausgang
 
   RolladenwerteAusEepromLesen(false);
   NeustartsLesen();
   Dachwert[88] = 3; //Damit wird dem Raspberry bei der ersten Datenübertragung der Neustart angezeigt, entspricht Messwerte[249] bein RaspArd



   //Aktuellen Zustand aller Läden zum PC
 for (int LadenNr = 0 ; LadenNr < 11; LadenNr++)
	   {
        ZustandLaden[LadenNr] = -1; //aktueller Zustand der Laden
		AlterZustandLaden[LadenNr] =-1;//Aktuellen Zustand der Laden bei Start auf -1 = nicht definiert
		AlteZeit[LadenNr] = 0;
		AlteHelligkeit[LadenNr] = 0;
		VonHand[LadenNr] = false;
	    }
  
   for (int Nr = 0 ; Nr < 22; Nr++)
        {
        int ADR = int(Nr/2) * 16 + 12 + Nr%2; //Adresse der Oeffnen- und Schliesszeit
        Anzugszeit[Nr] =   Rolladenwert[ADR];
		    AnzZeit[Nr] =  Rolladenwert[ADR];
		/* Serial.print("Relais ");
		  Serial.print(Nr);
		  Serial.print(" - ");
		  Serial.println(Anzugszeit[Nr]); */
        }



 // Feuchte1.begin();
  Feuchte2.begin();
  Luftdruck.begin();
  Lichtmesser.begin();  //mit BH1750 (GY 307)
  Serial.println(F("BH1750 begin"));
 
virtuino.password = "1234";
 //Udp.begin(localPort);
 //DachUdp.begin(DachPort);

  setSyncProvider(RTC.get); // Funktion um die Zeit- und Datumsangabe von der RTC zu bekommen
  SunRiseSet();    //Sonnenauf und Untergang berechnen
  wdt_enable(WDTO_4S);   // Watchdog auf 4s stellen  // Beschreibung siehe https://elektro.turanis.de/html/prj027/index.html
     //wdt_disable(); //schaltet den Watchdog wieder ab.
}
 
//###############################################################################################
void loop() {

	bool Z;
	static int Sec;
	int Testbit;
	virtuino.run();           //  neccesary command to communicate with Virtuino android app
	
	if (Simulation) PCMeldungText(97, "Simulationn ist eingeschaltet!");
	//Rolladen
	Relaiszustand = 0;
	for (int i = 0; i < 22; i++)
	{
		Z = !digitalRead(i + RelStart);
		if (i < 8) Z = !Z;

		bitWrite(Relaiszustand, i, Z);
		if (Z == HIGH) Ueberwachen(i);
	}
	//Serial.print("Z=");
	//Serial.println(Z);


	for (int i = 0; i < 11; i++)
	{
		Testbit = (Relaiszustand >> i * 2) && 0B11;
		if (Testbit==0)  LadenPruefen(i);
	}

	//StromTesten();

	if (millis() > Sendezeit + Sendepause) { //wenn die Zeit sich geändert hat
			Sendezeit = millis();
		    TesteDS1307RTC();
			ZeitInfo();
			Lichtsensoren();
			Lufdrucksensor();
			Windmessen();
						
			SendeMesswerteZumPC();
			//SendeDachrechner2();
			//SchreibeStromAufSD();
			if (DatenZumPC) SendeLadenzustand();
			PCMeldung(65, AnzNeustarts);
			if (AnzNeustarts > 250) wdt_disable(); //schaltet den Watchdog wieder ab.
			//StromimpulseVonSDlesen();
			//Debugen(10, "Lichtquelle", Lichtquelle);
	} 
	Feuchtesensor2();

	//HeisopoAbfragen();
	
	VituinoAbfragen();
	RolladendatenAnHeisopo();
	DatenZumPC = DatentimerPruefen();
	
	wdt_reset();	//Watchdog zurücksetzten

	//Ladentest();

}
//###############################################################################################
//################### Dachbyts werden über USB zum PC gesendet ##################################
//###############################################################################################
void SendeMesswerteZumPC()
	//über USB zum PC - bereits in neue Version geändert
{
 String Gesamtdaten = String("Dachdaten=");
  int Pruef = 0;
  for (int i = 0; i < AnzDachbyts; i++)
  {
	   Pruef += Dachwert[i];
	   Gesamtdaten =  Gesamtdaten + Dachwert[i] + ",";
  }

 
  Gesamtdaten = Gesamtdaten + Pruef;
  Serial.println(Gesamtdaten); //Gesamtdaten zum PC senden

}
//###############################################################################################
//####################### Befehl von PC lesen ###################################################
//###############################################################################################
void serialEvent ()
{
  String Bef;

  /*siehe
    http://startingelectronics.org/software/arduino/learn-to-program-course/18-strings/
  */

  if (Serial.available() > 1) {
    Bef = Serial.readStringUntil(13);
    Serial.print("Befehl erkannt: ");
    if (Bef.length() < 40) Serial.println(Bef);
  }
  else
	  return;
	 
  if  (Bef == "SendeDaten")
  {
    TimerBetriebsmeldung = millis() / 1000;
    DatenZumPC = true;
  }
  else if (Bef == "StoppeDaten")
  {
    DatenZumPC = false;
    TimerBetriebsmeldung = TimerBetriebsmeldung - Watchdog;
  }
  
 else if (Bef.startsWith("Relais") )
  {//RelaisSchliessen Nr 0-22
//Bsp. Relais1, Relais12
	RelaisSchliessen(Bef);
  }
  else if (Bef.startsWith("Dimme") )
  {//Dimmt Relais Nr 0-22
  //Bsp. Dimme1auf20, Dimme12auf3
	BefLadenDimmen(Bef);
  }
  else if (Bef.startsWith("Sammel"))
	  {
	  SammelBefehlBewegen(Bef);
	  }
  else if (Bef.startsWith("Rolladen"))
	  {
			//Bsp: Rolladen200    :alle Läden schliessen
	  		  RolladenBefehl(Bef);
	  }
 else if (Bef.startsWith("Simul"))
  {
	  String BefPart = Bef.substring(5, 8);

	  if (BefPart == "ein")
		{
		  Simulation = true;
		}
	  else if (BefPart == "aus")
		{
		  Simulation = false;
		}
	  else
		{
		  ParameterFestlegen(Bef.substring(5));
		}
  }
    else if (Bef == "AllesAus") 
  {
    AlleRelaisAus();
  }
    else if  (Bef == "LeseRolladen") 
  {
    RolladenwerteAusEepromLesen(true);

  }
 else if (Bef.indexOf("Rollteil") == 0)
  {
    RolladenwertInEepromSchreiben(Bef);
  }
 else if (Bef.indexOf("Strom") == 0)
  {	
	  /*
	    Strom0: Heizung
		Strom1: Büro
		Strom2: Waschen
		Strom3: Pool
	*/
	 // StromInEepromSchreiben(Bef);
  }
 else if (Bef == "LeseVerbrauch")
  {
	  //Liest die anzahl der Impulse der Stromzähler aus EEprom ab Adresse 200 
	  //StromverbrauchAusEepromLesen();
  }
   else if (Bef.indexOf(":") > 12)
  {
    Serial.println("Uhrzeit erkannt");
	Zeitsimulation = true;
    UhrzeitSetzen(Bef);
  }
   else if (Bef == "CLRStarts") //setzt die Neustarts im EProm auf 0
  {
	  AnzNeustarts = 0;
	  EEPROM.write(AdresseNeustart, AnzNeustarts);
  }

   else if (Bef.startsWith("SetzeImpHeizung"))
  {	
	 int Endpos = Bef.length() - 15;
	// ImpHeiz = Bef.substring(15, Endpos);
  }

  else if  (Bef == "Reset") //Reset des Dachrechners
  {
	  delay (500);
     resetFunc();
  } 
  else
  {
	  Serial.println("kein bekannter Befehl...");
	  Serial.println(Bef);
  }


}

//###############################################################################################
//########################### Kommunikation mit RaspArduino   ##################################
//###############################################################################################
void serialEvent2() {

	static int Zaehle_ok;
	
	int Pruef = 0;

	//Warte auf Daten
	if (Serial2.available() > 1) {
		Serial2.readBytes(RaspArd, RaspArdByts);
		//Serial.println("Empfang vom RaspArduino");
		//Serial2.flush();

		RaspDatenOK = DatenOKvomRaspArd();

		if (RaspDatenOK && RaspArd[88]) BefehlVomKuechendisplay();

		if (Neustart)
			{
			Dachwert[58] = 3; //Info an Raspberry Pi, das Neustart erfolgt ist
			Neustart = false;
			}
		//Daten werden auch zum RaspArd geschickt, wenn ankommende Daten nicht OK!
		for (int i = 0; i < AnzDachbyts; i++)
			{
				Serial2.write(Dachwert[i]);
				Pruef = Pruef + Dachwert[i];
			}
	
		Serial2.write(lowByte(Pruef));  //Zum Schluss Pruefsumme senden
		Dachwert[88] = 0;


	}

	/*
	Debugen(0, "Low", Dachwert[46]);
	Debugen(1, "High", Dachwert[47]);
	*/
}
//###############################################################################################
//###################### Ueberpruefe ankommenden Datenstrom vom Hauptrechner ####################
//###############################################################################################
boolean DatenOKvomRaspArd()
{
	  boolean result = false;
	  static int Fehler;

	   //Pruefsumme berechnen
    unsigned int Pruef = 0;
    for (int i = 0; i < RaspArdByts-1; i++)
      {	 
		Pruef += RaspArd[i];
	  }

	Serial.print("Pruef vom RaspArduino : ") ;
	Serial.print (lowByte(Pruef)) ;
	Serial.print("--") ;
	Serial.println (RaspArd[RaspArdByts-1]) ;
	    
	if (lowByte(Pruef) == RaspArd[RaspArdByts - 1] && Pruef > 0)
	{
		result = true;
		Fehler = 0;
	}
	else
	{
		Fehler++;
		if (Fehler > 10) resetFunc();  //Softwarereset des Dachrechners
	}
	PCMeldung(3, lowByte(Pruef));
	PCMeldung(4, RaspArd[RaspArdByts - 1]);
	PCMeldung(54, Fehler);

  return result;
}
//###############################################################################################
void BefehlVomKuechendisplay()
{
	//virtuino.vMemoryWrite(27, RaspArd[58]);
	BefehlRolladen(RaspArd[58]);

	RaspArd[58] = 0;

}

//###############################################################################################
void BefehlRolladen(int BefNr)
{
	switch (BefNr)
	{
	case 200:
		Serial.println("Alles schliessen");
		AndroidBefehl(2796202);
		break;
	case 201:
		Serial.println("Alles ohne Schlafen schliessen");
		AndroidBefehl(43690);
		break;
	case 202:
		Serial.println("Wohnbereich schliessen");
		AndroidBefehl(2730);
		break;
	case 203:
		Serial.println("Wohnbereich dimmen");
		AndroidBefehl(2730);
		break;
	case 204:
		Serial.println("Baeder schliessen");
		AndroidBefehl(40960);
		break;
	case 205:
		Serial.println("Schlafbereich schliessen");
		AndroidBefehl(2752512);
		break;
	case 206:
		Serial.println("Wohnzimmer schliessen");
		AndroidBefehl(42);
		break;
	case 207:
		Serial.println("Essbereich schliessen");
		AndroidBefehl(2688);
		break;

	case 210:
		Serial.println("Alles oeffnen");
		AndroidBefehl(1398101);
		break;
	case 211:
		Serial.println("Alles_ohne_Schlafen_oeffnen");
		AndroidBefehl(21845);
		break;
	case 212:
		Serial.println("Wohnbereich oeffnen");
		AndroidBefehl(1365);
		break;
	case 213:
		Serial.println("Baeder oeffnen");
		AndroidBefehl(20480);
		break;
	case 214:
		Serial.println("Schlafbereich oeffnen");
		AndroidBefehl(1376256);
		break;
	case 215:
		Serial.println("Wohnzimmer oeffnen");
		AndroidBefehl(21);
		break;
	case 216:
		Serial.println("Essbereich oeffnen");
		AndroidBefehl(1344);
		break;

	}
}
//###############################################################################################
void Windmessen()
{

	const double Fakt = 0.5;
	float Geschw;
	float DurchnGeschw;
	static unsigned int N;
	static unsigned int sumWindGeschw;

	//Windrichtung
	int RohRichtung = analogRead(sensRichtung);
    int Richtung = RohRichtung / 146;
	Dachwert[22] = Richtung;
	int Ri = Richtung + 4;

//	Dachwert[23] = highByte(Richtung);

	
	if (Richtung > 3) Ri = Richtung - 4;	//Richtung drehen für Virtuino
	virtuino.vMemoryWrite(27, float(Ri)); //Richtung an Virtuino

	//Windgeschwindigkeit
	int RohGeschwindigkeit = analogRead(sensGeschw);
	unsigned long Geschwindigkeit = map(RohGeschwindigkeit, 0, 400, 0, 30*360); //hundertfacher Wert in Km/h
	if (Geschw>0) Geschw =  Fakt * DurchnGeschw + (1 - Fakt) * Geschwindigkeit; //Dämpfungsfilter
	else  Geschw = Geschwindigkeit;

	
	Dachwert[23] = WindInBeaufort(Geschw/100);
	Dachwert[24] = lowByte(int(Geschw)); //Hundertfacher Wert der Geschwindigkeit in km/h
	Dachwert[25] = highByte(int(Geschw));

	virtuino.vMemoryWrite(22, float(Geschw/100)); //Windgeschwind an Virtuino
	

	sumWindGeschw = sumWindGeschw + Geschwindigkeit;
	N++;
	DurchnGeschw = sumWindGeschw / N;

/* 
	Debugen(3, "RohGeschwindigkeit", RohGeschwindigkeit);
	Debugen(4, "Geschwindigkeit", Geschwindigkeit);
	Debugen(5, "sumWindGeschw", sumWindGeschw);
	Debugen(6, "DurchnGeschw", DurchnGeschw);
	Debugen(7, "Beaufort", Dachwert[23]);
	Debugen(8, "Anzahl", N);
	Debugen(9, "Geschw", Geschw);
	*/
}
//###############################################################################################
void Lichtsensoren()
{
  int lux = Lichtmesser.readLightLevel();
	Dachwert[0]  =lowByte (lux);
	Dachwert[(1)] = highByte(lux);
	virtuino.vMemoryWrite(21, float(lux)); //Licht an Virtuino

  Licht[0] = analogRead(A0);	//Ost
  Licht[1] = analogRead(A1);	//Sued
  Licht[2] = analogRead(A2);	//West
  Licht[3] = lux;	
 
 

 
  for (int i = 0; i < 3; i++)
  {
	  Dachwert[(i*2 + 2)]  =lowByte (Licht[i]);
	  Dachwert[(i*2 + 3)] = highByte(Licht[i]);
	  virtuino.vMemoryWrite(i+7, float(Licht[i] / 10)); //Licht an Virtuino

	 // Debugen(i, "Licht", Licht[i]);
}
   /*
  Serial.print("Licht: ");
  Serial.print(lux);
  Serial.println(" lx");


  Debugen(9, "Quelle",Rolladenwert[180]);
  for (int i = 0; i < 8; i++)
  {
	  Debugen(i, "Licht", Dachwert[i]);
  }
   */
	}
	
//###############################################################################################
void  Lufdrucksensor() {
 char status;
  double T,P,p0,a;
 
  /*
 Serial.println();
  Serial.print("aktuelle Hoehe: ");
  Serial.print(ALTITUDE,0);
  Serial.print(" Meter, ");
  Serial.print(ALTITUDE*3.28084,0);
  Serial.println(" feet");
  */
  
  status = Luftdruck.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

      status = Luftdruck.getTemperature(T);
    if (status != 0)
    {
     /* Serial.print("Temperatur : ");
      Serial.print(T,2);
      Serial.print(" Grad C, ");
      Serial.print((9.0/5.0)*T+32.0,2);
      Serial.println(" Grad F");
      */
        // Druckmessung starten:
      status = Luftdruck.startPressure(3);
      if (status != 0)
      {
        delay(status);
		status = Luftdruck.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
        /*Serial.print("Absoluter Luftdruck: ");
          Serial.print(P,2);
          Serial.print(" mbar, ");
          Serial.print(P*0.0295333727,2);
          Serial.println(" inHg");*/

            // Der Drucksensor liefert den aboluten Druck, der mit der Hï¿½he variiert.
           // Um die Auswirkungen der Hï¿½he zu entfernen, verwenden Sie die Sealevel-Funktion und Ihre aktuelle Hï¿½he.
           // Diese Zahl wird bei Wetterberichten hï¿½ufig verwendet.
           // Parameter: P = Absolutdruck in mb, ALTITUDE = aktuelle Hï¿½he in m.
           // Ergebnis: p0 = Meeresspiegel-Kompensationsdruck in mbar

          p0 = Luftdruck.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          /*Serial.print("Relativer Luftdruck: ");
          Serial.print(p0,2);
          Serial.print(" mbar, ");
          Serial.print(p0*0.0295333727,2);
          Serial.println(" inHg");*/

          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.

          a = Luftdruck.altitude(P,p0);
      /*Serial.print("Berechnete Hoehe: ");
          Serial.print(a,0);
          Serial.print(" Meter, ");
          Serial.print(a*3.28084,0);
          Serial.println(" feet");*/
        }
      else PCFehlerMeldung("Fehlerabfrage Luftdrucksensor");
      }
     else PCFehlerMeldung("Fehlerausgangs Luftdrucksensor");
    }
   else PCFehlerMeldung("Fehler Temperaturabfrage Luftdrucksensor");
  }
  else PCFehlerMeldung("Fehler Temperaturabfrage Luftdrucksensor");


//Temperatur
   int tl = T *10;
 
   //Absoluter Luftdruck
   long FP = (unsigned long) (P*100);
   int WO = (int) (FP % 65536);
 
   //Relativer Luftdruck
  long FP0 = (unsigned long) (p0*100);
  int WO0 = (int) (FP0 % 65536);
  
  virtuino.vMemoryWrite(6, float(FP/100)); //Luftdruck


   //Temperatur
   Dachwert[14] = lowByte (tl);
   Dachwert[15] = highByte(tl);

   //Absoluter Luftdruck
   Dachwert[16] = (int)(FP / 65536);
   Dachwert[17] = highByte(WO);
   Dachwert[18] = lowByte(WO);

   //Relativer Luftdruck
   Dachwert[19] = (int)(FP0 / 65536);
   Dachwert[20] = highByte(WO0);
   Dachwert[21] = lowByte(WO0);

}
//###############################################################################################
void Feuchtesensor2() {
  
if (millis() < (SendeDHT + PauseDHT))  return;   //Verzögerung der Messung
SendeDHT = millis();

float h2 = Feuchte2.readHumidity();
float t2 = Feuchte2.readTemperature();

float hic2 = Feuchte2.computeHeatIndex(t2, h2, false);

/*
 Serial.print("Feuchte Feuchte1: "); 
   Serial.print(h2);
   Serial.println(" %\t");
   Serial.print("Temperatur2: "); 
   Serial.print(t2);
   Serial.println(" *C");
   Serial.print("Hitzeindex2: "); 
   Serial.print(hic2);
   Serial.println(" *C");
*/
 //Testen, ob das Ergebnis eine Zahl ist
 if (isnan(t2) || isnan(h2)) 
		{PCFehlerMeldung("Fehler beim Lesen des DHT2");} 
 else 
	 {
	 int tt2 = t2*10;
	 int tf2 = h2*10;
	 int thic2 = hic2*10;

	Dachwert[8] = lowByte(thic2);
	Dachwert[9] = highByte(thic2);

	Dachwert[10] = lowByte (tt2);
	Dachwert[11] = highByte(tt2);
	Dachwert[12] = lowByte (tf2);
	Dachwert[13] = highByte(tf2);

	
	virtuino.vMemoryWrite(5, h2); //Luftdruck
	
	 }
	
}
//###############################################################################################
int WindInBeaufort(float Geschw)
{
	//Rechne Geschwindigkeit in Beaufort um
	int Beau;
	
	if (Geschw < 1) Beau = 0;
	else if (Geschw >= 1 && Geschw < 5) Beau = 1;
	else if (Geschw >= 5 && Geschw < 11) Beau = 2;
	else if (Geschw >= 11 && Geschw < 19) Beau = 3;
	else if (Geschw >= 19 && Geschw < 28) Beau = 4;
	else if (Geschw >= 28 && Geschw < 38) Beau = 5;
	else if (Geschw >= 38 && Geschw < 49) Beau = 6;
	else if (Geschw >= 49 && Geschw < 61) Beau = 7;
	else if (Geschw >= 61 && Geschw < 74) Beau = 8;
	else if (Geschw >= 74 && Geschw < 88) Beau = 9;
	else if (Geschw >= 88 && Geschw < 102) Beau = 10;
	else if (Geschw >= 102 && Geschw < 117) Beau = 11;
	else if (Geschw >117) Beau = 12;

	return Beau;
}
