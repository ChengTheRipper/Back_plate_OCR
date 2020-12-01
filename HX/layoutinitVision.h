#pragma once
#ifndef _LAYOUT_INIT_H_
#define _LAYOUT_INIT_H_

#include "CLayout.h"

void InitLayoutVision(CLayout& layout, CWnd* parent)
{
	layout.Initial(parent);
	layout.RegisterControl(IDC_VISION_BUTTON_LOG, CLayout::e_stretch_all);
	layout.RegisterControl(IDC_VISION_BUTTON_EXIT, CLayout::e_stretch_all);
	
}

#endif