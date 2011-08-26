/*
 * BoidDescription.cpp
 *
 *  Created on: May 29, 2011
 *      Author: arild
 */

#include "Button.h"
#include "BoidSharedContext.h"
#include "Scene.h"
#include "ControlPanel.h"
#include <glog/logging.h>


void ControlPanel::OnInit()
{
	// Metric types
	Button *b = new Button(8, 170, 15, 15, QUAD);
	b->SetCallback(&CpuBoidButtonCallback);
	b->ButtonClick();

	b = new Button(31, 170, 15, 15, TRIANGLE);
	b->SetCallback(&MemoryBoidButtonCallback);
	b->ButtonClick();

	b = new Button(54, 170, 15, 15, DIAMOND);
	b->SetCallback(&NetworkBoidButtonCallback);

	b = new Button(77, 170, 15, 15, POLYGON);
	b->SetCallback(&StorageBoidButtonCallback);

	// Configuration
	b = new Button(8, 90, 15, 15);
	b->SetCallback(&BoidTailCallback);

	_font = new Font(4, true, true);
	_fontLarge = new Font(5, true, true);
}

void ControlPanel::OnLoop()
{
}

void ControlPanel::OnRender()
{
	glColor3ub(0, 100, 0);
	_font->RenderText("CPU", 15.5, 168);
	_font->RenderText("Memory", 38.5, 168);
	_font->RenderText("Network", 61.5, 168);
	_font->RenderText("Storage", 84.5, 168);

	_font->RenderText("Tail", 15.5, 88);

	glColor3f(1, 1, 1);
	_fontLarge->RenderText("Metric Types", 50, 192);
	_fontLarge->RenderText("Configuration", 50, 112);
}

void ControlPanel::OnCleanup()
{
}

void ControlPanel::CpuBoidButtonCallback()
{
	BoidSharedContext::showCpuBoid == true ? BoidSharedContext::showCpuBoid = false
			: BoidSharedContext::showCpuBoid = true;
}

void ControlPanel::MemoryBoidButtonCallback()
{
	BoidSharedContext::showMemoryBoid == true ? BoidSharedContext::showMemoryBoid = false
			: BoidSharedContext::showMemoryBoid = true;
}

void ControlPanel::NetworkBoidButtonCallback()
{
	BoidSharedContext::showNetworkBoid == true ? BoidSharedContext::showNetworkBoid = false
			: BoidSharedContext::showNetworkBoid = true;
}

void ControlPanel::StorageBoidButtonCallback()
{
	BoidSharedContext::showStorageBoid == true ? BoidSharedContext::showStorageBoid = false
			: BoidSharedContext::showStorageBoid = true;
}

void ControlPanel::BoidTailCallback()
{
	BoidSharedContext::tailLength == 0 ? BoidSharedContext::tailLength = 75
			: BoidSharedContext::tailLength = 0;
}


