#ifndef K_KNOBMULTI_23
#define K_KNOBMULTI_23

#include <vector>
#include <cmath>

namespace k
{
    class KnobMulti
    {
    public:

		KnobMulti()
		{

		}

        // KnobMulti(int shiftLayers) {
        //     windowSize = 0.05;
        //     layers = shiftLayers;
        //     initShiftValues();
        // }

        // KnobMulti(double w)
        // {
        //     windowSize = w;
        // }

        // KnobMulti(double w, double init)
        // {
        //     windowSize = w;
        //     value = init;
        //     shiftValues[0] = init;
        //     retValues[0] = init;
        //     retShifts[0] = init;
        //     sFlags[0] = true;
        //     layers = 1;
        // }

		void Init(int shiftLayers) {
			windowSize = 0.08;
            layers = shiftLayers;
            initShiftValues();
		}

       double getValue()
        {
            return value;
        }

        double getShiftValue(int layer = 0)
        {
            if (layer >= 0 && layer < layers) {
                return shiftValues[layer];
            }
            return shiftValue; // Fehlerfall: ungültige Shift-Layer
        }

        void shift(bool b, int layer = 0)
        {
            if (layer >= 0 && layer < layers) {
                if (b == shiftState)
                    return;
                if (b && !shiftState) // Von regulär zu Shift
                {
                    vFlag = true;
                    if (shiftWasAccessed[layer])
                    {
                        retShifts[layer] = value;
                    }
                    else
                    {
                        shiftValues[layer] = value;
                        retShifts[layer] = value;
                        shiftWasAccessed[layer] = true;
                    }
                }
                else if (!b && shiftState) // Von Shift zu regulär
                {
                    sFlag = true;
                    retValues[layer] = shiftValues[layer];
                }
                shiftState = b;
            }
            // Fehlerfall: ungültige Shift-Layer
        }

        void updateKnob(double v, int layer = 0)
        {
            if (layer >= 0 && layer < layers) {
                if (value < windowSize) {
                    value = 0.0f;
                }
                else if (v > 1 - windowSize) {
                    v = 1.0f;
                }

                if (!shiftState)
                {
					if(!vFlag)
					{
						value = v;
					}
					else if(.1 > abs(v - value))//(windowSize < abs(v - retValues[layer]))
					{
						value = v;
						vFlag = false;
					}
                }
                else
                {
                    if (!sFlag)
                    {
                        shiftValues[layer] = v;
                    }
                    else if (0.1 > abs(v - shiftValues[layer]))
                    {
                        shiftValues[layer] = v;
                        sFlag = false;
                    }
                }
            }
            // Fehlerfall: ungültige Shift-Layer
        }

        void updateKnob(double v, bool b, int layer = 0)
        {
            shift(b, layer);
            updateKnob(v, layer);
        }

        void setWindowSize(double w)
        {
            windowSize = w;
        }

    private:
        int layers;
		float windowSize = 0.05;
        double value = 0;
        double shiftValue = 0;
        std::vector<double> shiftValues;
        std::vector<double> retValues;
        std::vector<double> retShifts;
        std::vector<bool> shiftStates;
        bool vFlag = false;
        bool sFlag = false;
        bool shiftState = false;
		bool shiftWasAccessed[2] = {false, false};

        void initShiftValues() {
            shiftValues.resize(layers, 0);
            retValues.resize(layers, 0);
            retShifts.resize(layers, 0);
            shiftStates.resize(layers, false);
        }
    };
}

#endif