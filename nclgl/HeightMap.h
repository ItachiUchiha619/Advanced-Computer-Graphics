#pragma once
#include  <string>
#include "Mesh.h"
#include <vector>
class HeightMap: public  Mesh
{
public:
	HeightMap(const  std::string& name); 
	~HeightMap(void) {};
	Vector3  GetHeightmapSize()  const { 
		return  heightmapSize; 
	}
	vector<Vector3> GetRandPos() {
		return randPositions; 
	}
	
protected:
	Vector3  heightmapSize;
	vector<Vector3> randPositions;
};

