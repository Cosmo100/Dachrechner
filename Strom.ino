


//###############################################################################################
//################# Sollwerte aus Arduino-EEProm lesen und zum PC senden  #######################
//###############################################################################################

	
	unsigned long pot(int Ex)
	{
		//Potenzieren mit Basis 2
		Ex = Ex * 8;
		int i = 0;
		unsigned long Erg = 1;
		 while (i < Ex)
			{
			Erg = Erg * 2;
			i++;
			}
		return Erg;
	}

	