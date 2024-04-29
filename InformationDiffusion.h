#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <deque>
#include "time.h"
#include <sys/timeb.h>

struct FieldAttribute
{
	std::string FieldValue;
	FieldAttribute* NextField;
	FieldAttribute()
	{
		FieldValue = "";
		NextField = NULL;
	}
};

struct TWOINTONEDOUBLE
{
	int Value1;
	int Value2;
	double Value3;
	TWOINTONEDOUBLE()
	{
		Value1 = 0;
		Value2 = 0;
		Value3 = 0;
	}
};

struct SingleNet
{
	int EdgeIndex;
	int EndNodeIndex;
	SingleNet* NextEdge;
	SingleNet()
	{
		EdgeIndex = -1;
		EndNodeIndex = -1;
		NextEdge = NULL;
	}
};

class InformationDiffusion
{
public:
	std::string mEdgeStartNodeName = "StartNodeID";
	std::string mEdgeEndNodeName = "EndNodeID";
	std::string mNodeIDName = "NodeID";
	std::string mWeightName = "EdgeWeight";
	std::string mNodeLongitudeName = "Longitude";
	std::string mNodeLatitudeName = "Latitude";


	int mNodeSum;
	int mEdgeSum;
	std::vector<std::string> mNodeField;
	std::vector<std::string> mEdgeField;
	std::vector<FieldAttribute> mNodeSet;
	std::vector<FieldAttribute> mEdgeSet;
	SingleNet* mNetMatrix;                 
	SingleNet* mNetMatrixIn;  
	bool mBidirection;
	int mWeightIndex;

	//Information diffusion for online network
	int OnlineOfflineInformationDiffusion(int argc, char* argv[]);
	void ReadNetFile(std::string pFileNetNode, std::string pFileNetEdge);
	std::pair<std::vector<std::string>, std::vector<FieldAttribute>> ReadFile(std::string pFile);
	std::deque<int> SelectSeeds(int pSeedSum);
	int GenerateRandomNumber(int pStartNum, int pEndNum);
	void OutPutIntList(std::vector<int> pStoreResult, std::string pFileName);
	std::vector<int> SpreadDynamics(std::deque<int> pInitialSeedSet, float pOnSpreadingRate);
	std::vector<int> SingleSpreadDynamics(std::vector<int>& pNodeStatusSet, float pSpreadingRate, int pCurrentNode);
	std::vector<TWOINTONEDOUBLE> GetNeighbour(int pNodeID);
	void ConstructSingleGeoNet(bool pDirectionTag);
	void FreeSingleNetMemory(SingleNet& pValue);
	void ConstructIncomingEdge();
	double GetEdgeWeight(int pEdgeIndex);
	void NormalizeFieldName(std::string pNodeLongitude, std::string pNodeLatitude, std::string pNodeIDName, std::string pStartNodeIDName, std::string pEndNodeIDName, std::string pEdgeWeightName);


	//Constructor function
	InformationDiffusion();
	//destructor function
	~InformationDiffusion();
};

