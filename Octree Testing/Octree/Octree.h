#pragma once
#include <list>

#include "Node.h"

#define MAX_CHILDREN 8
#define MIN_OBJECTS 1
#define MAX_RECERSIVE_DEPTH 10

class Octree {
    int nodesExplored = 0;
	std::list<Node> nodes;

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
    Octree() : nodesExplored(0), nodes() {
        Reset();
    }
    void BuildTree(Node* node) {
        // max reccusion depth reached
        if (nodesExplored >= MAX_RECERSIVE_DEPTH) {
            return;
        }
        // node doesnt exist
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
        int i = -1;
        for (auto& box : subdivisions) {
            i++;
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
            BuildTree(child);
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
};