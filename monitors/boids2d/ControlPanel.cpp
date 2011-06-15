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
	Button *b = new Button(8, 75, 15, 15, QUAD);
	b->SetCallback(&CpuBoidButtonCallback);
	b->ButtonClick();

	b = new Button(31, 75, 15, 15, TRIANGLE);
	b->SetCallback(&MemoryBoidButtonCallback);

	b = new Button(54, 75, 15, 15, DIAMOND);
	b->SetCallback(&NetworkBoidButtonCallback);

	b = new Button(77, 75, 15, 15, POLYGON);
	b->SetCallback(&StorageBoidButtonCallback);

	// Data views
	b = new Button(8, 45, 15, 15, QUAD);
	b->SetCallback(&CoreViewButtonCallback);

	b = new Button(31, 45, 15, 15, QUAD);
	b->SetCallback(&ProcessViewButtonCallback);
	b->ButtonClick();

	b = new Button(54, 45, 15, 15, QUAD);
	b->SetCallback(&ProcessNameViewButtonCallback);

	b = new Button(77, 45, 15, 15, QUAD);
	b->SetCallback(&StorageBoidButtonCallback);

	// Configuration
	b = new Button(4, 4, 20, 20, QUAD, false);
	b->SetCallback(&BoidTailCallback);

	b = new Button(28, 4, 20, 20);
	b->SetCallback(&BoidTailCallback);

	b = new Button(52, 4, 20, 20);
	_font = new Font(4, true, true);
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
	_font->RenderText("Metric Types", 50, 95);
	_font->RenderText("CPU", 15.5, 72);
	_font->RenderText("Memory", 38.5, 72);
	_font->RenderText("Network", 61.5, 72);
	_font->RenderText("Storage", 84.5, 72);

	_font->RenderText("Data Views", 50, 65);
	_font->RenderText("Core", 15.5, 42);
	_font->RenderText("Process", 38.5, 42);
	_font->RenderText("Process Name", 61.5, 42);
	_font->RenderText("Node", 84.5, 42);

	_font->RenderText("Configuration", 50, 35);
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

void ControlPanel::CoreViewButtonCallback()
{
	BoidSharedContext::boidTypeToShow = BOID_TYPE_CORE;
}

void ControlPanel::ProcessViewButtonCallback()
{
	BoidSharedContext::boidTypeToShow = BOID_TYPE_PROCESS;
}

void ControlPanel::ProcessNameViewButtonCallback()
{
	BoidSharedContext::boidTypeToShow = BOID_TYPE_PROCESS_NAME;
}

void ControlPanel::NodeViewButtonCallback()
{
	BoidSharedContext::boidTypeToShow = BOID_TYPE_NODE;
}


