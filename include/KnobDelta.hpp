#ifndef M_KNOBDELTA_23
#define M_KNOBDELTA_23

#include <stdlib.h>

namespace k
{
	class KnobDelta
	{
	public:
		KnobDelta() {
			windowSize = 0.1;
		}

		KnobDelta(float w) {
			windowSize = w;
		}

		KnobDelta(float w, float init)
		{
			windowSize = w;
			value = init;
			deltaValue = init;
			retValue = init;
			retShift = init;
			sFlag = true;
		}
		float getValue() {
			return value;
		}

		float getModValue(float m) {
		
			if (!shiftWasAccessed) {
				out = value;
			}
			else {
				out = value + m * deltaValue;
			}

			if (out > 1) {
				out = 1;
			}
			else if (out < 0) {
				out = 0;
			}

			return out;	
		}

		void shift(bool b) {
				if(b == shiftState)
					return;
				if(b && !shiftState) //FROM REGULAR 2 SHIFT
				{
					vFlag = true;
					if(shiftWasAccessed)
					{
						retShift = value;
					}
					else
					{
						deltaValue = value;
						retShift = value;
						shiftWasAccessed = true;
					}
				}
				else if(!b && shiftState) //FROM SHIFT 2 REGULAR
				{
					sFlag = true;
					retValue = deltaValue;
				}
				shiftState = b;
		}
		void updateKnob(float v)
		{
			if(!shiftState)
			{
				if(!vFlag)
				{
					value = v;
				}
				else if(windowSize < abs(v - retValue))
				{
					value = v;
					vFlag = false;
				}
			}
			else
			{
				if(!sFlag)
				{
					deltaValue = v - value;
				}
				else if(windowSize < abs(v - retShift))
				{
					deltaValue = v - value;
					sFlag = false;
				}
			}
		}
		void updateKnob(float v, bool b)
		{
			shift(b);
			updateKnob(v);
		}
		void setWindowSize(float w)
		{
			windowSize = w;
		}
		void resetShiftAccessed(float v)
		{
			shiftWasAccessed = false;
			value = v;
			deltaValue = v;
			retValue = v;
			retShift = v;
		}
	private:
		float value = 0, deltaValue = 0, retValue = 0, retShift = 0;
		float windowSize = 0.1;
		float out;
		bool shiftState = false, vFlag = false, sFlag = false, shiftWasAccessed = false;
	};
}

#endif