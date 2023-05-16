#include "plugin.hpp"


struct _macrow : Module {
	enum ParamIds {
		ENUMS(GAIN_PARAMS, 6),
		MACRO_PARAMS,
		NUM_PARAMS
	};
	enum InputIds {
		ENUMS(IN_INPUTS, 6),
		NUM_INPUTS
	};
	enum OutputIds {
		ENUMS(OUT_OUTPUTS, 6),
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	dsp::ClockDivider paramDivider;

	_macrow() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (int i = 0; i < 6; i++) {
			configParam(GAIN_PARAMS + i, -1.f, 1.f, 0.f, string::f("Row %d gain", i + 1), "%", 0, 100);
			configInput(IN_INPUTS + i, string::f("Row %d", i + 1));
			configOutput(OUT_OUTPUTS + i, string::f("Row %d", i + 1));
		}

		configParam(MACRO_PARAMS, -1.f, 1.f, 0.f, string::f("MACRO %d gain", 1), "%", 0, 100);


		paramDivider.setDivision(2048);
	}

	void process(const ProcessArgs& args) override {
		float in[16] = {10.f};
		int channels = 1;

		for (int i = 0; i < 6; i++) {
			// Get input
			if (inputs[IN_INPUTS + i].isConnected()) {
				channels = inputs[IN_INPUTS + i].getChannels();
				inputs[IN_INPUTS + i].readVoltages(in);
			}

			if (outputs[OUT_OUTPUTS + i].isConnected()) {
				// Apply gain
				float out[16];

				float gain = params[GAIN_PARAMS + i].getValue();
				float macroGain = params[MACRO_PARAMS].getValue();

				for (int c = 0; c < channels; c++) {
					out[c] = macroGain * gain * in[c];
				}

				// Set output
				outputs[OUT_OUTPUTS + i].setChannels(channels);
				outputs[OUT_OUTPUTS + i].writeVoltages(out);
			}
		}
	}

	/** Set the gain param units to either V or %, depending on whether a cable is connected. */
	void refreshParamQuantities() {
		bool normalized = false;

		for (int i = 0; i < 6; i++) {
			ParamQuantity* pq = paramQuantities[GAIN_PARAMS + i];
			if (!pq)
				continue;

			if (inputs[IN_INPUTS + i].isConnected())
				normalized = false;
			if (normalized) {
				pq->unit = "V";
				pq->displayMultiplier = 10.f;
			}
			else {
				pq->unit = "%";
				pq->displayMultiplier = 100.f;
			}
		}
	}
};


struct _macrowWidget : ModuleWidget {
	_macrowWidget(_macrow* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/macrow.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.351, 21.968)), module, _macrow::GAIN_PARAMS + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.351, 34.982)), module, _macrow::GAIN_PARAMS + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.351, 48.004)), module, _macrow::GAIN_PARAMS + 2));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.351, 61.026)), module, _macrow::GAIN_PARAMS + 3));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.351, 74.048)), module, _macrow::GAIN_PARAMS + 4));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.351, 87.07)), module, _macrow::GAIN_PARAMS + 5));
		
		addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(Vec(20.351, 113.115)), module, _macrow::MACRO_PARAMS));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.331, 21.968)), module, _macrow::IN_INPUTS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.331, 34.982)), module, _macrow::IN_INPUTS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.331, 48.004)), module, _macrow::IN_INPUTS + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.331, 61.026)), module, _macrow::IN_INPUTS + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.331, 74.048)), module, _macrow::IN_INPUTS + 4));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.331, 87.07)), module, _macrow::IN_INPUTS + 5));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.371, 21.968)), module, _macrow::OUT_OUTPUTS + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.371, 34.982)), module, _macrow::OUT_OUTPUTS + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.371, 48.004)), module, _macrow::OUT_OUTPUTS + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.371, 61.026)), module, _macrow::OUT_OUTPUTS + 3));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.371, 74.048)), module, _macrow::OUT_OUTPUTS + 4));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.371, 87.07)), module, _macrow::OUT_OUTPUTS + 5));
	}
};


Model* model_macrow = createModel<_macrow, _macrowWidget>("macrow");
