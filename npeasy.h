#ifndef NPEASY_H
#define NPEASY_H
#include<iostream>
#include<vector>
#include<queue>
#include<limits.h>
#include<float.h>
#include<map>
#include"IndexPriorityQueue.h"
using namespace std;
class solution;
struct greatForSolPtr;

typedef priority_queue<solution*, vector<solution*>, greatForSolPtr>  pqForSolution;
class userNode//���ѽڵ㣬����ȷ��������棬��һ�ݾ���
{
	int adjNode;//��������ڵ�
	int demand;//����
	int index;//���
	int bandCost;//���д����
	vector<solution*> relatedSolution;
public:
	userNode();//Ĭ�Ϲ��캯��(news����ʱ�����)
	userNode(int adj_, int demand_, int index_);//���ι��죨���ǳ�����Ӧ��û�����õ���
	void set(int adj_, int demand_, int index_);//���ó�Ա�������ڶ�ȡ���ļ���Ϣ�����
	~userNode();
	void substructDemand(int toSub);//��ȥ����
	void addToBandCost(int cost);
	int get_current_demand() const { return demand; }
	int get_adj() const { return adjNode; }
	int get_index() const { return index; }
	int get_band_cost()const { return bandCost; }
	bool isSatisfied() const { return demand == 0; }
	bool isDirectlySatisfied(bool *hasServer)const { return hasServer[adjNode]; }//�������ڽڵ��жϽڵ��Ƿ�ֱ��
	void setSolutionPtr(solution *sol) {relatedSolution.push_back(sol); }
	pqForSolution getUnoptimisedSolution(int newPrice);
};


struct greatForUserPtr
{
	bool operator()(userNode* &a, userNode* &b)
	{
		return a->get_current_demand() < b->get_current_demand();//���ֵ����
	}
};

typedef priority_queue<userNode*, vector<userNode*>, greatForUserPtr> pqForUserPtr;

class Edge
{
	int from; //start node
	int to; //end node
	int price; //price 
	int capacity;  //residual capacity
	int capacityCopy;//residual capacity copy
public:
	Edge() { from = -1; to = -1; price = -1; capacity = -1; capacityCopy = -1; }
	Edge(int _from, int _to, int _price, int _capacity) : from(_from), to(_to), price(_price), capacity(_capacity), capacityCopy(_capacity) {}
	void print() const { cout << "( from : " << from << ", to : " << to << ", price : " << price << ", cap : " << capacity << ") "; }
	int getPrice() const{ return price; }
	int getFrom() const{ return from; }
	int getTo() const{ return to; }
	int getCapacity() const{ return capacity; }
	int getCapacityCopy() const{ return capacityCopy; }
	Edge* getPointerOfThis() { return this; }
	void syncDuplicate() {  capacityCopy = capacity; }//������������Ϊ������������ȣ���һ��cij�������ʱ�����бߵ��ã�
	//��ȥ��Ӧ������
	void substractCapacity(int tosub);//����ʱ����
	void substractCapacityDuplicate(int tosub);//�Ը�������
	friend class DijistraSP;
	friend class EdgeWeithtedGraph;
};

struct greatForEdgePtr
{
	bool operator()(Edge* &a, Edge* &b)
	{
		return a->getPrice() > b->getPrice();//��Сֵ����
	}
};

typedef priority_queue<Edge*, vector<Edge*>, greatForEdgePtr> pqForEdgePtr;

class EdgeWeithtedGraph
{
private:
	size_t vertaxs; // �������  
	size_t edges; // �ߵĸ���  
	vector<vector<Edge>> adjacent; // �ڽӱ� 
public:
	EdgeWeithtedGraph(size_t vertax_nums) : vertaxs(vertax_nums), edges(0), adjacent(vertax_nums) {}
	void addEdge(const Edge &e);
	void addEdge(size_t v1, size_t v2, int _price, int _capacity);
	vector<Edge*> adj(size_t v);
	vector<Edge> allEdges() const;        // ���ؼ�Ȩ����ͼ�����б�  
	size_t arrSize() const { return adjacent.size(); }
	size_t vertax() const { return vertaxs; }
	size_t edge() const { return edges; }
	void printVertax(size_t v) const;
	void printGraph() const;
	int getTotalCurrentForAdjEdge(int index);//���ؽڵ�index�����ıߵ�����֮��
	pqForEdgePtr adjPQ(size_t v);//����һ���ڵ������ڱߵ�ָ���������������Լ���
};

struct unitPriceFromServerToUser//һ�����·����¼(aij)
{
	vector<Edge*> path;//���α�����Ӧ�ı�
	int cost;
	userNode *user;//ָ���û���ָ��
	int server;//�������ڵ�
	int getCapacity()const//·������
	{
		int size = int(path.size());
		if (size == 0)
		{
			cout << " unitPriceFromServerToUser:: getCapacity()! NULL\n";
			return 0;
		}

		else if (size == 1 && path[0] == NULL)//�Լ����Լ�
			return INT_MAX;
		else
		{
			int max = path[0]->getCapacity();
			int temp;
			for (int i = 1; i < size; i++)
			{
				temp = path[i]->getCapacity();
				if (temp<max)
					max = temp;
			}
			return max;
		}
	}
	friend bool operator<(const unitPriceFromServerToUser &aijLeft, const unitPriceFromServerToUser &aijRight) { return aijLeft.cost < aijRight.cost; }
	friend bool operator>(const unitPriceFromServerToUser &aijLeft, const unitPriceFromServerToUser &aijRight) { return aijLeft.cost > aijRight.cost; }
	friend bool operator<=(const unitPriceFromServerToUser &aijLeft, const unitPriceFromServerToUser &aijRight) { return aijLeft.cost <= aijRight.cost; }
	friend bool operator>=(const unitPriceFromServerToUser &aijLeft, const unitPriceFromServerToUser &aijRight) { return aijLeft.cost >= aijRight.cost; }
};
typedef priority_queue<unitPriceFromServerToUser, vector<unitPriceFromServerToUser>, greater<unitPriceFromServerToUser>> pqForAij;

//���·��ʵ���࣬caculateCijҲ������
class DijistraSP {
	vector<Edge*> edgeTo;
	vector<int> distTo;
	//����double���͵�С����    greater ??? 
	IndexPriorityQueue<double> pq;
	void relax(EdgeWeithtedGraph &graph, int v);
	bool hasPathTo(int v) { return distTo[v] < INT_MAX; }
public:
	vector<Edge*> pathTo(int v);
	DijistraSP(EdgeWeithtedGraph &graph, int s, size_t V);
	int distanceTo(int v) { return distTo[v]; }
	//�������·��ֻ����price ������cap
	//caculateAij����:����usernode������nodeIndex(�ڹ���SP���ʱ����Դ��)��ÿ��usernode������δ�����㣩�����·����Ȼ����Aij���󣬷ŵ�pq��
	//���������δ���ԣ���Ϊusernode��δ���롣Ӧ��û�����⣬ֻ�Ƕ����·����һ����װ��
	void calculateAijCiteVersion(EdgeWeithtedGraph &graph, int numOfUsers, userNode *users, pqForAij& aij);
	void calculateAjiUserVersion(EdgeWeithtedGraph & graph, bool * hasServer, int nodeNum, userNode * users, int userIndex, pqForAij& aji);
	void calculateAijCiteVersionForAllUser(EdgeWeithtedGraph & graph, int numOfUsers, userNode * users, pqForAij& aij);
	void calculateAijCiteVersionForIndirectUsers(bool *hasServer, EdgeWeithtedGraph & graph, int numOfUsers, userNode * users, pqForAij& aij);
};


class solution;//������
class costFromServerToUser//cij�Լ���Ӧ��·��
{
	unitPriceFromServerToUser aij;//STL��list���ƾ��Ƕ�Ԫ�صĿ�����edge*��bool*���Ƕ���ָ�룬����Ҫ����д�������캯��
	int server;//�������ڵ�
	int value;//cij��ֵ
	int current;//��·���ߵ�����
	int price;//����
public:
	costFromServerToUser();
	costFromServerToUser(unitPriceFromServerToUser aij_, int server_, int current_, int value_);
	costFromServerToUser(unitPriceFromServerToUser aij_, int server_, int current_, int value_, int price_);
	void update(vector<solution> &solutions, bool *hasServer);//��aij��path����һ�飬����ÿ��Ԫ�ص���Ӧ��substract����
	void updateCopy();//����·������
	friend bool operator<(const costFromServerToUser &cijLeft, const costFromServerToUser &cijRight) { return cijLeft.value < cijRight.value; }
	friend bool operator>(const costFromServerToUser &cijLeft, const costFromServerToUser &cijRight) { return cijLeft.value > cijRight.value; }
	friend bool operator<=(const costFromServerToUser &cijLeft, const costFromServerToUser &cijRight) { return cijLeft.value <= cijRight.value;}
	friend bool operator>=(const costFromServerToUser &cijLeft, const costFromServerToUser &cijRight) { return cijLeft.value >= cijRight.value; }
	int getValue()const{return value;}
	int getServer()const { return server; }
	int getPrice()const { return price; }
	int getUserIndex()const { return aij.user->get_index(); }
	int getCurrent()const { return price == 0? 0:current; }
	double getSinggaabei()const { return price == 0 ? DBL_MAX : 1.0 / price; }
	bool isNoWay() { return aij.path.size() == 0; }

};
typedef priority_queue<costFromServerToUser, vector<costFromServerToUser>, greater<costFromServerToUser>> pqForCij;

class newServer// �½�վ��Ϣ
{
	double shareCost;
	int Node;
	pqForCij pq;
public:
	newServer();
	newServer(double _shareCost);
	newServer(double _shareCost, int _Node, pqForCij& _pq);
	friend bool operator> (const newServer &left, const newServer &right){return left.shareCost>right.shareCost;}
	friend bool operator< (const newServer &left, const newServer &right) { return left.shareCost<right.shareCost; }
	friend bool operator>= (const newServer &left, const newServer &right) { return left.shareCost>=right.shareCost; }
	friend bool operator<= (const newServer &left, const newServer &right) { return left.shareCost<=right.shareCost; }
	double getShareCost()const { return shareCost; }
	void calculateShareCost(int Node, int fi, priority_queue<int>& pqValue, pqForCij& pqCij); //�����̯�ɱ���������newServer����
	pqForCij &getCij() { return pq; }
	int getNode() { return Node; }
};

class newServerSE// �½�վ��Ϣ
{
	double singgaabei;//�Լ۱�
	int totalDemandSatisfied;//�����������
	int totalCost;//�ܷ���
	int serverNode;//�������ڵ�
	pqForCij pq;
public:
	newServerSE(pqForCij& cij, int deployCost);//��ʼ����Զ���ȶ��о͵�����
	newServerSE();
	friend bool operator> (const newServerSE &left, const newServerSE &right) { return left.singgaabei>right.singgaabei; }
	friend bool operator< (const newServerSE &left, const newServerSE &right) { return left.singgaabei<right.singgaabei; }
	friend bool operator>= (const newServerSE &left, const newServerSE &right) { return left.singgaabei >= right.singgaabei; }
	friend bool operator<= (const newServerSE &left, const newServerSE &right) { return left.singgaabei <= right.singgaabei; }
	void updateAll(bool *hasServer, vector<solution> &solutions, int userNum, userNode *users, EdgeWeithtedGraph &graph);
	double getSinggaabei()const { return singgaabei; }
	int getServerNode() const { return serverNode; }
	int getDemandAll() const { return totalDemandSatisfied; }
	int getCostAll() const { return totalCost; }
};


class solution
{
	int user;
	int current;
	vector<int> path;
	vector<Edge*>ptrForPath;//������Ϊ�ձ�ʾֱ��
	int price;//·������
public:
	static int currentCost;
	solution() :user(-1),current(-1),price(-1){}
	solution(int user_, int current_, vector<int>path_,int pri) :user(user_), current(current_), path(path_),price(pri) {};
	solution(int user_, int current_, vector<int>path_,vector<Edge*> ptr,int pri) :user(user_), current(current_), path(path_),ptrForPath(ptr), price(pri) {}

	static void addCost(int cost) { currentCost += cost; };
	vector<int> getPath() const{ return path; }
	vector<Edge*> getPathPtr() const{ return ptrForPath; }
	void resetPath(const vector<Edge*>&ptr_, const vector<int> &path_) 
	{
		path=path_;
		ptrForPath = ptr_;
	}
	int getUser() const{ return user; }
	int getCurrent() const{ return current; }
	int getPrice()const { return price; }
	solution* getThisPtr() { return this; }
	void substractReleatedEdgeCapacity(int capa);//�ָ����ͷŵ���������ȥ�����������ȥ�ͷ������Ļ���
	int getServerIndex()const { return path[0]; }
};
struct greatForSolPtr
{
	bool operator()(solution* &a, solution* &b)
	{
		return a->getPrice() < b->getPrice();//���ֵ����
	}
};

class ServerContribution {
	int serverIndex;
	double contribution;

public:
	ServerContribution();
	ServerContribution(int serverIndex_, double contribution_);
	int getContribution()const { return contribution; }
	int getServerIndex()const { return serverIndex; }
	friend bool operator> (const ServerContribution &left, const ServerContribution &right) { return left.contribution > right.contribution; }
	friend bool operator< (const ServerContribution &left, const ServerContribution &right) { return left.contribution < right.contribution; }
	friend bool operator>= (const ServerContribution &left, const ServerContribution &right) { return left.contribution >= right.contribution; }
	friend bool operator<= (const ServerContribution &left, const ServerContribution &right) { return left.contribution <= right.contribution; }
};

typedef priority_queue<ServerContribution, vector<ServerContribution>, less<ServerContribution>> pqForContribution;

void calculateCij(pqForAij&pq, pqForCij &cij);//�����վ��
void calculateCij(pqForAij&pq, pqForCij &cij, priority_queue<int> &cijValue);//������վ��(��ҪcijValue)
bool isAllDemandSatisfied(int numOfUsers, userNode *users);

void charToInt(char * a, int * res);// �������ļ�ÿһ��ת�����������飬��һ��Ԫ�������εĸ��������������Ǹ�������
string intToString(int a);//������ת���ַ���
void setInfoViaTopo(char * topo[], int line_num, int &numOfUsers, userNode *&users, int &numOfNodes, int &deployCost, int &numOfRoutes, EdgeWeithtedGraph *&currentGraph);//��line_num�������Ѿ�����topo��topo[i]��ʾ��i���ַ�������Ӧ�Ŀռ��У�����Щ��Ϣ���Գ�ʼ��Ϊuser,ͼ,ͨ�����õõ��û���������ڵ���������ɱ�
void setInfoViaTopo(char * topo[], int line_num, int &numOfUsers, userNode *&users, int &numOfNodes,int &deployCost, int &numOfRoutes, 
	                double &avgCapacity, double &avgDemand, double &avgPrice,pqForUserPtr &pqUsers, EdgeWeithtedGraph *&currentGraph);//��line_num�������Ѿ�����topo��topo[i]��ʾ��i���ַ�������Ӧ�Ŀռ��У�����Щ��Ϣ���Գ�ʼ��Ϊuser,ͼ,ͨ�����õõ��û���������ڵ���������ɱ�
void setInfoViaTopo(char * topo[], int line_num, int & numOfUsers, userNode *& users, int & numOfNodes, int & deployCost, int & numOfRoutes,
	                double & avgCapacity, double & avgDemand, double & avgPrice,
	                int & maxCapacity, int & maxDemand, int & maxPrice,
	                int & minCapacity, int & minDemand, int & minPrice,
	                pqForUserPtr &pqUsers, EdgeWeithtedGraph *& currentGraph);

void updateSolution(vector<solution> &solutions,int newServerIndex, userNode * users);//
void newUpdateSolution(bool *hasServer, EdgeWeithtedGraph &graph ,vector<solution> &solutions, int newServerIndex,int userNum,userNode * users);
void outputSolutions(string& s, vector<solution> &solutions);//���solutions
int calcTotalCost(int nodeNum,bool *hasServer,int deployCost);
int calcGinZaam(int nodeNum, bool *hasServer);
bool checkDirect(userNode & user, EdgeWeithtedGraph & graph, double deployCost);
bool checkDirect(userNode & user, EdgeWeithtedGraph & graph, int deployCost, pqForAij aji);
bool checkConverge(int nodeIndex, EdgeWeithtedGraph &graph, int deployCost, userNode * users, int userNum);
void clearDij(int nodeNum, DijistraSP **dij);
void directServer(int userIndex, userNode *users, bool *hasServer, vector<solution> &solutions);
void preSetServer(EdgeWeithtedGraph &graph,bool *hasServer,int numOfPreSet);
void seedGeneratorDirect(bool * ndstring, int nodeNum, double lambda, userNode * users, int userNum, EdgeWeithtedGraph & graph, double deployCost, double avgPrice);
void seedGeneratorDemand(bool * ndstring, int nodeNum, double percentage, int UserNum, pqForUserPtr pqUsers);
void seedGeneratorDirectRandom(bool * ndstring, int nodeNum, double percentage, int UserNum, userNode * users);
void seedGeneratorAllRandom(bool * ndstring, int nodeNum, double percentage, int UserNum);
void seedGeneratorFromCore(bool * ndstring, bool * core, int nodeNum, double percentage, int UserNum);
void seedGeneratorNotAdj(bool * ndstring, int nodeNum, pqForContribution pqCon, double percentage, userNode * users, int userNum);
void seedGeneratorCross(bool * ndstring, int nodeNum, pqForContribution pqCross, double percentage, int userNum);
void seedGeneratorCombine(bool * old, bool * add, int nodeNum);
void calculatePqCross(pqForContribution& pqCross, EdgeWeithtedGraph & graph, int nodeNum, double deployCost, int userNum, userNode * users);
void calculatePqBigSource(pqForContribution& pqCross, EdgeWeithtedGraph & graph, int nodeNum, double deployCost, int userNum, userNode * users);
void calculatePqNotAdj(pqForContribution &pqFinalCon, int * contribution, int nodeNum);
void showNdstring(bool * ndstring, int nodeNum);
bool isAdj(int serverIndex, userNode * users, int userNum);

void calculateServerContribution(int nodeNum, vector<solution> & solutions, pqForContribution & pqCon);

class MCMF//miminum cost maximum flow
{
	int userNum;//�û���
	int nodeNum;//�ڵ���
	int deployCost; //�����������
	int routeNum;//����
	userNode *users ;
	EdgeWeithtedGraph graph ;
	vector<solution> solutions;
	bool *hasServer;
	int roundCounter;//���·�����µ�����
//	bool isAvailabe;
public:
	MCMF(int userN, userNode *users_, int nodeN,bool *hasServer_,int routeN,int deployC,const EdgeWeithtedGraph& grap,int roundCounter_);
	MCMF(int userN, userNode *users_, int nodeN, bool *hasServer_, int routeN, int deployC, const EdgeWeithtedGraph& grap, int roundCounter_, bool fly);
	~MCMF();
	int getCurrentCost() { return solution::currentCost; }
	int getDeployedServer()
	{
		int count = 0;
		for (int i = 0; i < nodeNum; i++)
		{
			if (hasServer[i])
				count++;
		}
		return count;
	}
	int getDeployCost(){return getDeployedServer()*deployCost;}
	int getTotalCost() { 
//		if (isAvailabe)
			return getCurrentCost() + getDeployCost();
//		else
//	    	return 2*deployCost*userNum;
	}
//	bool isAvailable() { return isAvailabe; }
	vector<solution> &getSolution(){ return solutions; }
};




#endif // !NPEASY_H
