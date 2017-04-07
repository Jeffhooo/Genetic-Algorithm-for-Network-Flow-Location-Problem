#ifndef  GA_H
#define GA_H
#include "npeasy.h"
#define NODENUMMAX 1000
class GenAlg;
class nodeString
{
	bool ndstring[NODENUMMAX];
	int totalCost;//��������ֲ����ܷ��ã���ÿһ�ε�MCMF��Ա�����õ�
	int ndNum;//�ֲ��ڵ���
	int fitnessScore;
public:
	nodeString();
	nodeString(int ndNum_);
	nodeString(int ndNum_, bool *distr);
	nodeString(const nodeString &ns);
	~nodeString();
	int getTotalCost()const { return totalCost; }
	void setTotalCost(int totalCost_) { totalCost = totalCost_; }//�趨��ǰ�ֲ����ܷ���
	bool operator>(const nodeString& node) { return totalCost < node.totalCost; }
	bool operator<(const nodeString& node) { return totalCost < node.totalCost; }
	void getLayout(bool *toSet);//ͨ����������õ��������ֲ���toSetһ��Ҫ����ÿռ��ٷŽ���
	void mutate(double mutationPercentage);//�������
	int  getNodeNum()const { return ndNum; }
	bool * getNdstring() { return ndstring; }
	friend class GenAlg;
};

class GenAlg
{
	vector<nodeString> population;//��ǰ��Ⱥ
	int popSize;
	int chromoLength;
	double totalFitness;
	double bestFitness;//�����и��嵱������Ӧ�ĸ������Ӧ������
	double averageFitness;//���и������Ӧ�����ֵ�ƽ��ֵ
	double worstFitness;//�����и��嵱�����Ӧ�ĸ������Ӧ������
	int fittestLayout;//
	double mutationRate;//�����ʣ�����̫��
	double crossoverRate;//���򽻲�ĸ���һ����Ϊ0.7
	void calcFitnessScoreForAll();//Ϊ������ϼ�����Ӧ�Ⱥ���
	nodeString selector();//������ѡ��һ������
	void crossoverConsistent(nodeString &baby1, nodeString &baby2);
public:
	GenAlg(double MutRate, double CrossRate, const vector<nodeString> &originalLayout);
	void Epoch(vector<nodeString> &vecNewPop);//�⺯��������һ������
	void EpochWithoutCrossOver(vector<nodeString> &vecNewPop);//�⺯��������һ������

	//void init(int popsize, double MutRate, double CrossRate, int GenLenght);
	//void Reset();
	
	//double GetAverageFitness();

};



#endif // ! GA_H

