#include "stdafx.h"
#include "AStar.h"

std::array<std::list<Node>, 8> kStageNodeBuffers;
std::vector<NodeConnector> kNodeConnectors;

void a_star::ConnectNodes(const std::vector<NodeConnector>& node_connectors)
{
    //모든 노드 커텍터를 순회하여 각 노드의 이웃을 연결한다.
    for (auto& node_connector : node_connectors)
    {
        auto& neighbors = node_connector.node->neighbors;
        const auto& neighbors_id = node_connector.neighbors_id;
        for (const int& id : neighbors_id)
        {
            auto it = std::find_if(node_connectors.begin(), node_connectors.end(), [&id](const NodeConnector& node_connector)
                {
                    return id == node_connector.node->id;
                });
            if (it != node_connectors.end())
            {
                neighbors.push_back(it->node);
            }
            else
            {
                std::wstring str = L"Do not find node's neighbor!! \nid: " + std::to_wstring(id) + L"\n";
                OutputDebugString(str.c_str());
            }
        }
    }
    OutputDebugString(L"Complate Node Connect! \n");

   /* for (int stage = 0; stage < 8; ++stage)
    {
        int count = 0;
        for (const auto& node : kStageNodeBuffers[stage])
        {
            std::wstring str = L"[NodeDump] Stage " + std::to_wstring(stage) + L" | ID: " + std::to_wstring(node.id) +
                L", Pos: (" + std::to_wstring(node.position.x) + L", " + std::to_wstring(node.position.y) + L", " + std::to_wstring(node.position.z) + L")\n";
            OutputDebugString(str.c_str());
            ++count;
        }
        OutputDebugString((L"[NodeDump] Stage " + std::to_wstring(stage) + L" Total Nodes: " + std::to_wstring(count) + L"\n").c_str());
    }*/

}

std::vector<Node*> a_star::AStar(Node* start, Node* goal)
{
    // 필수 초기화
    for (auto& stage_nodes : kStageNodeBuffers)
    {
        for (Node& node : stage_nodes)
        {
            node.gCost = FLT_MAX;
            node.hCost = 0.0f;
            node.parent = nullptr;
        }
    }

    auto heuristic = [](Node* a, Node* b) {
        XMVECTOR pa = XMLoadFloat3(&a->position);
        XMVECTOR pb = XMLoadFloat3(&b->position);
        XMVECTOR d = XMVectorSubtract(pa, pb);
        return XMVectorGetX(XMVector3Length(d)); // 유클리드 거리
        };

   /* std::priority_queue < Node*, std::vector<Node*>,
        decltype([](Node* a, Node* b) { return a->fCost() > b->fCost(); }) > openSet;*/

    std::priority_queue<Node*, std::vector<Node*>, CompareNode> openSet;

    start->gCost = 0.0f;
    start->hCost = heuristic(start, goal);
    openSet.push(start);

    std::unordered_set<Node*> closedSet;

    while (!openSet.empty())
    {
        Node* current = openSet.top();
        openSet.pop();

        if (current == goal)
        {
            // 경로 복원
            std::vector<Node*> path;
            while (current)
            {
                path.push_back(current);
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        closedSet.insert(current);

        for (Node* neighbor : current->neighbors)
        {
            if (closedSet.count(neighbor))
                continue;

            float tentativeG = current->gCost + heuristic(current, neighbor);

            if (tentativeG < neighbor->gCost)
            {
                neighbor->parent = current;
                neighbor->gCost = tentativeG;
                neighbor->hCost = heuristic(neighbor, goal);
                openSet.push(neighbor);
            }
        }
    }

    return {}; // 경로 없음
}