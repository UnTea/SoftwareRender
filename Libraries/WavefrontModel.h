#pragma once


#include <vector>


#include "MyGeometry.h"


class Model
{
	private:
	std::vector<VectorThreeFloat> verts;
	std::vector<std::vector<int> > faces;

	public:
	Model (const char *Filename);
	~Model ();
	int n_verts ();
	int n_faces ();
	VectorThreeFloat vert (int Index);
	std::vector<int> face (int Index);
};