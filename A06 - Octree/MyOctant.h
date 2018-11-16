#ifndef __MYOCTANTCLASS_H_
#define __MYOCTANTCLASS_H_

#include "MyEntityManager.h"

namespace Simplex
{
	class MyOctant
	{
		MyOctant* root; //Root octant reference

		//Primitive variables
		int ID;
		int currentLevel;
		int children_count;
		float octant_size;
		static int octant_count;
		static int level_max;
		static int ideal_count;
		
		//Manager Instances
		MeshManager* mesh_manager;
		MyEntityManager* entity_manager;
		//-----------------

		//Lists and references
		vector3 center_point, min_point, max_point;
		MyOctant* parent_octant;
		MyOctant* children[8];
		std::vector<uint> entities;
		std::vector<MyOctant*> child_list;

	public:
		MyOctant(int p_maxLevel, int p_idealEntityCount);
		MyOctant(vector3 p_octCenter, float p_octSize);
		~MyOctant(void);

		void CreateOctant(void);
		void Display();
		void Subdivide(void);
		bool InSpace(int index);
		bool Exceeds(int numEntities);
		void Prune(void);
		void ResetEntities(void);
		void MakeOctree(int maxLevel);
		void Assign(void);
		void CreateBranch(void);
		MyOctant* GetChild(int numChild);
		int GetOctantCount();
	};
}
#endif