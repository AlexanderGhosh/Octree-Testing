#pragma once
#include <array>
#include <vector>
#include "BoundingBox.h"

struct Node {
	int id;
	std::vector<glm::vec3*> objects;
	std::list<Node*> children;
	BoundingBox box;
	Node* parent;

	Node() : children(), parent(0), box(), id(idCounter++), objects() { }

	bool AddChild(Node* tree) {
		if (ChildCount() == 8) {
			return false;
		}
		tree->parent = this;
		children.push_back(tree);
		return true;
	}

	void AddObject(glm::vec3& object) {
		objects.push_back(&object);
	}

	std::string ToString() {
		std::string childrenStr = "";
		for (Node* child : children) {
			childrenStr += child ? std::to_string(child->id) : " ";
			childrenStr += ", ";
		}
		childrenStr.pop_back();
		childrenStr.pop_back();
		return "[" + std::to_string(id) + ": (" + childrenStr + ")]";
	}

	int ChildCount() {
		return children.size();
	}
	inline static int idCounter = 1;
};