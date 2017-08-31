//// INPUT VARIABLES ////
TString material = "ptfe";  //for low-outgassing or background put background, high-outgassing put ptfe
const int new_data = 1;     //set to 0 for old data (< June 2014)
Double_t area = 1;          //set to 1 to get total outgassing rate (cm^2)
const int print = 0;        //set to 1 to save output plot
const int fit = 1;          //set to 1 to fit outgassing to exp
const int plot_number = 0;  //zero for single plot (black), one for first of many to be plotted on top, etc.
const int skip_measurements_number = 1;
const int skip_measurements [skip_measurements_number] = {};
const int print_time_outgassing = 1;    //prints data points [x,y] to stdout
const int draw_rga_scan = 1;

TString img_name = "20140725_ptfe"; //save name for output plots

/// Local Stuff ///
TString file_header = "/Users/josephhowlett/research/outgassing/"; //header for all .dat and prog directories

Double_t hwhm = .3; // can't remember how this gets used
gErrorIgnoreLevel = 2001;
const int max_measurements = 30;
const int maxbins = 1000000;

//// Conductance ////         //from O'Hanlon Vac Tech book
Double_t coeff = 12.23;       //assuming 293 Kelvin, hole diamter = 5mm
Double_t trans_prob = 0.245;  //assuming l/d = 3.56
                              //Surface area in cm^2; length in cm for cables

////  Declarations  /////
long int measurement_time [max_measurements];
Double_t measurement_pressure [max_measurements], fiterr [max_measurements];
int n_measurements, t_0;
Double_t input_mass = 0;
long int time_bound_low, time_bound_high;
Double_t p_bound_low, p_bound_high = 1.5;
const int skip_measurement = 0;

int my_colors [] = {kBlack, kRed, kBlue, 8, kMagenta+2, kOrange+1, kViolet-3, 28, 15};
int my_markers [] = {20, 20, 22, 21, 33, 34, 23, 5, 3, 4, 24, 25, 26, 27};

//// analyze rga dat ///


//// find asymptotes ///
int npts=0;


/// file import stuff ///
Double_t pressure [maxbins], fpressure;
long int time [maxbins], ftime;
ftime = 0;
string params;
ifstream infile1;
