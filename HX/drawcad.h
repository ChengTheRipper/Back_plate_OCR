#pragma once

#include <cstdio>
#include "pch.h"
#include "framework.h"
#include "CcadDlg.h"
#include "afxdialogex.h"

#include <windows.h>
#include <afxwin.h>
#include <memory>

using namespace std;
using namespace Gdiplus;
struct point4w
{
	int sx;
	int sy;
	int ex;
	int ey;
};

struct actual_path
{
	point4w real_path[50];
	unsigned int len;
	unsigned int width;
	unsigned int height;
	int rsx; // relative start.x
	int rsy;  // relative start,y
};




void draw_init(CcadDlg* this_ptr, const int& con_id, std::shared_ptr<Graphics>& g_ptr, CRect& rect);
void read_cad(actual_path* apath);
void draw_cad(Graphics& graphics, const CRect& rect, const actual_path& cad_data);


class m_mfc_drawer
{
public:
	m_mfc_drawer(const CcadDlg* dlg, const int& con_id);

	void draw_line(const ARGB& color, const float& width, const Point& p1, const Point& p2);
	void draw_line(const ARGB& color, const float& width, const int& x1, const int& y1, const int& x2, const int& y2);
	void draw_line_offset(const double& width_max, const double& height_max, const int& x1, const int& y1, const int& x2, const int& y2);
	bool draw_line_animated();


	void clear();

	void set_color(const ARGB& color);
	void set_width(const float& width);

	void img_flip();


	actual_path a1;


private:

	m_mfc_drawer(const m_mfc_drawer&);
	m_mfc_drawer& operator= (const m_mfc_drawer&);

	const CcadDlg* win_ptr;
	int con_id;
	Graphics graphics;
	CRect rect;

	ARGB pen_color;
	float m_width;

	//待输出的路径信息
};

