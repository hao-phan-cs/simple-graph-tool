#include "headers/Graph.h"
#include <iomanip>
#include <algorithm>

Graph::Graph(int nodeNum) {
    init(nodeNum);
}

void Graph::init(int nodeNum) {
    this->clear();
    if (nodeNum <= 0) return;
    for (int i = 0; i < nodeNum; i++)
        this->addNode();
}

void Graph::clear() {
    this->_nodeSet.clear();
    this->_arcSet.clear();
    this->_cachedNodeList.clear();
}

void Graph::readFromFile(const std::string &file) {
    clear();
    std::ifstream in(file);
    if (!in) throw "file not existed";

    int countNodes;
    in >> countNodes;
    if (countNodes <= 0) throw "number of nodes <= 0";
    std::string w, name;
    qreal x, y;
    for (int i = 0; i < countNodes; i++) {
        if (in.eof()) throw "eof";
        in >> name >> x >> y, addNode(Node(name, QPointF(x, y)));
    }
    int countArcs;
    in >> countArcs;
    if (countArcs > 0) {
        std::string start, end;
        int weight;
        if (in.eof()) throw "eof";
        in >> start >> end >> weight;
        setArc(start, end, weight);
    }
    in.close();
}

void Graph::writeToFile(const std::string &file) const {
    std::ofstream out(file);
    out << countNodes() << "\n";
    for (const auto &node: nodeList())
        out << node->name() << " " << node->euclidePos().x() << " " << node->euclidePos().y() << "\n";
    out << countArcs() << "\n";
    for (const auto &arc: arcList())
        out << arc.start()->name() << " " << arc.end()->name() << " " << arc.weight() << "\n";
    out.close();
}

AdjMatrix Graph::adjMatrix() const {
    AdjMatrix adj;
    adj.matrix = std::vector<std::vector<int>>(countNodes(), std::vector<int>(countNodes(), INT_MAX));
    auto nodes = nodeList();
    adj.nodes.assign(nodes.begin(), nodes.end());
    std::unordered_map<std::string, int> nameToIndex;
    auto it = adj.nodes.begin();
    for (int i = 0; i < countNodes(); ++i, ++it) {
        nameToIndex[(*it)->name()] = i;
        adj.matrix[i][i] = 0;
    }
    for (auto &arc: arcList())
        adj.matrix[nameToIndex[arc.start()->name()]][nameToIndex[arc.end()->name()]] = arc.weight();
    return adj;
}

std::list<Arc> Graph::arcList() const {
    std::list<Arc> arcList;
    for (auto &it: _arcSet) {
        arcList.emplace_back(Arc(it.first.first, it.first.second, it.second));
    }
    return arcList;
}

/*std::list<const Node *> Graph::nodeList() const {
    std::list<const Node *> nodeList;
    for (auto it = _nodeSet.begin(); it != _nodeSet.end(); ++it)
        nodeList.emplace_back(&(*it));
    std::sort(nodeList.begin(), nodeList.end(), [](const Node *&u, const Node *&v) -> bool {
        auto uname = u->getName();
        auto vname = v->getName();
        if (uname.substr(1, uname.length()) == vname.substr(1, vname.length()))
            return uname[0] < vname[0];
        return uname.substr(1, uname.length()) < vname.substr(1, vname.length());
    });
    return nodeList;
}*/

Node *Graph::node(const std::string &node_name) const {
    if (!hasNode(node_name))
        return nullptr;
    return const_cast<Node *>(&(*_nodeSet.find(Node(node_name))));
}

bool Graph::addNode(const Node &_node) {
    if (hasNode(_node.name())) return false;
    max_size++;
    _nodeSet.insert(_node);
    _cachedNodeList.emplace_back(node(_node.name()));
    return true;
}

std::string Graph::nextNodeName() const {
    return std::string(1, 'a' + (max_size % 26)) + std::to_string(int(max_size / 26));
}

bool Graph::addNode(std::string node_name) {
    if (node_name.empty())
        node_name = nextNodeName();
    return addNode(Node(node_name));
}

bool Graph::removeNode(Node *node) {
    if (!hasNode(node))
        return false;
    isolateNode(node);
    _nodeSet.erase(*node);
    _cachedNodeList.remove(node);
    return true;
}

bool Graph::removeNode(const std::string &name) {
    return removeNode(node(name));
}

bool Graph::isolateNode(Node *node) {
    if (!hasNode(node))
        return false;
    for (auto &arc : arcList()) {
        if (arc.start() == node || arc.end() == node)
            removeArc(arc.start(), arc.end());
    }
    return true;
}

bool Graph::isolateNode(const std::string &name) {
    return isolateNode(node(name));
}

bool Graph::setNodeName(Node *node, const std::string &node_name) const {
    if (!hasNode(node) || hasNode(node_name) || node->name() == node_name)
        return false;
    node->setName(node_name);
    return true;
}

bool Graph::setNodeName(const std::string &old_name, const std::string &new_name) const {
    return setNodeName(node(old_name), new_name);
}

int Graph::weight(Node *u, Node *v) const {
    if (!hasArc(u, v))
        return INT_MAX;
    return _arcSet.at(std::make_pair(u, v));
}

int Graph::weight(const std::string &uname, const std::string &vname) const {
    return weight(node(uname), node(vname));
}

bool Graph::setArc(Node *const u, Node *const v, int w) {
    if (u == v || !hasNode(u) || !hasNode(v) || w <= 0 || w >= INT_MAX)
        return false;
    if (hasArc(u, v))
        _arcSet.at(std::make_pair(u, v)) = w;
    else {
        _arcSet.insert({std::make_pair(u, v), w});
        u->incPositiveDeg();
        v->incNegativeDeg();
    }
    return true;
}

bool Graph::setArc(const std::string &uname, const std::string &vname, int w) {
    return setArc(node(uname), node(vname), w);
}

bool Graph::removeArc(Node *const u, Node *const v) {
    if (u == v || !hasNode(u) || !hasNode(v))
        return false;
    if (hasArc(u, v)) {
        _arcSet.erase(std::make_pair(u, v));
        u->decPositiveDeg();
        v->decNegativeDeg();
        return true;
    }
    return false;
}

bool Graph::removeArc(const std::string &uname, const std::string &vname) {
    return removeArc(node(uname), node(vname));
}

Graph Graph::transpose() const {
    Graph graph(*this);
    graph.clearArcs();
    for (const auto &it : _arcSet)
        graph.setArc(it.first.second->name(), it.first.first->name(), it.second);
    return graph;
}

std::ostream &operator<<(std::ostream &os, const Graph &graph) {
    if (graph._nodeSet.empty()) {
        if (!graph._arcSet.empty())
            throw "Error in clearing _graph: _nodeSet is empty but _arcSet";
        else {
            os << "";
            return os;
        }
    }
    os << "\n";
    auto adj = graph.adjMatrix();
    auto nodeNum = adj.nodes.size();
    auto maxLength = 3;

    for (auto &node: adj.nodes)
        maxLength = std::max(maxLength, (int) node->name().length());
    for (auto &arc: graph.arcList())
        maxLength = std::max(maxLength, (int) std::to_string(arc.weight()).length());

    os << std::string(maxLength + 1, ' ');
    for (auto &node: adj.nodes)
        os << std::left << std::setw(maxLength) << node->name() << " ";
    os << "\n";
    for (int i = 0; i < nodeNum; i++) {
        os << std::left << std::setw(maxLength) << adj.nodes[i]->name() << " ";
        for (int j = 0; j < nodeNum; j++) {
            if (adj.matrix[i][j] != INT_MAX)
                os << std::left << std::setw(maxLength) << adj.matrix[i][j] << " ";
            else
                os << std::left << std::setw(maxLength) << "INF" << " ";
        }
        os << "\n";
    }
    return os;
}

bool Graph::hasNode(Node *node) const {
    if (!node) return false;
    return _nodeSet.find(*node) != _nodeSet.end();
}

