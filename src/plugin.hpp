#pragma once
#include <rack.hpp>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;
// Declare each Model, defined in each module source file
extern Model* model_macrow;
extern Model* model_fiveStep;
extern Model* modelRectify;

template <typename TBase = GrayModuleLightWidget>
struct YellowRedLight : TBase {
	YellowRedLight() {
		this->addBaseColor(SCHEME_YELLOW);
		this->addBaseColor(SCHEME_RED);
	}
};