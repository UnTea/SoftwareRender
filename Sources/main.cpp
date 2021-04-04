#include "TGA.hpp"
#include "MyGeometry.h"
#include "WavefrontModel.h"

// 4k
// const int image_width(4096);
// const int image_height(3072);

// 1k
const int image_width(1000);
const int image_height(1000);

Model *model = nullptr;

const TGAColor red {TGAColor (255, 0, 0, 255)};
const TGAColor green {TGAColor (0, 255, 0, 255)};
const TGAColor blue {TGAColor (0, 0, 255, 255)};
const TGAColor white {TGAColor (255, 255, 255, 255)};
const TGAColor pink {TGAColor (255, 0, 128, 255)};

void Line (int x_0, int y_0, int x_1, int y_1, TGAImage &Image, TGAColor Color)
{
	bool steep = false;

	if (std::abs(x_0 - x_1) < std::abs(y_0 - y_1))
	{
		std::swap(x_0, y_0);
		std::swap(x_1, y_1);
		steep = true;
	}

	if (x_0 > x_1)
	{
		std::swap(x_0, x_1);
		std::swap(y_0, y_1);
	}

	int d_x = x_1 - x_0;
	int d_y = y_1 - y_0;
	int d_error = std::abs(d_y) * 2;
	int error = 0;
	int y = y_0;

	for (int x = x_0; x <= x_1; x++)
	{
		if (steep)
		{
			Image.Set(y, x, Color);
		}
		else
		{
			Image.Set(x, y, Color);
		}
		error += d_error;

		if (error > d_x)
		{
			y += (y_1 > y_0 ? 1 : -1);
			error -= d_x * 2;
		}
	}
}


int main ()
{
	//model = new Model("../WavefrontObjects/bunny.obj");
	model = new Model("../WavefrontObjects/teapot.obj");

	TGAImage image(image_width, image_height, TGAImage::RGB);

	for (int i = 0; i < model->n_faces(); i++)
	{
		std::vector<int> face = model->face(i);
		for (int j = 0; j < 3; j++)
		{
			VectorThreeFloat v_0 = model->vert(face[j]);
			VectorThreeFloat v_1 = model->vert(face[(j + 1) % 3]);
			int x_0 = (v_0.x + 1.) * image_width / 2.;
			int y_0 = (v_0.y + 1.) * image_height / 2.;
			int x_1 = (v_1.x + 1.) * image_width / 2.;
			int y_1 = (v_1.y + 1.) * image_height / 2.;
			Line(x_0, y_0, x_1, y_1, image, pink);
		}
	}

	//image.Flip_vertically();
	image.Flip_horizontally();
	image.Write_TGA_file("../Output/teapot.tga");

	delete model;
	model = nullptr;

	return 0;
}
