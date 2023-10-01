#include "tgaimage.h"
#include <iostream>
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const int pix_width = 800;
const int pix_height = 800;
struct pixel
{
	int x, y = 0;
	pixel(int x, int y)
	{
		this->x = x; this->y = y;
	}
};
void draw_line(const pixel& p1, const pixel& p2, TGAImage& ofile, const TGAColor& line_color);
using std::vector;
int main(int argc, char** argv) {
	TGAImage image(pix_width, pix_height, TGAImage::RGB);
	Model african_head = Model("C:/Users/msi/Desktop/african_head.obj");
	for (int i = 0; i < african_head.nfaces(); i++)
	{
		vector<int> face = african_head.face(i);
		for (int j = 0; j < 3; j++)
		{
			Vec3f temp_v0 = african_head.vert(face[j]);
			Vec3f temp_v1 = african_head.vert(face[(j+1)%3]);
			pixel p0{ static_cast<int>(((temp_v0.x + 1.0) / 2.0) * pix_width), static_cast<int>(((temp_v0.y + 1.0) / 2.0) * pix_height) };
			pixel p1{static_cast<int>( ((temp_v1.x + 1.0) / 2.0) * pix_width), static_cast<int>(((temp_v1.y + 1.0) / 2.0) * pix_height) };
			draw_line(p0, p1, image, white);
		}
	} 
	image.flip_vertically(); //origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}
//issue where lines are "diverging" SOLVED	
void draw_line(const pixel& p1, const pixel& p2, TGAImage& ofile, const TGAColor& line_color)
{
	int x0 = p1.x;
	int x1 = p2.x;
	int y0 = p1.y;
	int y1 = p2.y;

	bool steep = false;
	if (std::abs(x1 - x0) < std::abs(y0 - y1))	//if height more than width
	{
		//old implementation had x1-x0 without abs(),
		//but since this is the first statement now, we are not sure that x1>x0,
		//and as such have to use abs()
		std::swap(x0, y0), std::swap(x1, y1);	//trasnpose image
		steep = true;
	}
	if (x0 > x1)	//swap if x0 is to the right of x1
	{
		//#CAUTION 
		//executing this swapping operation before the tranpose operation above
		//introduces logic issues such as not rendering p(100, 700) to p(200,200)
		//this issue will be investigated further 
		std::swap(x0, x1), std::swap(y0, y1);
	}
	int y = y0;
	int dy = y1 - y0;
	int dx = x1 - x0;
	int derror = std::abs(2*dy);
	int error = 0;
	if (steep)
		for (int x = x0; x <= x1; x++)
		{
			ofile.set(y, x, line_color);
			error += derror;
			if (error > dx)
			{
				y += y1 > y0 ? 1 : -1;
				error -= 2 * dx;
			}
		}
	else
		for (int x = x0; x <= x1; x++)
		{
			ofile.set(x, y, line_color);
			error += derror;
			if (error > dx)
			{
				y += y1 > y0 ? 1 : -1;
				error -= 2 * dx;
			}
		}
}