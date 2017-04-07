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
class userNode//消费节点，个数确定用数组存，存一份就行
{
	int adjNode;//相邻网络节点
	int demand;//需求
	int index;//编号
	int bandCost;//所有带宽费
	vector<solution*> relatedSolution;
public:
	userNode();//默认构造函数(news数组时会调用)
	userNode(int adj_, int demand_, int index_);//带参构造（我们程序中应该没机会用到）
	void set(int adj_, int demand_, int index_);//设置成员变量，在读取了文件信息后调用
	~userNode();
	void substructDemand(int toSub);//减去需求
	void addToBandCost(int cost);
	int get_current_demand() const { return demand; }
	int get_adj() const { return adjNode; }
	int get_index() const { return index; }
	int get_band_cost()const { return bandCost; }
	bool isSatisfied() const { return demand == 0; }
	bool isDirectlySatisfied(bool *hasServer)const { return hasServer[adjNode]; }//根据相邻节点判断节点是否直连
	void setSolutionPtr(solution *sol) {relatedSolution.push_back(sol); }
	pqForSolution getUnoptimisedSolution(int newPrice);
};


struct greatForUserPtr
{
	bool operator()(userNode* &a, userNode* &b)
	{
		return a->get_current_demand() < b->get_current_demand();//最大值优先
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
	void syncDuplicate() {  capacityCopy = capacity; }//将副本容量置为和真正容量相等（在一轮cij计算结束时对所有边调用）
	//减去相应的容量
	void substractCapacity(int tosub);//连接时候用
	void substractCapacityDuplicate(int tosub);//对副本操作
	friend class DijistraSP;
	friend class EdgeWeithtedGraph;
};

struct greatForEdgePtr
{
	bool operator()(Edge* &a, Edge* &b)
	{
		return a->getPrice() > b->getPrice();//最小值优先
	}
};

typedef priority_queue<Edge*, vector<Edge*>, greatForEdgePtr> pqForEdgePtr;

class EdgeWeithtedGraph
{
private:
	size_t vertaxs; // 顶点个数  
	size_t edges; // 边的个数  
	vector<vector<Edge>> adjacent; // 邻接表 
public:
	EdgeWeithtedGraph(size_t vertax_nums) : vertaxs(vertax_nums), edges(0), adjacent(vertax_nums) {}
	void addEdge(const Edge &e);
	void addEdge(size_t v1, size_t v2, int _price, int _capacity);
	vector<Edge*> adj(size_t v);
	vector<Edge> allEdges() const;        // 返回加权无向图的所有边  
	size_t arrSize() const { return adjacent.size(); }
	size_t vertax() const { return vertaxs; }
	size_t edge() const { return edges; }
	void printVertax(size_t v) const;
	void printGraph() const;
	int getTotalCurrentForAdjEdge(int index);//返回节点index出发的边的容量之和
	pqForEdgePtr adjPQ(size_t v);//返回一个节点所有邻边的指针容器（不包含自己）
};

struct unitPriceFromServerToUser//一条最短路径记录(aij)
{
	vector<Edge*> path;//依次保存相应的边
	int cost;
	userNode *user;//指向用户的指针
	int server;//服务器节点
	int getCapacity()const//路径容量
	{
		int size = int(path.size());
		if (size == 0)
		{
			cout << " unitPriceFromServerToUser:: getCapacity()! NULL\n";
			return 0;
		}

		else if (size == 1 && path[0] == NULL)//自己到自己
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

//最短路径实现类，caculateCij也在其中
class DijistraSP {
	vector<Edge*> edgeTo;
	vector<int> distTo;
	//构建double类型的小顶堆    greater ??? 
	IndexPriorityQueue<double> pq;
	void relax(EdgeWeithtedGraph &graph, int v);
	bool hasPathTo(int v) { return distTo[v] < INT_MAX; }
public:
	vector<Edge*> pathTo(int v);
	DijistraSP(EdgeWeithtedGraph &graph, int s, size_t V);
	int distanceTo(int v) { return distTo[v]; }
	//计算最短路径只考虑price 不考虑cap
	//caculateAij方法:遍历usernode，计算nodeIndex(在构建SP类的时候传入源点)到每个usernode（需求未被满足）的最短路径，然后构造Aij对象，放到pq中
	//这个方法还未测试，因为usernode尚未读入。应该没有问题，只是对最短路径的一个包装。
	void calculateAijCiteVersion(EdgeWeithtedGraph &graph, int numOfUsers, userNode *users, pqForAij& aij);
	void calculateAjiUserVersion(EdgeWeithtedGraph & graph, bool * hasServer, int nodeNum, userNode * users, int userIndex, pqForAij& aji);
	void calculateAijCiteVersionForAllUser(EdgeWeithtedGraph & graph, int numOfUsers, userNode * users, pqForAij& aij);
	void calculateAijCiteVersionForIndirectUsers(bool *hasServer, EdgeWeithtedGraph & graph, int numOfUsers, userNode * users, pqForAij& aij);
};


class solution;//声明下
class costFromServerToUser//cij以及相应的路径
{
	unitPriceFromServerToUser aij;//STL的list复制就是对元素的拷贝，edge*和bool*又是都是指针，不需要另外写拷贝构造函数
	int server;//服务器节点
	int value;//cij的值
	int current;//该路径走的流量
	int price;//单价
public:
	costFromServerToUser();
	costFromServerToUser(unitPriceFromServerToUser aij_, int server_, int current_, int value_);
	costFromServerToUser(unitPriceFromServerToUser aij_, int server_, int current_, int value_, int price_);
	void update(vector<solution> &solutions, bool *hasServer);//对aij的path遍历一遍，调用每个元素的相应的substract函数
	void updateCopy();//更新路径副本
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

class newServer// 新建站信息
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
	void calculateShareCost(int Node, int fi, priority_queue<int>& pqValue, pqForCij& pqCij); //计算分摊成本，并返回newServer对象
	pqForCij &getCij() { return pq; }
	int getNode() { return Node; }
};

class newServerSE// 新建站信息
{
	double singgaabei;//性价比
	int totalDemandSatisfied;//满足的总需求
	int totalCost;//总费用
	int serverNode;//服务器节点
	pqForCij pq;
public:
	newServerSE(pqForCij& cij, int deployCost);//初始化后远优先队列就弹光了
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
	vector<Edge*>ptrForPath;//该容器为空表示直连
	int price;//路径单价
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
	void substractReleatedEdgeCapacity(int capa);//恢复被释放的容量，减去解的容量，减去释放容量的花费
	int getServerIndex()const { return path[0]; }
};
struct greatForSolPtr
{
	bool operator()(solution* &a, solution* &b)
	{
		return a->getPrice() < b->getPrice();//最大值优先
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

void calculateCij(pqForAij&pq, pqForCij &cij);//计算旧站的
void calculateCij(pqForAij&pq, pqForCij &cij, priority_queue<int> &cijValue);//计算新站的(需要cijValue)
bool isAllDemandSatisfied(int numOfUsers, userNode *users);

void charToInt(char * a, int * res);// 把输入文件每一行转换成整形数组，第一个元素是整形的个数，后面依次是各个整形
string intToString(int a);//把整形转成字符串
void setInfoViaTopo(char * topo[], int line_num, int &numOfUsers, userNode *&users, int &numOfNodes, int &deployCost, int &numOfRoutes, EdgeWeithtedGraph *&currentGraph);//共line_num行数据已经读到topo（topo[i]表示第i行字符串）对应的空间中，用这些信息可以初始化为user,图,通过引用得到用户数，网络节点数，部署成本
void setInfoViaTopo(char * topo[], int line_num, int &numOfUsers, userNode *&users, int &numOfNodes,int &deployCost, int &numOfRoutes, 
	                double &avgCapacity, double &avgDemand, double &avgPrice,pqForUserPtr &pqUsers, EdgeWeithtedGraph *&currentGraph);//共line_num行数据已经读到topo（topo[i]表示第i行字符串）对应的空间中，用这些信息可以初始化为user,图,通过引用得到用户数，网络节点数，部署成本
void setInfoViaTopo(char * topo[], int line_num, int & numOfUsers, userNode *& users, int & numOfNodes, int & deployCost, int & numOfRoutes,
	                double & avgCapacity, double & avgDemand, double & avgPrice,
	                int & maxCapacity, int & maxDemand, int & maxPrice,
	                int & minCapacity, int & minDemand, int & minPrice,
	                pqForUserPtr &pqUsers, EdgeWeithtedGraph *& currentGraph);

void updateSolution(vector<solution> &solutions,int newServerIndex, userNode * users);//
void newUpdateSolution(bool *hasServer, EdgeWeithtedGraph &graph ,vector<solution> &solutions, int newServerIndex,int userNum,userNode * users);
void outputSolutions(string& s, vector<solution> &solutions);//输出solutions
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
	int userNum;//用户数
	int nodeNum;//节点数
	int deployCost; //服务器建设费
	int routeNum;//边数
	userNode *users ;
	EdgeWeithtedGraph graph ;
	vector<solution> solutions;
	bool *hasServer;
	int roundCounter;//最短路径更新的轮数
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
