#ifndef _LAYOUT_INIT_H_
#define _LAYOUT_INIT_H_

#include "CLayout.h"

void InitLayoutModbus(CLayout& layout, CWnd* parent)
{
	layout.Initial(parent);
	layout.RegisterControl(IDC_COMBO1, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_COMBO2, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_COMBO3, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_COMBO4, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_COMBO5, CLayout::e_stretch_all);

	//layout.RegisterControl(IDC_STATIC1, CLayout::e_stretch_all);
	//layout.RegisterControl(IDC_STATIC2, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_STATIC3, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_STATIC4, CLayout::e_stretch_all);

	//´®¿ÚÑ¡Ôñ¿ò
	layout.RegisterControl(IDC_STATIC5, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_STATIC6, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_STATIC7, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_STATIC8, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_STATIC9, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_STATIC10, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_STATIC11, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_STATIC12, CLayout::e_stretch_all);

	//layout.RegisterControl(IDC_LIST1, CLayout::e_stretch_all);
	//layout.RegisterControl(IDC_LIST2, CLayout::e_stretch_all);

	layout.RegisterControl(IDC_EDIT1, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_EDIT2, CLayout::e_stretch_all);

	layout.RegisterControl(IDC_BUTTON1, CLayout::e_stretch_all);
	
	layout.RegisterControl(IDC_BUTTON_OPEN, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_BUTTON_SEND_ONCE, CLayout::e_stretch_all);


	//layout.RegisterControl(IDC_BUTTON3, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_BUTTON_CLEAN, CLayout::e_stretch_all);


}

#endif