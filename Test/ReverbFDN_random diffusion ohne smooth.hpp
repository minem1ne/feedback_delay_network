#ifndef M_REVERBFDN_24
#define M_REVERBFDN_24

#include <iterator>
#include <random>
#include "daisy_seed.h"
#include "daisysp.h"


using namespace daisy;
using namespace daisysp;

// ReverbParams[n][0] = delay time (in seconds)                     */
// ReverbParams[n][1] = random variation in delay time (in seconds) */
// ReverbParams[n][2] = random variation frequency (in 1/sec)       */
// ReverbParams[n][3] = random seed (0 - 32767)                     */


static const float ReverbParams[8][4] = {{0.031523, 0.0010, 3.100, 1966.0},
                                         {0.015451, 0.0011, 3.500, 29491.0},
                                         {0.025437, 0.0017, 1.110, 22937.0},
                                         {0.047899, 0.0006, 3.973, 9830.0},
                                         {0.036967, 0.0010, 2.341, 20643.0},
                                         {0.025559, 0.0011, 1.897, 22937.0},
                                         {0.014562, 0.0017, 0.891, 29491.0},
                                         {0.033847, 0.0006, 3.221, 14417.0}};

DSY_SDRAM_BSS DelayLine<float, ((size_t)(2.0f * 48000.0f))> delay[8];

class ReverbFDN
{
  private:
    float                                         sample_rate_;
    float feedback_ = .9, amount_ = 1, size_, density_ = 8, difuse_, tone_;

    float delay_variations[8]; // Verzögerungszeitvariationen
    int variation_counter[8]; // sample counter
    int variation_period[8]; // 1/frequency in samples


  public:
    void Init(float sample_rate)
    {
        sample_rate_ = sample_rate;

        for(int i = 0; i < density_; i++)
        {
            delay[i].Init();
            delay[i].SetDelay(ReverbParams[i][0] * sample_rate_);
        }

        GenerateDelayVariations(sample_rate);
    }


    void GenerateDelayVariations(float sample_rate)
    {
        std::random_device rd;
        std::mt19937 gen{rd()};
        std::uniform_real_distribution<float> dist{-1.0f, 1.0f};

        for(int i = 0; i < density_; i++)
        {
            // Zufällige Perioden für die Variationen basierend auf der Frequenz berechnen
            variation_period[i] = sample_rate / ReverbParams[i][2];

            // Zufällige Werte für die Variationen generieren
            delay_variations[i] = ReverbParams[i][1] * dist(gen);

            // Variationen-Zähler initialisieren
            variation_counter[i] = 0;
        }
    }

    float Process(float input)
    {
        float output = 0;

        for(int i = 0; i < density_; i++)
        {

            if (++variation_counter[i] >= variation_period[i])
            {
                variation_counter[i] = 0;
                std::random_device rd;
                std::mt19937 gen{rd()};
                std::uniform_real_distribution<float> dist{-1.0f, 1.0f};
                delay_variations[i] = ReverbParams[i][1] * dist(gen);
            }

            output += delay[i].Read();

            delay[i].Write(input + delay[i].Read() * feedback_);

            delay[i].SetDelay((ReverbParams[i][0] + delay_variations[i]) * sample_rate_);
        }

        output *= amount_;
        output += input * (-amount_ + 1);

        return output;
    }
};

#endif