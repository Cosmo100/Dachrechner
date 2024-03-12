
//***********************Betriebsmeldung f�r PC mit Zahlenwert **************************************************
void PCMeldung(int Nr,int Wert)  {

Serial.print("|BM|");
if (Nr < 10) Serial.print("0");
Serial.print(Nr);
Serial.print(Wert);
Serial.println("|");
}

//***********************Betriebsmeldung fuer PC mit Text***************************************************
void PCMeldungText(int Nr,String Text)  {

Serial.print("|BM|");
if (Nr < 10) Serial.print("0");
Serial.print(Nr);
Serial.print(Text);
Serial.println("|");
}
//***********************Statusmeldung f�r PC mit Text***************************************************
void PCStatusMeldung(String Text)  {
Serial.print("|BM|99");
Serial.print(Text);
Serial.println("|");
}
//***********************Statusmeldung f�r PC mit Text***************************************************
void PCFehlerMeldung(String Text)  {
Serial.print("|BM|98");
Serial.print(Text);
Serial.println("|");
}

//***********************Betriebsmeldung f�r PC in Prozent **************************************************
void PCMeldungProzent(int Nr,float Zaehle_ok, float Zaehle_Anz)  {

Serial.print("|BM|");
if (Nr < 10) Serial.print("0");
Serial.print(Nr);

int  N = 100 * (Zaehle_ok / Zaehle_Anz);
Serial.print((String) N);
Serial.println("|");
}
//#################################################### 
void Debugen(int Nr,String Text, double Wert)  {
//if (DatenZumPC) {
	Serial.print("*dbg*");
	if (Nr < 10) Serial.print("0");
	Serial.print(Nr);
	Serial.print("*");
	Serial.print(Text);
	Serial.print("*");
	Serial.print(Wert);
	Serial.println("*");
//	}
}
//#################################################### 
void DebugenS(int Nr,String Text, String Wert)  {
if (DatenZumPC) {
	Serial.print("*dbg*");
	if (Nr < 10) Serial.print("0");
	Serial.print(Nr);
	Serial.print("*");
	Serial.print(Text);
	Serial.print("*");
	Serial.print(Wert);
	Serial.println("*");
	}
}

//************************************************************************************************
//*******************************Rolladendaten an Heisopo schicken********************************
//************************************************************************************************
//Daten werden auf dem Sever in Datei 'Rolladendoku' gespeichert
void RolladendatenAnHeisopo()
{
	static bool Gesendet[11];

 for (int LadenNr = 0; LadenNr < 11; LadenNr++)
  {
	if (Ladendaten[LadenNr].length() > 0 && Sendelerlaubnis)
	{//wenn Rolladenmeldung vorhanden, sende an Heisopo
			Serial.print("Sende an Heisopo:");
			Serial.print(LadenNr);
			Serial.print(" -> ");
			Serial.println(Ladendaten[LadenNr]);
			if (!Gesendet[LadenNr])  Gesendet[LadenNr] = SendToInternet(Ladendaten[LadenNr]);

			if (Ladendaten[LadenNr].indexOf("---") < 0) 
				{
					Gesendet[LadenNr] = SendToInternet(Ladendaten[LadenNr]);
					if (Gesendet[LadenNr])
						{
							Ladendaten[LadenNr]="";
							Gesendet[LadenNr] = false;
					    }
			    }
}
	
 }
}
//###############################################################################################
//################################# Sende Daten ins Internet ####################################
//###############################################################################################
boolean SendToInternet(String Sendedaten)
{
/*
	//const int Pause = 2000; //Pause zwischen den Sendevorgängen (gilt nur für Rolladen

  boolean result = false;
   int OK = 0; //Meldung fuer PC
   String  Zieldatei = "";
  
   if (Sendedaten.startsWith("Werte="))
			{Zieldatei = "Auswertung/Eingang.php";} //Messdaten
   else if (Sendedaten.startsWith("Relais="))
			{
				//if (millis() < Sendezeit + Pause) return false;
				Zieldatei = "Auswertung/Laden.php";}  //Rolladenwerte
   else return false;

  
   PCMeldungText(60,"Versuche Internetverbindung");

  // Bei Verbindung: Rueckmeldung ueber Serial
  if (Heisopo.connect(server, 80)) {
	OK = 1;

   PCMeldungText(60,"Erfolg - sende zum Internet :");
   PCStatusMeldung("Erfolg - sende zum Internet :");
    // Make a HTTP request:
    Heisopo.print("POST //");
	  Heisopo.print(Zieldatei);
	  Heisopo.println(" HTTP/1.1");

    Heisopo.println("Host: www.heisopo.de");
    //Heisopo.println("Host: localhost");
    Heisopo.println("Content-Type: application/x-www-form-urlencoded");
    //Heisopo.println("User-Agent: Arduino/1.0");
    Heisopo.println("Connection: close");
    Heisopo.print("Content-Length: ");
    Heisopo.println(Sendedaten.length());
    Heisopo.println();
    Heisopo.println(Sendedaten);
    Heisopo.println();
    result = true;
	Sendelerlaubnis = false;
    Sendezeit = millis();
  }
  else {
    PCFehlerMeldung("keine Verbindung zum Internet");
  	PCMeldungText(60,"keine Verbindung zum Internet");
  }

  PCMeldung(2,OK);
  return result;
  */
}
//###############################################################################################
//########################### Datenstrom von Heisopo empfangen ##################################
//###############################################################################################
void HeisopoAbfragen()
{
	/*
	 String str;
 
   if (Heisopo.available()) //wenn byts vom Server kommen --> anzeigen
		{
		 str = Heisopo.readStringUntil(13);
		 Serial.print(str);
		 if (str.indexOf("HTTP/1.1 200 OK") > -1 ) Sendelerlaubnis= true;
		 PCMeldung(3,1);
		}


  // if the server's disconnected, stop the client:
  if (!Heisopo.connected()) { Heisopo.stop();}
 */  
}
//###############################################################################################
//########################### Virtuino abfragen ##################################
//###############################################################################################
void VituinoAbfragen()
{
	static unsigned long AltLadenbefehl;

	//Werte zu Android-Virtuino schicken
	virtuino.vMemoryWrite(0, Wert(14));	//Aussentemp
	virtuino.vMemoryWrite(1, Wert(10)); //Warmwasser
	virtuino.vMemoryWrite(2, Wert(12));	//Kessel
	virtuino.vMemoryWrite(3, Wert(8)); //Solarkollektor
	virtuino.vMemoryWrite(4, Wert(16)); //Poolwasser Saugseite
	/*vom Dachrechner
	5:Aussenfeuchte
	6:Luftdruck
	7:Licht O
	8:Licht S
	9:Licht W
	*/
	virtuino.vMemoryWrite(10, Wert(18)); //TempWohnzimmer
	virtuino.vMemoryWrite(11, Wert(20) / 10); //Druck Tauchpumpe
	virtuino.vMemoryWrite(12, Wert(22)); //Temp Poolkollektor
	virtuino.vMemoryWrite(13, 10*Wert(24) ); //Druck Filter
	virtuino.vMemoryWrite(14, 10 * Wert(26)); //Niveau Becken 1
	virtuino.vMemoryWrite(15, 10 * Wert(36)); //Niveau Becken 2
	virtuino.vMemoryWrite(16, 10 * Wert(38)); //Niveau Becken 3
	virtuino.vMemoryWrite(17, Wert(40)/ 10); //Wassserdruck
	virtuino.vMemoryWrite(18,10 * Wert(42)); //Öffnung Garagenladen
	virtuino.vMemoryWrite(19,Wert(44)*10); //Aktuelle PV-Leistung

	virtuino.vMemoryWrite(24, Wert(32)*10); //Einstrahlung
    virtuino.vMemoryWrite(25, Wert(34)); //Temp Arbeitszimmer
	virtuino.vMemoryWrite(26, Wert(30)); //Poolwasser Druckseite

	
	//Schaltzustände von RaspArd[28] 
	//virtuino.vMemoryWrite(28, RaspArd[28]);
	//virtuino.vMemoryWrite(29, RaspArd[29]);



	//Rolladenbefehle abfragen:
	unsigned long Ladenbefehl = virtuino.vMemoryRead(31);	//Rolladenwert lesen
	Dachwert[88] = virtuino.vMemoryRead(23) + 100;	//Befehl von Android wird zum Rasparduino geschickt

	if (Dachwert[88]>100)
		{
		Serial.print("Befehl von Android :");
		Serial.println(Dachwert[88]);
		if (Dachwert[88] == 104) resetFunc(); //Reset des Rechners
		}

	virtuino.vMemoryWrite(30, Ladenbefehl); //Rückmeldung an Android
	
	if (Ladenbefehl > 0 && AltLadenbefehl != Ladenbefehl) AndroidBefehl(Ladenbefehl); //Befehl zum Steuern der Rolladen auswerten
	
  
	
	Debugen(12, "Ladenbefehl", Ladenbefehl);
	Debugen(13, "VonHand[0]", VonHand[0]);

	AltLadenbefehl = Ladenbefehl;
	
}
//###############################################################################################
int HighbyteFirst(int BytNr)
{
int WE = RaspArd[BytNr]*256+RaspArd[BytNr+ 1] ;
if (RaspArd[BytNr] > 128) WE = WE - 65535;

return WE;
}

//###############################################################################################
float Wert(int BytNr)
{
	float WE = word(RaspArd[BytNr + 1], RaspArd[BytNr]);
	
	if (RaspArd[BytNr + 1] > 128) WE = WE - pow(2,16);
	
	WE /= 10;

	return WE;
}
//########################### Bitwert am Analogeingang umwandeln ##################################
int BitToTemp(int Bitwert)
 {
	 //Abgleichwerte des Messwandlers:
const int Tmax = 50;
const int Tmin = 0;

 { //Rückgabe ist Temp * 10
  int Tempwert = 10 * (float) (Tmax-Tmin)*Bitwert/1023;
  return Tempwert + Tmin*10 ;
 }

 }

//***********************************************************************************************
void UhrzeitSetzen (String Zeit) {
//Funktion setzt die Zeit, kann später entfernt werden

int Tag = Zeit.substring(0,2).toInt();
int Monat =  Zeit.substring(3,5).toInt();
int Jahr =  Zeit.substring(6,10).toInt();
int Stunde = Zeit.substring(11,13).toInt();
int Minute =  Zeit.substring(14,16).toInt();
int Sekunde =  Zeit.substring(17,19).toInt();


setTime(Stunde,Minute,Sekunde,Tag,Monat,Jahr);
SunRiseSet();    //Sonnenauf und Untergang berechnen

//T = 60*Stunde + Minute; //nur für Simulation
for (int LadenNr = 0 ; LadenNr < 11; LadenNr++)
{AlteZeit[LadenNr] = 0;}

}
//*********************** Feststellen, ob Anforderung vom PC  **************************************************
bool DatentimerPruefen () {
	//wenn nach "Watchdog" Sekunden Zeit keine Sendeanforderung vom PC gekommen ist, keine Meldungen mehr zum PC senden,
	unsigned long N = millis()/1000 - TimerBetriebsmeldung;
	
	//Debugen(3,"Timer",N);
	//Debugen(4,"DatenZumPC",DatenZumPC);

	if (N > Watchdog) return false; else return true;
}
//#################################################### 

void  ZeitInfo()
{
	//Wird bei jedem Durchlauf weitergegeben
	String Zeit = FormZeit();  //aktuelle Uhrzeit
	String Datum = FormDate();  //aktuelles Datum
	PCMeldungText(61, Zeit);
	PCMeldungText(62, Datum);

	PCMeldungText(63, Sunrise);
	PCMeldungText(64, Sunset);

	if (!ZeitOK)
	{
		ZeitSetzen();
		SunRiseSet();    //Sonnenauf und Untergang berechnen  -- 
	}
	//nachts um 00:00 Sonnenauf und Untergang neu berechnen 
	if (hour() == 0 && minute() == 0) ZeitOK = false;

}
//####################################################################### 
void ZeitSetzen()
{
	//Korrigiert die Zeit des DachArduino, wenn diese nicht in der
	if (RaspDatenOK)   //Zeit nur korrigieren, wenn Daten vom RaspArd korrekt sind
	{
	setTime(RaspArd[3], RaspArd[4], RaspArd[5], RaspArd[2], RaspArd[1], RaspArd[0] + 2000);  //setzt die Zeit des Arduino mit den Daten des RaspArd
	RTC.set(now());		//aktuelle Zeit in Echtzeituhr übertragen
	
	/*Debugen(0, "Stunde", RaspArd[3]);
	Debugen(1, "Minute", RaspArd[4]);
	Debugen(2, "Sekunde", RaspArd[5]);
	Debugen(3, "Tag", RaspArd[2]);
	Debugen(4, "Monat", RaspArd[1]);
	Debugen(5, "Jahr", RaspArd[0]);*/
	
	ZeitOK = true;
}
}

//#################### Uhrzeit weitergeben an Arduino Keller #######
void  SunRiseSet()
{
	//Berechnung von Sonnenauf und Untergang
	bool Sommerzeit = summertime_EU();    //ist Sommerzeit
	int laSunrise = Heimat.sunrise(year(), month(), day(), Sommerzeit);  //Sommerzeit aktiv: letztes Argument: true
	int laSunset = Heimat.sunset(year(), month(), day(), Sommerzeit);

	Dusk2Dawn::min2str(Sunrise, laSunrise);
	Dusk2Dawn::min2str(Sunset, laSunset);

	Serial.print("Sonnenaufgang ");
	Serial.println(Sunrise);
	Serial.print("Sonnenuntergang ");
	Serial.println(Sunset);


	int SAh = atoi(Sunrise);
	int SAm = atoi(&Sunrise[3]);

	int SUh = atoi(Sunset);
	int SUm = atoi(&Sunset[3]);

	SA = SAh * 60 + SAm;
	SU = SUh * 60 + SUm;

}


//#############################################################################################
String FormZeit() {
	return timepart(hour(), false) + timepart(minute(), true) + timepart(second(), true);
}

String timepart(int digit, bool Z)
{
	String tp = (String)digit;
	if (digit < 10)  tp = "0" + tp;
	if (Z)  tp = ":" + tp;
	return tp;
}

String FormDate() {
	return datepart(day(), false) + datepart(month(), true) + datepart(year(), true);
}

String datepart(int digit, bool Z)
{
	String dp = (String)digit;
	if (digit < 10)  dp = "0" + dp;
	if (Z)  dp = "." + dp;
	return dp;
}


void FormatierteUhrzeit() {
	printDigit(hour(), false);
	printDigit(minute(), true);
	printDigit(second(), true);
	Serial.println();
}

void printDigit(int digit, bool Z)
{
	if (Z)  Serial.print(":");
	if (digit < 10) Serial.print("0");
	Serial.print(digit);
}

//*********************************** Berechne Sommerzeit ************************************
boolean summertime_EU()
//Rückgabe true, wenn Sommer - false,wenn Winter
{
int Y = year();
byte M = month();
byte D = day();
byte h = hour();
const byte tzHours = 1;  //tzHours (0=UTC, 1=MEZ)

	if (M < 3 || M>10) return false; // keine Sommerzeit in Jan, Feb, Nov, Dez
	if (M > 3 && M < 10) return true; // Sommerzeit in Apr, Mai, Jun, Jul, Aug, Sep
	if (M == 3 && (h + 24 * D) >= (1 + tzHours + 24 * (31 - (5 * Y / 4 + 4) % 7)) || M == 10 && (h + 24 * D) < (1 + tzHours + 24 * (31 - (5 * Y / 4 + 1) % 7)))
		return true;
	else
		return false;
}
//*********************************** Neustart lesen und erhöhen ************************************
void NeustartsLesen()
{	
	AnzNeustarts = EEPROM.read(AdresseNeustart);
	AnzNeustarts++;
	EEPROM.write(AdresseNeustart, AnzNeustarts);
}
//********************************** Teste RTC 1307 ****************************
void TesteDS1307RTC()
{
	tmElements_t tm;
//
	if (RTC.read(tm)) {
	//Serial.print("Ok, Time = ");
	//	print2digits(tm.Hour);
	//	Serial.write(':');
	//	print2digits(tm.Minute);
	//	Serial.write(':');
	//	print2digits(tm.Second);
	//	Serial.print(", Date (D/M/Y) = ");
	//	Serial.print(tm.Day);
	//	Serial.write('/');
	//	Serial.print(tm.Month);
	//	Serial.write('/');
	//	Serial.print(tmYearToCalendar(tm.Year));
	//	Serial.println();
	}
	else {
		if (RTC.chipPresent()) {
			Serial.println("DS1307-Echtzeit laeuft nicht. SetTime starten");
			Serial.println("example to initialize the time and begin running.");
			Serial.println();
			ZeitSetzen();	//Zeit des Dachrechners und des RTC neu setzen
		}
		else {
			Serial.println("DS1307 read error! Chip ueberpruefen.");
			Serial.println();
		}
	
	}
	
}
//
////**********************************************************************
void print2digits(int number) {
	if (number >= 0 && number < 10) {
		Serial.write('0');
	}
	Serial.print(number);
}
////**********************************************************************
void ParameterFestlegen(String Argumente)
{

	int Pos = Argumente.indexOf(",");
	Testnummer = Argumente.substring(0, Pos).toInt();

	int Pos1 = Argumente.indexOf(",", Pos + 1);
	int Stunde = Argumente.substring(Pos + 1, Pos1).toInt();

	Pos = Pos1 + 1;
	Pos1 = Argumente.indexOf(",", Pos );
	int Minute = Argumente.substring(Pos, Pos1).toInt();

	Pos = Pos1 + 1;
	Pos1 = Argumente.indexOf(",", Pos);
	IntervLicht = 1000 * Argumente.substring(Pos, Pos1).toInt(); //Pause in Millisekunden

	Pos = Pos1 + 1;
	Pos1 = Argumente.indexOf(",", Pos);
	SimLicht = Argumente.substring(Pos, Pos1).toInt();

	Pos = Pos1 + 1;
	Pos1 = Argumente.indexOf(",", Pos);
	
	int Ab = Argumente.substring(Pos, Pos1).toInt();
	if (Ab==1) {SimAb = true;}	else	{SimAb = false;}
		
	Pos = Pos1 + 1;
	Pos1 = Argumente.indexOf(",", Pos);
	
	int Zu = Argumente.substring(Pos, Pos1).toInt();
	if (Zu==1) {ZustandLaden[Testnummer] = 100;}
	if (Zu == 2) { ZustandLaden[Testnummer] = 0; }
	if (Zu == 3) { ZustandLaden[Testnummer] = -1; }

	Pos = Pos1 + 1;
	Pos1 = Argumente.indexOf(",", Pos);
	int Sim = Argumente.substring(Pos, Pos1).toInt();
	if (Sim == 1)
	{ 
		Simulation = true; 	
		StartLicht = millis();
	}

	else { Simulation = false; }

	DeltaUhrzeit = (hour() * 60 + minute()) - (Stunde * 60 + Minute);


//Serial.println(Testnummer);
//Serial.println(Stunde);

//Serial.println(Minute);
//Serial.println(IntervLicht);
//Serial.println(SimLicht);
//Serial.println(Ab);
//Serial.println(Zu);
//Serial.println(Sim);

//Serial.println(Argumente);

}
////**********************************************************************
void Ladensimulation()
{
	if ( millis() > StartLicht + IntervLicht)
		
	{
		if (SimAb)
		{
			SimLicht++;
			if (SimLicht > 1000) SimLicht = 1000;  //Lichtstärke beim Erhöhen auf 1000 begrenzen
		}
		else
		{ 
			SimLicht--;
			if (SimLicht < 0) SimLicht = 0;  //Lichtstärke beim Vermindern auf Null begrenzen
		}

		StartLicht = millis();
		String th = String(int(SimZeit / 60));
		if (th.length() == 1) th = "0" + th;
		
		String tm = String(int(SimZeit % 60));
		if (tm.length() == 1) tm = "0" + tm;

		String tSim = th + ":" + tm;

		Debugen(14, "SimZeit", SimZeit);
		DebugenS(15, "Zeit", tSim);
	
	}
	SimZeit = hour() * 60 + minute() - DeltaUhrzeit;


}