/*#ifdef _DEBUG
#define WITH_DEBUG
#endif
*/
#define	ESP_NSP_PRECISION	0.0001
// already defined in WINBASE.H
// do not need to use this in either VC60 or VC70
//#define INFINITE            0xFFFFFFFF  // Infinite timeout
/*#ifdef VC70
#define INFINITE			10000000
#endif
*/
#define PI					3.14159265358979323846			

struct TrajectoryTableEntry
{
	double Time;		// Time when this period ends 
	double Jerk;		// jerk
	double Accel;		// acceleration
	double Velocity;	// velocity
	double CarPosition;	// Position
	double TrajectoryPos;	// the position of the trajectory other than car position
};

struct DelayTableEntry
{
	double tESP;		// ESP time
	double tNSP;		// NSP time
	double tDelay;		// delay should be tESP - tNSP
	double pESP;		// ESP position at time tESP
	double pNSP;		// NSP position at time tNSP
	int iESP;			// interval index of tESP
	int iNSP;			// interval index of tNSP
};


double ESPNSPDelay(double leadDist,			// lead car travel distance
				   double trailingDist,		// trailing car travel distance
				   double jerk,				// max jerk for both cars
				   double accel,			// max acceleration for both cars
				   double velocity,			// max velocity for both cars
				   double initSeparation,	// distance between the origin of the two cars
				   double MSD,				// minimum separation distance
				   double accelForced		// forced acceleration using break
				   );

void CalculateESPTable(double dist,				// travel distance
					   double jerk,				// max jerk for both cars
					   double accel,			// max acceleration for both cars
					   double velocity,			// max velocity for both cars
					   double accelForced,		// forced acceleration when brake is dropped
					   TrajectoryTableEntry *table	// table to hold the trajectory entries
					   );

void CalculateNSPTable(double dist,				// travel distance
					   double jerk,				// max jerk for both cars
					   double accel,			// max acceleration for both cars
					   double velocity,			// max velocity for both cars
					   double accelForced,		// forced acceleration when brake is dropped
					   TrajectoryTableEntry *table	// table to hold the trajectory entries
					   );

//****************************************************************************************************
//* Quadratic Equation Solver. Solve the equation ax^2 + bx + c = 0  
//* return the number of real roots. Roots are stored in array x If the roots are imaginary number,
//* returns 0. 
//****************************************************************************************************
int QuadraticEquationSolver(double a,
							double b,
							double c,
							double x[]);

//*******************************************************************************************************
//* Cubic Equation Solver. Solve the equation ax^3 + bx^2 + cx+ d  = 0  
//* return the number of real roots. Roots are stored in x array. x must have at least three entries
//* The funcition returns the number of real roots. 
//*******************************************************************************************************
int CubicEquationSolver(double a,
						double b,
						double c,
						double d,
						double x[]);

//*******************************************************************************************************
//* Cubic Equation Solver. Solve the equation ax^4 + bx^3 + cx^2+ dx + e  = 0  
//* return the number of real roots. Roots are stored in x array. x must have least 4 entries
//*******************************************************************************************************
int QuarticEquationSolver(double a,	
						  double b,
						  double c,
						  double d,
						  double e,
						  double x[]);
