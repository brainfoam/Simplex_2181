#include "MyOctant.h"

using namespace Simplex;

//Creates a new octant (for root instantiation)
Simplex::MyOctant::MyOctant(int level_max, int ideal_count)
{
	CreateOctant(); //Sets up octant variables
	octant_count = 0;
	this->level_max = level_max;
	this->ideal_count = ideal_count;
	ID = octant_count;

	root = this;
	child_list.clear();

	int entityCount = entity_manager->GetEntityCount();
	std::vector<vector3> minMaxList;
	for (auto i = 0; i < entityCount; i++)
	{
		MyRigidBody* octant_body = entity_manager->GetEntity(i)->GetRigidBody();
		minMaxList.push_back(octant_body->GetCenterGlobal());
		minMaxList.push_back(octant_body->GetCenterGlobal());
	}
	MyRigidBody* new_octant_body = new MyRigidBody(minMaxList);
	minMaxList.clear();

	vector3 halfWidth = new_octant_body->GetHalfWidth();
	float max_dist = halfWidth.x;
	for (auto i = 0; i < 3; i++)
		if (max_dist < halfWidth[i])
			max_dist = halfWidth[i];

	center_point = new_octant_body->GetCenterLocal();
	SafeDelete(new_octant_body);

	//Set min and max points for the root
	octant_size = max_dist * 2;
	min_point = center_point - (vector3(max_dist));
	max_point = center_point + (vector3(max_dist));

	octant_count++;
	MakeOctree(level_max);
}

//Creates a new octant (for subdivision instantiation)
Simplex::MyOctant::MyOctant(vector3 center, float size)
{
	CreateOctant(); //Sets up octant variables
	octant_count++;
	center_point = center;
	octant_size = size;
	//Get the new octant's min and max points
	min_point = center_point - (vector3(octant_size)) / 2;
	max_point = center_point + (vector3(octant_size)) / 2;
}

//Deletes the octant, and prunes all branches from the root recursively.
Simplex::MyOctant::~MyOctant() 
{ 
	if (this == root) //If this is the root, prune all of its branches.
		Prune();

	child_list.clear(); //Nullify lists
	entities.clear();
	octant_size = 0;
	children_count = 0;
}

//Set up the new octant's variables
void Simplex::MyOctant::CreateOctant()
{
	root = nullptr;
	parent_octant = nullptr;
	children_count = 0;
	octant_size = 0.0f;
	ID = octant_count;
	octant_count++;

	mesh_manager = MeshManager::GetInstance();
	entity_manager = MyEntityManager::GetInstance();

	center_point =
		min_point =
		max_point = vector3(0.0f);

	for (auto i = 0; i < 8; i++)
		children[i] = nullptr;
}

//Set the octant's dimensions to a new wire cube mesh, and add it to the render list
void Simplex::MyOctant::Display()
{
	mesh_manager->AddWireCubeToRenderList
	(
		glm::translate(IDENTITY_M4, center_point) * glm::scale(vector3(octant_size)), //Translation * scale matrix
		vector3(0), //Color in black (0, 0, 0)
		RENDER_WIRE //Render type
	);

	for (auto i = 0; i < children_count; i++)
		children[i]->Display(); //Display all children octants, if they exist
}

//Subdivide an octant into 8 smaller octants
void Simplex::MyOctant::Subdivide()
{
	//Only subdivide if it has not been subdivided yet
	if (children_count == 0 && currentLevel < level_max)
	{
		children_count = 8;
		float octant_size_new = octant_size / 4.0f;
		float octant_dimension = octant_size_new * 2.0f;

		vector3 sub_center = center_point;
		sub_center.x -= octant_size_new;
		sub_center.y -= octant_size_new;
		sub_center.z -= octant_size_new;

		//Create new octants in each corner of the current octant, from the subdivision center
		children[0] = new MyOctant(sub_center, octant_dimension); //0-0-0
		sub_center.x += octant_dimension;
		children[1] = new MyOctant(sub_center, octant_dimension); //d-0-0
		sub_center.z += octant_dimension;
		children[2] = new MyOctant(sub_center, octant_dimension); //d-0-d
		sub_center.x -= octant_dimension;
		children[3] = new MyOctant(sub_center, octant_dimension); //0-0-d
		sub_center.y += octant_dimension;
		children[4] = new MyOctant(sub_center, octant_dimension); //0-d-d
		sub_center.z -= octant_dimension;
		children[5] = new MyOctant(sub_center, octant_dimension); //0-d-0
		sub_center.x += octant_dimension;
		children[6] = new MyOctant(sub_center, octant_dimension); //d-d-0
		sub_center.z += octant_dimension;
		children[7] = new MyOctant(sub_center, octant_dimension); //d-d-d

		for (auto i = 0; i < 8; i++)
		{
			children[i]->parent_octant = this; //Set the parent of the new octants to the current octant
			children[i]->currentLevel = currentLevel + 1;
			children[i]->root = root; //Redistribute the root instance
			if (children[i]->Exceeds(ideal_count))
				children[i]->Subdivide(); //Subdivide the children again if their entities are above the ideal count
		}
	}
}

//Checks to see if an entity "index" is within the octant space
bool Simplex::MyOctant::InSpace(int index)
{
	uint objectCount = entity_manager->GetEntityCount();

	if (index >= objectCount)
		return false;

	MyRigidBody* entity = entity_manager->GetEntity(index)->GetRigidBody();
	vector3 entity_center = entity->GetCenterGlobal();
	vector3 entity_half_width = entity->GetHalfWidth();

	if (max_point.x < entity_center.x + entity_half_width.x || min_point.x > entity_center.x - entity_half_width.x) return false;
	if (max_point.y < entity_center.y + entity_half_width.y || min_point.y > entity_center.y - entity_half_width.y) return false;
	if (max_point.z < entity_center.z + entity_half_width.z || min_point.z > entity_center.z - entity_half_width.z) return false;
		
	return true;
}

//Checks to see if the octant's entities exceeds a certain limit of entities (ideal count, ideally)
bool Simplex::MyOctant::Exceeds(int entities)
{
	int count = 0;
	for (int i = 0; i < entity_manager->GetEntityCount(); i++)
	{
		if (InSpace(i))
			count++;
		if (count > entities)
			return true;
	}
	return false;
}

//Prunes the current branch, getting rid of all children below it
void Simplex::MyOctant::Prune(void)
{
	if (children_count != 0)
	{
		for (int i = 0; i < children_count; i++)
		{
			children[i]->Prune();
			delete children[i];
			children[i] = nullptr;
		}
		children_count = 0;
	}
}

//Clears the entitiy list of the current octant and all it's children
void Simplex::MyOctant::ResetEntities(void)
{
	entities.clear();
	for (auto i = 0; i < children_count; i++)
		children[i]->ResetEntities();
}

//re-instantiates a clean octree
void Simplex::MyOctant::MakeOctree(int p_maxLevel)
{
	if (currentLevel == 0)
	{
		level_max = p_maxLevel;
		octant_count = 1;
		entities.clear();
		child_list.clear();
		Prune();

		if (Exceeds(ideal_count))
			Subdivide();

		Assign();
		CreateBranch();
	}
}

//Assigns new entity dimensions per octant
void Simplex::MyOctant::Assign(void)
{
	for (auto i = 0; i < children_count; i++)
		children[i]->Assign();

	if (children_count == 0)
	{
		for (auto i = 0; i < entity_manager->GetEntityCount(); i++)
		{
			if (InSpace(i))
			{
				entities.push_back(i);
				entity_manager->AddDimension(i, ID);
			}
		}
	}
}

//Creates the octant's branch
void Simplex::MyOctant::CreateBranch(void)
{
	if (entities.size() > 0)
		root->child_list.push_back(this);

	for (int i = 0; i < children_count; i++)
		children[i]->CreateBranch();
}

#pragma region GETTERS/SETTERS

int MyOctant::octant_count = 0; //How many octants there currently are
int MyOctant::ideal_count = 5;  //The ideal entity count for spacial optimization
int MyOctant::level_max = 3;    //The maximum octant level

//Returns how many octants are currently in the scene
int Simplex::MyOctant::GetOctantCount(void)
{
	return octant_count;
}

//Returns an octant based on its index
MyOctant * Simplex::MyOctant::GetChild(int numChild)
{
	if (numChild <= 7)
		return children[numChild];
	else
		return nullptr;
}

#pragma endregion