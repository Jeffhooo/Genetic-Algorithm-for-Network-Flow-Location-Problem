#include"npeasy.h"
#define MAX_SOLUTION_NUM 10000

///////////////////////////////////////�û��ڵ�//////////////////////////////////////////////
userNode::userNode()
{
	adjNode = -1;
	demand = -1;
	index = -1;
	bandCost = 0;
}

userNode::userNode(int adj_, int demand_, int index_)
{
	if (adj_< 0 || demand_ < 0 || index_<0)
		cout << "Error in userNode initilisation\n";
	adjNode = adj_;
	demand = demand_;
	index = index_;
	bandCost = 0;
}

void userNode::set(int adj_, int demand_, int index_)
{
	if (adj_< 0 || demand_ < 0 || index_<0)
		cout << "Error in userNode initilisation\n";
	adjNode = adj_;
	demand = demand_;
	index = index_;
	bandCost = 0;
}

userNode::~userNode()
{
	adjNode = -1;
	demand = -1;
	index = -1;
	bandCost = 0;
}

void userNode::substructDemand(int toSub)
{
	if (toSub > demand || toSub < 0)
	{
		cout << "error toSub in userNode::substructDemand\nindex:" << index << endl;
	}
	demand -= toSub;
}

void userNode::addToBandCost(int cost)
{
	bandCost += cost;
}

pqForSolution userNode::getUnoptimisedSolution(int newPrice)
{
	int size=int(relatedSolution.size());
	pqForSolution temp;
	for (int i = 0; i < size; i++)
	{
		if (relatedSolution[i]->getPrice() > newPrice)
			temp.push(relatedSolution[i]);
	}
	return temp;
}







///////////////////////////////////////��//////////////////////////////////////////////
void Edge::substractCapacity(int tosub)
{
	if (tosub > capacity)
	{
		cout<<"Error substract in Edge::substractCapacity(int tosub):\nCheck Now!\n";
	}
	capacityCopy=capacity -= tosub;//�������ֻ���ڸ���ʱ����
}

void Edge::substractCapacityDuplicate(int tosub)
{
	if (tosub > capacityCopy)
	{
		cout<<"Error substract in Edge::substractCapacityDuplicate(int tosub):\nCheck Now!\n";
	}
	capacityCopy -= tosub;
}



///////////////////////////////////////ͼ//////////////////////////////////////////////
//����ӱߵ�ʱ��һ��Ҫ�����������ıߣ���Ȼ��Ŀ������ͼ�����������������������Ի���Ҫ���淽��
void EdgeWeithtedGraph::addEdge(const Edge &e)
{
	size_t from = e.from;
	size_t to = e.to;
	if (!(from < arrSize() && to < arrSize()))
		return;

	adjacent[from].push_back(e);
	edges++;
}

void EdgeWeithtedGraph::addEdge(size_t v1, size_t v2, int _price, int _capacity)
{
	//adjacent[v1].push_back(Edge(v1, v2, _price, _capacity));
	//adjacent[v2].push_back(Edge(v2, v1, _price, _capacity));
	addEdge(Edge((int)v1, (int)v2, _price, _capacity));
	addEdge(Edge((int)v2, (int)v1, _price, _capacity));
}

vector<Edge*> EdgeWeithtedGraph::adj(size_t v)
{
	if (v < arrSize())
	{
		vector<Edge*>temp;
		vector<Edge> &adj = adjacent[v];
		size_t length = adj.size();
		for (size_t i = 0; i < length; i++)
			temp.push_back(adj[i].getPointerOfThis());
		return temp;
	}
	else
	{
		cout << "Error vertex input in vector<Edge*> adj(size_t v)\n";
		return vector<Edge*>();
	}
}

vector<Edge> EdgeWeithtedGraph::allEdges() const
{
	vector<Edge> vec;

	for (size_t i = 0; i < arrSize(); i++)
	{
		for (size_t j = 0; j < adjacent[i].size(); j++)
		{
			//�����еı߶������
			vec.push_back(adjacent[i][j]);
		}
	}
	return vec;
}

void EdgeWeithtedGraph::printVertax(size_t v) const
{
	//��ӡ��ö��������ıߵ���Ϣ
	if (v >= arrSize())
		return;

	for (size_t i = 0; i < adjacent[v].size(); i++)
		adjacent[v][i].print();
	cout << endl;
}

void EdgeWeithtedGraph::printGraph() const
{
	//��ӡͼ��Ϣ������ӡÿ���������Ϣ
	for (size_t i = 0; i < arrSize(); i++)
	{
		cout << i << ": ";
		printVertax(i);
	}
}

int EdgeWeithtedGraph::getTotalCurrentForAdjEdge(int index)
{
	if (index < arrSize())
	{
		int currentTotal = 0;
		pqForEdgePtr temp;
		vector<Edge> &adj = adjacent[index];
		size_t length = adj.size();
		Edge* currentPtr;
		for (size_t i = 0; i < length; i++)
		{
			currentPtr = adj[i].getPointerOfThis();
			if (currentPtr)
				currentTotal += currentPtr->getCapacity();
		}
		return currentTotal;
	}
	else
	{
		cout << "int EdgeWeithtedGraph::getTotalCurrentForAdjEdge(int index)\n";
		return -1;
	}
}

pqForEdgePtr EdgeWeithtedGraph:: adjPQ(size_t v)
{
	if (v < arrSize())
	{
		pqForEdgePtr temp;
		vector<Edge> &adj = adjacent[v];
		size_t length = adj.size();
		Edge* currentPtr;
		for (size_t i = 0; i < length; i++)
		{
			currentPtr = adj[i].getPointerOfThis();
			if (currentPtr)
				temp.push(currentPtr);
		}
		return temp;
	}
	else
	{
		cout << "Error vertex input in vector<Edge*> adj(size_t v)\n";
		return pqForEdgePtr();
	}
}

DijistraSP::DijistraSP(EdgeWeithtedGraph &graph, int s, size_t V) : edgeTo(V), distTo(V), pq(V) {
	size_t edgeToSize= edgeTo.size();

	for (size_t i = 0; i < edgeToSize; i++)
		edgeTo[i] = NULL;


	size_t graphVertax = graph.vertax();
	for (size_t v = 0; v < graphVertax; v++) {
		distTo[v] = INT_MAX;
	}


	distTo[s] = 0;

	pq.insert(s, 0);
	while (!pq.empty()) {
		relax(graph, int(pq.removeMin()));
	}

}

void DijistraSP::relax(EdgeWeithtedGraph &graph, int v)
{
	const vector<Edge*> &tempAdj = graph.adj(v);
	size_t len = tempAdj.size();

	for (size_t i = 0; i < len; i++) {
		int to = tempAdj[i]->to;
		if (distTo[to] > distTo[v] + tempAdj[i]->price&&tempAdj[i]->capacity != 0/*��������Ѿ�Ϊ0�����ټ��㣬�����ظ�����*/) {
			distTo[to] = distTo[v] + tempAdj[i]->price;
			edgeTo[to] = tempAdj[i];
			//�������ȶ��У�������ȶ��а�������change�������� ��insert
			//TODO

			if (pq.contains(to))pq.changeKey(to, distTo[to]);
			else pq.insert(to, distTo[to]);

		}
	}


}

vector<Edge*> DijistraSP::pathTo(int v)
{
	if (!hasPathTo(v)) return vector<Edge*>(0);//·�������ڣ�����һ��������
	Edge *e = edgeTo[v];
	if(!e)//�Լ����Լ��򷵻�һ����СΪ1������Ϊ��ָ���vector
	{
		vector<Edge*> path;
		path.push_back(e);
		return path;
	}

	int count = 0;

	for (; e; e = edgeTo[e->from])
		count++;
	vector<Edge*> path(count);
	count--;
	e = edgeTo[v];
	for (; e; e = edgeTo[e->from])
		path[count--]=e;
	return path;
	/*
	for (; e; e = edgeTo[e->from])
		path.push_back(e);
	int size = int(path.size());


	vector<Edge*> reverse;
	for (int i = 0; i < size; i++)
	{
		reverse.push_back(path[size-1-i]);
	}
	return reverse;
	*/
}


void DijistraSP::calculateAijCiteVersion(EdgeWeithtedGraph & graph, int numOfUsers, userNode * users,pqForAij& aij)//ȷ���߲�ͨ��·�����ᱻ���
{
	//users��Ϣ�����ų���Щ�Ѿ�����Ҫ���j
	for (int i = 0; i < numOfUsers; i++) {
		if (!users[i].isSatisfied()) {
			//�Բ���������ĵ���м������·��
			int _cost = distTo[users[i].get_adj()];
			if (_cost == INT_MAX)//�߲�ͨ��·�����Ž����ȶ���
				continue;
			vector<Edge*> _path = pathTo(users[i].get_adj());

			bool ikeru = true;
			int size = int(_path.size());
			for (int i = 0; i < size; i++)
				if (_path[i]&&_path[i]->capacity == 0)
				{
					ikeru = false;
					break;
				}

			if (!ikeru)
				continue;

			unitPriceFromServerToUser unitPrice;
			
			unitPrice.cost = _cost;
			unitPrice.path = _path;
			unitPrice.user = users + i;
			aij.push(unitPrice);
		}
	}
}

void DijistraSP::calculateAijCiteVersionForAllUser(EdgeWeithtedGraph & graph, int numOfUsers, userNode * users, pqForAij& aij)//ȷ���߲�ͨ��·�����ᱻ���
{
	//users��Ϣ�����ų���Щ�Ѿ�����Ҫ���j
	for (int i = 0; i < numOfUsers; i++) 
	{
		int _cost = distTo[users[i].get_adj()];
		if (_cost == INT_MAX)//�߲�ͨ��·�����Ž����ȶ���
			continue;
		vector<Edge*> _path = pathTo(users[i].get_adj());

		bool ikeru = true;
		int size = int(_path.size());
		for (int i = 0; i < size; i++)
			if (_path[i] && _path[i]->capacity == 0)
			{
				ikeru = false;
				break;
			}

		if (!ikeru)
			continue;

		unitPriceFromServerToUser unitPrice;

		unitPrice.cost = _cost;
		unitPrice.path = _path;
		unitPrice.user = users + i;
		aij.push(unitPrice);
	}
}

void DijistraSP::calculateAijCiteVersionForIndirectUsers(bool *hasServer,EdgeWeithtedGraph & graph, int numOfUsers, userNode * users, pqForAij & aij)
{
	//users��Ϣ�����ų���Щ�Ѿ�����Ҫ���j
	for (int i = 0; i < numOfUsers; i++) {
		if (!users[i].isDirectlySatisfied(hasServer)) {
			//�Է�ֱ���ĵ���м������·��
			int _cost = distTo[users[i].get_adj()];
			if (_cost == INT_MAX)//�߲�ͨ��·�����Ž����ȶ���
				continue;
			vector<Edge*> _path = pathTo(users[i].get_adj());

			bool ikeru = true;
			int size = int(_path.size());
			for (int i = 0; i < size; i++)
				if (_path[i] && _path[i]->capacity == 0)
				{
					ikeru = false;
					break;
				}

			if (!ikeru)
				continue;

			unitPriceFromServerToUser unitPrice;

			unitPrice.cost = _cost;
			unitPrice.path = _path;
			unitPrice.user = users + i;
			aij.push(unitPrice);
		}
	}
}

void DijistraSP::calculateAjiUserVersion(EdgeWeithtedGraph & graph, bool * hasServer, int nodeNum, userNode * users, int userIndex, pqForAij& aji)//ȷ���߲�ͨ��·�����ᱻ���
{
	for (int i = 0; i < nodeNum; i++) {
		if (hasServer[i]) {
			//���з������Ľڵ���м������·��
//			cout << "hasserver " << i << endl;
			int _cost = distTo[i];
			if (_cost == INT_MAX)//�߲�ͨ��·�����Ž����ȶ���
				continue;
			vector<Edge*> _path = pathTo(i);

			bool ikeru = true;
			int size = int(_path.size());
			int minCapacity = INT_MAX;
			for (int i = 0; i < size; i++)
			{
                
				if (_path[i] && _path[i]->capacity == 0)
				{
					minCapacity = 0;
					ikeru = false;
					break;
				}
                minCapacity = min(_path[i]->capacity, minCapacity);
			}
			if (!ikeru) continue;
			unitPriceFromServerToUser unitPrice;

			unitPrice.cost = _cost;
			unitPrice.path = _path;
			unitPrice.user = &users[userIndex]; //�Բ��ԣ�
			unitPrice.server = i;
			aji.push(unitPrice);
		}
	}
}


///////////////////////////////////////Cij//////////////////////////////////////////////
costFromServerToUser::costFromServerToUser()
{
	server = -1;
	value = INT_MAX;
	current = -1;
	price = INT_MAX;
}

costFromServerToUser::costFromServerToUser(unitPriceFromServerToUser  aij_, int server_, int current_, int value_)
{
	aij = aij_;
	server = server_;
	current = current_;
	value = value_;
	price = aij.cost;
}

costFromServerToUser::costFromServerToUser(unitPriceFromServerToUser aij_, int server_, int current_, int value_, int price_)
{
	aij = aij_;
	server = server_;
	current = current_;
	value = value_;
	price = price_;
}

void costFromServerToUser::update(vector<solution>& solutions, bool *hasServer)	//��ȥ·���������������û����󣬸���hasServer��pushһ����
{
	solutions.reserve(MAX_SOLUTION_NUM);
	vector<Edge*> &path = aij.path;
	if (aij.user->isSatisfied())
	{
		cout << "Error in updata. Already satisfied.\n";
		return;
	}
	if (aij.user->get_current_demand() < current)
		current = aij.user->get_current_demand();//ȷ�����µ��������ᳬ���û�����

	size_t size = path.size();
	if (size == 0)//������Ӧ�ò��������cij���Ϊcij����ǰ�ͱ�֤��·���ض�����
	{
		cout << "�߲�ͨ��·��Ҫ��update�ˣ�ǧ�����console�Ͽ����������\n";
		return;
	}
	if (size == 1 && path[0] == NULL)//�Լ����Լ�
	{
		vector<int>nodePath;
		nodePath.push_back(aij.user->get_adj());
		aij.user->substructDemand(current);
		hasServer[aij.user->get_adj()] = true;
		solutions.push_back(solution(aij.user->get_index(), current, nodePath, vector<Edge*>(0),0));
		aij.user->setSolutionPtr(solutions[(int)solutions.size() -1].getThisPtr());
		return;
	}

	if (hasServer[path[size - 1]->getTo()])//������Ҳ���ó���
	{
		cout << "��һ��������·��������ʱ�����û��ڵ����ڵĽڵ��Ȼ�з������ˡ�����console�ϳ��֡�\n";
		return;
	}

	vector<int>nodePath;
	vector<Edge*>ptr;
	int serverNode = -1;//��¼�û�������Ѳ���������ı�
	for (int i = (int)size - 1; i >= 0; i--)
		if (hasServer[path[i]->getFrom()])
		{
			serverNode = int(i);
			break;
		}

	if (serverNode == -1)//��·��û�з���������aij��path��㽨��������
	{
		for (int i = 0; i < (int)size; i++)
			if (path[i]->getCapacity() < current)
				current = path[i]->getCapacity();//ȷ��������
		int bandCost = 0;
		for (int i = 0; i < (int)size; i++)
		{
			path[i]->substractCapacity(current);
			nodePath.push_back(path[i]->getFrom());
			bandCost += path[i]->getPrice();
		}
		int price = bandCost;
		bandCost *= current;
		solution::addCost(bandCost);
		aij.user->addToBandCost(bandCost);
		nodePath.push_back(path[size - 1]->getTo());
		hasServer[nodePath[0]] = true;
		aij.user->substructDemand(current);
		solutions.push_back(solution(aij.user->get_index(), current, nodePath, path, price));
		aij.user->setSolutionPtr(solutions[(int)solutions.size() - 1].getThisPtr());
		return;
	}
	else//��ij֮��������ѿ�ͨ�ķ���������ʱ��ֻ��Ҫ������û����ķ��������û�����
	{
		for (int i = serverNode; i < (int)size; i++)
			if (path[i]->getCapacity() < current)
				current = path[i]->getCapacity();//ȷ��������
		int bandCost = 0;
		for (int i = serverNode; i < (int)size; i++)
		{
			path[i]->substractCapacity(current);
			nodePath.push_back(path[i]->getFrom());
			ptr.push_back(path[i]);
			bandCost += path[i]->getPrice();
		}
		int price = bandCost;
		bandCost *= current;
		solution::addCost(bandCost);
		aij.user->addToBandCost(bandCost);
		nodePath.push_back(path[size - 1]->getTo());
		aij.user->substructDemand(current);
		solutions.push_back(solution(aij.user->get_index(), current, nodePath, ptr,price));
		aij.user->setSolutionPtr(solutions[(int)solutions.size() - 1].getThisPtr());
		return;
	}
}

void costFromServerToUser::updateCopy()	//����·������
{
	vector<Edge*> &path = aij.path;
	if (aij.user->isSatisfied())
	{
		cout << "Error in updata. Already satisfied.\n";
		return;
	}
	if (aij.user->get_current_demand() < current)
		current = aij.user->get_current_demand();//ȷ�����µ��������ᳬ���û�����

	size_t size = path.size();
	if (size == 0)//������Ӧ�ò��������cij���Ϊcij����ǰ�ͱ�֤��·���ض�����
	{
		cout << "�߲�ͨ��·��Ҫ��update�ˣ�ǧ�����console�Ͽ����������\n";
		return;
	}
	if (size == 1 && path[0] == NULL)//�Լ����Լ�
	{
		aij.user->substructDemand(current);
		return;
	}

	for (int i = 0; i < (int)size; i++)
		if (path[i]->getCapacity() < current)
			current = path[i]->getCapacity();//ȷ��������
	int bandCost = 0;
	for (int i = 0; i < (int)size; i++)
	{
		path[i]->substractCapacity(current);
		bandCost += path[i]->getPrice();
	}
//		int price = bandCost;
		bandCost *= current;
		aij.user->addToBandCost(bandCost);
		aij.user->substructDemand(current);
		return;
}





///////////////////////////////////////Tao//////////////////////////////////////////////
newServer::newServer()
{
	shareCost = DBL_MAX;
	Node = -1;
}

newServer::newServer(double _shareCost){shareCost = _shareCost;}

newServer::newServer(double _shareCost, int _Node, pqForCij& _pq)
{
	shareCost = _shareCost;
	Node = _Node;
	pq = _pq;
}

void newServer::calculateShareCost(int Node_, int fi, priority_queue<int>& pqValue, pqForCij& pqCij)
{
	shareCost = 0; // ÿ�û���̯�ɱ�
	int sum = 0;
	int num = int(pqValue.size());
	while (!pqValue.empty())
	{
		sum += pqValue.top();
		pqValue.pop();
	}
	if (num == 0)
		shareCost = DBL_MAX;
	else
	    shareCost = sum / num;
	//int fiSum = 0; // �û����ʵĽ������
	//int sNum = 0; // ���ʵ��û���
	//int lastCij = 0; // �����û����������

	//size_t size = pqValue.size();
	//for (int i = 0; i < (int)size; i++) {
	//	int low = pqValue.top();
	//	int dif = 0;
	//	pqValue.pop();
	//	if (i < (int)size - 1) {
	//		dif = pqValue.top() - low; //��������Cij��ֵ
	//	}
	//	int inc = dif * (i + 1);//�����������ʽ������

	//							//������ʽ������ﵽ�򳬳�����ѣ����߷��ʵ����һ��Cij
	//							//���¼��ǰ������𣬺����û��������˳�
	//	if (fiSum + inc >= fi || i == int(size) - 1) {
	//		sNum = i + 1;
	//		lastCij = low;
	//		break;
	//	}
	//	//������ʻ���δ�ﵽ����ѣ������
	//	else {
	//		fiSum += inc;
	//	}
	//}
	//����shareCost
	//shareCost = ((double)(fi - fiSum) / sNum) + lastCij;

	Node = Node_;
	pq = pqCij;
}





///////////////////////////////////////Methods//////////////////////////////////////////////
void calculateCij(pqForAij& pq, pqForCij& cij)
{
	//cij.empty();
	vector<Edge*> edgeRelated;
	while (!pq.empty())
	{
		unitPriceFromServerToUser aij = pq.top();
		vector<Edge*> &aijPath = aij.path;
		if (aijPath.size() == 0)//�߲�ͨ��������������Ӧ���ڼ���aij��ʱ��ͱ��ž��ˡ�
		{
			cout << "�Ф��ʤ���`�Ȥ�kҊ���ޤ�����\n";
			pq.pop();
		}
		else
		{
			if (aijPath.size() == 1 && aijPath[0] == NULL)//ij�����ڽڵ㣨�Լ����Լ�������������Ÿ����û��õ����ߵ����������û�����������
			{
				cij.push(costFromServerToUser(aij, aij.user->get_adj(), aij.user->get_current_demand(), 0));
				pq.pop();
			}
			else
			{
				int max_current = aij.path[0]->getCapacityCopy();
				int size = (int)aijPath.size();
				for (int i = 1; i < size; i++)
				{
					if (aijPath[i]->getCapacityCopy() < max_current)
						max_current = aijPath[i]->getCapacityCopy();
				}
				if (aij.user->get_current_demand() < max_current)
					max_current = aij.user->get_current_demand();
				if (max_current == 0)//����cij��¼û�����壬������
				{
					pq.pop();
					continue;
				}
				for (int i = 0; i < size; i++)
				{
					aijPath[i]->substractCapacityDuplicate(max_current);
					edgeRelated.push_back(aijPath[i]);
				}
				int value = aij.cost *max_current;//���ߵ������ʾ·�������ܺͲ�������󣬲�������·�������壨�������������û�����û���㣩
				cij.push(costFromServerToUser(aij, aijPath[0]->getFrom(), max_current, value));
				pq.pop();
			}
		}
	}
	int edgeSize = int(edgeRelated.size());
	for (int i = 0; i < edgeSize; i++)//ͬ�����и���ֵ��ԭʼֵ
		edgeRelated[i]->syncDuplicate();
}

void calculateCij(pqForAij &pq, pqForCij& cij, priority_queue<int> &cijValue)//�½�վcij
{
//	cij.empty();
	vector<Edge*> edgeRelated;
	while (!pq.empty())
	{
		unitPriceFromServerToUser aij = pq.top();
		vector<Edge*> &aijPath = aij.path;
		if (aijPath.size() == 0)//�߲�ͨ��������������Ӧ���ڼ���aij��ʱ��ͱ��ž��ˡ�
		{
			cout << "�Ф��ʤ���`�Ȥ�kҊ���ޤ�����\n";
			pq.pop();
		}
		else
		{
			if (aijPath.size() == 1 && aijPath[0] == NULL)//ij�����ڽڵ㣨�Լ����Լ�������������Ÿ����û��õ����ߵ����������û�����������
			{
				cijValue.push(0);
				cij.push(costFromServerToUser(aij, aij.user->get_adj(), aij.user->get_current_demand(), 0));
				pq.pop();
			}
			else
			{
				int max_current = aij.path[0]->getCapacityCopy();
				int size = (int)aijPath.size();
				for (int i = 1; i < size; i++)
				{
					if (aijPath[i]->getCapacityCopy() < max_current)
						max_current = aijPath[i]->getCapacityCopy();
				}
				if (aij.user->get_current_demand() < max_current)
					max_current = aij.user->get_current_demand(); 
				if (max_current == 0)//����cij��¼û�����壬������
				{
					pq.pop();
					continue;
				}
					
				for (int i = 0; i < size; i++)
				{
					aijPath[i]->substractCapacityDuplicate(max_current);
					edgeRelated.push_back(aijPath[i]);
				}
				int value = aij.cost *max_current;//���ߵ������ʾ·�������ܺͲ�������󣬲�������·�������壨�������������û�����û���㣩
				
				cijValue.push(value);
				cij.push(costFromServerToUser(aij, aijPath[0]->getFrom(), max_current, value));
				pq.pop();
			}
		}
		
	}
	if(cij.size()==0)
		cout << "Calculate cij (new server) : empty queue.\n";

	int edgeSize= int(edgeRelated.size());
	for (int i = 0; i < edgeSize; i++)//ͬ�����и���ֵ��ԭʼֵ
		edgeRelated[i]->syncDuplicate();
}

bool isAllDemandSatisfied(int numOfUsers, userNode * users)
{
	for (int i = 0; i < numOfUsers; i++)
		if (!users[i].isSatisfied())
			return false;
	return true;
	
	/*int count = 0;
	for (int i = 0; i < numOfUsers; i++)
		if (users[i].isSatisfied())
			count++;
	if (count != 0)
		cout << count << " satisfied!\n";

	return count==numOfUsers;*/
		
}

void charToInt(char * a, int * res)
{
	if (a[0] == '\n'|| a[0] == '\r') {
		return;
	}
	int n = 1;
	int i = 0;
	while (a[i] != '\r'&&a[i] != '\n') {
		if (a[i] != ' ') {
			res[n] = res[n] * 10 + (a[i] - '0');
		}
		else {
			++n;
		}
		++i;
	}
	res[0] = n;
	return;
}

string intToString(int a)
{
	string s;
	while (true)
	{
		s = (char)('0' + a % 10) + s;
		a /= 10;
		if (a == 0) break;
	}
	return s;
}


void setInfoViaTopo(char * topo[], int line_num, int & numOfUsers, userNode *& users, int & numOfNodes, int & deployCost, int & numOfRoutes, EdgeWeithtedGraph *& currentGraph)
{
	for (int i = 0; i < line_num; i++) {
		int ints[10] = { 0 };
		charToInt(topo[i], ints);
		if (i == 0 && ints[0] == 3) //��ȡͼ��Ϣ������ͼ���󣬹����û�����
		{
			numOfNodes = ints[1];
			numOfRoutes = ints[2];
			numOfUsers = ints[3];
			currentGraph = new EdgeWeithtedGraph(numOfNodes);
			users = new userNode[numOfUsers];
		}
		else if (ints[0] == 1)//��ȡ�����������
		{
			deployCost = ints[1];
		}
		else if (ints[0] == 4)//��ȡ����Ϣ
		{
			currentGraph->addEdge(ints[1], ints[2], ints[4], ints[3]);
		}
		else if (ints[0] == 3)//��ȡ�û���Ϣ
		{
			users[ints[1]].set(ints[2], ints[3], ints[1]);
		}
	}
}

void setInfoViaTopo(char * topo[], int line_num, int &numOfUsers, userNode *&users, int &numOfNodes,int &deployCost, int &numOfRoutes, 
	                double &avgCapacity, double &avgDemand, double &avgPrice,pqForUserPtr &pqUsers, EdgeWeithtedGraph *&currentGraph)
{
	for (int i = 0; i < line_num; i++) {
		int ints[10] = { 0 };
		charToInt(topo[i], ints);
		if (i == 0 && ints[0] == 3) //��ȡͼ��Ϣ������ͼ���󣬹����û�����
		{
			numOfNodes = ints[1];
			numOfRoutes = ints[2];
			numOfUsers = ints[3];
			currentGraph = new EdgeWeithtedGraph(numOfNodes);
			users = new userNode[numOfUsers];
		}
		else if (ints[0] == 1)//��ȡ�����������
		{
			deployCost = ints[1];
		}
		else if (ints[0] == 4)//��ȡ����Ϣ
		{
			currentGraph->addEdge(ints[1], ints[2], ints[4], ints[3]);
			avgCapacity += ints[3];
			avgPrice += ints[4];
		}
		else if (ints[0] == 3)//��ȡ�û���Ϣ
		{
			users[ints[1]].set(ints[2], ints[3], ints[1]);
			avgDemand += ints[3];
			pqUsers.push(users + ints[1]);
		}
	}
	avgCapacity = avgCapacity / numOfRoutes;
	//cout << "avgCapacity: " << avgCapacity << endl;
	avgPrice = avgPrice / numOfRoutes;
	//cout << "avgPrice: " << avgPrice << endl;
	avgDemand = avgDemand / numOfUsers;
	//cout << "avgDemand: " << avgDemand << endl;
}


void setInfoViaTopo(char * topo[], int line_num, int & numOfUsers, userNode *& users, int & numOfNodes,int & deployCost, int & numOfRoutes, 
	                double & avgCapacity, double & avgDemand, double & avgPrice,
	                int & maxCapacity, int & maxDemand, int & maxPrice,
	                int & minCapacity, int & minDemand, int & minPrice,
	                pqForUserPtr &pqUsers, EdgeWeithtedGraph *& currentGraph)
{
	for (int i = 0; i < line_num; i++) {
		int ints[10] = { 0 };
		charToInt(topo[i], ints);
		if (i == 0 && ints[0] == 3) //��ȡͼ��Ϣ������ͼ���󣬹����û�����
		{
			numOfNodes = ints[1];
			numOfRoutes = ints[2];
			numOfUsers = ints[3];
			currentGraph = new EdgeWeithtedGraph(numOfNodes);
			users = new userNode[numOfUsers];
		}
		else if (ints[0] == 1)//��ȡ�����������
		{
			deployCost = ints[1];
		}
		else if (ints[0] == 4)//��ȡ����Ϣ
		{
			currentGraph->addEdge(ints[1], ints[2], ints[4], ints[3]);
			avgCapacity += ints[3];
			maxCapacity = max(ints[3], maxCapacity);
			minCapacity = min(ints[3], minCapacity);
			avgPrice += ints[4];
			maxPrice = max(ints[4], maxPrice);
			minPrice = min(ints[4], minPrice);
		}
		else if (ints[0] == 3)//��ȡ�û���Ϣ
		{
			users[ints[1]].set(ints[2], ints[3], ints[1]);
			avgDemand += ints[3];
			maxDemand = max(ints[3], maxDemand);
			minDemand = min(ints[3], minDemand);
			pqUsers.push(users+ints[1]);
		}
	}
	avgCapacity = avgCapacity / numOfRoutes;
	avgPrice = avgPrice / numOfRoutes;
	avgDemand = avgDemand / numOfUsers;
}

void updateSolution(vector<solution>& solutions, int newServerIndex, userNode * users)
{
	solutions.reserve(MAX_SOLUTION_NUM);
	int size = (int)solutions.size();//��ĸ���
	for (int i = 0; i < size; i++)
	{

		const vector<int> &path = solutions[i].getPath();
		int pathSize = int(path.size());
		int current_ = solutions[i].getCurrent();
		if (pathSize > 1)//solution��Ч,���Ҳ���ֱ��
		{
			for (int ii = 0; ii < pathSize; ii++)
			{
				if (path[ii] == newServerIndex)
				{
					vector<int> tempPath;
					vector<Edge*> tempPtr;
					const vector<Edge*> &ptr = solutions[i].getPathPtr();

					for (int iii = ii; iii < pathSize; iii++)
					{
						if (iii != pathSize - 1)
							tempPtr.push_back(ptr[iii]);
						tempPath.push_back(path[iii]);
					}

					int recoveryCost = 0;
					for (int iii = 0; iii < ii; iii++)
					{
						ptr[iii]->substractCapacity(-current_);
						recoveryCost += ptr[iii]->getPrice();
					}
					recoveryCost *= current_;
					solution::currentCost -= recoveryCost;
					solutions[i].resetPath(tempPtr, tempPath);
					int userIndex = solutions[i].getUser();
					users[userIndex].addToBandCost(-recoveryCost);
				}
			}
		}
	}
}

void newUpdateSolution(bool *hasServer, EdgeWeithtedGraph &graph, vector<solution>& solutions, int newServerIndex,int userNum, userNode * users)
{
	DijistraSP dij(graph, newServerIndex, graph.vertax());//Ѱ����վΪ�������·��
	pqForAij aij;
 	dij.calculateAijCiteVersionForIndirectUsers(hasServer,graph,userNum,users,aij);//�ҳ���վ�����з�ֱ���û������·��
	if (aij.size() == 0)
		return;
	unitPriceFromServerToUser temp;//��סһ��aij
	while (!aij.empty())//������aij�ļ�¼�ӱ��˵��󵯳�
	{
		temp = aij.top();
		int currentPrice = temp.cost;//��·������
		int currentCapacity = temp.getCapacity();//��·�������������
		int size_ = int(temp.path.size());//��·���ߴ�

		if (size_==0|| (size_ == 1 && temp.path[0] == NULL))//��·��Ϊֱ��·����û��·
		{
			aij.pop();
			continue;
		}
		userNode &currentUser = *temp.user;//��·����Ӧ���û�
		pqForSolution toUpdatedSol= currentUser.getUnoptimisedSolution(currentPrice);//��õ��۸�����·��aij�Ľ�����ȶ���
		if (toUpdatedSol.size() == 0)
			return;

		while ((!toUpdatedSol.empty()) && (currentCapacity>0))//�ӵ��۹󵽱��˵������Ż���
		{
			solution *currentSolution = toUpdatedSol.top();//��ǰ
			vector<Edge*> currentPath = currentSolution->getPathPtr();//��ǰ���漰���ıߵ�ָ��

			if (currentPath.size() == 0)//�ɽ�Ϊֱ����
			{
				toUpdatedSol.pop();
				continue;
			}
			int oldSolCurrent = currentSolution->getCurrent();//���Ż����ߵ�����
			if (oldSolCurrent > currentCapacity)//�����·���������Ƚ��٣�ȫ���߹�
			{
				currentSolution->substractReleatedEdgeCapacity(currentCapacity);
			
				vector<int> pathIndex;
				for (int i = 0; i < size_; i++)
				{
					temp.path[i]->substractCapacity(currentCapacity);//��ȥ��·���ϵ�����
					pathIndex.push_back(temp.path[i]->getFrom());
				}
				pathIndex.push_back(temp.path[size_-1]->getTo());
				
				solutions.push_back(solution(currentUser.get_index(), currentCapacity, pathIndex,temp.path, currentPrice));//pushһ���½�
				currentCapacity = 0;//��·�������߹�
			}
			else//�����·���������Ƚ϶�
			{
				currentSolution->substractReleatedEdgeCapacity(oldSolCurrent);//��·���漰������������·���������
				vector<int> pathIndex;
				for (int i = 0; i < size_; i++)
				{
					temp.path[i]->substractCapacity(oldSolCurrent);//��ȥ��·���ϵ�����
					pathIndex.push_back(temp.path[i]->getFrom());
				}
				pathIndex.push_back(temp.path[size_ - 1]->getTo());
				*currentSolution = solution(currentUser.get_index(), oldSolCurrent, pathIndex, temp.path, currentPrice);//ԭsolutionû���������ˣ�ֱ������solution�滻
				currentCapacity -= oldSolCurrent;//������·�����ߵ�����
			}
			toUpdatedSol.pop();
		}
		aij.pop();
	}


}

void outputSolutions(string& s, vector<solution>& solutions)
{
	int lengthOfsolutions = (int)solutions.size();
	s = intToString(lengthOfsolutions) + "\r\n\r\n";
	for (int i = 0; i < lengthOfsolutions; i++)
	{
		vector<int> path = solutions[i].getPath();
		int lengthOfPath = int(path.size());
		string p;
		for (int j = 0; j < lengthOfPath; j++)
		{
			p += intToString(path[j]) + " ";
		}
		p += intToString(solutions[i].getUser()) + " " + intToString(solutions[i].getCurrent());
		s += p;
		if (i < lengthOfsolutions - 1)
			s += "\r\n";
	}
	return;
}

int calcTotalCost(int nodeNum, bool * hasServer, int deployCost)
{
	int depServer = 0;
	for (int i = 0; i < nodeNum; i++)
	{
		if (hasServer[i])
			depServer++;
	}
	deployCost *= depServer;
	return deployCost + solution::currentCost;
}

int calcGinZaam(int nodeNum, bool * hasServer)
{
	int depServer = 0;
	for (int i = 0; i < nodeNum; i++)
	{
		if (hasServer[i])
			depServer++;
	}
	return depServer;
}


bool checkDirect(userNode & user, EdgeWeithtedGraph & graph, double deployCost)
{
    int cost = 0;
	int demand = user.get_current_demand();
	double avgPrice = 0;
	int counter = 0;
	pqForEdgePtr pqAdj = graph.adjPQ(user.get_adj());
	while (!pqAdj.empty())
	{
		Edge* temp = pqAdj.top();
		int capacity = temp->getCapacity();
		int price = temp->getPrice();
		avgPrice += price;
		if (capacity <= demand)
		{
			cost += price*capacity;
			demand -= capacity;
		}
		else
		{
			cost += price*demand;
			demand = 0;
			break;
		}
		pqAdj.pop();
		++counter;
	}
	avgPrice /= counter;
	if (cost >= deployCost) return true;
	if (demand > 0) return true;
	return false;
}

bool checkDirect(userNode & user, EdgeWeithtedGraph & graph, int deployCost, pqForAij aji)
{
	int cost = 0;
	int demand = user.get_current_demand();
	double avgPrice = 0;
	int counter = 0;
	while (!aji.empty())
	{
		int capacity = aji.top().getCapacity();
		int price = aji.top().cost;
		avgPrice += price;
		if (capacity <= demand)
		{
			cost += price*capacity;
			demand -= capacity;
		}
		else
		{
			cost += price*demand;
			demand = 0;
			break;
		}
		aji.pop();
		++counter;
	}
	avgPrice /= counter;
	if (cost >= deployCost) { return true; }
	if (demand > 0) { return true; }
	return false;
}

bool checkConverge(int nodeIndex, EdgeWeithtedGraph & graph, int deployCost, userNode * users, int userNum)
{
	pqForEdgePtr pqAdj = graph.adjPQ(nodeIndex);
	int cost = 0;
	int adjCounter = 0;
	while(!pqAdj.empty())
	{
		Edge* temp = pqAdj.top();
		int adjNode = temp->getTo();
		for (int i = 0; i < userNum; i++)
		{
			if (users[i].get_adj() == adjNode)
			{
				cost += min(temp->getCapacity(), users[i].get_current_demand())*temp->getPrice();
				++adjCounter;
			}
		}
		pqAdj.pop();
	}
	if (adjCounter > 5 && cost + deployCost < deployCost*adjCounter)
		return true;
	else 
	    return false;
}

void clearDij(int nodeNum, DijistraSP **dij)
{
	for (int i = 0; i < nodeNum; i++)
		if (dij[i])
		{
            delete dij[i];
			dij[i] = NULL;
		}
}

void directServer(int userIndex, userNode * users, bool * hasServer, vector<solution>& solutions)
{
	vector<int> path;
	hasServer[users[userIndex].get_adj()] = true;
	int sizeOfSolution = (int)solutions.size();
	solutions.reserve(MAX_SOLUTION_NUM);
	if(sizeOfSolution > 0)
	    updateSolution(solutions, users[userIndex].get_adj(), users);
	path.push_back(users[userIndex].get_adj());
	solutions.push_back(solution(users[userIndex].get_index(), users[userIndex].get_current_demand(), path,0));
	int size__ = (int)solutions.size();
	users[userIndex].setSolutionPtr(solutions[size__-1].getThisPtr());//��������ָ������û��Ľ����ȶ�����
	users[userIndex].substructDemand(users[userIndex].get_current_demand());
}

void preSetServer(EdgeWeithtedGraph & graph, bool * hasServer,int numOfPreset)
{
	int numNode = (int)graph.vertax();
	IndexPriorityQueue<int,greater<int>> ipq(numNode);
	for (int i = 0; i < numNode; i++)
	{
		vector<Edge*> currentEdges= graph.adj(i);
		int sizeOfAdj = (int)currentEdges.size();
		int capacityTotal = 0;
		for (int j = 0; j < sizeOfAdj; j++)
		{
			capacityTotal += currentEdges[j]->getCapacity();
		}
		ipq.insert(i,capacityTotal);
	}

	int index;
	for (int i = 0; i < numOfPreset; i++)
	{
		index = (int)ipq.removeMin();
		hasServer[index] = true;
	}

}

void seedGeneratorDirect(bool * ndstring, int nodeNum, double lambda, userNode * users, int userNum, EdgeWeithtedGraph & graph, double deployCost, double avgPrice)
{
	for (int i = 0; i < nodeNum; i++)
		ndstring[i] = false;
	for(int i = 0; i < userNum; i++)
	{
		if (checkDirect(users[i], graph, deployCost) /*|| users[i].get_current_demand() > (double) lambda*deployCost / avgPrice*/)
			ndstring[users[i].get_adj()] = true;
	}
}

void seedGeneratorDemand(bool * ndstring, int nodeNum, double percentage, int UserNum, pqForUserPtr pqUsers)
{
	for (int i = 0; i < nodeNum; i++)
	{
		ndstring[i] = false;
	}
	for (int i = 0; i < (double)percentage*UserNum; i++)
	{
		ndstring[pqUsers.top()->get_adj()] = true;
		pqUsers.pop();
	}
}

void seedGeneratorDirectRandom(bool * ndstring, int nodeNum, double percentage, int UserNum, userNode * users)
{
	for (int i = 0; i < nodeNum; i++)
	{
		ndstring[i] = false;
	}
	int serverNum = UserNum*percentage;
	int counter = 0;
	while (counter < serverNum)
	{
		int index = UserNum*((double)rand() / RAND_MAX);
		if (ndstring[users[index].get_adj()])
			continue;
		else
		{
			ndstring[users[index].get_adj()] = true;
			++counter;
		}
	}
}

void seedGeneratorAllRandom(bool * ndstring, int nodeNum, double percentage, int UserNum)
{
	for (int i = 0; i < nodeNum; i++)
	{
		ndstring[i] = false;
	}
	int serverNum = UserNum*percentage;
	int counter = 0;
	while (counter < serverNum)
	{
		int index = nodeNum*((double)rand() / RAND_MAX);
		if (ndstring[index])
			continue;
		else
		{
			ndstring[index] = true;
			++counter;
		}
	}
}

void seedGeneratorFromCore(bool * ndstring, bool * core, int nodeNum, double percentage, int UserNum)
{
	int serverNum = UserNum*percentage;
	int counter = 0;

	for (int i = 0; i < nodeNum; i++)
	{
		if (core[i])
		{
			ndstring[i] = true;
			++counter;
		}
		else         ndstring[i] = false;
	}
	
	while (counter < serverNum)
	{
		int index = nodeNum*((double)rand() / RAND_MAX);
		if (ndstring[index])
			continue;
		else
		{
			ndstring[index] = true;
			++counter;
		}
	}

}

void seedGeneratorNotAdj(bool * ndstring, int nodeNum, pqForContribution pqCon, double percentage, userNode * users, int userNum)
{
	for (int i = 0; i < nodeNum; i++)
		ndstring[i] = false;
	int counter = 0;
	int num = userNum*percentage;
	while (counter < num)
	{
		if (!pqCon.empty() && isAdj(pqCon.top().getServerIndex(), users, userNum))
		{
			ndstring[pqCon.top().getServerIndex()] = true;
			++counter;
		}
		pqCon.pop();
	}
}

void seedGeneratorCross(bool * ndstring, int nodeNum, pqForContribution pqCross, double percentage, int userNum)
{
	for (int i = 0; i < nodeNum; i++)
		ndstring[i] = false;
	int counter = 0;
	int num = userNum*percentage;
	while (counter < num)
	{
		if (!pqCross.empty())
		{
			ndstring[pqCross.top().getServerIndex()] = true;
			++counter;
		}
		pqCross.pop();
	}
}

void seedGeneratorCombine(bool * old, bool * add, int nodeNum)
{
	for (int i = 0; i < nodeNum; i++)
	{
		if (add[i]) old[i] = true;
	}
}

void calculatePqCross(pqForContribution& pqCross, EdgeWeithtedGraph & graph, int nodeNum, double deployCost, int userNum, userNode * users)
{
	for (int i = 0; i < nodeNum; i++)
	{
		DijistraSP dij(graph, i, nodeNum);
		pqForAij pqAij;
		dij.calculateAijCiteVersionForAllUser(graph, userNum, users, pqAij);
		pqForCij pqCij;
		calculateCij(pqAij, pqCij);
		int totalCurrent = 0;
		int totalCost = 0;
		while (!pqCij.empty())
		{
			if (pqCij.top().getPrice() == 0)
			{
				totalCurrent += users[pqCij.top().getUserIndex()].get_current_demand();
			}
			else {
                totalCurrent += pqCij.top().getValue() / pqCij.top().getPrice();
			}
			
			totalCost += pqCij.top().getValue();
			pqCij.pop();
		}
		pqCross.push(ServerContribution(i, (double)totalCurrent / (totalCost + deployCost)));
	}
}

void calculatePqBigSource(pqForContribution& pqCross, EdgeWeithtedGraph & graph, int nodeNum, double deployCost, int userNum, userNode * users)
{
	for (int i = 0; i < nodeNum; i++)
	{
		DijistraSP dij(graph, i, nodeNum);
		pqForAij pqAij;
		dij.calculateAijCiteVersionForAllUser(graph, userNum, users, pqAij);
		pqForCij pqCij;
		calculateCij(pqAij, pqCij);
		int totalCurrent = 0;
//		int totalCost = 0;
		while (!pqCij.empty())
		{
			if (pqCij.top().getPrice() == 0)
			{
				totalCurrent += users[pqCij.top().getUserIndex()].get_current_demand();
			}
			else {
				totalCurrent += pqCij.top().getValue() / pqCij.top().getPrice();
			}

//			totalCost += pqCij.top().getValue();
			pqCij.pop();
		}
		pqCross.push(ServerContribution(i, totalCurrent));
	}
}

void calculatePqNotAdj(pqForContribution& pqFinalCon, int * contribution, int nodeNum)
{
	for (int i = 0; i < nodeNum; i++)
		pqFinalCon.push(ServerContribution(i, contribution[i]));
}

void showNdstring(bool * ndstring, int nodeNum)
{
	for (int i = 0; i < nodeNum; i++)
	{
		cout << ndstring[i] << " ";
	}
	cout << endl;
}

bool isAdj(int serverIndex, userNode * users, int userNum)
{
	for (int i = 0; i < userNum; i++) 
	{
		if (users[i].get_adj() == serverIndex) return true;
	}
	return false;
}

void calculateServerContribution(int nodeNum, vector<solution>& solutions, pqForContribution & pqCon)
{
	map<int, int> mapForCon;
	int size = (int)solutions.size();
	for (int i = 0; i < size; i++)
	{
		int serverIndex = solutions[i].getServerIndex();
		int current = solutions[i].getCurrent();
		if (mapForCon.find(serverIndex) != mapForCon.end())
			mapForCon[serverIndex] += current;
		else
			mapForCon[serverIndex] = current;		
	}
	map<int, int>::iterator iter;
	for (iter = mapForCon.begin(); iter != mapForCon.end(); ++iter)
	{
		pqCon.push(ServerContribution(iter->first, iter->second));
	}
}

void solution::substractReleatedEdgeCapacity(int capa)
{
	int size=int(ptrForPath.size());
	if (size == 0)//ֱ�����
		return;
	for (int i = 0; i < size; i++)
	{
		ptrForPath[i]->substractCapacity(-capa);//�ָ�ԭ·��������
	}
	current -= capa;//solution�ߵ�current����
	currentCost -= price*capa;//��ȥ�ͷŵ��������ļ�Ǯ

}

MCMF::MCMF(int userN, userNode *users_, int nodeN, bool *hasServer_, int routeN, int deployC, const EdgeWeithtedGraph & grap, int roundCounter_) : userNum(userN), nodeNum(nodeN), deployCost(deployC), routeNum(routeN), graph(grap)
{
	solutions.reserve(MAX_SOLUTION_NUM);
	roundCounter = roundCounter_;
	solution::currentCost = 0;
	users = new userNode[userNum];
	for (int i = 0; i < userNum; i++)
	{
		users[i] = users_[i];
	}

	hasServer = new bool[nodeNum];
	for (int i = 0; i < nodeNum; i++)
	{
		hasServer[i] = hasServer_[i];
	}


	//for (int i = 0; i < userNum; i++)
	//{

	//	if (hasServer[users[i].get_adj()] || checkDirect(users[i], graph, deployCost))
	//	{
	//		directServer(i, users, hasServer, solutions);
	//		hasServer[users[i].get_adj()] = true;
	//	}
	//}

	DijistraSP **dij = new DijistraSP*[nodeNum];
	for (int i = 0; i < nodeNum; i++)
		dij[i] = NULL;
	int roundC = 0;
	int newnew = 0;
	while (!isAllDemandSatisfied(userNum, users))
	{
		costFromServerToUser minCij;
		newServer minServer;
		for (int i = 0; i < nodeNum; i++)//�ѿ�ͨ�ڵ�
		{
			if (hasServer[i])
			{
				pqForAij aij;
				pqForCij cij;
				if (!dij[i])
					dij[i] = new DijistraSP(graph, i, nodeNum);
				dij[i]->calculateAijCiteVersion(graph, userNum, users, aij);//�ѱ�֤�����·�����ǿ����ߵ�
				if (aij.size() == 0)//i��j��·���ߣ���������ѭ��
					continue;
				pqForAij topAij;
				unitPriceFromServerToUser best = aij.top();
				int lowCost = best.cost;
				while (!aij.empty() && aij.top().cost <= lowCost)
				{
					if (best.getCapacity() < aij.top().getCapacity())
						best = aij.top();
					aij.pop();
				}
				topAij.push(best);
				calculateCij(topAij, cij);
				costFromServerToUser temp = cij.top();
				if (minCij.getPrice() > temp.getPrice() || (minCij.getPrice() == temp.getPrice() && minCij.getValue() < temp.getValue()))
					minCij = temp;
			}
		}
		//t1 = minCij.getValue();
		//		cout << "t1 = " << t1  << endl;
		newServerSE doiGinZaam;
		double pcr_old = minCij.getSinggaabei(), pcr_new = doiGinZaam.getSinggaabei();
		if (minCij.isNoWay())
		{
			newnew++;
			for (int i = 0; i < nodeNum; i++)//�ѿ�ͨ�ڵ�
			{
				if (!hasServer[i])
				{
					pqForAij aijForNew;
					pqForCij cijForNew;
					if (!dij[i])
						dij[i] = new DijistraSP(graph, i, nodeNum);
					dij[i]->calculateAijCiteVersion(graph, userNum, users, aijForNew);//�ѱ�֤�����·�����ǿ����ߵ�
					if (aijForNew.size() == 0)//i��j��·���ߣ���������ѭ��
						continue;
					calculateCij(aijForNew, cijForNew);
					newServerSE temp(cijForNew, deployCost);
					if (temp.getDemandAll() > doiGinZaam.getDemandAll())
						doiGinZaam = temp;
					if (temp.getDemandAll() == doiGinZaam.getDemandAll() && temp.getCostAll() < doiGinZaam.getCostAll())
						doiGinZaam = temp;
				}
			}
			//doiGinZaam.updateAll(hasServer,solutions,userNum,users,graph);
			hasServer[doiGinZaam.getServerNode()] = true;
			//cout << "A new server is deployed, " << doiGinZaam.getDemandAll() << " demand satisfied with cost " << doiGinZaam.getCostAll() << " PCR: " << doiGinZaam.getSinggaabei() << endl;
		}
		else//��վ����·��
		{
			minCij.update(solutions, hasServer);
			updateSolution(solutions, minCij.getServer(), users);
			newUpdateSolution(hasServer, graph, solutions, minCij.getServer(), userNum, users);
			int userIndex = minCij.getUserIndex();
			if (users[userIndex].get_band_cost() > deployCost)
			{
				directServer(userIndex, users, hasServer, solutions);
				updateSolution(solutions, users[userIndex].get_adj(), users);
				newUpdateSolution(hasServer, graph, solutions, users[userIndex].get_adj(), userNum, users);
			}
		}



		//{
		//	newnew++;
		//	int nextServerIndex = -1;
		//	int currentMax = 0;
		//	for (int i = 0; i < nodeNum; i++)//�ѿ�ͨ�ڵ�
		//	{
		//		if (!hasServer[i])
		//		{
		//			int current=graph.getTotalCurrentForAdjEdge(i);
		//			if (current > currentMax)
		//			{
		//				currentMax = current;
		//				nextServerIndex = i;
		//			}
		//		}
		//	}
		//	if (nextServerIndex == -1)
		//		cout << "Error in new server step.\n";
		//	hasServer[nextServerIndex] = true;
		//	cout << currentMax << endl;
		//}














		//else if (t1 < 2000000000)
		//{
		//	directServer(minCij.getUserIndex(), users, hasServer, solutions);
		//	updateSolution(solutions, minCij.getServer(), users);
		//	newUpdateSolution(hasServer, graph, solutions, users[minCij.getUserIndex()].get_adj(), userNum, users);

		//}
		//else
		//{
		//	for (int i = 0; i < nodeNum; i++)
		//	{
		//		if (!hasServer[i])
		//		{
		//			pqForAij aij;
		//			pqForCij cij;
		//			if (!dij[i])
		//				dij[i] = new DijistraSP(graph, i, nodeNum);
		//			dij[i]->calculateAijCiteVersion(graph, userNum, users, aij);//�ѱ�֤�����·�����ǿ����ߵ�
		//			if (aij.size() == 0)//i��j��·���ߣ���������ѭ��
		//				continue;
		//			priority_queue< int> cijValue;
		//			calculateCij(aij, cij, cijValue);
		//			newServer temp;
		//			temp.calculateShareCost(i, deployCost, cijValue, cij);
		//			if (minServer > temp)
		//				minServer = temp;
		//		}
		//	}
		//			cout << "build a new server at " << minServer.getNode() << endl;
		//hasServer[minServer.getNode()] = true;
		//updateSolution(solutions, minServer.getNode(), users);
		//newUpdateSolution(hasServer, graph, solutions, minServer.getNode(), userNum, users);
		//}

		++roundC;
		if (roundC == roundCounter)
		{
			roundC = 0;
			clearDij(nodeNum, dij);
		}
	}
	int count_for_direct_server = 0, new_server = 0;
	for (int i = 0; i < userNum; i++)
	{
		if (hasServer[users[i].get_adj()])
			count_for_direct_server++;
	}
	for (int i = 0; i < nodeNum; i++)
	{
		if (hasServer[i])
			new_server++;
	}

//	cout << "Zik lin server num: " << count_for_direct_server << endl;
//	cout << "New server num: " << new_server << endl;


	//cout << newnew << "  New stations are established after initial distribution.\n";
	for (int i = 0; i < nodeNum; i++)
		if (dij[i])
			delete dij[i];
	if (dij)
		delete[]dij;
	solutions.resize(solutions.size());
}

MCMF::~MCMF()
{
	if(users)
		delete[]users;
	if(hasServer)
		delete[]hasServer;
	users = NULL;
	hasServer = NULL;
}

ServerContribution::ServerContribution()
{
	serverIndex = -1;
	contribution = -1;
}

ServerContribution::ServerContribution(int serverIndex_, double contribution_)
{
	serverIndex = serverIndex_;
	contribution = contribution_;
}

newServerSE::newServerSE(pqForCij & cij, int deployCost)
{
	singgaabei = 0;
	totalDemandSatisfied = 0;
	totalCost = 0;
	if (cij.size() == 0)
	{
		cout << "Error cij PQ input in newServerSE initialisation.\n";
		return;
	}
	if (deployCost < 0)
	{
		cout << "Error deployCost input in newServerSE initialisation.\n";
		return;
	}
	serverNode = cij.top().getServer();
	pq = cij;
	costFromServerToUser temp;
	while (!cij.empty())
	{
		temp = cij.top();
		totalDemandSatisfied += temp.getCurrent();
		totalCost += temp.getValue();
		cij.pop();
	}
	totalCost += deployCost;
	singgaabei = totalDemandSatisfied / double(totalCost);
}

newServerSE::newServerSE()
{
	singgaabei = 0;//�Լ۱�
	totalDemandSatisfied = 0;//�����������
	totalCost = 0;//�ܷ���
	serverNode = -1;//�������ڵ�
}

void newServerSE::updateAll(bool * hasServer, vector<solution> & solutions, int userNum, userNode *users, EdgeWeithtedGraph &graph)
{
	costFromServerToUser temp;
	while (!pq.empty())
	{
		temp = pq.top();
		temp.update(solutions, hasServer);
		updateSolution(solutions, serverNode, users);
		newUpdateSolution(hasServer, graph, solutions, serverNode, userNum, users);
		pq.pop();
	}
}