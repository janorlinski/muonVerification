# muonVerification
This is a set of ROOT / C++ macros for verification of the Space Radiation Observatory at Faculty of Physics. 

### verification3Parser.C

This macro contains `verification3Parser (TString path)` which loads a .txt file and then parses it. It saves the resulting ROOT histograms to a .root file called 'verification3.root'. Keep in mind that new files may overwrite old files.

### verification3.C

This macro analyzes the histograms saved to .root file by the parser. Currently it generates the total assymetry, polar assymetry and average profile histograms for all 24 wires. The results are saved to a .root file.

