#if !defined(K_SimpleFilters_23)
#define K_SimpleFilters_23
// #include "../../Utilities/Constants.hpp"
#include <math.h>

#define PI 3.14159265358979323846

namespace k
{
	class OnePoleFilter
	{
	public:

		OnePoleFilter(){}

		void init(double freq, unsigned int sample_rate)	
		{
			sampleRate = sample_rate;
			setTime(freq);
		}

		float process(double in)
		{
			return buffer = in * a0 + buffer * b1;
		}

		void setTime(double freq)
		{
			b1 = exp(-2.0 * PI * freq/sampleRate);
    		a0 = 1.0 - b1;
		}
	private:
		double buffer;
		double a0, b1;
		double sampleRate;
	};
}
#endif