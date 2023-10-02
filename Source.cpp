#include "tgaimage.h"
#include <iostream>
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const int pix_width = 2000;
const int pix_height = 2000;
struct pixel
{
	int x, y = 0;
	pixel(int x, int y)
	{
		this->x = x; this->y = y;
	}
};

void draw_line(const pixel& p1, const pixel& p2, TGAImage& ofile, const TGAColor& line_color);
void draw_triangle_fill(const pixel& p0, const pixel& p1, const pixel& p2, TGAImage& image, const TGAColor& color);
void draw_triangle_wireframe(const pixel& p0, const pixel& p1, const pixel& p2, TGAImage& image, const TGAColor& color);
using std::vector;
using std::swap;
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
void draw_triangle_wireframe(const pixel& p0, const pixel& p1, const pixel& p2, TGAImage& image, const TGAColor& color)
{
	draw_line(p0, p1, image, color);
	draw_line(p1, p2, image, color);
	draw_line(p2, p0, image, color);
}
void draw_triangle_fill(const pixel& t0, const pixel& t1, const pixel& t2, TGAImage& image, const TGAColor& color)
{	
	pixel p0 = t0, p1 = t1, p2 = t2;
	//sort points by y value in ascending order, i.e. p0.y<p1.y<p2.y
	if (p0.y > p1.y)
		swap(p0, p1);
	if (p0.y > p2.y)
		swap(p0, p2);
	if (p1.y > p2.y)
		swap(p1, p2);

	int total_height = p2.y - p0.y;		//no need for abs()
	int segment_height;
	//render partial triangle then fill by drawing lines between the emergent points 
	//lower half
	segment_height = p1.y - p0.y;
	for (int y = p0.y; y <= p1.y; y++)
	{	
		//track ratio of partial line to segment with alpha, and to total ratio with beta
		float alpha, beta;
		alpha = (float)(y-p0.y) / (segment_height+1);	//+1 to ensure no division by zero occurs
		beta = (float)(y - p0.y) / total_height;
		pixel A{ p0.x + (static_cast<int>((alpha) * (p1.x - p0.x))), y };
		pixel B{ p0.x + static_cast<int>((beta) * (p2.x - p0.x)),y};
		draw_line(A, B, image, color);
	}
	//upper half
	segment_height = p2.y - p1.y;
	for (int y = p1.y; y <= p2.y; y++)
	{
		//track ratio of partial line to segment with alpha, and to total ratio with beta
		float alpha, beta;
		alpha = (float)(y - p1.y) / (segment_height + 1); //+1 to ensure no division by zero occurs
		beta = (float)(y - p0.y) / (total_height + 1);
		pixel A{ p1.x + static_cast<int>((alpha) * (p2.x - p1.x)), y };
		pixel B{ p0.x + static_cast<int>((beta) * (p2.x - p0.x)), y };
		draw_line(A, B, image, color);
	}
}
