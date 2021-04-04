#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>


#include "WavefrontModel.h"


Model::Model(const char *Filename)
: verts(), faces()
{
	std::ifstream input;
	input.open (Filename, std::ifstream::in);

	if (input.fail())
		return;

	std::string line;

	while (!input.eof())
	{
		std::getline(input, line);
		std::istringstream iss(line.c_str());
		char trash;

		if (!line.compare(0, 2, "v "))
		{
			iss >> trash;
			VectorThreeFloat v;

			for (int i = 0; i < 3;i++)
				iss >> v.raw[i];

			verts.push_back(v);
		}
		else if (!line.compare(0, 2, "f "))
		{
			std::vector<int> Float;
			int i_trash, index;
			iss >> trash;

			while (iss >> index >> trash >> i_trash >> trash >> i_trash)
			{
				index--;
				Float.push_back(index);
			}

			faces.push_back(Float);
		}
	}
	std::cerr << "# v# " << verts.size() << " f# "  << faces.size() << std::endl;
}

Model::~Model() = default;

int Model::n_verts()
{
	return (int)verts.size();
}

int Model::n_faces()
{
	return (int)faces.size();
}

std::vector<int> Model::face(int Index)
{
	return faces[Index];
}

VectorThreeFloat Model::vert(int Index)
{
	return verts[Index];
}