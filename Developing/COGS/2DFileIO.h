// 2DFileIO.h: interface for the C2DFileIO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_2DFILEIO_H__C30FF2F2_23FE_4748_8BD2_B14FE3DCC2F2__INCLUDED_)
#define AFX_2DFILEIO_H__C30FF2F2_23FE_4748_8BD2_B14FE3DCC2F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class C2DFileIO  
{
public:
	int NumberOfTrafficFile;
	CString FileNameOf2DP[MAX_NUMBER_TRAFFIC_FILE];
	int InitParameters(PARAMETER *param);
	int InitBuilding(BUILDING_STRUCTURE *building);
	int Save2DPFile(CString file_name,CString *file_memo,BUILDING_STRUCTURE *building,PARAMETER *param,
			PASSENGER pax[MAXPAX],MOVE_PARAMETER move[MAXMOVE],
			int paxCount,int moveCount);
	int Load2DPFile(CString file_name,CString *file_memo,
			BUILDING_STRUCTURE *building,PARAMETER *param,
			PASSENGER pax[MAXPAX],MOVE_PARAMETER move[MAXMOVE],
			int *paxCount,int *movCount);
	int LoadBatFile(CString file_name);
	C2DFileIO();
	virtual ~C2DFileIO();

};

#endif // !defined(AFX_2DFILEIO_H__C30FF2F2_23FE_4748_8BD2_B14FE3DCC2F2__INCLUDED_)
