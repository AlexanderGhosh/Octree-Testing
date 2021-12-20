#pragma once
#include <list>

#include "Node.h"
#include <stack>

#define MAX_CHILDREN 8
#define MIN_OBJECTS 1
#define MAX_RECERSIVE_DEPTH 1000

#define MIN_BOX_LENGTH_X 0.5
#define MIN_BOX_LENGTH_Y 0.5
#define MIN_BOX_LENGTH_Z 0.5

class Octree {
    int nodesExplored = 0;

    std::array<BoundingBox, MAX_CHILDREN> Subdivide(BoundingBox& box) {
        std::array<BoundingBox, MAX_CHILDREN> res{};
        const glm::vec3 length = box.Length();
        const float half_x = length.x * 0.5f;
        const float half_y = length.y * 0.5f;
        const float half_z = length.z * 0.5f;

        const glm::vec3 half(half_x, half_y, half_z);

        const BoundingBox translated = box - box.min;

        const BoundingBox tr1 = translated * 0.5f + half;
        const BoundingBox tl1 = tr1 + glm::vec3(-half_x, 0, 0);
        const BoundingBox tr2 = tr1 + glm::vec3(0, 0, -half_z);
        const BoundingBox tl2 = tr1 + glm::vec3(-half_x, 0, -half_z);

        const BoundingBox br1 = translated * 0.5f + glm::vec3(half_x, 0, half_z);
        const BoundingBox bl1 = br1 + glm::vec3(-half_x, 0, 0);
        const BoundingBox br2 = br1 + glm::vec3(0, 0, -half_z);
        const BoundingBox bl2 = br1 + glm::vec3(-half_x, 0, -half_z);

        res[0] = tr1 + box.min;
        res[1] = tl1 + box.min;
        res[2] = tr2 + box.min;
        res[3] = tl2 + box.min;

        res[4] = br1 + box.min;
        res[5] = bl1 + box.min;
        res[6] = br2 + box.min;
        res[7] = bl2 + box.min;

        return res;
    }

    Node* CreateNode(BoundingBox& bb) {
        nodes.emplace_back();
        nodes.back().box = bb;
        return &nodes.back();
    }
public:
    std::list<Node> nodes;
    Octree() : nodesExplored(0), nodes() {
        Reset();
    }
    void BuildTreeRecersive(Node* node) {
        // max reccusion depth reached
        if (nodesExplored >= MAX_RECERSIVE_DEPTH) {
            std::cout << "reccusion depth met\n";
            return;
        }
        // node doesn't exist
        if (!node) {
            return;
        }
        // already subdivided enough
        if (node->objects.size() <= MIN_OBJECTS) {
            return;
        }

        nodesExplored++;

        // subdivide the bounding box
        auto subdivisions = Subdivide(node->box);
        // create children as needed and check bounding
        for (auto& box : subdivisions) {
            auto len = box.Length();
            if (glm::any(glm::lessThanEqual(box.Length(), 
                glm::vec3(MIN_BOX_LENGTH_X, MIN_BOX_LENGTH_Y, MIN_BOX_LENGTH_Z)))) {
                std::cout << "skiped subdivison\n";
                break; // not contiune because all cudes should be the same dimentions
            }
            Node* child = nullptr;
            for (auto itt = node->objects.begin(); itt != node->objects.end();) {
                glm::vec3& obj = **itt;
                if (box.Contains(obj)) {
                    // create child and add to parent
                    if (!child) {
                        child = CreateNode(box);
                        node->AddChild(child);
                    }
                    child->AddObject(obj);
                    itt = node->objects.erase(itt);
                }
                else {
                    itt++;
                }
            }
        }
        // recersive on each child
        for (auto itt = node->children.begin(); itt != node->children.end(); itt++) {
            Node* child = *itt;
            BuildTreeRecersive(child);
        }

    }

    void BuildTreeItterative(Node* node) {
        // node doesn't exist
        if (!node) {
            return;
        }
        std::stack<Node*> s;
        s.push(node);

        while(!s.empty()) {
            Node* n = s.top();
            s.pop();
            // already subdivided enough
            if (n->objects.size() <= MIN_OBJECTS) {
                continue;
            }
            auto subdivisions = Subdivide(n->box);
            for (auto box : subdivisions) {

                // if box has reached the min size
                auto len = box.Length();
                if (glm::all(glm::lessThan(len,
                    glm::vec3(MIN_BOX_LENGTH_X, MIN_BOX_LENGTH_Y, MIN_BOX_LENGTH_Z)))) {
                    break;
                }

                Node* child = nullptr;
                for (auto itt = n->objects.begin(); itt != n->objects.end();) {
                    auto& o = **itt;
                    if (box.Contains(o)) {
                        if (!child) {
                            child = CreateNode(box);
                            s.push(child);
                            n->AddChild(child);
                        }
                        child->AddObject(o);
                        itt = n->objects.erase(itt);
                        continue;
                    }
                    itt++;
                }

            }
        }
    }

    Node* CreateTree(BoundingBox& enclosingSpace) {
        return CreateNode(enclosingSpace);
    }

    Node* GetRoot() {
        return &nodes.front();
    }

    void Reset() {
        Node::idCounter = 0;
        nodes.clear();
        nodesExplored = 0;
    }

    std::vector<BoundingBox> GetBBs() const {
        std::vector<BoundingBox> bbs{};
        bbs.reserve(nodes.size());
        for (auto& node : nodes) {
            bbs.push_back(node.box);
        }
        return bbs;
    }

    std::vector<Node*> GetIntersectionRecersive(Ray ray, Node* node) {
        glm::vec3 hit;
        if (!node->box.Intersects(ray, hit)) {
            return {};
        }
        std::vector<Node*> res{};

        res.push_back(node);
        node->box.hit = true;

        for (auto& child : node->children) {
            auto intersection = GetIntersectionRecersive(ray, child);
            res.insert(res.end(), intersection.begin(), intersection.end());
        }

        return res;
    }

    std::vector<Node*> GetIntersectionItterative(Ray ray, Node* node) {
        std::vector<Node*> intersections;
        std::stack<Node*> s;
        int maxStack = 0;
        Node* curr = node;
        s.push(curr);

        while (!s.empty()) {
            maxStack = glm::max((int)s.size(), maxStack);
            curr = s.top();
            s.pop();

            glm::vec3 hitPoint;
            if (curr->box.Intersects(ray, hitPoint)) {
                curr->box.hit = true;
                intersections.push_back(curr);
                for (auto& child : curr->children) {
                    s.push(child);
                }
            }
        }
        return intersections;
    }
};