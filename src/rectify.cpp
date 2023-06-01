#include "plugin.hpp"


struct Rectify : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		IN1_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Rectify() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(IN1_INPUT, "Ch 1");
		configOutput(OUT1_OUTPUT, "Postive");
		configOutput(OUT2_OUTPUT, "Negative");
	}

	void process(const ProcessArgs& args) override {

		float in[1] = {10.0f};
		float out[1] = {10.0f};
		float outNegative[1] = {10.0f};

        if (inputs[IN1_INPUT].isConnected()) {
            inputs[IN1_INPUT].readVoltages(in);
        }

        if (in[0] < 0) {
            out[0] = in[0] * -1.0;
        } else {
            out[0] = in[0];
        }

        outNegative[0] = out[0] * -1.0;

        if (outputs[OUT1_OUTPUT].isConnected()) {
            outputs[OUT1_OUTPUT].setChannels(1);
            outputs[OUT1_OUTPUT].writeVoltages(out);
        }

        if (outputs[OUT2_OUTPUT].isConnected()) {
            outputs[OUT2_OUTPUT].setChannels(1);
            outputs[OUT2_OUTPUT].writeVoltages(outNegative);
        }
	}
};


struct RectifyWidget : ModuleWidget {
	RectifyWidget(Rectify* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/rectify.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 82.732)), module, Rectify::IN1_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 97.923)), module, Rectify::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 113.115)), module, Rectify::OUT2_OUTPUT));
	}
};


Model* modelRectify = createModel<Rectify, RectifyWidget>("Rectify");