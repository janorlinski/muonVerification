# muonVerification
This is a set of ROOT / C++ macros for verification of the Space Radiation Observatory at Faculty of Physics. 

### verification3Parser.C

This macro contains `verification3Parser (TString path)` which loads a .txt file and then parses it. It saves the resulting ROOT histograms to a .root file called 'verification3.root'. Keep in mind that new files may overwrite old files.

### verification3.C

This macro analyzes the histograms saved to .root file by the parser. Currently it generates the total assymetry and polar assymetry histograms as well as an average profile for the given wire. 

To do list:

 - loop such an analysis over all wires in all chambers
 - generate a histogram that for all wires shows total assymetry score
 - generate a histogram that for all wires shows polar assymetry score
 - generate a PDF booklet that shows the average noise profile in all wires
 - optionally for a given wire generate a booklet with all repetitions, one repetition per page: positive signal, reflected negative signal, in red the difference

