Initial Version 050120
----------------------

050309 Hansoo - changes to 2DFileIO.cpp
  Include changes from recent version Cheng-Shuo fixed for C++.net.

  Also, I found bug in changing position to Floor. It results in the display cars that should
  be at overhead to pit position. Cars at other floors were not affected by this bug.
  The cause of the reason is that calculation of height didn't include the top floor height.

  I only changed as below in 2DFileIO.cpp, Load2DPFile(...) function for correction.

		for (floorNo=1;floorNo<=building->nTotalFloors;floorNo++) // shs bug
		{
			building->floorPosition[floorNo]=
				building->floorPosition[floorNo-1]+building->interfloorHeight[floorNo-1];
		}

050310 Art - a new version of 2DSimDlg.cpp and 2DSIM_DEFS.H has been released which incorporates a series
    of 5 sets of changes:
  050308 CheongSik:  the implemented dwell time logic has at most 0.1 second error from the comparison
  test. (I didn't test full cases. Please refer to below excel spread sheet.) I think that we can use
  the logic in the COGS.

  The dwell time logic is as follows:
    Dwell Time = max ( 2.0 + (n_board + n_deboard)/2.0, DefaultDwellTime );
    DefaultDwellTime: It depends on stop type (hall call stop, car call stop and both) and
    floor type (lobby and others).

  050308 Art: some changes were made to door dwell model, handle car operation, ScanCalls and predirection

  050309 CheongSik: For the ideal case, the door dwell timing is changed to 2.1 seconds.

    3 kinds of dwell time calculation are implemented in the latest COGS code as we discussed. Also,
    the dwell time logic is tested for the current CAGS behavior. There is no discrepancy.
    The 3 kinds of logic are (0) current CAGS behavior (which sometimes ignores the minimum lobby
    dwell time), (1) idealized door behavior (2) old COGS logic. Default setting is 0.

  050309 Art: changes were made to fix a case where doors where re-opening without picking up passengers
    due to not handling all direction values (when passenger arrives with car dropping off last demand).

  050310 CheongSik: I found some problem from GetNextCommittableFloor() and CheckMargin().
    1. GetNextCommittableFloor
	Next committable was not displayed correctly.
	To fix it, variables are initializes. (posOrg, posDest)
    2. CheckMargin
	Closest margin was not calculated correctly for some cases.
	The period of checking margin is changes for for those cases.

  050315 Hansoo : - Version Informations in About box are described in 2DSIM_DEFS.H as macro 
		ENGINEVERSION, VERSION_SUMMARY,VERSION_DETAIL. 
		So if you change version details please update the contents of that macros.
		- Release Executable compilation enabled by fixing 2DSIM.DSP
		<2DSIM_DEFS.H,2DSim.rc,2DSimDlg.cpp,Resource.h,ReadMe.txt,2DSim.dsp,2DSim.asp,2DSim.clw,2DSim.ncb> affected

050608 SeongRak : - add NCF, add absolute Position for dispatcher DLL interface

050613 CheongSik : Enhancement according to list
Removed "#include "2dsim_defs.h"" in the file UserConfiguration.cpp.
Numerical display for NCP and Current Position in the function DrawState()..
Initialize for the Car[c].NCF and Car[c].currentPos only in InitializeEngine() function.
Moved function call UpdateFloorForAllCars() from ProcessDispatcher() to ProcessNextEvent() to update floor information without dispatcher DLL as well as with dispatcher DLL.
"Violation Display" settings are stored at the end of 2dp traffic profile file.


050614 CheongSik : Enhancement according to list
"currentFloor" is added in the structure CAR_STATE and implemented. (Interface should be considered)
Direction setup with move
Reopen feature: the option could be changed in 2dp file and "User Configuration" dialog.
--------------------
Developing Version based on 0707B
- Added COGS related interface
  Linking Dispatcher DLL
  Linking HP DLL
  Linking HE DLL
- Add functionality to 
  Call to Dispatcher DLL
  Call to HP
  Call to HE
Developing Version based on 0708A
<Processing>
- Add interface to Hoistway Planner
- Add db functionality in Hoistway Executor
