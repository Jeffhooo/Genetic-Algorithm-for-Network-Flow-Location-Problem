#include"ga.h"


nodeString::nodeString()
{
	ndNum = -1;
	for (int i = 0; i < NODENUMMAX; i++)
	{
		ndstring[i] = false;
	}
	totalCost = INT_MAX;
	fitnessScore = 0;
}

nodeString::nodeString(int ndNum_)
{
	ndNum = ndNum_;
	if (ndNum < 1)
		cout << "Error in nodeString initilisation.\n";

	for (int i = 0; i < ndNum; i++)
	{
		ndstring[i] = false;
	}
	totalCost = INT_MAX;
	fitnessScore = 0;
}

nodeString::nodeString(int ndNum_, bool * distr)
{
	ndNum = ndNum_;
	if (ndNum < 1)
		cout << "Error in nodeString initilisation.\n";
	for (int i = 0; i < ndNum; i++)
	{
		ndstring[i] = distr[i];
	}
	totalCost = INT_MAX;
	fitnessScore = 0;
}

nodeString::nodeString(const nodeString & ns)
{
	fitnessScore = ns.fitnessScore;
	ndNum = ns.ndNum;
	totalCost = ns.totalCost;

	for (int i = 0; i < ndNum; i++)
	{
		ndstring[i] = ns.ndstring[i];
	}
}


nodeString::~nodeString()
{
	for (int i = 0; i < NODENUMMAX; i++)
	{
		ndstring[i] = false;
	}
	ndNum = -1;
	totalCost = INT_MAX;
	fitnessScore = 0;
}

void nodeString::getLayout(bool * toSet)
{
	for (int i = 0; i < ndNum; i++)
	{
		toSet[i] = ndstring[i];
	}
}

void nodeString::mutate(double mutationPercentage)
{
	if (mutationPercentage > 1 || mutationPercentage < 0)
		cout << "Error int void nodeString::mutate.\n";
	for (int i = 0; i < ndNum; i++)
	{
		if (rand() / (double)RAND_MAX<= mutationPercentage)
			ndstring[i] = !ndstring[i];
	}
}


GenAlg::GenAlg(double MutRate, double CrossRate, const vector<nodeString>& originalLayout):population(originalLayout)
{
	popSize=(int)originalLayout.size();
	if (popSize < 1)
	{
		cout << "Too few population in GenAlg initialisation.\n";
		return;
	}
	chromoLength=population[0].getNodeNum();
	totalFitness=0;
	bestFitness= INT_MAX;//�����и��嵱������Ӧ�ĸ������Ӧ������
	averageFitness = INT_MAX;//���и������Ӧ�����ֵ�ƽ��ֵ
	worstFitness = INT_MAX;//�����и��嵱�����Ӧ�ĸ������Ӧ������
	fittestLayout=-1;
	mutationRate= MutRate;
	crossoverRate= CrossRate;//���򽻲�ĸ���һ����Ϊ0.7
}

void GenAlg::calcFitnessScoreForAll()
{
	int sizeOfPop = (int)population.size();
	int max = population[0].totalCost;
	int min = population[0].totalCost;
	fittestLayout = 0;
	for (int i = 1; i < sizeOfPop; i++)
	{
		if (population[i].totalCost > max)
			max = population[i].totalCost;

		if (population[i].totalCost < min)
		{
			min = population[i].totalCost;
			fittestLayout = i;
		}
	}
	totalFitness = 0;
	worstFitness = 1;
	bestFitness = max - min + 1;
	for (int i = 0; i < sizeOfPop; i++)
	{
		totalFitness += population[i].fitnessScore = max - population[i].totalCost+1;
	}
	averageFitness = totalFitness / popSize;
}

nodeString GenAlg::selector()
{
	double selector = rand() / (double)RAND_MAX*totalFitness;
	int size = (int)population.size();
	int accumulation = 0;
	for (int i = 0; i < size; i++)
	{
		accumulation += population[i].fitnessScore;
		if (accumulation >= selector)
			return population[i];
	}
	return population[totalFitness];
}

void GenAlg::Epoch(vector<nodeString>& vecNewPop)
{
	vecNewPop.clear();
	calcFitnessScoreForAll();
	while ((int)vecNewPop.size() < popSize)
	{
		//��������������
		nodeString baby1 = selector();
		nodeString baby2 = selector();
		//��baby1��baby2���н������
		crossoverConsistent(baby1, baby2);

		//ʹ�Ӵ�����������ͻ��
		baby1.mutate(mutationRate);
		baby2.mutate(mutationRate);

		//�������Ӵ�������ŵ��µĻ�������������
		vecNewPop.push_back(baby1);
		vecNewPop.push_back(baby2);

	}//�Ӵ��������
}

void GenAlg::EpochWithoutCrossOver(vector<nodeString>& vecNewPop)
{
	vecNewPop.clear();
	calcFitnessScoreForAll();
	while ((int)vecNewPop.size() < popSize)
	{
		//������һ������
		nodeString baby = selector();


		baby.mutate(mutationRate);

		//�������Ӵ�������ŵ��µĻ�������������
		vecNewPop.push_back(baby);

	}//�Ӵ��������
}

void GenAlg::crossoverConsistent(nodeString & baby1, nodeString & baby2)
{
	for (int i = 0; i < chromoLength; i++)
	{
		if ((double)rand() / RAND_MAX > 0.75)
		{
			bool temp = baby1.ndstring[i];
			baby1.ndstring[i] = baby2.ndstring[i];
			baby2.ndstring[i] = temp;
		}
	}
}




