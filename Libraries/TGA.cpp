#include <cstring>
#include <iostream>


#include "TGA.hpp"


TGAImage::TGAImage ()
: data(), width(0), height(0), bytes_per_pixel(0) { }

TGAImage::TGAImage (const int Width, const int Height, const int Bytes_per_pixel)
: data(Width * Height * Bytes_per_pixel, 0), width(Width), height(Height), bytes_per_pixel(Bytes_per_pixel) { }

bool TGAImage::Read_TGA_file(const std::string &Filename)
{
	std::ifstream input;
	input.open (Filename, std::ios::binary);

	if (!input.is_open())
	{
		std::cerr << "Can'T open file " << Filename << "\n";
		input.close();
		return false;
	}

	TGAHeader header;

	input.read(reinterpret_cast<char *>(&header), sizeof(header));

	if (!input.good())
	{
		input.close();
		std::cerr << "An error occurred while reading the header\n";
		return false;
	}

	width = header.width;
	height = header.height;
	bytes_per_pixel = header.bits_per_pixel >> 3;

	if (width <= 0 || height <= 0 || (bytes_per_pixel != GRAYSCALE && bytes_per_pixel != RGB && bytes_per_pixel != RGBA))
	{
		input.close();
		std::cerr << "Bad bytes per pixel (or width/height) value\n";
		return false;
	}
	
	size_t quantity_bytes = bytes_per_pixel * width * height;
	data = std::vector<uint8_t>(quantity_bytes, 0);

	if (3 == header.data_type_code || 2 == header.data_type_code)
	{
		input.read(reinterpret_cast<char *>(data.data()), quantity_bytes);

		if (!input.good())
		{
			input.close();
			std::cerr << "An error occurred while reading the data\n";
			return false;
		}
	}
	else if (10 == header.data_type_code || 11 == header.data_type_code)
	{
		if (!Load_RLE_data(input))
		{
			input.close();
			std::cerr << "An error occurred while reading the data\n";
			return false;
		}
	}
	else
	{
		input.close();
		std::cerr << "Unknown file format " << (int)header.data_type_code << "\n";
		return false;
	}

	if (!(header.image_descriptor & 0x20))
		Flip_vertically();

	if (header.image_descriptor & 0x10)
		Flip_horizontally();

	std::cerr << width << "x" << height << "/" << bytes_per_pixel * 8 << "\n";
	input.close();
	return true;
}

bool TGAImage::Load_RLE_data(std::ifstream &Input)
{
	size_t pixel_quantity = width * height;
	size_t current_pixel = 0;
	size_t current_byte  = 0;
	TGAColor color_buffer;

	do
	{
		uint8_t chunk_header(0);
		chunk_header = Input.get();

		if (!Input.good())
		{
			std::cerr << "An error occurred while reading the data\n";
			return false;
		}

		if (chunk_header < 128)
		{
			chunk_header++;

			for (int i = 0; i < chunk_header; i++)
			{
				Input.read(reinterpret_cast<char *>(color_buffer.BGRA), bytes_per_pixel);

				if (!Input.good())
				{
					std::cerr << "An error occurred while reading the header\n";
					return false;
				}

				for (int t = 0; t < bytes_per_pixel; t++)
					data[current_byte++] = color_buffer.BGRA[t];

				current_pixel++;

				if (current_pixel > pixel_quantity)
				{
					std::cerr << "Too many pixels read\n";
					return false;
				}
			}
		}
		else
		{
			chunk_header -= 127;
			Input.read(reinterpret_cast<char *>(color_buffer.BGRA), bytes_per_pixel);

			if (!Input.good())
			{
				std::cerr << "An error occurred while reading the header\n";
				return false;
			}

			for (int i = 0; i < chunk_header; i++)
			{
				for (int t = 0; t < bytes_per_pixel; t++)
					data[current_byte++] = color_buffer.BGRA[t];

				current_pixel++;

				if (current_pixel > pixel_quantity)
				{
					std::cerr << "Too many pixels read\n";
					return false;
				}
			}
		}
	}
	while (current_pixel < pixel_quantity);
	return true;
}

bool TGAImage::Write_TGA_file (const std::string &Filename, const bool Flip_vertically, const bool RLE) const
{
	uint8_t developer_area[4] = { 0, 0, 0, 0 };
	uint8_t extension_area[4] = { 0, 0, 0, 0 };
	uint8_t footer[18] = {'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O',
					      'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'};
	std::ofstream output;

	output.open (Filename, std::ios::binary);

	if (!output.is_open())
	{
		std::cerr << "Can'T open file " << Filename << "\n";
		output.close();
		return false;
	}

	TGAHeader header;
	header.bits_per_pixel = bytes_per_pixel << 3;
	header.width  = width;
	header.height = height;
	header.data_type_code = (bytes_per_pixel == GRAYSCALE ? (RLE ? 11 : 3) : (RLE ? 10 : 2));
	header.image_descriptor = Flip_vertically ? 0x00 : 0x20;
	output.write(reinterpret_cast<const char *>(&header), sizeof(header));

	if (!output.good())
	{
		output.close();
		std::cerr << "Can'T dump the TGA file\n";
		return false;
	}

	if (!RLE)
	{
		output.write(reinterpret_cast<const char *>(data.data()), width * height * bytes_per_pixel);

		if (!output.good())
		{
			std::cerr << "Can'T unload raw data\n";
			output.close();
			return false;
		}
	}
	else
	{
		if (!Unload_RLE_data(output))
		{
			output.close();
			std::cerr << "Can'T unload RLE data\n";
			return false;
		}
	}

	output.write(reinterpret_cast<const char *>(developer_area), sizeof(developer_area));

	if (!output.good())
	{
		std::cerr << "Can'T dump the TGA file\n";
		output.close();
		return false;
	}

	output.write(reinterpret_cast<const char *>(extension_area), sizeof(extension_area));

	if (!output.good())
	{
		std::cerr << "Can'T dump the TGA file\n";
		output.close();
		return false;
	}

	output.write(reinterpret_cast<const char *>(footer), sizeof(footer));

	if (!output.good())
	{
		std::cerr << "Can'T dump the TGA file\n";
		output.close();
		return false;
	}

	output.close();
	return true;
}

bool TGAImage::Unload_RLE_data(std::ofstream &Output) const
{
	const uint8_t max_chunk_length = 128;
	size_t pixels_quantity = width * height;
	size_t current_pixel = 0;

	while (current_pixel < pixels_quantity)
	{
		size_t chunk_start = current_pixel * bytes_per_pixel;
		size_t current_byte = current_pixel * bytes_per_pixel;
		uint8_t run_length = 1;
		bool raw = true;

		while (current_pixel + run_length < pixels_quantity && run_length < max_chunk_length)
		{
			bool success_equality = true;

			for (int t = 0; success_equality && t < bytes_per_pixel; t++)
				success_equality = (data[current_byte + t] == data[current_byte + t + bytes_per_pixel]);

			current_byte += bytes_per_pixel;

			if (1 == run_length)
				raw = !success_equality;

			if (raw && success_equality)
			{
				run_length--;
				break;
			}

			if (!raw && !success_equality)
				break;

			run_length++;
		}

		current_pixel += run_length;
		Output.put(raw ? run_length - 1 : run_length + 127);

		if (!Output.good())
		{
			std::cerr << "Can'T dump the TGA file\n";
			return false;
		}

		Output.write(reinterpret_cast<const char *>(data.data() + chunk_start), (raw ? run_length * bytes_per_pixel : bytes_per_pixel));

		if (!Output.good())
		{
			std::cerr << "Can'T dump the TGA file\n";
			return false;
		}
	}
	return true;
}

TGAColor TGAImage::Get (const int X, const int Y) const
{
	if (!data.size() || X < 0 || Y < 0 || X >= width || Y >= height)
		return { };
	return TGAColor(data.data()+ (X + Y * width) * bytes_per_pixel, bytes_per_pixel);
}

void TGAImage::Set (const int X, const int Y, const TGAColor &Color)
{
	if (!data.size() || X < 0 || Y < 0 || X >= width || Y >= height)
		return;
	memcpy(data.data()+ (X + Y * width) * bytes_per_pixel, Color.BGRA, bytes_per_pixel);
}

[[nodiscard]] int TGAImage::Get_bytes_per_pixel () const
{
	return bytes_per_pixel;
}

[[nodiscard]] int TGAImage::Get_width () const
{
	return width;
}

[[nodiscard]] int TGAImage::Get_height () const
{
	return height;
}

void TGAImage::Flip_horizontally ()
{
	if (!data.size())
		return;

	int half = width >> 1;

	for (int i = 0; i < half; i++)
	{
		for (int j = 0; j < height; j++)
		{
			TGAColor first_color = Get(i, j);
			TGAColor second_color = Get(width - 1 - i, j);
			Set(i, j, second_color);
			Set(width - 1 - i, j, first_color);
		}
	}
}

void TGAImage::Flip_vertically ()
{
	if (!data.size())
		return;

	size_t bytes_per_line = width * bytes_per_pixel;
	std::vector<uint8_t> line(bytes_per_line, 0);
	int half = height >> 1;

	for (int j = 0; j < half; j++)
	{
		size_t first_line = j * bytes_per_line;
		size_t second_line = (height - 1 - j) * bytes_per_line;
		std::copy(data.begin() + first_line, data.begin() + first_line + bytes_per_line, line.begin());
		std::copy(data.begin() + second_line, data.begin() + second_line + bytes_per_line, data.begin() + first_line);
		std::copy(line.begin(), line.end(), data.begin() + second_line);
	}
}

uint8_t *TGAImage::Buffer ()
{
	return data.data();
}

void TGAImage::Clear ()
{
	data = std::vector<uint8_t>(width * height * bytes_per_pixel, 0);
}

void TGAImage::Scale (int Width, int Height)
{
	if (Width <= 0 || Height <= 0 || !data.size())
		return;

	std::vector<uint8_t> temp_data(Width * Height * bytes_per_pixel, 0);
	int n_scan_line = 0;
	int o_scan_line = 0;
	int err_y = 0;
	size_t n_line_bytes = Width * bytes_per_pixel;
	size_t o_line_bytes = width * bytes_per_pixel;

	for (int j = 0; j < height; j++)
	{
		int err_x = width - Width;
		int n_x = -bytes_per_pixel;
		int o_x = -bytes_per_pixel;

		for (int i=0; i<width; i++)
		{
			o_x += bytes_per_pixel;
			err_x += Width;

			while (err_x >= (int)width)
			{
				err_x -= width;
				n_x += bytes_per_pixel;
				memcpy(temp_data.data() + n_scan_line + n_x, data.data() + o_scan_line + o_x, bytes_per_pixel);
			}
		}

		err_y += Height;
		o_scan_line += o_line_bytes;

		while (err_y >= (int)height)
		{
			if (err_y >= (int)height << 1)
				memcpy(temp_data.data() + n_scan_line + n_line_bytes, temp_data.data() + n_scan_line, n_line_bytes);
			err_y -= height;
			n_scan_line += n_line_bytes;
		}
	}

	data = temp_data;
	width = Width;
	height = Height;
}