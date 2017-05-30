#include "deploy.h"
#include <stdio.h>
#include"lib_io.h"
#include"npeasy.h"
#include"time.h"
#include"ga.h"


int solution::currentCost = 0;
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename)
{
	time_t start = clock();
	time_t stop_criteria = start + 80 * CLOCKS_PER_SEC;

	//你要完成的功能总入口
	solution::currentCost = 0;
	int userNum;//用户数
	int nodeNum;//节点数
	int deployCost; //服务器建设费
	int routeNum;//边数
	double avgCapacity = 0;//边平均容量
    double avgPrice = 0;//边平均单价
	double avgDemand = 0;//用户平均需求 
	int maxCapacity = 0;//边最大容量
	int maxPrice = 0;//边最大单价
	int maxDemand = 0;//用户最大需求
	int minCapacity = INT_MAX;//边最小容量
	int minPrice = INT_MAX;//边最小单价
	int minDemand = INT_MAX;//用户最小需求
	int DijUpdateRound = 10;//最短路径更新轮数
	int stringNum;//群体中个体的数量
	double muteRate = 0.005;//变异率
	double crossRate = 0.05;//交叉率
	srand(time(NULL));//以当前时间作为随机数种子

	userNode *users = NULL;
	EdgeWeithtedGraph *graph=NULL;
	pqForUserPtr pqUsers;
	setInfoViaTopo(topo, line_num, userNum, users, nodeNum, deployCost, routeNum, 
		           avgCapacity, avgDemand, avgPrice, 
		           maxCapacity, maxDemand, maxPrice, 
		           minCapacity, minDemand, minPrice, 
		           pqUsers, graph); //初始化,已验证	

	if (deployCost < 700) stringNum = 73;
	else                  stringNum = 18;

	int GenCounter = 0; //记录遗传算法进行了多少代

	vector<solution> solutions;

	if (deployCost > 900) 
	{
		vector<solution> bestSolutions;
		int LowestCost = INT_MAX;

		for (int j = 0; j < 1; j++)
		{
			bool *nsCheckDirect = new bool[nodeNum];
			for (int i = 0; i < nodeNum; i++)
				nsCheckDirect[i] = false;
			
			//for (int i = 0; i < userNum; i++)
			//{
			//	if (checkDirect(users[i], *graph, (double)deployCost) || users[i].get_current_demand() > deployCost*(0.3+j*0.1) / avgPrice) {
			//		nsCheckDirect[users[i].get_adj()] = true;
			//	}
			//}

			MCMF minCostMaxFlowAlgorithm(userNum, users, nodeNum, nsCheckDirect, routeNum, deployCost, *graph, DijUpdateRound);
			int cost = minCostMaxFlowAlgorithm.getTotalCost();
			vector<solution> tempSolutions = minCostMaxFlowAlgorithm.getSolution();
			if (cost < LowestCost)
			{
//				cout << "Miracle! " << (double)cost / (deployCost*userNum) << endl;
				bestSolutions = tempSolutions;
				LowestCost = cost;
			}
			cout << cost << " " << (double)cost / (deployCost*userNum) << endl;
			delete[] nsCheckDirect;
		}
        solutions = bestSolutions;
	}
	else
	{	
        vector<nodeString> GenGroup;
	
        bool *nsNotAdj = new bool[nodeNum];
		bool *nsDirect = new bool[nodeNum];
		bool *nsDemand = new bool[nodeNum];
		bool *nsCross = new bool[nodeNum];
		bool *nsBigSource = new bool[nodeNum];
		bool *nsConbine = new bool[nodeNum];
		bool *nsEmpty = new bool[nodeNum];

		vector<solution> bestSolutions;
		int LowestCost = INT_MAX;
		double percentage;
		if (deployCost < 700)
			percentage = 0.05;
		else
			percentage = 0.05;
        int * contribution = new int[nodeNum];
		for (int i = 0; i < nodeNum; i++)
		{
			nsConbine[i] = false;
			nsEmpty[i] = false;
			contribution[i] = 0;
		}
		for (int i = 0; i < 10; i++)
		{
			bool * ndstring = new bool[nodeNum];
			seedGeneratorAllRandom(ndstring, nodeNum, 0.4, userNum);
			MCMF minCostMaxFlowAlgorithm(userNum, users, nodeNum, ndstring, routeNum, deployCost, *graph, DijUpdateRound);
			int cost = minCostMaxFlowAlgorithm.getTotalCost();
			vector<solution> tempSolutions = minCostMaxFlowAlgorithm.getSolution();
			if (cost < LowestCost)
			{
//				cout << "Miracle! " << (double)cost / (deployCost*userNum) << endl;
				bestSolutions = tempSolutions;
				LowestCost = cost;
			}
			pqForContribution pqCon;
			calculateServerContribution(nodeNum, tempSolutions, pqCon);
			for (int i = 0; i < 10; i++)
			{
				contribution[pqCon.top().getServerIndex()] += 1;
				pqCon.pop();
			}
			delete[] ndstring;
		}

		pqForContribution pqFinalCon;		
		calculatePqNotAdj(pqFinalCon, contribution, nodeNum);		
		seedGeneratorNotAdj(nsNotAdj, nodeNum, pqFinalCon, 0.3, users, userNum);

		pqForContribution pqCross;
        calculatePqCross(pqCross, *graph, nodeNum, deployCost, userNum, users);
		seedGeneratorCross(nsCross, nodeNum, pqCross, 0.1, userNum);

		pqForContribution pqBigSource;
		calculatePqCross(pqBigSource, *graph, nodeNum, deployCost, userNum, users);
		seedGeneratorCross(nsCross, nodeNum, pqBigSource, 0.1, userNum);

		seedGeneratorDirect(nsDirect, nodeNum, 1.0, users, userNum, *graph, deployCost, avgPrice);

		seedGeneratorDemand(nsDemand, nodeNum, 0.2, userNum, pqUsers);

//		seedGeneratorCombine(nsConbine, nsNotAdj, nodeNum);
		seedGeneratorCombine(nsConbine, nsDirect, nodeNum);
		seedGeneratorCombine(nsConbine, nsDemand, nodeNum);
		seedGeneratorCombine(nsConbine, nsCross, nodeNum);
//		seedGeneratorCombine(nsConbine, nsBigSource, nodeNum);

		GenGroup.push_back(nodeString(nodeNum, nsConbine));
		GenGroup.push_back(nodeString(nodeNum, nsCross));
		GenGroup.push_back(nodeString(nodeNum, nsBigSource));
		GenGroup.push_back(nodeString(nodeNum, nsDemand));
		GenGroup.push_back(nodeString(nodeNum, nsDirect));
		GenGroup.push_back(nodeString(nodeNum, nsNotAdj));
		GenGroup.push_back(nodeString(nodeNum, nsEmpty));

		for (int i = 0; i < (stringNum-7)/4; i++)
		{
			bool *nsAllRandom0 = new bool[nodeNum];
			seedGeneratorDirectRandom(nsAllRandom0, nodeNum, 0.05, userNum, users);
			GenGroup.push_back(nodeString(nodeNum, nsAllRandom0));
			delete[] nsAllRandom0;
		}

		for (int i = 0; i < (stringNum-7)/4; i++)
		{
			bool *nsAllRandom0 = new bool[nodeNum];
			seedGeneratorDirectRandom(nsAllRandom0, nodeNum, 0.1, userNum, users);
			GenGroup.push_back(nodeString(nodeNum, nsAllRandom0));
			delete[] nsAllRandom0;
		}

		for (int i = 0; i < (stringNum-7)/4; i++)
		{
			bool *nsAllRandom0 = new bool[nodeNum];
			seedGeneratorDirectRandom(nsAllRandom0, nodeNum, 0.2, userNum, users);
			GenGroup.push_back(nodeString(nodeNum, nsAllRandom0));
			delete[] nsAllRandom0;
		}

		for (int i = 0; i < (stringNum-7)/4; i++)
		{
			bool *nsAllRandom0 = new bool[nodeNum];
			seedGeneratorDirectRandom(nsAllRandom0, nodeNum, 0.3, userNum, users);
			GenGroup.push_back(nodeString(nodeNum, nsAllRandom0));
			delete[] nsAllRandom0;
		}

		while (true)
		{
			for (int i = 0; i < stringNum; i++)
			{
				bool * ndstring = GenGroup[i].getNdstring();
				MCMF minCostMaxFlowAlgorithm(userNum, users, nodeNum, ndstring, routeNum, deployCost, *graph, DijUpdateRound);
				int cost = minCostMaxFlowAlgorithm.getTotalCost();
				GenGroup[i].setTotalCost(cost);
				vector<solution> tempSolutions = minCostMaxFlowAlgorithm.getSolution();
				if (cost < LowestCost)
				{
					cout << "Miracle! " << i << " "<< cost << " " << (double)cost / (deployCost*userNum) << endl;
					bestSolutions = tempSolutions;
					LowestCost = cost;
				}

				//stop = clock();
				//double elapse = double(stop - start) / CLOCKS_PER_SEC;
				//if (elapse>1) cout << elapse << " s\nTotal cost:\n" << cost << endl;
				//else          cout << elapse*1e3 << " ms\nTotal cost:\n" << cost << endl;
//				cout << i << " Compared to direct: " << (double)cost / (deployCost*userNum) << endl;
				
				if (clock() > stop_criteria)				
                    break;	
//                delete[] ndstring;
			}
			if (clock() > stop_criteria)
				break;
			GenAlg genAlg(muteRate, crossRate, GenGroup);
			genAlg.Epoch(GenGroup);
            ++GenCounter;
//			system("pause");
		}
		solutions = bestSolutions;
	}

	cout << "GenCounter: " << GenCounter << endl;
	delete[]users;
	delete graph;  

	// 需要输出的内容
	//char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";
	string s;
    outputSolutions(s, solutions);
	const char * topo_file = s.c_str();
	//cout << topo_file << "\n";
	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);
}







