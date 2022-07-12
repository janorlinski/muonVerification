# muonVerification
This is a set of ROOT / C++ macros for verification of the Space Radiation Observatory detectors at Faculty of Physics. 
Done as part of the "Advanced Laboratory for Students".

### verification3Parser.C

This macro contains `verification3Parser (TString path)` which loads a .txt file and then parses it. It saves the resulting ROOT histograms to a .root file called 'verification3.root'. Keep in mind that old files may be overwritten.

### verification3.C

This macro analyzes the histograms saved to .root file by the parser or calls the parser to generate a fresh file. Currently it generates the total assymetry, polar assymetry, average profile histograms and current profiles for all 24 wires. Individual noise profiles are also saved in corresponding directories. The results are saved to the verification3results.root file.

