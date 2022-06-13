#include "verification3Parser.C"

Double_t smartIntegral (TH1* hist, TString option = "total") {
	
	Double_t sum;
	if (option=="total" || option=="polar") {
		
		TAxis* 	xAxis = hist->GetXaxis();
		Int_t	nBins = xAxis->GetNbins();
		
		for (Int_t i=1; i<=nBins; i++) {
			
			Double_t binValue = 1.0 * hist->GetBinContent(i) / xAxis->GetBinWidth(i);
			if (option == "total") binValue = TMath::Abs(binValue);
			sum += binValue;
			
		}
		
	}
	return sum;
} 

void verification3 (Bool_t parseFile = false) {

	string setString;
	if (!parseFile) setString = "9 100"; // settings need to be manually set if they are not auto accessed by parsing the file

	// parse .txt into .root if needed, open .root file 
	if (parseFile) setString = verification3Parser("m115-test3-1.txt");
	TFile*	fin			= new TFile ("verification3.root", "read");
	TFile*	fout		= new TFile ("verification3results.root", "recreate");
	
	// setup
	TH1D* 	ProbeAvg;
	Int_t	selectChamber 	= 0;
	Int_t	selectWire		= 0;
	Int_t	numberOfPads, numberOfReps;
	TH1D* 	PolarAssymetry = new TH1D ("PolarAssymetry", Form("Relative polar assymetry for wire %i in chamber %i; rel. assymetry (%%)", selectWire+1, selectChamber+1), 200, -200, 200);
	TH1D* 	TotalAssymetry = new TH1D ("TotalAssymetry", Form("Relative total assymetry for wire %i in chamber %i; rel. assymetry (%%)", selectWire+1, selectChamber+1), 200, 0, 300);
	
	
	// this section of the code will reconstruct the key settings read from the .txt file
	// it comes as a string with structure "[numberOfPads] [numberOfReps]"
	
	stringstream setStringStream(setString);
	string pads, reps;
	setStringStream >> pads >> reps;
	numberOfPads = stoi(pads);
	numberOfReps = stoi(reps);
	
	// initialize histogram arrays, get them from the TFile
	
	TH1D* Wires           [3][8][numberOfReps][2]; 	// The first dimension is number of chamber (only for wires), second is number of element (wire or pad)
	TH1D* Pads    [numberOfPads][numberOfReps][2];	// third is number of repetitions and fourth indexes polarity of signal (0 - positive, 1 - negative
	
	for (Int_t a = 0; a < numberOfReps; a++) { 			//loop over 100 reps
		for (Int_t b = 0; b < 2; b++) {		            //loop over 2 polarities
			
			for (Int_t c = 0; c < numberOfPads; c++) {	//loop over 9 pads
				Pads[c][a][b] = (TH1D*) fin->Get(Form("Pads_nr%i_rep%i_polar%i", c+1, a+1, b));
			}
			
			for (Int_t c = 0; c < 3; c++) {				//loop over 3 chambers
				for (Int_t d = 0; d < 8; d++) {			//loop over 8 wires
					Wires[c][d][a][b] = (TH1D*) fin->Get(Form("Wires_chamber%i_nr%i_rep%i_polar%i", c+1, d+1, a+1, b));
				}
			}					
		}
	}
	
	// do the actual verification with the histograms
	// first approach: add "+" signal to "-" signal, average that result over all reps
	// how to evaluate this assymetry?
	// calculate the integral of difference (sensitive to sign) and integral of absolute difference (indifferent to sign). 
	// the first integral will detect systematic preference of one polarity but will be blind to any assymetries that cancel out on average
	// or cancel out periodically - for example even an obvious sine wave would be overlooked by this measure
	// the second approach will measure the overall noise, even if it cancels out with regard to polarity
	// we expect the first value to be very close to zero and the second value to be below some percentage (10% ?).
	
	if (selectChamber == 0 || selectChamber == 1 || selectChamber == 2) {
	
		ProbeAvg 		= (TH1D*) Wires[selectChamber][selectWire][0][0]->Clone("ProbeAvg");
		ProbeAvg->Reset();
		ProbeAvg->SetTitle(Form("Average profile of positive + negative for wire %i in chamber %i; channel number; I (uA)", selectWire+1, selectChamber+1));
		
		for (Int_t i=0; i<numberOfReps; i++) { // loop over all reps, for wires
		
			Double_t averageIntegral = 0.5 * (Wires[selectChamber][selectWire][i][0]->Integral("width") - Wires[selectChamber][selectWire][i][1]->Integral("width"));
		
			TH1D* ProbeTemp = (TH1D*) Wires[selectChamber][selectWire][i][0]->Clone("ProbeTemp");
			ProbeTemp			->Add(Wires[selectChamber][selectWire][i][1]);
			
			TotalAssymetry->Fill(100 * smartIntegral(ProbeTemp, "total") / averageIntegral);
			PolarAssymetry->Fill(100 * smartIntegral(ProbeTemp, "polar") / averageIntegral);
			ProbeAvg			->Add(ProbeTemp, 1./numberOfReps);
			
			cout << "+ integral: " << Wires[selectChamber][selectWire][i][0]->Integral("width") << ", - integral: " << Wires[selectChamber][selectWire][i][1]->Integral("width") << ". avgInt: " << averageIntegral << endl;
			cout << "polar integral: " << smartIntegral(ProbeTemp, "polar") << ", total integral: " << smartIntegral(ProbeTemp, "total") << endl;
			
		}
		
		ProbeAvg->Write();
		TotalAssymetry->Write();
		PolarAssymetry->Write();
		
	}
	
	fout->Close();
}
