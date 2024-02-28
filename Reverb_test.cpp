#include <iterator>
#include <vector>
#include "daisy_seed.h"
#include "daisysp.h"

#include "constants.h"

#include "KnobMulti.hpp"
#include "ReverbFDN.hpp"
#include "MapPot.hpp"

using namespace daisy;
using namespace daisysp;
using namespace k;

DaisySeed hw;

// #define SAMPLE_RATE 48000

Metro                   metro;
Adsr                    envelope;
SyntheticSnareDrum      snare;
ReverbFDN reverb;

AdcHandle adc;
KnobMulti knob[7];

GPIO modSwitch, effectSwitch;

float sampleRate;
float parameters[7];
float volume;

TimerHandle timerUI, timerVisual;
void        UICallback(void *data);

void initTimer()
{
    TimerHandle::Config timcfg;

    timcfg.periph               = daisy::TimerHandle::Config::Peripheral::TIM_5;
    timcfg.dir                  = daisy::TimerHandle::Config::CounterDir::UP;
    uint32_t      tim_base_freq = daisy::System::GetPClk2Freq();
    unsigned long tim_period    = tim_base_freq / 50;
    timcfg.period               = tim_period;
    timcfg.enable_irq           = true;
    timerUI.Init(timcfg);
    timerUI.SetCallback(UICallback, nullptr);
    timerUI.Start();
}


void Callback(AudioHandle::InterleavingInputBuffer  in,
              AudioHandle::InterleavingOutputBuffer out,
              size_t                                size)
{
    float Signal_Snare;

    for(size_t i = 0; i < size; i+= 2)
    {
        if(metro.Process())
        {
            snare.Trig();
        }

        
        Signal_Snare = reverb.Process(snare.Process()) * volume;


        out[i] = Signal_Snare;
        out[i+1] = Signal_Snare;
    }
}


void setup()
{
    hw.Configure();
    hw.Init(true);
    hw.SetAudioBlockSize(1);
    sampleRate = hw.AudioSampleRate();

    modSwitch.Init(seed::D10, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
    effectSwitch.Init(seed::D9, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);

    AdcChannelConfig channelConfigArray[7];

    channelConfigArray[0].InitSingle(seed::A6);
    channelConfigArray[1].InitSingle(seed::A5);
    channelConfigArray[2].InitSingle(seed::A4);
    channelConfigArray[3].InitSingle(seed::A3);
    channelConfigArray[4].InitSingle(seed::A2);
    channelConfigArray[5].InitSingle(seed::A1);
    channelConfigArray[6].InitSingle(seed::A0);

    adc.Init(channelConfigArray, 7);
    adc.Start();

    for(int i = 0; i < 7; i++)
    {
        knob[i].Init(2);
    }


    // Sequencer
    metro.Init(.5, sampleRate);

    // Audio

    snare.Init(sampleRate);
    snare.SetDecay(.2);
    reverb.Init(sampleRate);

    initTimer();
    hw.StartLog();
    hw.StartAudio(Callback);
}


void UICallback(void *data)
{
    float amount, feedback, size, diffusion, smoothing;
    int density;

    int  shiftLayer = 0;
    bool shift      = false;

    if(!modSwitch.Read())
    {
        shift      = true;
        shiftLayer = 0;
    }
    else if(!effectSwitch.Read())
    {
        shift      = true;
        shiftLayer = 1;
    }
    else
        shift = false;


    for(int i = 0; i < 7; i++)
    {
        parameters[i] = static_cast<float>(adc.Get(i)) / 65535.0f;
        knob[i].updateKnob(parameters[i], shift, shiftLayer);
    }


    amount = knob[0].getValue();
    feedback = mapPot(knob[1].getValue(), 0,1,.25);
    density = (int) mapPot(knob[2].getValue(), 0,8);
    size = mapPot(knob[3].getValue(), 0,10,4);
    diffusion = mapPot(knob[4].getValue(), 0,10,2);
    smoothing = mapPot(knob[5].getValue(), 0,2);
    volume = knob[6].getValue();

    reverb.SetAmount(amount);
    reverb.SetFeedback(feedback);
    reverb.SetDensity(density);
    reverb.SetSize(size);
    reverb.SetDiffusion(diffusion);
    reverb.SetSmoothing(smoothing);
}


int main()
{
    setup();
    // for(;;) {

    // }
};