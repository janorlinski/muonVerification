string verification3Parser (TString testDataFilePath = "m115-test3-1.txt") {

	// These are some setup values
	
	const Bool_t debugTotalSilence 		= false;			// this is connected with AND(NOT()) to all debug booleans => setting to true will silence all debug prompts
	const Bool_t debugPrintInputPath 	= false;			// print the input path for .txt file with data
	const Bool_t debugPrintFileLines	= false;			// this wil print every line from the input file
	const Bool_t debugCountLines		= false;			// this wil print every counter value
	const Bool_t debugPrintPadsReps		= false;			// this wil print read words and values of NPads and NReps
	
	const Int_t timeout			= 3000;
	const Int_t bins			= 500; 						// number of bins in each histogram
	
	// All settings below should get overwritten when reading the input file
	// or at least the important ones... 

	Double_t 	timeBase			= 0;
	Double_t 	scaleCH1			= 0;
	Double_t 	scaleCH2			= 0;
	Double_t 	wireResistance		= 0;
	Double_t 	padResistance		= 0;
	Double_t 	wireTrigger			= 0;
	Double_t 	padTrigger			= 0;
	Int_t    	numberOfPads		= 0;
	Double_t 	setHV				= 0;			
	Int_t 		numberOfReps		= 0;
	
	Int_t		numberOfChamber 	= 0;		
	Int_t		numberOfWire	  	= 0;		
	Int_t		numberOfRep		  	= 0;		
	Int_t		binNumber		  	= 1;
	
	TFile*		fout				= new TFile ("verification3.root", "recreate");	

	// This section of the code will load the .txt file and start parsing the header with settings/info

	if (debugPrintInputPath && !debugTotalSilence) cout << "DEBUG: Path to input text file is: '" << testDataFilePath << "'." << endl;
	
	ifstream inputfile(testDataFilePath);
	string line;
	stringstream linestream;
	
	for (int i=0; i<6; i++) { //this loop ideally should read ALL settings from the header, but oh well it reads only the important ones for now
		
		// reading line, converting to stringstream
		getline(inputfile, line);
		if (debugPrintFileLines && !debugTotalSilence) cout << "DEBUG: Reading line >" << line << endl;
		linestream = stringstream(line);
		
		//iterating over all "words" in current line
		string word;
		vector < string > words;
		while (linestream  >> word) {
			words.push_back(word);
		}
		
		if (i == 4) { //load nPads and nReps - the "important" settings
			
			if (debugPrintPadsReps && !debugTotalSilence) {
				cout << "'" << words[2] <<"', '" << words[8] << "'." << endl;
				cout << "nPads == " << stoi(words[2]) <<", nReps == " << stoi(words[8]) << "." << endl;
			}
			
			numberOfPads = stoi(words[2]);
			numberOfReps = stoi(words[8]);
		}
	}
	
	// initialize histogram arrays outside of any loops to declare them in the global scobe
	
	TH1D* Wires           [3][8][numberOfReps][2]; 	// The first dimension is number of chamber (only for wires), second is number of element (wire or pad)
	TH1D* Pads    [numberOfPads][numberOfReps][2];	// third is number of repetitions and fourth indexes polarity of signal (0 - positive, 1 - negative
	
	for (Int_t a = 0; a < numberOfReps; a++) { 			//loop over 100 reps
		for (Int_t b = 0; b < 2; b++) {		            //loop over 2 polarities
			
			for (Int_t c = 0; c < numberOfPads; c++) {	//loop over 9 pads
				Pads[c][a][b] = new TH1D (Form("Pads_nr%i_rep%i_polar%i", c+1, a+1, b), Form("Pads_nr%i_rep%i_polar%i", c+1, a+1, b), bins, 0.5, bins+0.5);
			}
			
			for (Int_t c = 0; c < 3; c++) {				//loop over 3 chambers
				for (Int_t d = 0; d < 8; d++) {			//loop over 8 wires
					Wires[c][d][a][b] = new TH1D (Form("Wires_chamber%i_nr%i_rep%i_polar%i", c+1, d+1, a+1, b), Form("Wires_chamber%i_nr%i_rep%i_polar%i", c+1, d+1, a+1, b), bins, 0.5, bins+0.5);
				}
			}					
		}
	}
	
	Int_t counter = 0;
	
	while (true) { //and back to loops - now reading the main body of the file
		
		// reading line, converting to stringstream
		getline(inputfile, line);
		if (debugPrintFileLines && !debugTotalSilence) cout << "DEBUG: Reading line >" << line << endl;
		else linestream = stringstream(line);
		counter++;
		if (debugCountLines && !debugTotalSilence) cout << counter << endl;
		
		//iterating over all "words" in current line
		string word;
		vector < string > words;
		while (linestream  >> word) {
			words.push_back(word);
		}
		
		if (words[0] == "Komora/pad") {
			
			cout << line << endl;        					//get the line with current number of chamber, wire and repetition
			
			numberOfChamber 	= stoi(words[1]) - 1;		//-1 to make it OK with C++ array numbering convention
			numberOfWire	  	= stoi(words[3]) - 1;		
			numberOfRep		  	= stoi(words[5]) - 1;		
			binNumber		  	= 1;		
			
			getline(inputfile, line);    					//and omit one line
			
		}
		
		else if (words.size() == 2) {
			
			if (numberOfChamber == 3) { 					//this is a pad
				
				Pads[numberOfWire][numberOfRep][0]->SetBinContent(binNumber, stoi(words[0]));
				Pads[numberOfWire][numberOfRep][1]->SetBinContent(binNumber, stoi(words[1]));
			}
			
			else { 											//this is a wire
	
				Wires[numberOfChamber][numberOfWire][numberOfRep][0]->SetBinContent(binNumber, stoi(words[0]));
				Wires[numberOfChamber][numberOfWire][numberOfRep][1]->SetBinContent(binNumber, stoi(words[1]));
				
			}
			
			binNumber++;
			
			if (binNumber > bins) { //if we just finished filling a pair of histograms
				
				if (numberOfChamber == 3 && numberOfWire == numberOfPads - 1 && numberOfRep==numberOfReps - 1) { //check whether the file is over
					cout << "File read correctly -- terminating the program." << endl;
					break;
				}
				
				//save the given pair of histograms
				
				if (numberOfChamber == 3) { 					//this is a pad
				
				Pads[numberOfWire][numberOfRep][0]->Write();
				Pads[numberOfWire][numberOfRep][1]->Write();
				}
			
				else { 											//this is a wire
	
					Wires[numberOfChamber][numberOfWire][numberOfRep][0]->Write();
					Wires[numberOfChamber][numberOfWire][numberOfRep][1]->Write();
				
				}
				getline(inputfile, line);    				//omit one line after reaching the last value
			}		
		}
	
	}
	
	inputfile.close();
	fout->Close();
	return to_string(numberOfPads) + " " + to_string(numberOfReps);
	

} 
