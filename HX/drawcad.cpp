#include "pch.h"
#include "drawcad.h"


void draw_init(CcadDlg* this_ptr, const int& con_id, std::shared_ptr<Graphics>& g_ptr, CRect& rect)
{
	CWnd* pic; pic =  this_ptr->GetDlgItem(con_id);    // 用此句，得到图片控件的CWnd，图片将被绘制在控件上，IDC_PIC_VIEW为picture control的控件ID
	this_ptr->GetDlgItem(con_id)->GetClientRect(&rect);
	std::shared_ptr<Graphics> p(new Graphics(pic->GetDC()->m_hDC) );
	g_ptr = p;
}

void read_cad( actual_path* apath)
{
	const char* FILE_PATH = "path.cof";

	FILE* fp;
	fopen_s(&fp, "path.cof", "rb");
	if (fp == nullptr)
	{
		printf("error open file!\n");
		FreeConsole();
		exit(-1);
	}

	fread( apath, sizeof(actual_path), 1, fp);
	fclose(fp);

}
void draw_cad( Graphics& graphics, const CRect& rect, const actual_path& cad_data)
{
	using namespace Gdiplus;

	Gdiplus::Pen pen(Gdiplus::Color((ARGB)Gdiplus::Color::Black ), 2);
	Gdiplus::Point p1(0, 0);
	Gdiplus::Point p2(30, 60);

	graphics.DrawLine(&pen, p1, p2);
	Sleep(2000);
	graphics.Clear(Gdiplus::Color::White);
}

m_mfc_drawer::m_mfc_drawer(const CcadDlg* dlg, const int& _con_id) : 
	win_ptr(dlg) , con_id(_con_id), graphics(win_ptr->GetDlgItem(con_id)->GetDC()->m_hDC)
{
	win_ptr->GetDlgItem(con_id)->GetClientRect(&rect);
	pen_color = Color::Black;
	m_width = 2.0f;
}

void m_mfc_drawer::draw_line(const ARGB& color, const float& width, const Point& p1, const Point& p2)
{
	Pen pen(Color(color), width);
	graphics.DrawLine(&pen, p1, p2);

}

void m_mfc_drawer::draw_line(const ARGB& color, const float& width, const int& x1, const int& y1, const int& x2, const int& y2)
{
	Point p1(x1, y1);
	Point p2(x2, y2);
	draw_line(color, width, p1, p2);
}

void m_mfc_drawer::draw_line_offset(const double& width_max, const double& height_max, const int& x1, const int& y1, const int& x2, const int& y2)
{

	const float scale_x = (rect.Width() * 0.8f) / width_max;
	const float scale_y = (rect.Height() * 0.8f) / height_max;

	Point offset(rect.Width() / 10, rect.Height() / 10);

	Point p1(x1 * scale_x, y1 * scale_y);
	Point p2(x2 * scale_x, y2 * scale_y);
	draw_line(pen_color, m_width, p1 + offset, p2 + offset);
}

bool m_mfc_drawer::draw_line_animated()
{
	static size_t i = 0;
	if (i < a1.len)
	{
		if (i % 2)
		{
			pen_color = Color::Black;
		}
		else
		{
			pen_color = Color::Red;
		}
		int tmp_sx = 0, tmp_sy = 0, tmp_ex = 0,tmp_ey = 0;
		tmp_sx = a1.real_path[i].sx - a1.rsx;
		tmp_sy = abs(a1.real_path[i].sy - int(a1.height) - a1.rsy);
		tmp_ex = a1.real_path[i].ex - a1.rsx;
		tmp_ey = abs(a1.real_path[i].ey - int(a1.height) - a1.rsy);
		printf("sx: %d, sy: %d, ex: %d, ey: %d\n", tmp_sx, tmp_sy, tmp_ex, tmp_ey);
		draw_line_offset((double)a1.width, (double)a1.height, tmp_sx, tmp_sy, tmp_ex, tmp_ey);
		i++;
	}
	else
	{
		i = 0;
		return false;
	}
	return true;

}

void m_mfc_drawer::clear()
{
	graphics.Clear(Color::White);
}

void m_mfc_drawer::set_color(const ARGB& color)
{
	pen_color = color;
}

void m_mfc_drawer::set_width(const float& width)
{
	m_width = width;
}

void m_mfc_drawer::img_flip()
{
	
}