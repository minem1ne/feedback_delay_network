#ifndef M_REVERBFDN_24
#define M_REVERBFDN_24

#include <iterator>
#include <random>
#include "daisy_seed.h"
#include "daisysp.h"

#include "OnePoleFilter.hpp"

using namespace daisy;
using namespace daisysp;

// ReverbParams[n][0] = delay time (in seconds)                     */
// ReverbParams[n][1] = random variation in delay time (in seconds) */
// ReverbParams[n][2] = random variation frequency (in 1/sec)       */ 
// ReverbParams[n][3] = random seed (0 - 32767)                     */ NICHT GENUTZT


static const float ReverbParams[8][4] = {{0.031523, 0.010, 3.100, 1966.0},
                                         {0.015451, 0.011, 3.500, 29491.0},
                                         {0.025437, 0.017, 1.110, 22937.0},
                                         {0.047899, 0.006, 3.973, 9830.0},
                                         {0.036967, 0.010, 2.341, 20643.0},
                                         {0.025559, 0.011, 1.897, 22937.0},
                                         {0.014562, 0.017, 0.891, 29491.0},
                                         {0.033847, 0.006, 3.221, 14417.0}};

DSY_SDRAM_BSS DelayLine<float, ((size_t)(2.0f * 48000.0f))> delay[8]; // max delay

class ReverbFDN
{
  private:

    k::OnePoleFilter slewLimiter;

    float sample_rate_;
    float feedback_ = .9, amount_ = 1, size_ = 1, diffusion_, tone_;
    int density_ = 8;

    float delay_variations[8]; // Verzögerungszeitvariationen
    int variation_counter[8]; // sample counter
    int variation_period[8]; // 1/frequency in samples
    float delay_time[8];

  public:

    void Init(float sample_rate)
    {
        sample_rate_ = sample_rate;

        for(int i = 0; i < density_; i++)
        {
            delay[i].Init();
            delay[i].SetDelay(ReverbParams[i][0] * sample_rate_);
        }
        slewLimiter.init(800, sample_rate_);
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

    /** Sets the reverb Feedback
        works > 0
        warning >= 1
    */
    void SetFeedback(float feedback = .7) { 
        if (feedback < 0) {
            feedback = 0;
        }

        feedback_ = feedback;
    }

    /** Sets the reverb Amount (Dry/Wet)
        between 0 - 1
    */
    void SetAmount(float amount = 1) { 
        if (amount < 0) {
            amount = 0;
        }
        if (amount > 1) {
            amount = 1;
        }

        amount_ = amount;
    }

    /** Sets the room Size (delaytime)

    */
    void SetSize(float size = 1) {
        if (size < 0.01) {
            size = 0.01;
        }
        if (size > 10) {
            size = 10;
        }

        size_ = size;
    }

    /** Sets the Reverb Density
        between 0 - 8
    */
    void SetDensity(int density = 8) {
        if (density < 1) {
            density = 1;
        }
        if (density > 8) {
            density = 8;
        }

        density_ = density;
    }

    /** Sets the Reverb diffusion
        best between 0 - 1
    */
    void SetDiffusion(float diffusion = 1) {
        if (diffusion < 0) {
            diffusion = 0;
        }
        if (diffusion > 10) {
            diffusion = 10;
        }

        diffusion_ = diffusion;
    }

/** Sets the diffusion smoothing
    works best around 1
*/

    void SetSmoothing(float smoothing = 1) {
        
        smoothing *= 800;
        slewLimiter.setTime(smoothing);
    }

    /** Processes the input Signal
        Mono reverb
    */
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
                delay_time[i] = (ReverbParams[i][0] + delay_variations[i] * diffusion_);
            }

            output += delay[i].Read();

            delay[i].Write(input + delay[i].Read() * feedback_);

            delay[i].SetDelay(slewLimiter.process(delay_time[i]) * sample_rate_ * size_);
        }


        // output = 1 - output;
        output *= amount_;
        output += input * (-amount_ + 1);

        return output;
    }
};

#endif