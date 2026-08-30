#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>

#include <vector>
#include <cmath>
#include <algorithm>

#include "BassDrum.hpp"
#include "Clap.hpp"
#include "HiHats.hpp"
#include "Snare.hpp"
#include "Toms.hpp"

// Authentic 1981 TR-606 Crash Cymbal Specification
static constexpr SynthDrums606::HiHatSpec kCymbalSpec = {
    SynthDrums606::kOpenHatPartials,
    47,                  // partialCount
    4000.0f,             // noiseHighPassHz
    15500.0f,            // noiseLowPassHz
    0.180f,              // tonalMix
    0.820f,              // noiseMix
    0.50f,               // saturationDrive
    1.55f,               // outputTrim
    0.0004f,             // attackTimeConstantSeconds
    0.45f,               // clickAmount
    0.005f,              // clickDecaySeconds
    0.85f,               // bellAccentAmount
    0.060f,              // bellAccentDecaySeconds
    0.08f,               // envelopeFastWeight
    0.040f,              // fastDecaySeconds
    1.450f,              // slowDecaySeconds (long shimmering metallic cymbal decay)
    true,                // decayScalesTimeConstants
    95000.0f / 44100.0f, // reference duration
    0.005f,              // minimumDecaySeconds
    0.100f,              // minimumDurationSeconds
    0.800f,              // gateFadeMaxSeconds
    0.0012f,             // lineWobbleDepth
    0.025f               // lineWobbleCorrelationSeconds
};

// Authentic 1980s 12-Bit Sampler Engine
struct Vintage12BitSampler
{
    float holdL = 0.0f, holdR = 0.0f;
    double phase = 0.0;
    float analogWarmthL = 0.0f, analogWarmthR = 0.0f;

    void reset()
    {
        holdL = holdR = 0.0f;
        phase = 0.0;
        analogWarmthL = analogWarmthR = 0.0f;
    }

    void process(float& left, float& right, double hostSampleRate)
    {
        const double targetClockRate = 26041.66;
        const double phaseIncrement = targetClockRate / hostSampleRate;

        phase += phaseIncrement;
        if (phase >= 1.0)
        {
            phase -= 1.0;
            float inL = std::tanh(left * 1.15f);
            float inR = std::tanh(right * 1.15f);

            const float levels = 2048.0f;
            holdL = std::round(inL * levels) / levels;
            holdR = std::round(inR * levels) / levels;
        }

        analogWarmthL += 0.65f * (holdL - analogWarmthL);
        analogWarmthR += 0.65f * (holdR - analogWarmthR);

        left  = analogWarmthL;
        right = analogWarmthR;
    }
};

// Zero-Delay Feedback TPT State Variable Filter
struct SmoothSVF
{
    float s1L = 0.0f, s2L = 0.0f;
    float s1R = 0.0f, s2R = 0.0f;
    float s3L = 0.0f, s4L = 0.0f;
    float s3R = 0.0f, s4R = 0.0f;

    float smoothedCutoff = 20000.0f;
    float smoothedRes    = 0.0f;

    void reset()
    {
        s1L = s2L = s1R = s2R = 0.0f;
        s3L = s4L = s3R = s4R = 0.0f;
    }

    void process(float& l, float& r, float targetCutoff, float targetRes, bool isHP, bool is24dB, double sampleRate)
    {
        const float smoothCoeff = 0.008f;
        smoothedCutoff += smoothCoeff * (targetCutoff - smoothedCutoff);
        smoothedRes    += smoothCoeff * (targetRes - smoothedRes);

        float safeCutoff = juce::jlimit(20.0f, static_cast<float>(sampleRate * 0.46), smoothedCutoff);
        float g = std::tan(juce::MathConstants<float>::pi * safeCutoff / static_cast<float>(sampleRate));
        float rDamp = 1.0f / (0.707f + smoothedRes * 7.5f);
        float h = 1.0f / (1.0f + rDamp * g + g * g);

        auto processStage = [g, rDamp, h, isHP](float in, float& s1, float& s2) -> float
        {
            float hp = (in - rDamp * s1 - g * s1 - s2) * h;
            float v1 = g * hp;
            float bp = v1 + s1;
            s1 = v1 + bp;
            float v2 = g * bp;
            float lp = v2 + s2;
            s2 = v2 + lp;

            return isHP ? hp : lp;
        };

        float outL = processStage(l, s1L, s2L);
        float outR = processStage(r, s1R, s2R);

        if (is24dB)
        {
            outL = processStage(outL, s3L, s4L);
            outR = processStage(outR, s3R, s4R);
        }

        l = outL;
        r = outR;
    }
};

// True stereo bouncing Ping-Pong Delay
struct StereoDelay
{
    std::vector<float> bufL, bufR;
    int writePos = 0;
    double sampleRate = 44100.0;

    void init(double sr)
    {
        sampleRate = sr;
        int maxSamples = static_cast<int>(sr * 3.0);
        bufL.assign(maxSamples, 0.0f);
        bufR.assign(maxSamples, 0.0f);
        writePos = 0;
    }

    void process(float& left, float& right, float timeSec, float feedback, float mix, bool pingPong)
    {
        if (mix <= 0.001f || bufL.empty()) return;

        int delaySamples = juce::jlimit(1, static_cast<int>(bufL.size()) - 1, static_cast<int>(timeSec * sampleRate));
        int readPos = writePos - delaySamples;
        if (readPos < 0) readPos += static_cast<int>(bufL.size());

        float dL = bufL[readPos];
        float dR = bufR[readPos];

        float dryL = left;
        float dryR = right;
        float inMono = (left + right) * 0.5f;

        if (pingPong)
        {
            bufL[writePos] = inMono + dR * feedback;
            bufR[writePos] = dL * feedback;
        }
        else
        {
            bufL[writePos] = left + dL * feedback;
            bufR[writePos] = right + dR * feedback;
        }

        writePos = (writePos + 1) % static_cast<int>(bufL.size());

        left  = dryL * (1.0f - mix) + dL * mix;
        right = dryR * (1.0f - mix) + dR * mix;
    }
};

// Dattorro Plate Reverb
struct DattorroPlateReverb
{
    struct DelayLine
    {
        std::vector<float> buf;
        int pos = 0;
        void setSize(int s) { buf.assign(std::max(1, s), 0.0f); pos = 0; }
        float read(int offset) const
        {
            if (buf.empty()) return 0.0f;
            int r = pos - offset;
            while (r < 0) r += static_cast<int>(buf.size());
            return buf[r % buf.size()];
        }
        float process(float in)
        {
            if (buf.empty()) return in;
            float out = buf[pos];
            buf[pos] = in;
            pos = (pos + 1) % static_cast<int>(buf.size());
            return out;
        }
    };

    struct Allpass
    {
        std::vector<float> buf;
        int pos = 0;
        float coef = 0.5f;
        void setSize(int s, float c = 0.5f) { buf.assign(std::max(1, s), 0.0f); pos = 0; coef = c; }
        float process(float in)
        {
            if (buf.empty()) return in;
            float bufOut = buf[pos];
            float out = -in * coef + bufOut;
            buf[pos] = in + out * coef;
            pos = (pos + 1) % static_cast<int>(buf.size());
            return out;
        }
    };

    Allpass inDiff[4];
    Allpass tankApL1, tankApL2, tankApR1, tankApR2;
    DelayLine tankDelL1, tankDelL2, tankDelR1, tankDelR2;

    float lpL = 0.0f, lpR = 0.0f;
    float hpInL = 0.0f, hpInR = 0.0f;
    double sampleRate = 44100.0;

    void init(double sr)
    {
        sampleRate = sr;
        float scale = static_cast<float>(sr / 44100.0);

        inDiff[0].setSize(static_cast<int>(142 * scale), 0.70f);
        inDiff[1].setSize(static_cast<int>(107 * scale), 0.70f);
        inDiff[2].setSize(static_cast<int>(379 * scale), 0.625f);
        inDiff[3].setSize(static_cast<int>(277 * scale), 0.625f);

        tankApL1.setSize(static_cast<int>(672 * scale), 0.60f);
        tankDelL1.setSize(static_cast<int>(4453 * scale));
        tankApL2.setSize(static_cast<int>(1800 * scale), -0.55f);
        tankDelL2.setSize(static_cast<int>(3720 * scale));

        tankApR1.setSize(static_cast<int>(908 * scale), 0.60f);
        tankDelR1.setSize(static_cast<int>(4217 * scale));
        tankApR2.setSize(static_cast<int>(2656 * scale), -0.55f);
        tankDelR2.setSize(static_cast<int>(3163 * scale));
    }

    void process(float inL, float inR, float& outL, float& outR, float size, float damp, float mix, bool isHall)
    {
        if (mix <= 0.001f) { outL = inL; outR = inR; return; }

        hpInL += 0.08f * (inL - hpInL);
        hpInR += 0.08f * (inR - hpInR);
        float diffIn = (inL - hpInL + inR - hpInR) * 0.5f;

        for (int i = 0; i < 4; ++i)
            diffIn = inDiff[i].process(diffIn);

        float decay = isHall ? (0.50f + size * 0.44f) : (0.15f + size * 0.50f);
        float d     = isHall ? (0.15f + damp * 0.55f) : (0.45f + damp * 0.45f);

        float lIn = diffIn + tankDelR2.read(0) * decay;
        float apL1 = tankApL1.process(lIn);
        float delL1 = tankDelL1.process(apL1);
        lpL += (1.0f - d) * (delL1 - lpL);
        float apL2 = tankApL2.process(lpL);
        tankDelL2.process(apL2);

        float rIn = diffIn + tankDelL2.read(0) * decay;
        float apR1 = tankApR1.process(rIn);
        float delR1 = tankDelR1.process(apR1);
        lpR += (1.0f - d) * (delR1 - lpR);
        float apR2 = tankApR2.process(lpR);
        tankDelR2.process(apR2);

        float wetL = tankDelL1.read(266) + tankDelL1.read(2974) - apL2 + tankDelL2.read(1913) - tankDelR1.read(1996);
        float wetR = tankDelR1.read(353) + tankDelR1.read(3627) - apR2 + tankDelR2.read(1228) - tankDelL1.read(2673);

        wetL *= 0.35f;
        wetR *= 0.35f;

        outL = inL * (1.0f - mix) + wetL * mix;
        outR = inR * (1.0f - mix) + wetR * mix;
    }
};

class Super606AudioProcessor : public juce::AudioProcessor
{
public:
    Super606AudioProcessor();
    ~Super606AudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Simple606"; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 1.8; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void triggerVoice(int voiceIndex, bool accented = false);

    // 9 Tracks (Track 0: ACC, Tracks 1-8: 8 Drum Voices) x 64 Steps
    std::atomic<bool> stepPattern[9][64];
    std::atomic<int> currentSeqStep { 0 };
    std::atomic<bool> seqIsPlaying { false };
    std::atomic<bool> isDawPlaying { false };

    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Voice Params
    std::atomic<float>* kickTransientParam = nullptr;
    std::atomic<float>* kickDecayParam     = nullptr;
    std::atomic<float>* kickTuneParam      = nullptr;
    std::atomic<float>* kickHeatParam      = nullptr;
    std::atomic<float>* kickMode608Param   = nullptr;

    std::atomic<float>* snareDecayParam    = nullptr;
    std::atomic<float>* snarePitchParam    = nullptr;
    std::atomic<float>* snareSnappyParam   = nullptr;
    std::atomic<float>* snareColorParam    = nullptr;

    std::atomic<float>* clapDecayParam     = nullptr;
    std::atomic<float>* clapPitchParam     = nullptr;
    std::atomic<float>* clapNoiseParam     = nullptr;

    std::atomic<float>* hatsChDecayParam   = nullptr;
    std::atomic<float>* hatsOhDecayParam   = nullptr;
    std::atomic<float>* hatsPitchParam     = nullptr;

    std::atomic<float>* cyDecayParam       = nullptr;
    std::atomic<float>* cyPitchParam       = nullptr;

    std::atomic<float>* ltomDecayParam     = nullptr;
    std::atomic<float>* ltomPitchParam     = nullptr;
    std::atomic<float>* htomDecayParam     = nullptr;
    std::atomic<float>* htomPitchParam     = nullptr;

    // Per-Voice Pans (8 Voices: BD, SN, CL, CH, OH, CY, LT, HT)
    std::atomic<float>* voicePanParams[8]  = { nullptr };

    // Mixer Channel Volumes, Mutes, Solos + Master Vol
    std::atomic<float>* mixerVolParams[8]  = { nullptr };
    std::atomic<float>* mixerMuteParams[8] = { nullptr };
    std::atomic<float>* mixerSoloParams[8] = { nullptr };
    std::atomic<float>* masterVolParam     = nullptr;

    // 3 x 8 FX Routing Matrix
    std::atomic<float>* fxMatrixParams[3][8] = { { nullptr } };

    // FX Units
    std::atomic<float>* fxDriveParam       = nullptr;
    std::atomic<float>* fxToneParam        = nullptr;
    std::atomic<float>* fxDriveMixParam    = nullptr;

    std::atomic<float>* fxDelayTimeSyncParam = nullptr;
    std::atomic<float>* fxDelayFdbkParam   = nullptr;
    std::atomic<float>* fxDelayMixParam    = nullptr;
    std::atomic<float>* fxDelayPingPongParam = nullptr;

    std::atomic<float>* fxReverbSizeParam  = nullptr;
    std::atomic<float>* fxReverbDampParam  = nullptr;
    std::atomic<float>* fxReverbMixParam   = nullptr;
    std::atomic<float>* fxReverbModeParam  = nullptr;

    std::atomic<float>* fxFilterCutoffParam = nullptr;
    std::atomic<float>* fxFilterResParam   = nullptr;
    std::atomic<float>* fxFilterOnParam    = nullptr;
    std::atomic<float>* fxFilterHpParam    = nullptr;
    std::atomic<float>* fxFilter24dbParam  = nullptr;

    // 12-Bit Vintage Engine
    std::atomic<float>* global12BitParam   = nullptr;
    std::atomic<float>* seqBpmParam        = nullptr;
    std::atomic<float>* seqLengthParam     = nullptr;

    std::atomic<bool> manualTrigger[8];
    std::atomic<bool> manualTriggerAccent[8];
    float activeVoiceAccentGain[8];

    // 8 Dedicated Synthesizer Voice Engines
    SynthDrums606::BassDrumVoice bassDrum;
    SynthDrums606::ClapVoice clap;
    SynthDrums606::MetalHiHatVoice closedHat;
    SynthDrums606::MetalHiHatVoice openHat;
    SynthDrums606::MetalHiHatVoice cymbal;
    SynthDrums606::SnareVoice snare;
    SynthDrums606::TomVoice lowTom;
    SynthDrums606::TomVoice highTom;

    // FX DSP
    float driveToneLowZ = 0.0f;
    float driveToneHighZ = 0.0f;
    StereoDelay stereoDelay;
    DattorroPlateReverb plateReverb;
    SmoothSVF masterFilter;
    Vintage12BitSampler vintageSampler;

    double sampleRate_ = 44100.0;
    double seqSampleCounter_ = 0.0;
    int lastTimelineStep_ = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Super606AudioProcessor)
};
