// 2DFileIO.cpp: implementation of the C2DFileIO class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "2dsim.h"
#include "2DFileIO.h"
#include "all.h"

// #define VC70   // Only used when using C++.net version. Comment this line in earlier versins CSW 3/4/05
#ifndef VC70
  #include "istream.h"
  #include "fstream.h"
  #include "ostream.h"
  #include "iomanip.h"
#else
  #include <istream>
  #include <fstream>
  #include <ostream>
  #include <iomanip>
  using namespace std;     // CSW:  added for the new version 
#endif



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define USERFLOOR(x) (x+param->f_offset)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2DFileIO::C2DFileIO()
{

}

C2DFileIO::~C2DFileIO()
{

}
int DUTY_KG[5] = {900,1000,1150,1350,1600};
/*
  Output :
	<return value>
	-1	: Only traffic profile data
	0	: No error
	1	: File open error
	2	: File don't have specified format
	3	: File corruption	
*/
int C2DFileIO::Load2DPFile(CString file_name,CString *file_memo,BUILDING_STRUCTURE *building,PARAMETER *param,
						   PASSENGER pax[MAXPAX],MOVE_PARAMETER move[MAXMOVE],int *paxCount,int *moveCount)
{
	ifstream fin;
	char charTemp[201];
	char trafficId;
	double doubleTemp;
	int intTemp;
	int floorNo;
	unsigned int carNo;
	unsigned int passengerCount,movCount;
	int endflag=0;
	int returnVal;
	unsigned int strPos;
	unsigned int defNoCars;

	passengerCount=0;
	movCount=0;
	defNoCars=MAXCAR; //shin050810
	building->nCars=defNoCars;

#ifndef VC70
	fin.open(file_name,ios::in|ios::nocreate);   // Old description in V6.0
#else
	fin.open(file_name,ios::in);                 // CSW: 2/7/2005 revised in new verison
#endif
		
	if (fin.good() !=0)
	{
		// Traffic profile reading
		while(!fin.eof() && endflag==0)
		{
			fin >> trafficId;
			switch (trafficId)
			{
			case 'I':
				for (carNo=0;(signed)carNo<defNoCars;carNo++)
				{
					fin >> intTemp;
					if (fin.good()==0)
					{
						AfxMessageBox("No of car and No of Initial Position are not matched. Please check traffic profile contents starting with G");
						break;
					}
					param->initial_position[carNo]=intTemp-param->f_offset;
				}
				fin.getline(charTemp,200);
				if (fin.good()==0)
				{
					fin.clear();
				}
				break;
			case 'P':
				fin >> pax[passengerCount].t_arrival;
				fin >> pax[passengerCount].kindFromTo; //type
				fin >> intTemp;
				pax[passengerCount].f_origin=intTemp-param->f_offset;
				fin >> intTemp;
				pax[passengerCount].f_dest=intTemp-param->f_offset;
				fin >> doubleTemp;
				pax[passengerCount].weight=(unsigned int) doubleTemp;
				// optional
				fin >> intTemp;
				returnVal=fin.good();
				if (returnVal!=0)
				{  // assignment specified
					pax[passengerCount].assignment=intTemp;
					if (intTemp >= building->nCars)
					{  // error
						pax[passengerCount].assignment=-1;
					}
					fin.getline(charTemp,200);
				}
				else
				{
					pax[passengerCount].assignment=-1;
					if (fin.rdstate()==ios::badbit)
					{  // serious IO error
						fin.close();
						return 3;
					}
					fin.clear();
				}
				
				passengerCount++;
				break;
			case 'M':
				fin >> move[movCount].car;
				fin >> intTemp;
				move[movCount].destination=intTemp-param->f_offset;
				fin >> move[movCount].t_initiation;
				fin >> move[movCount].t_release;

				move[movCount].min_dwell = 0.0;
				move[movCount].initiation_basis = 0;
				move[movCount].initiation_threshold = 0.0;
				move[movCount].initiation_relativity = 0;
				move[movCount].initiation_direction = 0;
				move[movCount].initiation_delay = 0.0;
				move[movCount].release_basis = 0;
				move[movCount].release_threshold = 0.0;
				move[movCount].release_relativity = 0;
				move[movCount].release_direction = 0;
				move[movCount].release_delay = 0.0;

				// optional shs
				fin >> doubleTemp;
				returnVal=fin.good();
				if (returnVal!=0)
				{  // min dwell specified
					move[movCount].min_dwell=doubleTemp;
					fin.getline(charTemp,200);
				}
				else
				{	// min dwell not specified
					move[movCount].min_dwell=0.0;
					if (fin.rdstate()==ios::badbit)
					{  // serious IO error
						fin.close();
						return 3;
					}
					fin.clear();
				}
				move[movCount].initiation_basis=0; // time base
				move[movCount].release_basis=0;    // time base
				movCount++;
				break;
			case 'C': 
				fin >> move[movCount].car >> move[movCount].destination
					>> move[movCount].min_dwell >> move[movCount].t_initiation
					>> move[movCount].initiation_basis >> move[movCount].initiation_threshold
					>> move[movCount].initiation_relativity >> move[movCount].initiation_direction
					>> move[movCount].initiation_delay;
				//fin.getline(charTemp,200);
				fin >> move[movCount].t_release
					>> move[movCount].release_basis >> move[movCount].release_threshold
					>> move[movCount].release_relativity >> move[movCount].release_direction
					>> move[movCount].release_delay;
				move[movCount].destination-=(param->f_offset);
				fin.getline(charTemp,200);
				movCount++;
				break;
			case 'Z':
				endflag=1;
				break;
			case 'B': 
				fin >> intTemp;
				building->nUserFloors=intTemp+1;
				//Already set in the function InitBuilding() or UserConfiguration
				//building->pit = 1;       // AH: assume for now
				//building->overhead = 1;
				building->nTotalFloors=building->nUserFloors+building->pit+building->overhead;
				fin >> intTemp;
				building->lobby1Floor=intTemp;
				fin.getline(charTemp,200);
				break;
			case 'G':
				fin >> intTemp;
				building->nCars=intTemp+1;
				defNoCars = intTemp+1; //shin050810
				fin >> param->dutyID; // dutyID

				if (param->dutyID>=0 && param->dutyID <5)
				{
					param->duty=DUTY_KG[param->dutyID];
					param->capacity=param->duty/65;
				}
				else if (param->dutyID==15) // custom duty
				{
					param->duty=1600; // temporary
					param->capacity=17;
				}

				fin.getline(charTemp,200);
				break;
			case 'T': // ignore it
				fin.getline(charTemp,200);
				break;
			default:
				break;
			}
		}
		
		*paxCount=passengerCount;
		*moveCount=movCount;
		do
		{
			fin.getline(charTemp,200);
			if (fin.eof())
			{	// only traffic profile data
				fin.close();
				return -1; // traffic profile data
			}
		} while(strcmp(charTemp,"2D")!=0);
		file_memo->Empty();
		
		// Reading file description
		while(fin.good())
		{
			fin.getline(charTemp,200);
			if (charTemp[0]==NULL)
				break;
			*file_memo += charTemp;
			*file_memo +="\n";
		}
		while(fin.good())
		{
			fin.getline(charTemp,200); // Read // Elevator Parameters
			if (charTemp[0]='/')
				break;
		}
		fin.getline(charTemp,200,'@'); // read Building name, delimiter is @
		if (!fin.good())
		{
			fin.close();
			return 3; // corrupted file
		}
		strncpy(building->mBuildingName,charTemp,100);
		building->mBuildingName[99]=0; // null appending
		fin.getline(charTemp,200); // end of line
		fin >> building->nHoistways;
		fin.getline(charTemp,200);
		fin.getline(charTemp,200); // skip comments
		fin >> building->nCars;		// number of cars
		fin.getline(charTemp,200); // skip comments
		fin >> building->pit;
		fin.getline(charTemp,200); // skip comments
		fin >> building->overhead;
		fin.getline(charTemp,200);
		fin >> param->AdjacentLimit; // minimum required floor difference
		fin.getline(charTemp,200);
		fin.getline(charTemp,200); // skip comments
		fin >> param->dutyID;  // duty
		if (param->dutyID==15) // custom duty
		{
			fin >> param->duty;
			param->capacity=param->duty/65; // translate to person capacity
		}
		else if (param->dutyID <5)
		{
			param->duty=DUTY_KG[param->dutyID];
			param->capacity=param->duty/65; // translate to person capacity
		}
		else
		{
			param->duty=0;
			param->capacity=0;
		}
		fin.getline(charTemp,200);  // skip comments
		fin >> param->MaxVelocity; // velocity
		fin.getline(charTemp,200);
		fin >> param->MaxAcceleration; // accel
		fin.getline(charTemp,200);
		fin >> param->MaxJerk; // jerk
		fin.getline(charTemp,200);
		fin >> building->nDoors; // door present - ignored
		fin.getline(charTemp,200);
		fin >> building->doorType; // door type-not used
		fin >> building->doorWidth; // door width - not used
		fin.getline(charTemp,200);
		fin >> param->door_open; // door opening time
		fin.getline(charTemp,200);
		fin >> param->door_close; // door closing time
		fin.getline(charTemp,200);
		fin >> param->t_car_min_dwell; // door dwell minimum car time
		fin.getline(charTemp,200);
		fin >> param->t_hall_min_dwell; // door dwell minimum hall time
		fin.getline(charTemp,200);
		fin >> param->t_hall_max_dwell; // door dwell maximum hall time
		fin.getline(charTemp,200);
		fin >> param->t_lobby_dwell; // door dwell lobby time
		fin.getline(charTemp,200);
		fin >> param->StartDelay; // start delay
		fin.getline(charTemp,200);
		fin >> param->StopDelay; // stop delay
		fin.getline(charTemp,200);
		fin >> param->bReopenEnabled; // reopen enable flag
		fin.getline(charTemp,200);
		fin >> param->additionalStopTime;
		fin.getline(charTemp,200);
		fin >> param->boardingRatio;
		fin.getline(charTemp,200);
		fin >> param->boardingDeboardingTime;
		fin.getline(charTemp,200);
		fin >> param->bMotorGen;
		fin.getline(charTemp,200);

		while(fin.good()) // skip to comment
		{
			fin.getline(charTemp,200);
			if (charTemp[0]=='/')
				break;
		}
		
		fin >> building->nUserFloors;
		building->nTotalFloors=building->nUserFloors+building->pit+building->overhead;  // AH

		fin.getline(charTemp,200);
		fin >> building->lobby1Floor;
		//building->lobby1Floor -= param->f_offset;
		fin.getline(charTemp,200);
		fin >> building->lobby2Floor;
		//building->lobby2Floor -= param->f_offset;
		fin.getline(charTemp,200);
		fin >> building->rest1Floor;
		//building->rest1Floor -= param->f_offset;
		fin.getline(charTemp,200);
		fin >> building->rest2Floor;
		//building->rest2Floor -= param->f_offset;
		fin.getline(charTemp,200);
		fin >> building->expressZoneTop;
		//building->expressZoneTop -= param->f_offset;
		fin.getline(charTemp,200);
		fin >> building->expressZoneBottom;
		//building->expressZoneBottom -= param->f_offset;
		fin.getline(charTemp,200);
		fin >> param->transferTime;
		fin.getline(charTemp,200);
		fin >> param->transferFloorTime;
		fin.getline(charTemp,200);

		while(fin.good()) // skip to comment
		{
			fin.getline(charTemp,200);
			if (charTemp[0]=='/')
				break;
		}
		
		for (floorNo=0;floorNo<building->nTotalFloors;floorNo++)
		{
			fin >> building->interfloorHeight[floorNo];
			fin >> charTemp; // '//'
			fin >> charTemp; // 'floor'
			fin >> charTemp; // 'height'
			fin >> charTemp; // 'of'
			fin >> building->floorLabel[floorNo];
		}

		building->floorPosition[0]=0;
		for (floorNo=1;floorNo<=building->nTotalFloors;floorNo++)
		{
			building->floorPosition[floorNo]=
				building->floorPosition[floorNo-1]+building->interfloorHeight[floorNo-1];
		}
		
		while(fin.good()) // skip to comment
		{
			fin.getline(charTemp,200);
			if (charTemp[0]=='/')
				break;
		}

		for (floorNo=1;floorNo< building->nTotalFloors-1;floorNo++)
		{
			fin >> building->floorPopulation[floorNo];
			fin.getline(charTemp,200);
		}

		while(fin.good()) // skip to comment
		{
			fin.getline(charTemp,200);
			if (charTemp[0]=='/')
				break;
		}
		
		for (floorNo=1;floorNo<building->nTotalFloors-1;floorNo++) // shs temp
		{
			fin >> intTemp; // floor
			for (carNo=0; carNo< (unsigned)building->nCars;carNo++)
			{
				fin >> setiosflags(ios::hex)>> intTemp;
				param->AllowMask[carNo][floorNo]=intTemp;
			}
			fin.getline(charTemp,200);
		}		
		
		while(fin.good()) // skip to comment
		{
			fin.getline(charTemp,200);
			if (charTemp[0]=='/')
				break;
		}
		
		for (floorNo=1;floorNo < building->nTotalFloors-1;floorNo++) // shs temp
		{
			fin >> setiosflags(ios::dec) >> param->DestinationEntryInstalled[floorNo];
			fin.getline(charTemp,200);
		}

		while(fin.good()) // skip to comment
		{
			fin.getline(charTemp,200);
			if (charTemp[0]=='/')
				break;
		}

		fin >> param->simulationType;
		fin.getline(charTemp,200);
		fin >> param->buildingPopulation;
		fin.getline(charTemp,200);

		// DLL information loading
		while(fin.good()) // skip to comment
		{
			fin.getline(charTemp,200);
			if (charTemp[0]=='/')
			{
				fin >> param->bAutomaticDispatching;
				fin.getline(charTemp,80); // skip comment
				break;
			}
			if (charTemp[0]=='Z')
			{
				param->bAutomaticDispatching=FALSE;
				strcpy(param->dispatcherDLLName,"Manual");
				strcpy(param->dispatchingAlgorithmName,"Human Intelligence");

				fin.close();
				return 0;
			}
		}
		
		if (param->bAutomaticDispatching==TRUE)
		{
			fin.getline(charTemp,150,'@'); // dll file name
			strncpy(param->dispatcherDLLName,charTemp,80);
			fin >> charTemp; //
			fin.getline(charTemp,200,'@');
			// trim left space
			for (strPos=0;strPos<strlen(charTemp);strPos++)
			{
				if (charTemp[strPos]!=' ')
					break;
			}
			strncpy(param->dispatcherDLLPath,&(charTemp[strPos]),150); // only path
			strcat(param->dispatcherDLLPath,"\\");
			strcat(param->dispatcherDLLPath,param->dispatcherDLLName);
			fin >> param->algorithmID;
			fin >> charTemp; //skip //
			fin.getline(charTemp,80,'@');
			for (strPos=0;strPos<strlen(charTemp);strPos++)
			{
				if (charTemp[strPos]!=' ')
					break;
			}
			strncpy(param->dispatchingAlgorithmName,&(charTemp[strPos]),80);
		}
		else
		{
			strcpy(param->dispatcherDLLName,"Manual");
			strcpy(param->dispatchingAlgorithmName,"Human Intelligence");
		}

		// Added Violation Display data
		fin.getline(charTemp,200);
		if (charTemp[0]=='Z')
		{
			fin.close();
			return 0;
		}
			
		while(fin.good()) // skip to comment
		{
			fin.getline(charTemp,200);
			if (charTemp[0]=='/')
				break;
		}

		fin >> param->m_violationHCD;

		fin.getline(charTemp,200);	
		if (charTemp[0]=='Z')
		{   // end of file
			fin.close();
			return 0;
		}

		while(fin.good()) // skip to comment
		{
			fin.getline(charTemp,200);
			if (charTemp[0]=='/')
				break;
		}
		fin >> param->MinSeparationDist;

		fin.getline(charTemp,200);
		if (charTemp[0]=='Z')
		{
			fin.close();
			return 0;
		}
				
	}
	fin.close();
	return 0;
}


 /*	Output :
		<return value>
		0	: No error
		1	: File creation error */
int C2DFileIO::Save2DPFile(CString file_name,CString *file_memo,BUILDING_STRUCTURE *building,PARAMETER *param,
						   PASSENGER pax[MAXPAX],MOVE_PARAMETER move[MAXMOVE],int paxCount,int moveCount)
{
	ofstream fout;
	int floorNo;
	int carNo;
	int passNo,moveNo;

	int defNoCars;
	float defWeight;
	int retval;
	CString onlyPath;
	int strPos;

	defNoCars=building->nCars; //shin050810
	retval=1;
	fout.open(file_name,ios::out|ios::trunc);

	if (fout.good() !=0)
	{
		retval=0;
		// Traffic profile writing
		fout << "T" << setw(6) << "0.0" << endl;
		fout << "T" << setw(6) << "0.0" << endl;
		fout << "B" << setw(5) << building->nUserFloors-1 << setw(5) << 
			building->lobby1Floor << setw(5) << "0" << endl;
		fout << "G" << setw(5) << building->nCars-1;
		fout << setw(5) << param->dutyID << endl;

		fout << "I" ; // init position writing
		for (carNo=0;carNo<defNoCars;carNo++)
		{
			fout << setw(5) << USERFLOOR(param->initial_position[carNo]);
		}
		fout << endl;

		fout << "T" << setw(6) << "0.0" << endl;
		fout << "T" << setw(6) << "0.0" << endl;
		defWeight=65.00;
		for (passNo=0;passNo<paxCount;passNo++)
		{
			fout << "P" << setiosflags(ios::fixed) << setprecision(2)
				<< setw(10) << pax[passNo].t_arrival << resetiosflags(ios::fixed)
				<< setw(5) << pax[passNo].kindFromTo << setw(5) << USERFLOOR(pax[passNo].f_origin)
				<< setw(5) << USERFLOOR(pax[passNo].f_dest) << setw(9) << setiosflags(ios::fixed) 
				<< setprecision(2) << defWeight << resetiosflags(ios::fixed)
				<< setw(5) << pax[passNo].assignment << endl;
		}
		for (moveNo=0;moveNo<moveCount;moveNo++)
		{
				/* conditional move replace old move */
				fout << "C" << setw(5) << move[moveNo].car 
					<< setw(5) << USERFLOOR(move[moveNo].destination)
					<< setw(9) << setiosflags(ios::fixed)<< setprecision(2)<< move[moveNo].min_dwell
					<< setw(9) << setiosflags(ios::fixed)<< setprecision(2)<< move[moveNo].t_initiation
					<< setw(9) << move[moveNo].initiation_basis 
					<< setw(9) << setiosflags(ios::fixed)<< setprecision(2)<< move[moveNo].initiation_threshold
					<< setw(5) << move[moveNo].initiation_relativity 
					<< setw(5) << move[moveNo].initiation_direction
					<< setw(9) << setiosflags(ios::fixed)<< setprecision(2)<< move[moveNo].initiation_delay
				//	<< endl;
					<< "\t";
				fout << setw(9) << setiosflags(ios::fixed)<< setprecision(2)<< move[moveNo].t_release
					<< setw(5) << move[moveNo].release_basis 
					<< setw(9) << setiosflags(ios::fixed)<< setprecision(2)<< move[moveNo].release_threshold
					<< setw(5) << move[moveNo].release_relativity 
					<< setw(5) << move[moveNo].release_direction
					<< setw(9) << setiosflags(ios::fixed)<< setprecision(2)<< move[moveNo].release_delay
					<< endl;


			/* fout << "M" << setw(5) << move[moveNo].car
				<< setw(5) << USERFLOOR(move[moveNo].destination)
				<< setw(9) << setiosflags(ios::fixed)<< setprecision(2) << move[moveNo].t_initiation 
				<< setw(9) << setiosflags(ios::fixed)<< setprecision(2) << move[moveNo].t_release
				<< setw(9) << setiosflags(ios::fixed)<< setprecision(2) << move[moveNo].min_dwell
				<< resetiosflags(ios::fixed)
				<< endl;
				*/
		}


		fout << "Z" << endl;
		
		fout << setiosflags(ios::left);
		fout << "2D" << endl;

/*		if (file_memo->GetLength()==0)
			fout << endl;
		else
			fout << *file_memo << endl;
			*/
		fout << endl;
		fout << "// ELEVATOR PARAMETERS" << endl;
		fout <<  building->mBuildingName << "@" << "\t// Building Name" << endl;

		fout << setw(20) << building->nHoistways << "// Number of Hoistways" << endl;
		fout << "// for each hoistway" << endl;
		fout << setw(20) << building->nCars << "// Number of Cars" << endl;
		fout << setw(20) << building->pit << "// pit existence" << endl;
		fout << setw(20) << building->overhead << "// overhead existence" << endl;
		fout << setw(20) << param->AdjacentLimit 
				<< "// Minimum Adj Floor for all hoistways" << endl; 
		fout << "//	for each car" << endl;
		
		if (param->dutyID==15) // custom duty
		{
			fout << setw(5) << param->dutyID << setw(15) << param->duty;
		}
		else
		{
			fout << setw(20) << param->dutyID;
		}

		fout << "// Duty 0:900KG,1:1000KG,2:1150,..15:custom" << endl;
		
		fout << setiosflags(ios::fixed) << setprecision(2)
			<< setw(20) << param->MaxVelocity << "// Velocities for all cars[m/s]" 
			<< resetiosflags(ios::fixed) << endl;
		fout << setiosflags(ios::fixed) << setprecision(2)
			<< setw(20) << param->MaxAcceleration << "// Accels for all cars[m/ss]" 
			<< resetiosflags(ios::fixed) << endl;
		fout << setiosflags(ios::fixed) << setprecision(2)
			<< setw(20) << param->MaxJerk << "// Jerks for all cars[m/sss]" 
			<< resetiosflags(ios::fixed) << endl;

		fout <<setw(10) << building->nDoors << "// Cars with Single Door(1) or Double Door(2)" <<endl;
		fout << setw(5) << building->doorType << setw(10) << building->doorWidth  
			<< "// DoorType ID & Opening width[mm]" << endl;

		fout << setiosflags(ios::fixed) << setprecision(2) << setw(20) << param->door_open
			<< resetiosflags(ios::fixed) << "// Door Opening Time [s]" << endl;
		fout << setiosflags(ios::fixed) << setprecision(2) << setw(20) << param->door_close
			<< resetiosflags(ios::fixed) << "// Door Closing Time [s]" << endl;

		fout << setiosflags(ios::fixed) << setprecision(2) << setw(20) << param->t_car_min_dwell
			<< resetiosflags(ios::fixed) << "// Door Dwell MIN_CAR_NT [s]" << endl;
		fout << setiosflags(ios::fixed) << setprecision(2) << setw(20) << param->t_hall_min_dwell
			<< resetiosflags(ios::fixed) << "// Door Dwell MIN_HALL_NT [s]" << endl;
		fout << setiosflags(ios::fixed) << setprecision(2) << setw(20) << param->t_hall_max_dwell
			<< resetiosflags(ios::fixed) << "// Door Dwell MAX_HALL_NT [s]" << endl;
		fout << setiosflags(ios::fixed) << setprecision(2) << setw(20) << param->t_lobby_dwell
			<< resetiosflags(ios::fixed) << "// Door Dwell LOBBY_NT [s]" << endl;

		fout << setiosflags(ios::fixed) << setprecision(3) << setw(20) << param->StartDelay
			<< resetiosflags(ios::fixed) << "// START LOSS BASE [s]" << endl;
		fout << setiosflags(ios::fixed) << setprecision(3) << setw(20) << param->StopDelay
			<< resetiosflags(ios::fixed) << "// STOP LOSS BASE [s]" << endl;

		fout << setw(20) << param->bReopenEnabled << "// Re-Open for each car 0: disable, 1: enable" 
			<< endl; // reopen enable flag

		fout << setiosflags(ios::fixed) << setprecision(2) << setw(20) << param->additionalStopTime
			<< resetiosflags(ios::fixed) << "// Additional Time at Each Stop" << endl;
		fout << setiosflags(ios::fixed) << setprecision(2) << setw(20) << param->boardingRatio
			<< resetiosflags(ios::fixed) << "// % Boarding Ratio" << endl;
		fout << setiosflags(ios::fixed) << setprecision(2) << setw(20) << param->boardingDeboardingTime
			<< resetiosflags(ios::fixed) << "// Board + Deboard time per person" << endl;

		fout << setw(20) << param->bMotorGen << "// 0: No MG set, 1:MG[MotorGenerator]" 
			<< endl;
		fout << endl;

		fout << "// BUILDING PARAMETERS" << endl;

		
		fout << setw(20) << building->nUserFloors << "// Number of floors"
			<< endl;
		fout << setw(20) << (building->lobby1Floor) << "// Lobby Floor (Floor Index starts from 0)"
			<< endl;

		fout << setw(20) << (building->lobby2Floor) << "// 2nd Lobby Floor [255:none]"
			<< endl;

		fout << setw(20) << (building->rest1Floor) << "// Restaurant Floor [255:none]"
			<< endl;

		fout << setw(20) << (building->rest2Floor) << "// 2nd Restaurant Floor [255:none]"
			<< endl;

		fout << setw(20) << (building->expressZoneTop) << "// Top of Exp. zone for each car"
			<< endl;

		fout << setw(20) << (building->expressZoneBottom) << "// Bottom of Exp. zone for each car"
			<< endl;
		
		fout << setw(20) << param->transferTime << "// Arrive Delay Time [sec] for each car"
			<< endl;

		fout << setw(20) << param->transferFloorTime << "// Arrive Delay Time for Stairs [sec] for each car"
			<< endl;
		fout << endl;
		
		fout << "// FLOOR HEIGHT [m]" << endl;
		
		for (floorNo=0;floorNo<building->nTotalFloors;floorNo++)
		{
			fout << setw(20) << setiosflags(ios::fixed) << setprecision(3) 
				<< building->interfloorHeight[floorNo] << "// floor height of "
				<< building->floorLabel[floorNo] << endl;
		}
		
		fout << endl;
		fout << "// POPULATION OF EACH FLOOR" << endl;		

		for (floorNo=1;floorNo< building->nTotalFloors-1;floorNo++)
		{
			fout << setw(20) << building->floorPopulation[floorNo]
				<< "// floor population of "
				<< building->floorLabel[floorNo] << endl;
		}

		fout << endl;
		fout << "// Allowed_Mask for each car per floor (Hexa)" << endl;

		for (floorNo=1;floorNo<building->nTotalFloors-1;floorNo++) // shs temp
		{
			fout << setiosflags(ios::left | ios::uppercase) << setw(5) 
				<< setfill(' ') << floorNo-1 <<resetiosflags(ios::left);
			for (carNo=0; carNo< building->nCars;carNo++)
			{
				fout << "  " << setw(2) << setfill('0') << hex    
				//	<< hex << setfill('0') <<setw(2)// << setiosflags(ios::hex|ios::uppercase|ios::left)
					<< param->AllowMask[carNo][floorNo];
			}
			fout << resetiosflags( ios::hex |ios::uppercase) << endl;
		}		
		
		fout << setfill(' ') << endl;
		fout << "// Destination Entry Device Installation" << endl;

		fout << setiosflags(ios::left);
		for (floorNo=1;floorNo < building->nTotalFloors-1;floorNo++) // shs temp
		{
			fout << setw(10) << param->DestinationEntryInstalled[floorNo]
				<< "// " << building->floorLabel[floorNo] << endl;
		}

		fout << endl;
		fout << "// Simulation Configuration" << endl;

		fout << setw(20) << param->simulationType
			<< "// Simulation Hour 0: None 1:Up 2:Dn 3:Lunch 4:Business" << endl;
		fout << setw(20) << param->buildingPopulation
			<< "// Building Population" << endl;
		
		fout << endl;
		fout << "// Simulation Algorithm Selection" << endl;

		fout << setw(20) << param->bAutomaticDispatching << "// "
			<< "Manual:0, Automatic:1" << endl;
		if (param->bAutomaticDispatching==TRUE)
		{
			onlyPath=param->dispatcherDLLPath;
			strPos=onlyPath.ReverseFind('\\');
			if (strPos!=-1) // extract only file path
				onlyPath=onlyPath.Left(strPos);
		}
		else
		{
			strcpy(param->dispatcherDLLName,"NONE");
			onlyPath = "NONE";
			strcpy(param->dispatchingAlgorithmName,"NONE");
		}
		fout << param->dispatcherDLLName << "@";
		fout << " // " << onlyPath << "@" << endl;
		
		fout << setw(20) << param->algorithmID << " // "
			<< param->dispatchingAlgorithmName << "@" << endl;

		// Added Violation Display data
		fout << endl;
		fout << "// Violation Display" << endl;

		fout << setw(20) << param->m_violationHCD << endl;
		fout << endl;
		fout << "//Minimum Separation Distance" << endl;
		fout << setiosflags(ios::fixed) << setprecision(2) << setw(20) << param->MinSeparationDist
			<< resetiosflags(ios::fixed) << "// Minimum Separation Distance [m]" << endl;

		fout << setw(20) ;
		fout << "Z";
	}

	fout.close();
	return 0;
}

// <return>  0: parameter memory allocation error
//			 1: success
int C2DFileIO::InitBuilding(BUILDING_STRUCTURE *building)
{
	if (building==NULL)
		return 0; // error

	strcpy(building->mBuildingName,"2D Building");

	building->nUserFloors=20;
	building->pit=TRUE;
	building->overhead=TRUE;
	building->nTotalFloors=building->nUserFloors+building->pit+building->overhead;
	building->lobby1Floor=0;
	building->lobby2Floor=255;	// Lobby Floor 2
	building->lobby3Floor=255;	// Lobby Floor 3
	building->rest1Floor=255;	// Restaurant Floor 1
	building->rest2Floor=255;	// Restaurant floor 2

	building->interfloorHeight[0]=4.0f;
	strcpy(building->floorLabel[0],"Pit");
	building->floorPosition[0]=0;
	building->floorPopulation[0]=0;

	for (int f = 1; f < MAXFLOOR; f++) {
		building->interfloorHeight[f]=4.0;	
		building->floorPosition[f]=building->floorPosition[f-1]+building->interfloorHeight[f-1]; 
		itoa(f-1,building->floorLabel[f],10);
		strcat(building->floorLabel[f],"F");
		building->floorPopulation[f]=80;
	}
	strcpy(building->floorLabel[building->nTotalFloors-building->overhead],"Overhead");
	building->floorPopulation[building->nTotalFloors-building->overhead]=0;

	building->expressZoneBottom=255;
	building->expressZoneTop=255;

	building->nCars=8;  // No of Cars
	building->nHoistways=4; // No of Hoistways
	building->doorWidth=700;
	building->doorType=0;
	building->nDoors=1;
	return 1;
}

// <return>  0: parameter memory allocation error
//			 1: success
int C2DFileIO::InitParameters(PARAMETER *param)
{
	int floor,carNo;

	if (param==NULL)
		return 0;
	for (carNo=0;carNo<MAXCAR;carNo++)
	{
		param->initial_position[carNo]=carNo%2+1;
	}
	param->door_open=1.5;
	param->door_close=2.5;
	param->capacity=16;
    param->f_offset=-1;
  
	param->MaxJerk=2.0;
	param->MaxAcceleration=1.0;
	param->MaxVelocity=4.0;
	param->StartDelay=0.45;
	param->StopDelay=0.40;
	param->MinSeparationDist=4.0;

	param->AdjacentLimit=0;

	param->t_car_min_dwell=2.00;
	param->t_hall_min_dwell=4.60;
	param->t_hall_max_dwell=6.60;
	param->t_lobby_dwell=4.60;
	param->bReopenEnabled=1; // reopen enabled
	param->additionalStopTime=0.00; // additional stop time
	param->boardingRatio=80.00;	// boarding ratio(%)
	param->boardingDeboardingTime=1.00; // boarding + deboarding time per person
	param->bMotorGen=0; // Motor Generator?
	param->transferTime=12;
	param->transferFloorTime=60;
	for (floor=0;floor<MAXFLOOR;floor++)
	{
		param->DestinationEntryInstalled[floor]=1;
		for (carNo=0;carNo<MAXCAR;carNo++)
			param->AllowMask[carNo][floor]=0x07;
	}
	param->simulationType=0;
	param->buildingPopulation=720;
	param->duty=1600;
	param->dutyID=5;
	param->bDwellTimeExtendACS = TRUE;
	param->bRunDelayACS = TRUE;
	param->bReducedProfile = FALSE;

	// DLL related
	param->algorithmID=-1;
	param->bAutomaticDispatching=FALSE;//manual default
	strcpy(param->dispatcherDLLName,"Manual");
	strcpy(param->dispatchingAlgorithmName,"Human Intelligence");
	param->dispatcherDLLName[0]=NULL;
	param->dispatcherDLLPath[0]=NULL;
	param->dispatchingAlgorithmName[0]=NULL;
	return 1;
}

int C2DFileIO::LoadBatFile(CString file_name)
{
	ifstream fin;
	int iFile; //index of 2dp file name including path
	char charTemp[201];

	iFile = 0;

#ifndef VC70
	fin.open(file_name,ios::in|ios::nocreate);   // Old description in V6.0
#else
	fin.open(file_name,ios::in);                 // CSW: 2/7/2005 revised in new verison
#endif
		
	if (fin.good() !=0)
	{
		// Bat Traffic profile reading
		while(!fin.eof())
		{
			if(iFile >= MAX_NUMBER_TRAFFIC_FILE)
			{
				//Too many traffic profile in the batch file.
				return 0;
			}
			fin.getline(charTemp, 200);
			FileNameOf2DP[iFile] = charTemp;
			iFile++;
		}
	}
	NumberOfTrafficFile = iFile;

	fin.close();
	return 0;
}
