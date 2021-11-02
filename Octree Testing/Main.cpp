#include <iostream>
#include <list>

#include "Timer.h"
#include "Octree.h"

#define MAX_OBJECTS 8
#define MIN_OBJECTS 1
#define MAX_CHILDREN 8
#define WORLD_SPACE_X 5
#define WORLD_SPACE_Y 5
#define WORLD_SPACE_Z 5

#define MAX_RECERSIVE_DEPTH 10
#define MAX_ITTERATIONS 1000000

using namespace std;

list<Octree> trees;

float randRange(float min, float max) {
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

std::array<BoundingBox, MAX_CHILDREN> Subdivide(BoundingBox& box) {
	std::array<BoundingBox, MAX_CHILDREN> res{};
	const Vec3 length = box.Length();
	const float half_x = length.x * 0.5f;
	const float half_y = length.y * 0.5f;
	const float half_z = length.z * 0.5f;

	const Vec3 half(half_x, half_y, half_z);

	const BoundingBox translated = box - box.min;

	const BoundingBox tr1 = translated * 0.5f + half;
	const BoundingBox tl1 = tr1 + Vec3(-half_x, 0, 0);
	const BoundingBox tr2 = tr1 + Vec3(0, 0, -half_z);
	const BoundingBox tl2 = tr1 + Vec3(-half_x, 0, -half_z);

	const BoundingBox br1 = translated * 0.5f + Vec3(half_x, 0, half_z);
	const BoundingBox bl1 = br1 + Vec3(-half_x, 0, 0);
	const BoundingBox br2 = br1 + Vec3(0, 0, -half_z);
	const BoundingBox bl2 = br1 + Vec3(-half_x, 0, -half_z);

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

Octree* CreateNode() {
	return &trees.emplace_back();
}

int recursion = 0;
void BuildTree(Octree* node) {
	// max reccusion depth reached
	if (recursion >= MAX_RECERSIVE_DEPTH) {
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

	recursion++;

	// subdivide the bounding box
	auto subdivisions = Subdivide(node->box);
	// create children
	for (int i = 0; i < MAX_CHILDREN; i++) {
		Octree* child = CreateNode();
		child->box = subdivisions[i];
		node->AddChild(child);
	}

	// test objects against children
	for (int i = 0; i < MAX_CHILDREN; i++) {
		Octree* child = node->children[i];
		for (auto itt = node->objects.begin(); itt != node->objects.end();) {
			Vec3& obj = *itt;
			if (child->box.Contains(obj)) {
				child->AddObject(obj);
				itt = node->objects.erase(itt);
			}
			else {
				itt++;
			}
		}
	}
	// recersive on each child
	for (int i = 0; i < MAX_CHILDREN; i++) {
		Octree* child = node->children[i];
		BuildTree(child);
	}
	
}

void main() {
	srand(0);
	vector<Vec3> objects;
	objects.reserve(MAX_OBJECTS);
	objects.resize(MAX_OBJECTS);
	trees.resize(1);
	// generate random objects within the world space
	for (int i = 0; i < MAX_OBJECTS; i++) {
		objects[i] = Vec3(
			randRange(-WORLD_SPACE_X, WORLD_SPACE_X),
			randRange(-WORLD_SPACE_Y, WORLD_SPACE_Y),
			randRange(-WORLD_SPACE_Z, WORLD_SPACE_Z));
	}

	Vec3 worldMax(WORLD_SPACE_X, WORLD_SPACE_Y, WORLD_SPACE_Z);
	Vec3 worldMin(-WORLD_SPACE_X, -WORLD_SPACE_Y, -WORLD_SPACE_Z);
	
	BoundingBox worldBox(worldMax, worldMin);
	auto subDivisions = Subdivide(worldBox);

	Octree* root = &trees.front();
	root->box = worldBox;
	root->objects = objects;

	float total = 0;

	Timer timer;
	for (int i = 0; i < MAX_ITTERATIONS; i++)
	{
		timer.start();
		BuildTree(root);
		Octree::idCounter = 1;
		trees.clear();
		trees.emplace_back();
		root = &trees.front();
		root->box = worldBox;
		root->objects = objects;
		timer.stop();
		total += timer.getDuration(0);
	}
	total /= (float)MAX_ITTERATIONS;
	cout << to_string(total);
}