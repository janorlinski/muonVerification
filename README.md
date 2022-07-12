# muonVerification
This is a set of ROOT / C++ macros for verification of the Space Radiation Observatory detectors at Faculty of Physics. 
Done as part of the "Advanced Laboratory for Students".

### how to run me?

Simple version:

1. Download and unpack this repository.

2. Run verification3.C, e.g. using `root -l verification3.C` (attention! if you wish to compile the code, you should attach all necessary libraries first).

3. You should see two files generated: verification3.root will contain all histograms directly parsed from the input file, verification3results.root will contain the results of data analysis.

### verification3Parser.C

This macro contains `verification3Parser (TString path)` which loads a path+".txt" file and then parses it. It saves the resulting ROOT histograms to a .root file called 'verification3.root'. Keep in mind that old files may be overwritten.

### verification3.C

`verification3(Bool_t parseFile = true)` analyzes the histograms saved to .root file by the parser or calls the parser to generate a fresh file (this is controlled by the `parseFile` argument - the latter option is default). Currently it generates the total assymetry, polar assymetry, average profile histograms and current profiles for all 24 wires. Individual noise profiles are also saved in corresponding directories. The results are saved to the verification3results.root file. Keep in mind that old files may be overwritten (this includes the results file and the parsing result if the Parser was called).

