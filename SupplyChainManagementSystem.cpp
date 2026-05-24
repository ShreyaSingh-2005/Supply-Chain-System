#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <map>
#include <set>
#include <algorithm>
#include <climits>
#include <numeric>
#include <string>

using namespace std;

//  DSU
class DSU {
private:
    vector<int> parent, size;

public:
    DSU(int n = 0) {
        parent.resize(n);
        size.assign(n, 1);
        iota(parent.begin(), parent.end(), 0);
    }

    void reset(int n) {
        parent.resize(n);
        size.assign(n, 1);
        iota(parent.begin(), parent.end(), 0);
    }

    int findParent(int node) {
        if (parent[node] == node) return node;
        return parent[node] = findParent(parent[node]);
    }

    void unionBySize(int u, int v) {
        int pu = findParent(u);
        int pv = findParent(v);

        if (pu == pv) return;

        if (size[pu] < size[pv]) {
            parent[pu] = pv;
            size[pv] += size[pu];
        } else {
            parent[pv] = pu;
            size[pu] += size[pv];
        }
    }

    bool isConnected(int u, int v) {
        return findParent(u) == findParent(v);
    }
};

struct Edge {
    int to;
    int cost;
};

class SupplyChainSystem {
private:
    int n;
    vector<string> nodeNames;
    unordered_map<string, int> nodeId;
    vector<vector<Edge>> graph;

    vector<tuple<int, int, int>> allRoutes;
    vector<pair<int, int>> dependencies;

public:
    SupplyChainSystem() {
        n = 0;
    }

    void initializeSystem(int nodes) {
        n = nodes;
        nodeNames.assign(n, "");
        graph.assign(n, {});
        nodeId.clear();
        allRoutes.clear();
        dependencies.clear();
    }

    bool isInitialized() const {
        return n > 0;
    }

    int getNodeCount() const {
        return n;
    }

    void setNodeName(int id, const string &name) {
        if (id < 0 || id >= n) {
            cout << "Invalid node id.\n";
            return;
        }

        if (nodeId.find(name) != nodeId.end()) {
            cout << "Node name already exists.\n";
            return;
        }

        nodeNames[id] = name;
        nodeId[name] = id;
    }

    int getNodeId(const string &name) const {
        auto it = nodeId.find(name);
        if (it == nodeId.end()) return -1;
        return it->second;
    }

    bool allNodesAssigned() const {
        for (const string &name : nodeNames) {
            if (name.empty()) return false;
        }
        return true;
    }

    void displayNodes() const {
        if (!isInitialized()) {
            cout << "System not initialized.\n";
            return;
        }

        cout << "\nList of Nodes:\n";
        for (int i = 0; i < n; i++) {
            cout << i << " -> " << nodeNames[i] << "\n";
        }
    }

    void addRoute(const string &from, const string &to, int cost) {
        int u = getNodeId(from);
        int v = getNodeId(to);

        if (u == -1 || v == -1) {
            cout << "Invalid node names.\n";
            return;
        }

        graph[u].push_back({v, cost});
        graph[v].push_back({u, cost});
        allRoutes.push_back({u, v, cost});

        cout << "Route added successfully.\n";
    }

    void displayRoutes() const {
        if (allRoutes.empty()) {
            cout << "No routes available.\n";
            return;
        }

        cout << "\nAvailable Routes:\n";
        for (auto &route : allRoutes) {
            int u, v, cost;
            tie(u, v, cost) = route;
            cout << nodeNames[u] << " <-> " << nodeNames[v] << " | Cost = " << cost << "\n";
        }
    }

    void addDependency(const string &prerequisite, const string &dependent) {
        int u = getNodeId(prerequisite);
        int v = getNodeId(dependent);

        if (u == -1 || v == -1) {
            cout << "Invalid node names.\n";
            return;
        }

        dependencies.push_back({u, v});
        cout << "Dependency added successfully.\n";
    }

    void displayDependencies() const {
        if (dependencies.empty()) {
            cout << "No dependencies available.\n";
            return;
        }

        cout << "\nDependencies:\n";
        for (auto &dep : dependencies) {
            cout << nodeNames[dep.first] << " -> " << nodeNames[dep.second] << "\n";
        }
    }

    void resolveDependencies() {
        if (dependencies.empty()) {
            cout << "No dependencies to resolve.\n";
            return;
        }

        vector<vector<int>> dependencyGraph(n);
        vector<int> indegree(n, 0);

        for (auto &dep : dependencies) {
            int u = dep.first;
            int v = dep.second;
            dependencyGraph[u].push_back(v);
            indegree[v]++;
        }

        queue<int> q;
        vector<int> topoOrder;

        for (int i = 0; i < n; i++) {
            if (indegree[i] == 0) q.push(i);
        }

        while (!q.empty()) {
            int node = q.front();
            q.pop();
            topoOrder.push_back(node);

            for (int next : dependencyGraph[node]) {
                indegree[next]--;
                if (indegree[next] == 0) {
                    q.push(next);
                }
            }
        }

        if ((int)topoOrder.size() != n) {
            cout << "Cycle detected. Valid dependency order is not possible.\n";
            return;
        }

        cout << "\nTopological Order:\n";
        for (int id : topoOrder) {
            cout << nodeNames[id] << " ";
        }
        cout << "\n";
    }

    void findOptimalRoute(const string &source, const string &destination) {
        int src = getNodeId(source);
        int dest = getNodeId(destination);

        if (src == -1 || dest == -1) {
            cout << "Invalid source or destination.\n";
            return;
        }

        vector<int> dist(n, INT_MAX);
        vector<int> parent(n, -1);

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

        dist[src] = 0;
        pq.push({0, src});

        while (!pq.empty()) {
            auto current = pq.top();
            pq.pop();

            int currDist = current.first;
            int node = current.second;

            if (currDist > dist[node]) continue;

            for (auto &edge : graph[node]) {
                int next = edge.to;
                int wt = edge.cost;

                if (dist[node] != INT_MAX && dist[node] + wt < dist[next]) {
                    dist[next] = dist[node] + wt;
                    parent[next] = node;
                    pq.push({dist[next], next});
                }
            }
        }

        if (dist[dest] == INT_MAX) {
            cout << "No route exists between " << source << " and " << destination << ".\n";
            return;
        }

        vector<int> path;
        int curr = dest;
        while (curr != -1) {
            path.push_back(curr);
            curr = parent[curr];
        }
        reverse(path.begin(), path.end());

        cout << "\nOptimal Route:\n";
        for (int i = 0; i < (int)path.size(); i++) {
            cout << nodeNames[path[i]];
            if (i + 1 < (int)path.size()) cout << " -> ";
        }
        cout << "\nMinimum Cost = " << dist[dest] << "\n";
    }

    bool canDeliverWithinThreshold(int src, int dest, int threshold) {
        vector<int> visited(n, 0);
        queue<int> q;

        q.push(src);
        visited[src] = 1;

        while (!q.empty()) {
            int node = q.front();
            q.pop();

            if (node == dest) return true;

            for (auto &edge : graph[node]) {
                if (!visited[edge.to] && edge.cost <= threshold) {
                    visited[edge.to] = 1;
                    q.push(edge.to);
                }
            }
        }
        return false;
    }

    void minimumThresholdForDelivery(const string &source, const string &destination) {
        int src = getNodeId(source);
        int dest = getNodeId(destination);

        if (src == -1 || dest == -1) {
            cout << "Invalid source or destination.\n";
            return;
        }

        if (allRoutes.empty()) {
            cout << "No routes available.\n";
            return;
        }

        int low = 0, high = 0;
        for (auto &route : allRoutes) {
            high = max(high, get<2>(route));
        }

        int ans = -1;

        while (low <= high) {
            int mid = low + (high - low) / 2;

            if (canDeliverWithinThreshold(src, dest, mid)) {
                ans = mid;
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }

        if (ans == -1) {
            cout << "Delivery not possible.\n";
        } else {
            cout << "Minimum threshold cost required = " << ans << "\n";
        }
    }

    void analyzeConnectivity() {
        if (!isInitialized()) {
            cout << "System not initialized.\n";
            return;
        }

        DSU dsu(n);
        for (auto &route : allRoutes) {
            int u, v, cost;
            tie(u, v, cost) = route;
            dsu.unionBySize(u, v);
        }

        map<int, vector<string>> components;
        for (int i = 0; i < n; i++) {
            components[dsu.findParent(i)].push_back(nodeNames[i]);
        }

        cout << "\nConnected Components:\n";
        for (auto &comp : components) {
            for (auto &name : comp.second) {
                cout << name << " ";
            }
            cout << "\n";
        }
    }

    void simulateRouteFailure(const string &from, const string &to) {
        int u = getNodeId(from);
        int v = getNodeId(to);

        if (u == -1 || v == -1) {
            cout << "Invalid node names.\n";
            return;
        }

        DSU dsu(n);

        for (auto &route : allRoutes) {
            int a, b, cost;
            tie(a, b, cost) = route;

            if ((a == u && b == v) || (a == v && b == u)) {
                continue;
            }
            dsu.unionBySize(a, b);
        }

        cout << "\nAfter failure of route " << from << " <-> " << to << ":\n";
        if (dsu.isConnected(u, v)) {
            cout << "Alternative connectivity exists in the network.\n";
        } else {
            cout << "This failure disconnects part of the network.\n";
        }
    }

    void shipmentCostAnalysis(const vector<int> &dailyCosts) {
        if (dailyCosts.empty()) {
            cout << "No daily costs provided.\n";
            return;
        }

        vector<int> prefix(dailyCosts.size());
        prefix[0] = dailyCosts[0];

        for (int i = 1; i < (int)dailyCosts.size(); i++) {
            prefix[i] = prefix[i - 1] + dailyCosts[i];
        }

        cout << "\nDaily Costs: ";
        for (int x : dailyCosts) cout << x << " ";
        cout << "\nPrefix Sum: ";
        for (int x : prefix) cout << x << " ";
        cout << "\n";

        int l, r;
        cout << "Enter range [l r] to find cumulative cost: ";
        cin >> l >> r;

        if (l < 0 || r >= (int)dailyCosts.size() || l > r) {
            cout << "Invalid range.\n";
            return;
        }

        int sum = prefix[r] - (l > 0 ? prefix[l - 1] : 0);
        cout << "Cumulative cost from day " << l << " to day " << r << " = " << sum << "\n";
    }

    void maxCostInKDays(const vector<int> &dailyCosts, int k) {
        int m = dailyCosts.size();
        if (m == 0 || k <= 0 || k > m) {
            cout << "Invalid input for sliding window.\n";
            return;
        }

        int windowSum = 0;
        for (int i = 0; i < k; i++) {
            windowSum += dailyCosts[i];
        }

        int maxSum = windowSum;
        int startIndex = 0;

        for (int i = k; i < m; i++) {
            windowSum += dailyCosts[i] - dailyCosts[i - k];
            if (windowSum > maxSum) {
                maxSum = windowSum;
                startIndex = i - k + 1;
            }
        }

        cout << "Maximum shipment cost in " << k << " consecutive days = " << maxSum << "\n";
        cout << "Best window starts from day index " << startIndex << "\n";
    }
};

void showMenu() {
    cout << "\n================ SUPPLY CHAIN ROUTE OPTIMIZATION SYSTEM ================\n";
    cout << "1. Initialize System\n";
    cout << "2. Display Nodes\n";
    cout << "3. Add Route\n";
    cout << "4. Display Routes\n";
    cout << "5. Add Dependency\n";
    cout << "6. Display Dependencies\n";
    cout << "7. Resolve Dependencies \n"; //Topological Sort
    cout << "8. Find Optimal Route \n"; //Dijkstra
    cout << "9. Minimum Threshold for Delivery \n"; //Binary Search
    cout << "10. Analyze Connectivity \n"; //DSU
    cout << "11. Simulate Route Failure\n";
    cout << "12. Shipment Cost Analysis \n"; //Prefix Sum
    cout << "13. Maximum Cost in K Days \n"; //Sliding Window
    cout << "14. Load Sample Data\n";
    cout << "0. Exit\n";
    cout << "Enter your choice: ";
}

void loadSampleData(SupplyChainSystem &system) {
    system.initializeSystem(6);

    system.setNodeName(0, "SupplierA");
    system.setNodeName(1, "WarehouseX");
    system.setNodeName(2, "WarehouseY");
    system.setNodeName(3, "HubZ");
    system.setNodeName(4, "DeliveryP");
    system.setNodeName(5, "DeliveryQ");

    system.addRoute("SupplierA", "WarehouseX", 4);
    system.addRoute("SupplierA", "WarehouseY", 2);
    system.addRoute("WarehouseX", "HubZ", 5);
    system.addRoute("WarehouseY", "HubZ", 8);
    system.addRoute("HubZ", "DeliveryP", 3);
    system.addRoute("HubZ", "DeliveryQ", 6);
    system.addRoute("WarehouseX", "DeliveryQ", 10);

    system.addDependency("SupplierA", "WarehouseX");
    system.addDependency("SupplierA", "WarehouseY");
    system.addDependency("WarehouseX", "HubZ");
    system.addDependency("WarehouseY", "HubZ");
    system.addDependency("HubZ", "DeliveryP");
    system.addDependency("HubZ", "DeliveryQ");

    cout << "Sample data loaded successfully.\n";
}


int main() {
    SupplyChainSystem system;
    int choice;

    do {
        showMenu();
        cin >> choice;

        switch (choice) {
            case 1: {
                int nodes;
                cout << "Enter number of nodes: ";
                cin >> nodes;

                system.initializeSystem(nodes);

                cout << "Enter names of " << nodes << " nodes:\n";
                cin.ignore();

                for (int i = 0; i < nodes; i++) {
                    string name;
                    cout << "Node " << i << ": ";
                    getline(cin, name);
                    system.setNodeName(i, name);
                }

                cout << "System initialized successfully.\n";
                break;
            }

            case 2: {
                system.displayNodes();
                break;
            }

            case 3: {
                string from, to;
                int cost;
                cin.ignore();
                cout << "Enter source node: ";
                getline(cin, from);
                cout << "Enter destination node: ";
                getline(cin, to);
                cout << "Enter route cost: ";
                cin >> cost;
                system.addRoute(from, to, cost);
                break;
            }

            case 4: {
                system.displayRoutes();
                break;
            }

            case 5: {
                string pre, dep;
                cin.ignore();
                cout << "Enter prerequisite node: ";
                getline(cin, pre);
                cout << "Enter dependent node: ";
                getline(cin, dep);
                system.addDependency(pre, dep);
                break;
            }

            case 6: {
                system.displayDependencies();
                break;
            }

            case 7: {
                system.resolveDependencies();
                break;
            }

            case 8: {
                string src, dest;
                cin.ignore();
                cout << "Enter source node: ";
                getline(cin, src);
                cout << "Enter destination node: ";
                getline(cin, dest);
                system.findOptimalRoute(src, dest);
                break;
            }

            case 9: {
                string src, dest;
                cin.ignore();
                cout << "Enter source node: ";
                getline(cin, src);
                cout << "Enter destination node: ";
                getline(cin, dest);
                system.minimumThresholdForDelivery(src, dest);
                break;
            }

            case 10: {
                system.analyzeConnectivity();
                break;
            }

            case 11: {
                string from, to;
                cin.ignore();
                cout << "Enter first node of failed route: ";
                getline(cin, from);
                cout << "Enter second node of failed route: ";
                getline(cin, to);
                system.simulateRouteFailure(from, to);
                break;
            }

            case 12: {
                int days;
                cout << "Enter number of days: ";
                cin >> days;

                vector<int> costs(days);
                cout << "Enter daily shipment costs:\n";
                for (int i = 0; i < days; i++) {
                    cin >> costs[i];
                }

                system.shipmentCostAnalysis(costs);
                break;
            }

            case 13: {
                int days, k;
                cout << "Enter number of days: ";
                cin >> days;

                vector<int> costs(days);
                cout << "Enter daily shipment costs:\n";
                for (int i = 0; i < days; i++) {
                    cin >> costs[i];
                }

                cout << "Enter window size k: ";
                cin >> k;
                system.maxCostInKDays(costs, k);
                break;
            }

            case 14: {
                loadSampleData(system);
                break;
            }

            case 0: {
                cout << "Exiting program...\n";
                break;
            }

            default: {
                cout << "Invalid choice. Try again.\n";
            }
        }

    } while (choice != 0);

    return 0;
}
