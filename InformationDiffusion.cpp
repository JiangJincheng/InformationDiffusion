#include "InformationDiffusion.h"



//Constructor function
InformationDiffusion::InformationDiffusion()
{

}
//destructor function
InformationDiffusion::~InformationDiffusion()
{

}


//Information diffusion for online network
int InformationDiffusion::OnlineOfflineInformationDiffusion(int argc, char* argv[])
{
	//Input
	std::string tNetFilePath = argv[1];
	std::string tResultFile = argv[2];

	//Input Network
	std::string pFileOnlineNode = tNetFilePath + "OnlineNetNode.csv";
	std::string pFileOnlineEdge = tNetFilePath + "OnlineNetEdge.csv";
	ReadNetFile(pFileOnlineNode, pFileOnlineEdge);

	NormalizeFieldName("", "", "NodeID", "StartNode", "EndNodeID", "");
	ConstructSingleGeoNet(true);

	//information diffusion
	int tOriSeedNum = atoi(argv[3]);         //Initial number of spreader
	int tSeedNumIncrement = atoi(argv[4]);   
	float tIntialInfectionRate = atof(argv[5]);       //spreading rate
	float tInfectionRateIncreament = atof(argv[6]);  


	for (int tInitialNum = tOriSeedNum; tInitialNum <= mNodeSum; tInitialNum += tSeedNumIncrement)
	{
		if (tOriSeedNum != tInitialNum)
			tIntialInfectionRate = tInfectionRateIncreament;

		for (float tInfectionRate = tIntialInfectionRate; tInfectionRate <= 1; tInfectionRate += tInfectionRateIncreament)
		{
			std::vector<int> tInfluencedNumber;
			float tSum(0);
			int tLoopNumber(50000);
			for (int i = 0; i < tLoopNumber; i++)
			{
				std::deque<int> tSeedSet = SelectSeeds(tInitialNum);
				std::vector<int> tInfluencedNodeSet = SpreadDynamics(tSeedSet, tInfectionRate);
				
				tSum += tInfluencedNodeSet.size();
				tInfluencedNumber.push_back(tInfluencedNodeSet.size());
				tInfluencedNodeSet.~vector();
				tSeedSet.~deque();
			}
			
			std::string tOutFileName = tResultFile;
			tOutFileName += std::to_string(tInitialNum);
			tOutFileName += "-";
			tOutFileName += std::to_string(tInfectionRate);
			tOutFileName += ".csv";
			
			OutPutIntList(tInfluencedNumber, tOutFileName);
		}
	}


	return 1;
}


void InformationDiffusion::ReadNetFile(std::string pFileNetNode, std::string pFileNetEdge)
{
	std::pair<std::vector<std::string>, std::vector<FieldAttribute>> tNodeSet = ReadFile(pFileNetNode);
	mNodeField = tNodeSet.first;
	mNodeSet = tNodeSet.second;
	mNodeSum = mNodeSet.size();

	std::pair<std::vector<std::string>, std::vector<FieldAttribute>> tEdgeSet = ReadFile(pFileNetEdge);
	mEdgeField = tEdgeSet.first;
	mEdgeSet = tEdgeSet.second;
	mEdgeSum = mEdgeSet.size();

	return;
}


std::pair<std::vector<std::string>, std::vector<FieldAttribute>> InformationDiffusion::ReadFile(std::string pFile)
{
	std::pair<std::vector<std::string>, std::vector<FieldAttribute>> Result;
	char tRowData[10000];
	std::ifstream tInput(pFile);

	tInput.getline(tRowData, 10000, '\n');
	std::string tFiled("");
	for (int i = 0; i < 10000; i++)
	{
		if (tRowData[i] == ',' || tRowData[i] == ' ' || tRowData[i] == '\n' || tRowData[i] == '\0')
		{
			Result.first.push_back(tFiled);
			tFiled = "";

			if (tRowData[i] == '\n' || tRowData[i] == '\0')
				break;
		}
		else
			tFiled += tRowData[i];
	}

	float tMaxTravelTime(0);
	long tmpCount(1);
	while (!tInput.eof())
	{
		tInput.getline(tRowData, 10000, '\n');
		int tmyCount = 0;
		FieldAttribute tNodeAttribute;
		FieldAttribute* tCurrentNeedle = &tNodeAttribute;
		for (int i = 0; i < 10000; i++)
		{
			if (tRowData[i] == ',' || tRowData[i] == ' ' || tRowData[i] == '\n' || tRowData[i] == '\0')
			{
				if (tmyCount++ > 0)
				{
					FieldAttribute* tNeedle = new FieldAttribute;
					tCurrentNeedle->NextField = tNeedle;
					tCurrentNeedle = tNeedle;
				}
				tCurrentNeedle->FieldValue = tFiled;
				tFiled = "";
				if (tRowData[i] == '\n' || tRowData[i] == '\0')
					break;
			}
			else
				tFiled += tRowData[i];
		}
		Result.second.push_back(tNodeAttribute);
	}
	tInput.close();


	return Result;
}

std::deque<int> InformationDiffusion::SelectSeeds(int pSeedSum)
{
	std::deque<int> tQ;
	if (pSeedSum > mNodeSum)
		pSeedSum = mNodeSum;
	std::vector<int> tCandidateSet;
	for (int i = 0; i < mNodeSum; i++)
		tCandidateSet.push_back(i);
	
	int tLength = tCandidateSet.size();
	for (int i = 0; i < pSeedSum; i++)
	{
		int tSelectedNodeID = GenerateRandomNumber(0, tLength - 1);
		int tSelectedNode = tCandidateSet[tSelectedNodeID];
		tCandidateSet[tSelectedNodeID] = tCandidateSet[tLength - 1];
		tCandidateSet.pop_back();
		tLength--;

		tQ.push_back(tSelectedNode);
	}
	tCandidateSet.~vector();

	return tQ;
}

int InformationDiffusion::GenerateRandomNumber(int pStartNum, int pEndNum)
{
	if (pEndNum < pStartNum)
	{
		exit(1);
	}
	if (pStartNum == pEndNum)
		return pStartNum;

	struct timeb timeSeed;
	ftime(&timeSeed);
	srand(timeSeed.time % (pEndNum - pStartNum) + timeSeed.millitm);

	int tValue = pStartNum;
	if (pEndNum > pStartNum)
		tValue = rand() % (pEndNum - pStartNum) + pStartNum;

	return tValue;
}


void InformationDiffusion::OutPutIntList(std::vector<int> pStoreResult, std::string pFileName)
{
	std::ofstream tOutPut(pFileName);
	for (int i = 0; i < pStoreResult.size(); i++)
	{
		tOutPut << pStoreResult[i];
		if (i < pStoreResult.size() - 1)
			tOutPut << std::endl;
	}
	tOutPut.close();
	return;
}


std::vector<int> InformationDiffusion::SpreadDynamics(std::deque<int> pInitialSeedSet, float pOnSpreadingRate)
{
	std::vector<int> tAllAffectedNode;
	
	std::vector<bool> tTagSet;
	tTagSet.resize(mNodeSum, false);

	std::vector<int> tNodeStatusSet;
	for (int i = 0; i < mNodeSum; i++)
		tNodeStatusSet.push_back(1);            

	for (int i = 0; i < pInitialSeedSet.size(); i++)
	{
		tNodeStatusSet[pInitialSeedSet[i]] = 2; 
		tTagSet[pInitialSeedSet[i]] = true;
	}

	int tCount(1);
	while (!pInitialSeedSet.empty())
	{
		int tCurrentNode = pInitialSeedSet.front();
		pInitialSeedSet.pop_front();

		tNodeStatusSet[tCurrentNode] = 2;
		tTagSet[tCurrentNode] = false;

		
		std::vector<int> tNewInfluencedNodeSet;
		tNewInfluencedNodeSet = SingleSpreadDynamics(tNodeStatusSet, pOnSpreadingRate, tCurrentNode);

		
		for (int i = 0; i < tNewInfluencedNodeSet.size(); i++)
		{
			if (false == tTagSet[tNewInfluencedNodeSet[i]])
			{
				pInitialSeedSet.push_back(tNewInfluencedNodeSet[i]);
				tTagSet[tNewInfluencedNodeSet[i]] = true;
				tNodeStatusSet[tNewInfluencedNodeSet[i]] = 2;
			}
		}
		
		tNewInfluencedNodeSet.clear();
		tNewInfluencedNodeSet.~vector();

	}

	
	for (int i = 0; i < mNodeSum; i++)
	{
		if (tNodeStatusSet[i] > 1)
			tAllAffectedNode.push_back(i);
	}

	
	tTagSet.~vector();
	tNodeStatusSet.~vector();

	return tAllAffectedNode;
}

std::vector<int> InformationDiffusion::SingleSpreadDynamics(std::vector<int>& pNodeStatusSet, float pSpreadingRate, int pCurrentNode)
{
	std::vector<TWOINTONEDOUBLE> tNeighbourSet;
	tNeighbourSet = GetNeighbour(pCurrentNode);

	int tLength = tNeighbourSet.size();
	for (int i = 0; i < tLength; i++)
	{
		if (pNodeStatusSet[tNeighbourSet[i].Value1] > 1) 
		{
			tNeighbourSet[i] = tNeighbourSet[tLength - 1];
			tNeighbourSet.pop_back();
			tLength--;
			i--;
		}
	}

	std::vector<int> tNewInfluencedNodeSet;
	int tNewAddInfluencedSum = round(tNeighbourSet.size() * pSpreadingRate);
	tLength = tNeighbourSet.size();
	for (int i = 0; i < tNewAddInfluencedSum; i++)
	{
		int tSelectedNodeID = GenerateRandomNumber(0, tLength - 1);
		int tSelectedNode = tNeighbourSet[tSelectedNodeID].Value1;
		tNeighbourSet[tSelectedNodeID] = tNeighbourSet[tLength - 1];
		tNeighbourSet.pop_back();
		tLength--;

		if (2 == pNodeStatusSet[tSelectedNode])
			printf("Not right!\n");
		tNewInfluencedNodeSet.push_back(tSelectedNode);
		pNodeStatusSet[tSelectedNode] = 2;
	}

	tNeighbourSet.~vector();

	return tNewInfluencedNodeSet;
}


std::vector<TWOINTONEDOUBLE> InformationDiffusion::GetNeighbour(int pNodeID)
{
	std::vector<TWOINTONEDOUBLE> tNeighbourSet;
	TWOINTONEDOUBLE tNeighbour;

	SingleNet* tNeedle = &mNetMatrix[pNodeID];
	while (tNeedle && tNeedle->EdgeIndex >= 0)
	{
		tNeighbour.Value1 = tNeedle->EndNodeIndex;
		tNeighbour.Value2 = tNeedle->EdgeIndex;
		tNeighbour.Value3 = GetEdgeWeight(tNeedle->EdgeIndex);
		tNeighbourSet.push_back(tNeighbour);

		tNeedle = tNeedle->NextEdge;
	}

	if (mNetMatrixIn != NULL)
	{
		tNeedle = &mNetMatrixIn[pNodeID];
		while (tNeedle && tNeedle->EdgeIndex >= 0)
		{
			tNeighbour.Value1 = tNeedle->EndNodeIndex;
			tNeighbour.Value2 = tNeedle->EdgeIndex;
			tNeighbour.Value3 = GetEdgeWeight(tNeedle->EdgeIndex);
			tNeighbourSet.push_back(tNeighbour);

			tNeedle = tNeedle->NextEdge;
		}
	}

	return tNeighbourSet;
}

void InformationDiffusion::ConstructSingleGeoNet(bool pDirectionTag)
{
	mNodeSum = mNodeSet.size();
	mEdgeSum = mEdgeSet.size();

	int  tStartIDFieldIndex(-1), tEndIDFieldIndex(-1);
	for (int i = 0; i < mEdgeField.size(); i++)
	{
		if (mEdgeField[i] == mEdgeStartNodeName)
			tStartIDFieldIndex = i;
		if (mEdgeField[i] == mEdgeEndNodeName)
			tEndIDFieldIndex = i;
	}
	if (tStartIDFieldIndex < 0 || tEndIDFieldIndex < 0)
		printf("Stop!: Index is wrong!\n");

	int tNodeIDIndex(-1);
	for (int i = 0; i < mNodeField.size(); i++)
	{
		if (mNodeField[i] == mNodeIDName)
		{
			tNodeIDIndex = i;
			break;
		}
	}
	if (tNodeIDIndex < 0)
		printf("Stop!: Index is wrong!\n");

	mNetMatrix = new SingleNet[mNodeSum];
	for (int k = 0; k < mEdgeSum; k++)
	{
		FieldAttribute * tNeedle = &mEdgeSet[k];

		int tStartID(-1), tEndID(-1);
		for (int i = 0; i < mEdgeField.size(); i++)
		{
			if (i == tStartIDFieldIndex)
				tStartID = std::stoi(tNeedle->FieldValue);
			if (i == tEndIDFieldIndex)
				tEndID = std::stoi(tNeedle->FieldValue);

			if (tStartID >= 0 && tEndID >= 0)
				break;
			else
				tNeedle = tNeedle->NextField;
		}


		int tStartIDIndex(-1), tEndIDIndex(-1);
		int tMyCount = 0;
		int tFrontIndex = tStartID - 1;
		int tLaterIndex = tStartID;
		while (tFrontIndex >= 0 || tLaterIndex < mNodeSum)
		{
			int i = tFrontIndex;
			if ((tMyCount % 2 == 0 || tFrontIndex < 0) && tLaterIndex < mNodeSum)
			{
				i = tLaterIndex;
				tLaterIndex++;
			}
			else
				tFrontIndex--;
			tMyCount++;

			FieldAttribute* tNeedle = &mNodeSet[i];
			for (int u = 0; u < tNodeIDIndex; u++)
				tNeedle = tNeedle->NextField;

			if (std::stoi(tNeedle->FieldValue) == tStartID && tStartIDIndex < 0)
			{
				tStartIDIndex = i;
				tFrontIndex = tEndID - 1;
				tLaterIndex = tEndID;
			}

			if (std::stoi(tNeedle->FieldValue) == tEndID && tEndIDIndex < 0)
			{
				tEndIDIndex = i;
				tFrontIndex = tStartID - 1;
				tLaterIndex = tStartID;
			}

			if (tStartIDIndex >= 0 && tEndIDIndex >= 0)
				break;
		}
		if (tStartIDIndex < 0 && tEndIDIndex < 0)
			printf("Node Index is not found!\n");

		SingleNet* tNetNeedle = &mNetMatrix[tStartIDIndex];
		if (tNetNeedle->EdgeIndex < 0)
		{
			tNetNeedle->EdgeIndex = k;
			tNetNeedle->EndNodeIndex = tEndIDIndex;
		}
		else
		{
			bool tTag(false);
			if (false == tTag)
			{
				SingleNet* tNewEdge = new SingleNet;
				tNewEdge->EdgeIndex = k;
				tNewEdge->EndNodeIndex = tEndIDIndex;
				tNewEdge->NextEdge = tNetNeedle->NextEdge;
				tNetNeedle->NextEdge = tNewEdge;
			}

		}
	}

	if (pDirectionTag)
		ConstructIncomingEdge();
	else
	{
		if (mNetMatrixIn != NULL)
		{
			for (int i = 0; i < mNodeSum; i++)
				FreeSingleNetMemory(mNetMatrixIn[i]);
			mNetMatrixIn = NULL;
		}
	}
	mBidirection = pDirectionTag;

	return;
}

void InformationDiffusion::FreeSingleNetMemory(SingleNet& pValue)
{
	SingleNet* tNeedle = pValue.NextEdge;
	pValue.NextEdge = NULL;

	SingleNet* tNextNeedle(NULL);
	while (tNeedle)
	{
		tNextNeedle = tNeedle->NextEdge;
		delete tNeedle;
		tNeedle = tNextNeedle;
	}
	return;
}

void InformationDiffusion::ConstructIncomingEdge()
{
	if (NULL == mNetMatrixIn)
		mNetMatrixIn = new SingleNet[mNodeSum];
	else
	{
		for (int i = 0; i < mNodeSum; i++)
			FreeSingleNetMemory(mNetMatrixIn[i]);
		delete[] mNetMatrixIn;
		mNetMatrixIn = NULL;
		mNetMatrixIn = new SingleNet[mNodeSum];
	}

	for (int i = 0; i < mNodeSum; i++)
	{
		SingleNet* tNeedle = &mNetMatrix[i];
		while (tNeedle && tNeedle->EndNodeIndex >= 0)
		{
			if (mNetMatrixIn[tNeedle->EndNodeIndex].EdgeIndex < 0)
			{
				mNetMatrixIn[tNeedle->EndNodeIndex].EdgeIndex = tNeedle->EdgeIndex;
				mNetMatrixIn[tNeedle->EndNodeIndex].EndNodeIndex = i;
			}
			else
			{
				SingleNet* tNewEdge = new SingleNet;
				tNewEdge->EdgeIndex = tNeedle->EdgeIndex;
				tNewEdge->EndNodeIndex = i;
				tNewEdge->NextEdge = mNetMatrixIn[tNeedle->EndNodeIndex].NextEdge;
				mNetMatrixIn[tNeedle->EndNodeIndex].NextEdge = tNewEdge;
			}

			tNeedle = tNeedle->NextEdge;
		}
	}

	return;
}

double InformationDiffusion::GetEdgeWeight(int pEdgeIndex)
{
	int tCount(0);
	FieldAttribute* tNeedle = &mEdgeSet[pEdgeIndex];
	while (tNeedle)
	{
		if (tCount++ == mWeightIndex)
			return stod(tNeedle->FieldValue);
		tNeedle = tNeedle->NextField;
	}
	return 0;
}

void InformationDiffusion::NormalizeFieldName(std::string pNodeLongitude, std::string pNodeLatitude, std::string pNodeIDName, std::string pStartNodeIDName, std::string pEndNodeIDName, std::string pEdgeWeightName)
{
	//pNodeLongitude --> Longitude
	//pNodeLatitude  --> Latitude
	//pNodeIDName    --> NodeID
	for (int i = 0; i < mNodeField.size(); i++)
	{
		if (mNodeField[i] == pNodeLongitude)
			mNodeField[i] = mNodeLongitudeName;
		if (mNodeField[i] == pNodeLatitude)
			mNodeField[i] = mNodeLatitudeName;
		if (mNodeField[i] == pNodeIDName)
			mNodeField[i] = mNodeIDName;
	}

	//pStartNodeIDName --> StartNodeID
	//pEndNodeIDName   --> EndNodeID
	for (int i = 0; i < mEdgeField.size(); i++)
	{
		if (mEdgeField[i] == pStartNodeIDName)
			mEdgeField[i] = mEdgeStartNodeName;
		if (mEdgeField[i] == pEndNodeIDName)
			mEdgeField[i] = mEdgeEndNodeName;
	}

	//Õë¶Ô¼ÓÈ¨ÍøÂç
	//pEdgeWeightName ---> mWeightName
	if (mWeightName != "")
	{
		for (int i = 0; i < mEdgeField.size(); i++)
		{
			if (mEdgeField[i] == pEdgeWeightName)
			{
				mWeightIndex = i;
				mEdgeField[i] = mWeightName;
				break;
			}
		}
	}


	return;
}
