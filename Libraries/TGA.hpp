#pragma once


#include <vector>
#include <fstream>


#pragma pack(push,1)
struct TGAHeader
{
	uint8_t  id_length { };
	uint8_t  color_map_type { };
	uint8_t  data_type_code { };
	uint16_t color_map_origin { };
	uint16_t color_map_length { };
	uint8_t  color_map_depth { };
	uint16_t x_origin { };
	uint16_t y_origin { };
	uint16_t width { };
	uint16_t height { };
	uint8_t  bits_per_pixel { };
	uint8_t  image_descriptor { };
};
#pragma pack(pop)

struct TGAColor
{
	uint8_t BGRA[4] = {0, 0, 0, 0 };
	uint8_t bytes_per_pixel = {0};

	TGAColor() = default;

	TGAColor(const uint8_t Red, const uint8_t Green, const uint8_t Blue, const uint8_t Alpha = 255)
	: BGRA {Blue, Green, Red, Alpha}, bytes_per_pixel(4) { }

	explicit TGAColor(const uint8_t V)
	: BGRA {V, 0, 0, 0}, bytes_per_pixel(1) { }

	TGAColor(const uint8_t *Pointer, const uint8_t Bytes_per_pixel)
	: BGRA {0, 0, 0, 0}, bytes_per_pixel(Bytes_per_pixel)
	{
		for (int i = 0; i < Bytes_per_pixel; i++)
			BGRA[i] = Pointer[i];
	}

	uint8_t& operator[](const int I)
	{
		return BGRA[I];
	}

	TGAColor operator *(const double Intensity) const
	{
		TGAColor result = *this;
		double clamped = std::max(0., std::min(Intensity, 1.));

		for (int i = 0; i < 4; i++)
			result.BGRA[i] = BGRA[i] * clamped;

		return result;
	}
};

class TGAImage
{
protected:
	std::vector<uint8_t> data;
	int width;
	int height;
	int bytes_per_pixel;

	bool Load_RLE_data(std::ifstream &Input);
	bool Unload_RLE_data(std::ofstream &Output) const;

public:
	enum Format
	{
		GRAYSCALE=1,
		RGB=3,
		RGBA=4
	};

	TGAImage ();
	TGAImage (int Width, int Height, int Bytes_per_pixel);
	[[nodiscard]] TGAColor Get (int X, int Y) const;
	void Clear ();
	void Flip_vertically ();
	void Flip_horizontally ();
	void Scale (int Width, int Height);
	void Set (int X, int Y, const TGAColor &Color);
	bool Read_TGA_file (const std::string &Filename);
	[[nodiscard]] bool Write_TGA_file (const std::string &Filename, bool Flip_vertically = true, bool RLE = true) const;
	uint8_t *Buffer ();
	[[nodiscard]] int Get_width () const;
	[[nodiscard]] int Get_height () const;
	[[nodiscard]] int Get_bytes_per_pixel () const;
};