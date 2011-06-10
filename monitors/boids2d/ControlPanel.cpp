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


ControlPanel::ControlPanel()
{
	// Metric types
	Button *b = new Button(4, 75, 20, 20, QUAD);
	b->slowActivationApproach = true;
	b->SetCallback(&ControlPanel::CpuBoidButtonCallback);
	b->ButtonClick();

	b = new Button(28, 75, 20, 20, TRIANGLE);
	b->slowActivationApproach = true;
	b->SetCallback(&ControlPanel::MemoryBoidButtonCallback);

	b = new Button(52, 75, 20, 20, DIAMOND);
	b->slowActivationApproach = true;
	b->SetCallback(&ControlPanel::NetworkBoidButtonCallback);

	b = new Button(76, 75, 20, 20, POLYGON);
	b->slowActivationApproach = true;
	b->SetCallback(&ControlPanel::StorageBoidButtonCallback);

	// Data views


	// Configuration
	b = new Button(4, 4, 20, 20);
	b->slowActivationApproach = false;
	b->SetCallback(&ControlPanel::BoidTailCallback);

	b = new Button(28, 4, 20, 20);
	b->slowActivationApproach = true;
	b->SetCallback(&ControlPanel::BoidTailCallback);

	b = new Button(52, 4, 20, 20);
	_font = new Font(4);
}

ControlPanel::~ControlPanel()
{
}

void ControlPanel::OnLoop()
{
}

void ControlPanel::OnRender()
{
	glColor3f(0, 0, 1);
	_font->RenderText("Metric Types", 50, 99, true, true);
	_font->RenderText("CPU", 14, 73, true, true);
	_font->RenderText("Memory", 38, 73, true, true);
	_font->RenderText("Network", 62, 73, true, true);
	_font->RenderText("Storage", 86, 73, true, true);

	_font->RenderText("Data Views", 50, 68, true, true);

	_font->RenderText("Configuration", 50, 38, true, true);
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


