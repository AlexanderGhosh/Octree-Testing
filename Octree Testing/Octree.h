#pragma once
#include <array>
#include <vector>
#include "BoundingBox.h"

struct Octree {
	int id;
	std::list<Octree*> children;
	Octree* parent;
	BoundingBox box;
	std::vector<Vec3*> objects;

	Octree() : children(), parent(0), box(), id(idCounter++), objects() { }

	bool AddChild(Octree* tree) {
		if (ChildCount() == 8) {
			return false;
		}
		tree->parent = this;
		children.push_back(tree);
		return true;
	}

	void AddObject(Vec3& object) {
		objects.push_back(&object);
	}

	std::string ToString() {
		std::string childrenStr = "";
		for (Octree* child : children) {
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