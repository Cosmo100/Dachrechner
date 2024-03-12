/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++++++++++++++++++++++++++++ Steuerung der Rolladen ++++++++++++++++++++++++++
+++++++++++ Version fuer Arduino +++++++++ Stand: 05.07.2017 +++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	Platine 1 :
0	Digital 22 (0) --> Wohnzimmertuer oeffnen 				 --> set Value 31  =	1
1	Digital 23 (1) --> Wohnzimmertuer schliessen			 --> set Value 31  =	2
2	Digital 24 (2) --> Wohnzimmerelement oeffnen			 --> set Value 31  =	4
3	Digital 25 (3) --> Wohnzimmerelement schliessen			 --> set Value 31  =	8
4	Digital 26 (4) --> Wohnzimmerfenster oeffnen			 --> set Value 31  =	16
5	Digital 27 (5) --> Wohnzimmerfenster schliessen			 --> set Value 31  =	32
6	Digital 28 (6) --> Kuechenfenster oeffnen				 --> set Value 31  =	64
7	Digital 29 (7) --> Kuechenfenster schliessen 			 --> set Value 31  =	128
					
	Platine 2 :				
8	Digital 30 (8) --> Esszimmertuer oeffnen				 --> set Value 31  =	256
9	Digital 31 (9) --> Esszimmertuer schliessen				 --> set Value 31  =	512
10	Digital 32 (10) --> Esszimmerfenster oeffnen			 --> set Value 31  =	1024
11	Digital 33 (11) --> Esszimmerfenster schliessen			 --> set Value 31  =	2048
12	Digital 34 (12) --> Bad Juergen oeffnen					 --> set Value 31  =	4096
13	Digital 35 (13) --> Bad Juergen schliessen				 --> set Value 31  =	8192
14	Digital 36 (14) --> Bad Pia oeffnen						 --> set Value 31  =	16384
15	Digital 37 (15) --> Bad Pia schliessen					 --> set Value 31  =	32768
				
	Platine 3 :				 
16	Digital 38 (16) --> Schlafzimmerfenster oeffnen			 --> set Value 31  =	65536
17	Digital 39 (17) --> Schlafzimmerfenster schliessen		 --> set Value 31  =	131072
18	Digital 40 (18) --> Zimmer Alexander oeffnen			 --> set Value 31  =	262144
19	Digital 41 (19) --> Zimmer Alexander schliessen			 --> set Value 31  =	524288
20	Digital 42 (20) --> Zimmer Stefanie oeffnen				 --> set Value 31  =	1048576
21	Digital 43 (21) --> Zimmer Stefanie schliessen			 --> set Value 31  =	2097152

Nr der Rolladen:
0    --> Wohnzimmertuer
1	 --> Wohnzimmerelement
2	 --> Wohnzimmerfester
3	 --> Esszimmerfenster

4	 --> Esszimmertuer
5	 --> Kuechenfenster
6	 --> Bad Juergen
7	 --> Bad Pia

8	 --> Schlafzimmerfenster
9	 --> Zimmer Alexander
10 --> Zimmer Stefanie
11 --> frei


Definition der Betriebsart:
(wird fuer oeffnen u. Schliessen getrennt eingestellt.
0 --> Von Hand
1 --> ueber Zeit
2 --> ueber Sonnenstand
3 --> ueber Helligkeit
*/
//###############################################################################################
//################# Sollwerte aus Arduino-EEProm lesen und zum PC senden  #######################
//###############################################################################################
void RolladenwerteAusEepromLesen(bool Senden) {


 if (Senden) Serial.print("Rolladenwerte=");
  int Pruef = 0;

  for (int i = 0; i < AnzSoll; i++)
  {
    Rolladenwert[i] = EEPROM.read(AdresseEepromRolladen + i);
    Pruef += Rolladenwert[i];
   if (Senden) Serial.print((String) Rolladenwert[i] + ",");
  }

 if (Senden) Serial.println(lowByte(Pruef));
}


//###############################################################################################
//################# Rolladenwerte in Arduino-EEProm schreiben  #####################################
//###############################################################################################
void RolladenwertInEepromSchreiben(String Bef) {

   int Endpos = 0;
   DatenZumPC = false;
  
    int Nr =  Bef.substring(8, 9).toInt(); 
    Serial.print("TeilNr ");
    Serial.print(Nr);
     Serial.println(" erkannt");
     String Rolladenstring = Bef.substring(10); 
     Serial.println(Rolladenstring);

      int Start = Nr * 60;
      int Adresse = Start;
     
  while (Rolladenstring.indexOf(',') > 0)
    {
    Endpos = Rolladenstring.indexOf(',');       //naechstes Komma suchen
    Rolladenwert[Adresse] = Rolladenstring.substring(0,Endpos).toInt();
    Rolladenstring = Rolladenstring.substring(Endpos+1,Rolladenstring.length());
    Adresse++;
    }
    Rolladenwert[Adresse] = Rolladenstring.substring(0,Rolladenstring.length()).toInt();
 

for (int i = Start; i < Adresse+1; i++){
      EEPROM.write(AdresseEepromRolladen + i, Rolladenwert[i]);
	  
     }
     
if (Nr < 3 )
  {
    Nr ++;
    Serial.print("OK Roll");   //anfordern der Sollwerte
    Serial.println(Nr);   //anfordern der Sollwerte
  }
  else { 
      DatenZumPC = true;  
      PCMeldung(40, 1); //Erfolgreich uebertragen und in Eprom geschrieben
	  RolladenwerteAusEepromLesen(false); //Und neu lesen
  }
}

//###############################################################################################
//#################################### Steuerung der Rolladen ##################################
//###############################################################################################
void LadenPruefen(int Nr)
{


int BA = AdresseEepromRolladen + 16 * Nr; //Basisadresse des Rolladens im Eprom
int AktZeit;
int OefArt = Rolladenwert[BA];
int SchlieArt = Rolladenwert[BA+1];
int OefZeit = 60 * Rolladenwert[BA+2] + Rolladenwert[BA+3];
int SchlieZeit = 60 *  Rolladenwert[BA+4] + Rolladenwert[BA+5];
int OefHell = 10*Rolladenwert[BA+6]; 
int SchlieHell = 10*Rolladenwert[BA+7];
int OefOffset = Rolladenwert[BA+8];
	if (OefOffset > 128) OefOffset = OefOffset - 256;
int SchlieOffset = Rolladenwert[BA+9];
	if (SchlieOffset > 128) SchlieOffset = SchlieOffset - 256;

bool BlendschutzAktiv = Rolladenwert[BA+14];

//if (BlendschutzAktiv) DimmenUeberwachen(Nr);

if (Simulation)
 {
	Ladensimulation();
	AktZeit = SimZeit;
	Lichtquelle = SimLicht;
 }
else
{
  AktZeit = 60 * hour() + minute();
  Lichtquelle = Licht[Rolladenwert[180]];
 }


  if (Nr == Testnummer && Simulation)
  {
	Debugen(0, "Test LadenNr", Testnummer);
	Debugen(1,"Lichtquelle", Lichtquelle);
	Debugen(2,"SchlieHell", SchlieHell);
	Debugen(3,"AlteHelligkeit", AlteHelligkeit[Nr]);
	Debugen(4,"Anzahl", Anzahl[Nr]);
	Debugen(5,"OefArt", OefArt);
	Debugen(6,"Anzahl",Anzahl[Nr]),
    Debugen(7,"ZustandLaden[Nr]", ZustandLaden[Nr]);
	Debugen(8, "AktZeit", AktZeit);
	Debugen(9, "Simulation", Simulation);
	Debugen(10, "OefZeit", OefZeit);
 }
    
  
  
  if (SchonGemessen)  //Nach Rechnerstart -> zunaechst Werte setzen
   { 
	 //oeffnen ueberpruefen
  if ((AktZeit > 360 && AktZeit <600) && (ZustandLaden[Nr] < 80))//Öffnen nur,Zeit zwischen 7:00 Uhr und 10:00 Uhr und Laden geschlossen oder nicht definiert
				{
	 			switch (OefArt) {
					case 1: //Zeit
						//Serial.println("Oeffnen Zeit");}
						if (AktZeit >= OefZeit) Anzahl[Nr]++;
					break;
					case 2: //Sonnenaufgang
						 //Serial.println("Oeffnen Sonne");
						 if (AktZeit >= SA + OefOffset) Anzahl[Nr]++;
					break;
					case 3: //Helligkeit
						 //Serial.println("Oeffnen Helligkeit");
						//if ((LichtOst >= OefHell) && (AlteHelligkeit[Nr] < OefHell))  Anzahl[Nr]++;
						if (Lichtquelle >= OefHell)  Anzahl[Nr]++;
					break;
					}
				if (Anzahl[Nr] >= WieOft) //die Bedingung zum Öffnen muss "WieOft" mal erfüllt sein, um zu öffnen
					{
					RelaisEin(Nr * 2, OefArt);
					Anzahl[Nr] = 0;
					}
			}
					   //schliessen ueberpruefen
	if ((AktZeit > 980) && ((ZustandLaden[Nr] < 0) || (ZustandLaden[Nr] > 10))) //Schließen nur, wenn aktuelle Zeit nach 16.20 Uhr
						{																				//und Laden offen(>90) oder nicht definiert
						//VonHandgeschlossen[Nr] = false;
						switch (SchlieArt) {
							case 1: //Zeit
								
    							if (AktZeit == SchlieZeit)  Anzahl[Nr]++;
    							  break;
							case 2: //Sonnenuntergang
							
								if (AktZeit == SU + SchlieOffset)  Anzahl[Nr]++;
      							break;
							case 3: //Helligkeit
								 //if ((LichtOst <= SchlieHell) && (AlteHelligkeit[Nr] > SchlieHell)) RelaisEin(Nr*2+1,SchlieArt);
								 if (Lichtquelle <= SchlieHell)  Anzahl[Nr]++;
								  break;
							
						}
						if (Anzahl[Nr] >= WieOft) //die Bedingung zum Öffnen muss "WieOft" mal erfüllt sein, um zu schliessen
							{
							RelaisEin(Nr * 2 + 1, SchlieArt);
							//Debugen(5, "Anzahl[1]", Anzahl[1]);
							Anzahl[Nr] = 0;
							}
						}


	/*Debugen(2,"Alte Zeit",AlteZeit[Nr]);
	String At = timepart(int(AlteZeit[Nr]/60),false) + timepart(AlteZeit[Nr]%60,true);
	DebugenS(4,"Alte Zeit",At);
	Debugen(6,"Alte Helligkeit",AlteHelligkeit[Nr]);
*/
   }

       AlteZeit[Nr] = AktZeit;
	   AlteHelligkeit[Nr] = Lichtquelle;
	   SchonGemessen = true;

}

//###############################################################################################
void DimmenUeberwachen (int Nr)
{
int BA = AdresseEepromRolladen + 16 * Nr;	

int OefBlend = Rolladenwert[BA+10];
int SchlieBlend = Rolladenwert[BA+11];
int Dimmgrad = Rolladenwert[BA+15];


//Debugen(Nr,"Blendschutz",Dimmgrad);
//Laden zufahren, wenn zu hell und Laden offen
if ((Lichtquelle >= SchlieBlend) && (AlteHelligkeit[Nr] < SchlieBlend) && (ZustandLaden[Nr] > 90)) LadenDimmen(Nr,Dimmgrad);

//nach dimmen wieder komplett öffnen, wenn Lichtwert zu niedrig
else if ((Lichtquelle < OefBlend) && (AlteHelligkeit[Nr] > OefBlend) && (ZustandLaden[Nr] < 90))  RelaisEin(Nr*2,4);

}
//####################### Laden dimmen ###################################################
void BefLadenDimmen(String Bef)
{
int Pos = Bef.indexOf("auf");
int Nr =  Bef.substring(5,Pos).toInt(); 
int Dimmgrad = Bef.substring(Pos+3,Bef.length()).toInt(); 
LadenDimmen(Nr,Dimmgrad);

}
//####################### Laden dimmen ###################################################
void LadenDimmen(int Nr, int Dimmgrad)
{
 
int RelNr = Nr*2;
int DeltaDimm = 0;

//Zustand festlegen auf Grund der Helligkeit
if (ZustandLaden[Nr] ==-1)  //Laden ist nicht definiert
		{
			if (Lichtquelle < 400) ZustandLaden[Nr] = 0; else ZustandLaden[Nr] = 100;
		}


if (ZustandLaden[Nr] > 90)  //Laden ist offen
		{
		DebugenS(5,"Laden", "offen");
		AnzZeit[RelNr+1] = Anzugszeit[Nr*2+1]; //Schließzeit verwenden
		AnzZeit[RelNr+1] = AnzZeit[Nr*2+1]*(100-Dimmgrad)/100;
		DebugenS(10,"Bef", "Schliessen");
		RelaisEin(RelNr+1,4); //Relais Schliessen ein, Grund Dimmen
		}
else if (ZustandLaden[Nr] < 10 )  //Laden ist geschlossen
		{
		DebugenS(5,"Laden", "geschlossen");
		DebugenS(10,"Bef", "Oeffnen");
		AnzZeit[RelNr] = Anzugszeit[Nr*2]; //Oeffnenzeit verwenden
		AnzZeit[RelNr] = AnzZeit[Nr*2]*Dimmgrad/100;
		RelaisEin(RelNr,4); //Relais Oeffnen ein, Grund Dimmen
		}
else	//Laden ist schon gedimmt, anderer Dimmwert
		{
		DeltaDimm = ZustandLaden[Nr] - Dimmgrad;
		if (DeltaDimm > 0)
			{
			DebugenS(5,"Laden", "gedimmt");
			AnzZeit[RelNr+1] = Anzugszeit[Nr*2+1]; //Schließzeit verwenden
			AnzZeit[RelNr+1] = AnzZeit[Nr*2+1]*DeltaDimm/100;
			DebugenS(10,"Bef", "Schliessen");
			RelaisEin(RelNr+1,4); //Relais Schliessen ein, Grund Dimmen
			}
		else 
			{
			DeltaDimm = abs(DeltaDimm);
			DebugenS(5,"Laden", "gedimmt");
			DebugenS(10,"Bef", "Oeffnen");
			AnzZeit[RelNr] = Anzugszeit[Nr*2]; //Oeffnenzeit verwenden
			AnzZeit[RelNr] = AnzZeit[Nr*2]*DeltaDimm/100;
			RelaisEin(RelNr,4); //Relais Oeffnen ein, Grund Dimmen
			}

		}


//Debugen(0,"Pos", Pos);
//Debugen(1,"String", Nr);
//DebugenS(2,"Bef", Bef);
//Debugen(3,"Grad", Dimmgrad);
//Debugen(4,"Zustand", ZustandLaden[Nr]);
//Debugen(5,"Oeffnenzeit",Anzugszeit[Nr*2]);
//Debugen(6,"Schliesszeit", Anzugszeit[Nr*2+1]);
//Debugen(7,"AnzZeit", AnzZeit[Nr*2+1]);
//Debugen(8,"DeltaDimm", DeltaDimm);

}

//###############################################################################################
//################### Ladenzustand wird über USB zum PC gesendet ##################################
//###############################################################################################
void SendeLadenzustand()
	//über USB zum PC
{
 String Ladendaten = String("Laden="); //+ String(LadenOffen) + "," + String(Relaiszustand,BIN) ;

 for (int i = 0; i < 11; i++)
    {
    Ladendaten = Ladendaten + ZustandLaden[i]  + ",";
    }

  Ladendaten = Ladendaten + "->" + String(Relaiszustand) ;
  Serial.println(Ladendaten); //Ladendaten zum PC senden

}
//*********************************************************************************************************************
void RelaisEin (int Nr, int Grund) {
	//Relais 0-23 
	if (Grund == 0) VonHand[Nr] = true;

	String Sun;

	//Oeffnen oder Schliessen?  Komplementäres Relais ausschalten
	if (Nr % 2 == 0) RelaisAus(Nr+1); else RelaisAus(Nr-1);
	if (Nr % 2 == 0) Sun = Sunrise; else Sun = Sunset; 

 RelaisEinZeit[Nr] = millis()/1000; 
 int LadenNr = Nr/2;
 /*
//Nur zur Dokumentation
int BA = AdresseEepromRolladen + 16 * LadenNr;
int AktZeit = hour()*60 + minute();   //Vergangene Zeit seit Mitternacht
int OefArt = Rolladenwert[BA];
int SchlieArt = Rolladenwert[BA+1];
int OefZeit = 60 * Rolladenwert[BA+2] + Rolladenwert[BA+3];
int SchlieZeit = 60 *  Rolladenwert[BA+4] + Rolladenwert[BA+5];
int OefHell = 10*Rolladenwert[BA+6];
int SchlieHell = 10*Rolladenwert[BA+7];
int OefOffset = Rolladenwert[BA+8]; //bezieht sich nur auf Sonnenaufgang
int SchlieOffset = Rolladenwert[BA+9];//bezieht sich nur auf Sonnenuntergang

  Serial.print("Relais ");
  Serial.print(Nr);
  Serial.print(" - Dauer ein=");
  Serial.print(Anzugszeit[Nr]); 
  
  Serial.print("s - Grund: ");
  Serial.print(Grund); 

	//String TS = timepart(T/60,false) + timepart(T % 60,true)+ timepart(0,true); //Nur zur Simulation
  String TS = timepart(hour(),false) + timepart(minute(),true)+ timepart(second(),true);
  Serial.print(" - Uhrzeit: ");
  Serial.print(TS);
  Serial.print(" - Licht: ");
  Serial.println(Lichtquelle);
 
 
  //Zufallszahl zur Identifizierung in der Datenbank auf dem Server
  int ID = random(1000);

     Ladendaten[LadenNr] = String("Relais=");
	 Ladendaten[LadenNr]+= (String) ID + ",";				//Zufalls-ID
	 Ladendaten[LadenNr]+= (String) Nr + ",";				//RelaisNr
	 Ladendaten[LadenNr]+= TS + ",";		//ZeitEin
	 Ladendaten[LadenNr]+="---,";		//ZeitAus
	 Ladendaten[LadenNr]+=(String) Lichtquelle + ",";		    //Helligkeit
	 Ladendaten[LadenNr]+= (String) Grund + ",";				//Grund
	 Ladendaten[LadenNr]+= Sun + ",";				//Sonnenauf oder untergang
	 Ladendaten[LadenNr]+="999";		    //Zustand
*/
	//SendToInternet(Ladendaten[LadenNr]);
	 //Serial.println(Ladendaten[LadenNr]);
	 if (Nr < 8) aus = HIGH; else aus = LOW;

	digitalWrite(RelStart + Nr,aus);  

}
//*********************************************************************************************************************
void RelaisAus (int Nr) {
	if ( Nr < 8) aus = LOW; else aus = HIGH;

  Serial.print("Relais ");
  Serial.print(Nr);
   Serial.print(" aus - Uhrzeit: ");
     //String TS = timepart(T/60,false) + timepart(T % 60,true) + timepart(0,true);
  String TS = timepart(hour(),false) + timepart(minute(),true) + timepart(second(),true);
  Serial.println(TS);

int LadenNr = Nr/2;
digitalWrite(RelStart + Nr,aus);		//Relais ausschalten

Ladendaten[LadenNr].replace("---",TS);  //Ausschaltzeit ersetzen
Ladendaten[LadenNr].replace("999", (String) ZustandLaden[Nr/2]);  //Ausschaltzeit ersetzen

SendToInternet(Ladendaten[LadenNr]);
Ladendaten[LadenNr]= "";


AlterZustandLaden[LadenNr] = ZustandLaden[LadenNr]; 
AnzZeit[Nr] =  Anzugszeit[Nr] ;  //Anzugszeit wiederherstellen

/*
if (VonHand[LadenNr] == true)	//Wenn der Laden von Hand betätigt wurde
{
ZustandLaden[LadenNr] = -1;
VonHand[Nr] == false;
 }
*/
}

//*********************************************************************************************************************
void Ueberwachen (int Nr) {
  
 static int Sec;
 int LadenNr = Nr/2;
 bool Oeffnen = true;

 if ((Nr % 2) == 1) Oeffnen = false; //Oeffnen oder Schliessen?
    int verbleibendeAnzugszeit = Anzugszeit[Nr] * AlterZustandLaden[Nr/2]/100;
    int Restzeit = RelaisEinZeit[Nr] + Anzugszeit[Nr] - millis()/1000;
    int Proz =  100 * Restzeit/ Anzugszeit[Nr];
	
	if (Oeffnen)  
		{
		Proz = AlterZustandLaden[LadenNr] + (100 - Proz);
		if (Proz > 100) Proz = 100;
		}
	else
		{
		Proz = AlterZustandLaden[LadenNr] + Proz-100;
		if (Proz < 0) Proz = 0;
		}


     //if (second() != Sec) 
     //          {
     //          Serial.print("Relais ");
     //          Serial.print(Nr);
     //          Serial.print(" - ");
     //          Serial.print(Restzeit); //in Zentelsekunden
     //          Serial.print("s ");
     //           Serial.print(" Rest ");
     //          Serial.print(verbleibendeAnzugszeit); //in Zentelsekunden
     //          Serial.print("s ");
			  //
			  //  Serial.println();

			  // Sec = second() ;
     //          }

	   ZustandLaden[LadenNr] = Proz;
       if (millis()/1000 > RelaisEinZeit[Nr] + AnzZeit[Nr]) RelaisAus(Nr);
	 /*  if (Nr == Testnummer)
	   {
		   Debugen(12, "verbl.  RelaisEinZeit", RelaisEinZeit[Nr]);
		   Debugen(13, "verbl. Zeit", RelaisEinZeit[Nr] + AnzZeit[Nr]);
	   }

*/
 }

//*********************************************************************************************************************

void AlleRelaisAus () {
for (int i = 0; i < 24; i++)
    {
     RelaisAus (i);
     }
 }

//###############################################################################################
void RelaisSchliessen (String Bef)
{
	int Nr =  Bef.substring(6, 9).toInt(); 
    if (Nr > 24) return; 

	 RelaisEin(Nr,0);
   
}

//###############################################################################################
void RolladenBefehl(String Bef)
{
 String Nrs = Bef.substring(8); 
    int Laenge = Nrs.length(); 
	int Nr = Nrs.toInt();
    
    Serial.print("Rolladenbefehl ");
    Serial.print(Nr);
    Serial.print(" - ");
    Serial.println(Laenge);
	BefehlRolladen(Nr);


}
//###############################################################################################
void SammelBefehlBewegen (String Bef)
{
  
   String Nr =  Bef.substring(6); 
    int Laenge = Nr.length(); 
    int Z = 0;
    
    Serial.print("Sammelbefehl ");
    Serial.print(Nr);
    Serial.print(" - ");
    Serial.println(Laenge);

   for (int i = 0; i < Laenge; i++)
  {
    Serial.print(i);
    Serial.print(" - ");
    Z = Nr.substring(Laenge - i-1,Laenge - i ).toInt();
    Serial.println(Z); 
    if (Z == 1)  RelaisEin(i,0);
    
  }
}
// ###############################################################################################
void AndroidBefehl(unsigned long Bef)
{
	
	long N = 0;
  
	
		for (int i = 0; i < 22; i++)
		{
			N = Bef >> i;
			if (N > 0)
				{ 
				Serial.print(i);
				Serial.print(" - ");
				N = Bef >> i;
				
				/*Serial.print(N);
				Serial.print(" - ");
				Serial.print(RelaisEinZeit[i]);
				Serial.print(" - ");
				Serial.println(millis() / 1000 + 2);*/
				if (i < 8)
					{
						if (N & 1) 
						if (!digitalRead(i + RelStart))
						{ RelaisEin(i, 0);}
						else
						{RelaisAus(i);	}

					}
				else
				{
					if (N & 1) 
						if (digitalRead(i + RelStart))
						{ RelaisEin(i, 0);}
						else
						{RelaisAus(i);	}
				}
			}
		}
		

}
// ###############################################################################################
void Ladentest()
{
	//Relais werden nacheinander eingeschaltet und wieder ausgeschaltet
	


	int Pause = 2000; 
	int AnzRel = 22;
	int LetzRelais = RelStart + AnzRel;

	for (int i = 0; i < AnzRel; i++)
	{
		if (i < 8) aus = LOW; else aus = HIGH;

		digitalWrite(LetzRelais, aus);  //letzes Relais aus
		digitalWrite(RelStart + i, !aus); //akt Relais ein
		delay(Pause);
		LetzRelais = RelStart + i;
		
	}

}