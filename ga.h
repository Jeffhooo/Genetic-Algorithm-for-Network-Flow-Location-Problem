#ifndef  GA_H
#define GA_H
#include "npeasy.h"
#define NODENUMMAX 1000
class GenAlg;
class nodeString
{
	bool ndstring[NODENUMMAX];
	int totalCost;//利用这个分布的总费用，由每一次的MCMF成员函数得到
	int ndNum;//分布节点数
	int fitnessScore;
public:
	nodeString();
	nodeString(int ndNum_);
	nodeString(int ndNum_, bool *distr);
	nodeString(const nodeString &ns);
	~nodeString();
	int getTotalCost()const { return totalCost; }
	void setTotalCost(int totalCost_) { totalCost = totalCost_; }//设定当前分布的总费用
	bool operator>(const nodeString& node) { return totalCost < node.totalCost; }
	bool operator<(const nodeString& node) { return totalCost < node.totalCost; }
	void getLayout(bool *toSet);//通过这个函数得到服务器分布，toSet一定要申请好空间再放进来
	void mutate(double mutationPercentage);//变异概率
	int  getNodeNum()const { return ndNum; }
	bool * getNdstring() { return ndstring; }
	friend class GenAlg;
};

class GenAlg
{
	vector<nodeString> population;//当前种群
	int popSize;
	int chromoLength;
	double totalFitness;
	double bestFitness;//在所有个体当中最适应的个体的适应性评分
	double averageFitness;//所有个体的适应性评分的平均值
	double worstFitness;//在所有个体当中最不适应的个体的适应性评分
	int fittestLayout;//
	double mutationRate;//变异率，不能太大
	double crossoverRate;//基因交叉的概率一般设为0.7
	void calcFitnessScoreForAll();//为所有组合计算适应度函数
	nodeString selector();//按概率选择一个对象
	void crossoverConsistent(nodeString &baby1, nodeString &baby2);
public:
	GenAlg(double MutRate, double CrossRate, const vector<nodeString> &originalLayout);
	void Epoch(vector<nodeString> &vecNewPop);//这函数产生新一代基因
	void EpochWithoutCrossOver(vector<nodeString> &vecNewPop);//这函数产生新一代基因

	//void init(int popsize, double MutRate, double CrossRate, int GenLenght);
	//void Reset();
	
	//double GetAverageFitness();

};



#endif // ! GA_H

