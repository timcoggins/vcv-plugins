#include "plugin.hpp"


struct fiveStep : Module {
	enum ParamIds {
		STEPS_PARAM,
		ENUMS(LEVEL_PARAMS, 5),
		NUM_PARAMS,
	};
	enum InputIds {
		CLOCK_INPUT,
		RESET_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		ENUMS(OUT_OUTPUTS, 1),
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(CHANNEL_LIGHTS, 5 * 2),
		NUM_LIGHTS
	};

	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger resetTrigger;
	int index = 0;
	dsp::ClockDivider lightDivider;
	dsp::SlewLimiter clickFilters[4];

	fiveStep() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configSwitch(STEPS_PARAM, 0.0, 2.0, 2.0, "Steps", {"3", "4", "5"});
		configInput(CLOCK_INPUT, "Clock");
		configInput(RESET_INPUT, "Reset");

		for (int i = 0; i < 5; i++) {
			configParam(LEVEL_PARAMS + i, -1.f, 1.f, 0.f, string::f("value %d", 1), "%", 0, 100);
        }
		
		configOutput(OUT_OUTPUTS + 0, "Main");
		lightDivider.setDivision(512);
	}

	void process(const ProcessArgs& args) override {
		// Determine current index
		if (clockTrigger.process(rescale(inputs[CLOCK_INPUT].getVoltage(), 0.1f, 2.f, 0.f, 1.f))) {
			index++;
		}
		if (resetTrigger.process(rescale(inputs[RESET_INPUT].getVoltage(), 0.1f, 2.f, 0.f, 1.f))) {
			index = 0;
		}
		int length = 3 + (int) std::round(params[STEPS_PARAM].getValue());
		if (index >= length)
			index = 0;

		int channels = 1;


        float out[16] = {};
		for (int c = 0; c < channels; c++) {
			float in = params[LEVEL_PARAMS + index].getValue();
			out[c] += in * 10.0;
		}

        // Set output
        outputs[OUT_OUTPUTS + 0].setChannels(channels);
        outputs[OUT_OUTPUTS + 0].writeVoltages(out);

		// Set lights
		if (lightDivider.process()) {
			for (int i = 0; i < 5; i++) {
				lights[CHANNEL_LIGHTS + 2 * i + 0].setBrightness(index == i);
				lights[CHANNEL_LIGHTS + 2 * i + 1].setBrightness(i >= length);
			}
		}
	}
};


struct fiveStepWidget : ModuleWidget {
	fiveStepWidget(fiveStep* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/fiveStep.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<CKSSThreeHorizontal>(mm2px(Vec(7.8, 20.942)), module, fiveStep::STEPS_PARAM));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.8, 66.379)), module, fiveStep::LEVEL_PARAMS + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.8, 76.539)), module, fiveStep::LEVEL_PARAMS + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.8, 86.699)), module, fiveStep::LEVEL_PARAMS + 2));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.8, 96.859)), module, fiveStep::LEVEL_PARAMS + 3));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.8, 107.000)), module, fiveStep::LEVEL_PARAMS + 4));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.8, 33.831)), module, fiveStep::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.8, 50.126)), module, fiveStep::RESET_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.8, 120.115)), module, fiveStep::OUT_OUTPUTS + 0));

		addChild(createLightCentered<TinyLight<YellowRedLight<>>>(mm2px(Vec(11.526, 63.259)), module, fiveStep::CHANNEL_LIGHTS + 2 * 0));
		addChild(createLightCentered<TinyLight<YellowRedLight<>>>(mm2px(Vec(11.526, 72.795)), module, fiveStep::CHANNEL_LIGHTS + 2 * 1));
		addChild(createLightCentered<TinyLight<YellowRedLight<>>>(mm2px(Vec(11.526, 82.955)), module, fiveStep::CHANNEL_LIGHTS + 2 * 2));
		addChild(createLightCentered<TinyLight<YellowRedLight<>>>(mm2px(Vec(11.526, 93.115)), module, fiveStep::CHANNEL_LIGHTS + 2 * 3));
		addChild(createLightCentered<TinyLight<YellowRedLight<>>>(mm2px(Vec(11.526, 103.300)), module, fiveStep::CHANNEL_LIGHTS + 2 * 4));
	}
};


Model* model_fiveStep = createModel<fiveStep, fiveStepWidget>("fiveStep");
