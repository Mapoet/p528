#define DLLEXPORT extern "C" __declspec(dllexport)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define PI                                  3.1415926535897932384
#define a_0__km                             6370.0
#define T_eo__km                            3.25
#define T_ew__km                            1.36
#define N_0                                 301
#define N_9000                              329
#define epsilon                             15.0
#define sigma                               0.005
#define LOS_EPSILON                         0.0001
#define LOS_ITERATION                       25
#define THIRD                               1.0 / 3.0

// Consts
#define CONST_MODE__SEARCH                  0
#define CONST_MODE__DIFFRACTION             1
#define CONST_MODE__SCATTERING              2

#define PROP_MODE__NOT_SET                  0
#define PROP_MODE__LOS                      1
#define PROP_MODE__DIFFRACTION              2
#define PROP_MODE__SCATTERING               3

//
// RETURN CODES
///////////////////////////////////////////////

#define	SUCCESS                             0
#define ERROR_VALIDATION__D_KM              1
#define ERROR_VALIDATION__H_1               2
#define ERROR_VALIDATION__H_2               3
#define ERROR_VALIDATION__TERM_GEO          4
#define ERROR_VALIDATION__F_MHZ_LOW         5
#define ERROR_VALIDATION__F_MHZ_HIGH        6
#define ERROR_VALIDATION__PERCENT_LOW       7
#define ERROR_VALIDATION__PERCENT_HIGH      8
#define ERROR_HEIGHT_AND_DISTANCE			10
#define WARNING__DFRAC_TROPO_REGION         0xFF1
#define WARNING__LOW_FREQUENCY              0xFF2

//
// DATA STRUCTURES
///////////////////////////////////////////////

struct Path
{
    // Distances
    double d_ML__km;            // Maximum line of sight distance
    double d_0__km;             //
    double d_d__km;             // Distance where smooth earth diffraction is 0 dB

    // Earth
    double a_e__km;             // Effective earth radius
};

struct Terminal
{
    // Heights
    double h_r__km;             // Real terminal height
    double h_e__km;             // Effective terminal height
    double h__km;               // Terminal height used in model
    double delta_h__km;         //

    // Distances
    double d_r__km;             // Ray traced horizon distance
    double d__km;               // Horizon distance used in model

    // Angles
    double phi__rad;            // Central angle between the terminal and its smooth earth horizon
    double theta__rad;          // Incident angle of the grazing ray at the terminal
};

struct LineOfSightParams
{
    // Heights
    double z__km[2];

    // Distances
    double d__km;               // Path distance between terminals
    double r_0__km;             // Direct ray length
    double r_12__km;            // Indirect ray length
    double D__km[2];

    // Angles
    double theta_h1;
    double theta_h2;
    double theta[2];

    // Misc
    double a_a__km;             // Adjusted earth radius
    double delta_r;             // Ray length path difference
    double A_LOS__db;           // Loss due to LOS path
};

struct TroposcatterParams
{
    // Distances
    double d_s__km;             // Scattering distance 
    double d_z__km;             // Half the scattering distance

    // Heights
    double h_v__km;             // Height of the common volume cross-over point

    // Angles
    double theta_s;             // Scattering angle
    double theta_A;             // cross-over angle

    // Losses
    double A_s__db;             // Troposcattter Loss
    double A_s_prev__db;        // Troposcatter Loss of Previous Test Point

    // Misc
    double M_s;                 // Troposcatter Line Slope
};

struct Result {
    int propagation_mode;       // Mode of propagation

    double d__km;               // Path distance used in calculations
    double A__db;               // Total loss
    double A_fs__db;            // Free space path loss
};

class data
{
public:
    const static int NUM_OF_PROBS = 17;
    const static int K_ROWS = 17;

    const static double NR_Data[17][18];    // Data table with K-values and corresponding Nakagami-Rice distributions
    const static double P[NUM_OF_PROBS];    // Probabilities for interpolation and data tables
};

//
// FUNCTIONS
///////////////////////////////////////////////

// Private Functions
void GetPathLoss(double psi, Path path, Terminal terminal_1, Terminal terminal_2,
    double f__mhz, double psi_limit, double A_dML__db, double A_d_0__db, LineOfSightParams* params, double *R_Tg);
void RayOptics(Path path, Terminal terminal_1, Terminal terminal_2, double psi, LineOfSightParams *result);
void TerminalGeometry(double N_s, double a_e__km, Terminal *terminal);
void Troposcatter(Path path, Terminal terminal_1, Terminal terminal_2, double d__km, double f__mhz, double N_s, TroposcatterParams *tropo_params);
int TranshorizonSearch(Path* path, Terminal terminal_1, Terminal terminal_2, double f__mhz,
    double N_s, double A_dML__db, double *M_d, double *A_d0, double* d_crx__km, int* MODE);
double LinearInterpolation(double x1, double y1, double x2, double y2, double x);
void AtmosphericAbsorptionParameters(double f__mhz, double *gamma_oo, double *gamma_ow);
double CalculateEffectiveRayLength(double z_1__km, double z_2__km, double a_e__km, double d_arc__km, double beta__rad, double T_e__km);
void ReflectionCoefficients(double psi, double f__mhz, double *R_g, double *phi_g);
void LineOfSight(Path *path, Terminal terminal_1, Terminal terminal_2, LineOfSightParams *los_params, double f__mhz, double A_dML__db,
    double time_percentage, double d__km, Result *result, double *K_LOS);
double SmoothEarthDiffraction(double d_1__km, double d_2__km, double f__mhz, double d_0__km);
double InverseComplementaryCumulativeDistributionFunction(double q);
void LongTermVariability(double h_r1__km, double h_r2__km, double d__km, double f__mhz, double time_percentage, double f_theta_h, double PL, double *Y_e__db, double *A_Y);
double TranshorizonAtmosphericAbsorptionLoss(Terminal terminal_1, Terminal terminal_2, Path path, TroposcatterParams tropo, double f__mhz);
void RayTrace(double N_s, double h_rx__km, double *arc_distance, double *theta_rx);
double FindKForYpiAt99Percent(double Y_pi__db);
double NakagamiRice(double K, double q);
double CombineDistributions(double A_M, double A_i, double B_M, double B_i, double q);
int ValidateInputs(double d__km, double h_1__meter, double h_2__meter, double f__mhz, double time_percentage);

// Public Functions
DLLEXPORT int P528(double d__km, double h_1__meter, double h_2__meter, double f__mhz, double time_percentage, Result *result);
DLLEXPORT int P528_Ex(double d__km, double h_1__meter, double h_2__meter, double f__mhz, double time_percentage, Result* result,
    Terminal* terminal_1, Terminal* terminal_2, TroposcatterParams* tropo, Path* path, LineOfSightParams* los_params);
