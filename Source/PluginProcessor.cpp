#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout Super606AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // --- Drum Voices ---
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("kick_transient", 1), "Kick Transient", 0.0f, 1.0f, 0.30f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("kick_decay",     1), "Kick Decay",     0.0f, 1.0f, 0.80f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("kick_tune",      1), "Kick Tuning",   -12.0f, 12.0f, 2.22f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("kick_heat",      1), "Kick Heat",     0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("kick_608_mode",   1), "608 XL Mode",    false));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("snare_decay",  1), "Snare Decay",  0.01f, 1.0f, 0.80f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("snare_pitch",  1), "Snare Pitch",  0.5f,  2.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("snare_snappy", 1), "Snare Snappy", 0.0f,  1.0f, 0.75f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("snare_color",  1), "Snare Color",  0.5f,  2.0f, 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("clap_decay", 1), "Clap Decay", 0.05f, 1.0f, 0.80f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("clap_pitch", 1), "Clap Pitch", 0.5f,  2.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("clap_noise", 1), "Clap Noise", 0.0f,  1.0f, 0.50f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("hats_ch_decay", 1), "Closed Hat Decay", 0.05f, 1.0f, 0.70f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("hats_oh_decay", 1), "Open Hat Decay",   0.05f, 1.0f, 0.70f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("hats_pitch",    1), "Hi-Hat Pitch",     0.5f,  2.0f, 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("ltom_decay", 1), "Low Tom Decay", 0.05f, 1.0f, 0.80f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("ltom_pitch", 1), "Low Tom Pitch", 0.5f,  2.0f, 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("htom_decay", 1), "High Tom Decay", 0.05f, 1.0f, 0.80f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("htom_pitch", 1), "High Tom Pitch", 0.5f,  2.0f, 1.0f));

    // --- Per-Voice Pan & Mixer Channels ---
    const juce::String vNames[7] = { "bd", "sn", "cl", "ch", "oh", "lt", "ht" };
    for (int v = 0; v < 7; ++v)
    {
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("pan_" + vNames[v], 1), "Pan " + vNames[v].toUpperCase(), -1.0f, 1.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("vol_" + vNames[v], 1), "Vol " + vNames[v].toUpperCase(), 0.0f, 1.5f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("mute_" + vNames[v], 1), "Mute " + vNames[v].toUpperCase(), false));
        layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("solo_" + vNames[v], 1), "Solo " + vNames[v].toUpperCase(), false));
    }
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("vol_master", 1), "Master Volume", 0.0f, 1.5f, 1.0f));

    // --- 3 x 7 FX Routing Matrix ---
    const juce::String fxRowIds[3] = { "drive", "delay", "reverb" };
    for (int r = 0; r < 3; ++r)
    {
        for (int v = 0; v < 7; ++v)
        {
            juce::String id = "route_" + fxRowIds[r] + "_" + vNames[v];
            bool defVal = (r == 2 && v == 0) ? false : true;
            layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID(id, 1), id, defVal));
        }
    }

    // --- FX Units ---
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_drive",     1), "Drive",     0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_tone",      1), "Tone",      0.0f, 1.0f, 0.50f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_drive_mix", 1), "Drive Mix", 0.0f, 1.0f, 0.0f));

    juce::StringArray delayTimes = { "1/32", "1/16T", "1/16", "1/8T", "1/8", "1/8D", "1/4", "1/2" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("fx_delay_time_sync", 1), "Delay Time", delayTimes, 4));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_delay_fdbk", 1), "Feedback", 0.0f, 0.85f, 0.35f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_delay_mix",  1), "Delay Mix", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("fx_delay_pingpong", 1), "Ping Pong", true));

    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_reverb_size", 1), "Reverb Size", 0.0f, 1.0f, 0.35f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_reverb_damp", 1), "Damping",     0.0f, 1.0f, 0.60f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_reverb_mix",  1), "Reverb Mix",  0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("fx_reverb_mode", 1), "Hall Mode",   false));

    // Master SVF Filter
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_filter_cutoff", 1), "Filter Cutoff",
                                                           juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.28f), 20000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fx_filter_res", 1), "Filter Res", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("fx_filter_on", 1), "Filter On", true));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("fx_filter_hp", 1), "Filter HP", false));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("fx_filter_24db", 1), "Filter 24dB", false));

    // 12-Bit Crunch Mode
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("global_12bit", 1), "12-Bit Mode", false));

    // Themes & Settings
    juce::StringArray themeNames = {
        "NEON VIOLET",
        "ELECTRIC BLUE",
        "TR RED",
        "SUNSET YELLOW",
        "AMBER CRT",
        "ACID GREEN",
        "CYBER TEAL",
        "HOT PINK",
        "TRANS PRIDE"
    };
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("ui_theme_color", 1), "Theme Color", themeNames, 0));

    juce::StringArray accNames = { "ORANGE", "YELLOW", "RED", "WHITE", "PURPLE" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("ui_accent_color", 1), "Accent Color", accNames, 0));

    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("ui_trans_pride", 1), "Trans Pride Accent", false));

    // Sequencer Settings
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("seq_bpm", 1), "Tempo BPM", 40.0f, 240.0f, 120.0f));
    juce::StringArray seqLengths = { "16 STEPS", "32 STEPS", "48 STEPS", "64 STEPS" };
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("seq_length", 1), "Pattern Length", seqLengths, 0));

    return layout;
}

Super606AudioProcessor::Super606AudioProcessor()
: AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    kickTransientParam = apvts.getRawParameterValue("kick_transient");
    kickDecayParam     = apvts.getRawParameterValue("kick_decay");
    kickTuneParam      = apvts.getRawParameterValue("kick_tune");
    kickHeatParam      = apvts.getRawParameterValue("kick_heat");
    kickMode608Param   = apvts.getRawParameterValue("kick_608_mode");

    snareDecayParam    = apvts.getRawParameterValue("snare_decay");
    snarePitchParam    = apvts.getRawParameterValue("snare_pitch");
    snareSnappyParam   = apvts.getRawParameterValue("snare_snappy");
    snareColorParam    = apvts.getRawParameterValue("snare_color");

    clapDecayParam     = apvts.getRawParameterValue("clap_decay");
    clapPitchParam     = apvts.getRawParameterValue("clap_pitch");
    clapNoiseParam     = apvts.getRawParameterValue("clap_noise");

    hatsChDecayParam   = apvts.getRawParameterValue("hats_ch_decay");
    hatsOhDecayParam   = apvts.getRawParameterValue("hats_oh_decay");
    hatsPitchParam     = apvts.getRawParameterValue("hats_pitch");

    ltomDecayParam     = apvts.getRawParameterValue("ltom_decay");
    ltomPitchParam     = apvts.getRawParameterValue("ltom_pitch");

    htomDecayParam     = apvts.getRawParameterValue("htom_decay");
    htomPitchParam     = apvts.getRawParameterValue("htom_pitch");

    const juce::String vNames[7] = { "bd", "sn", "cl", "ch", "oh", "lt", "ht" };
    for (int v = 0; v < 7; ++v)
    {
        voicePanParams[v]  = apvts.getRawParameterValue("pan_" + vNames[v]);
        mixerVolParams[v]  = apvts.getRawParameterValue("vol_" + vNames[v]);
        mixerMuteParams[v] = apvts.getRawParameterValue("mute_" + vNames[v]);
        mixerSoloParams[v] = apvts.getRawParameterValue("solo_" + vNames[v]);
        activeVoiceAccentGain[v] = 1.0f;
    }
    masterVolParam = apvts.getRawParameterValue("vol_master");

    const juce::String fxRowIds[3] = { "drive", "delay", "reverb" };
    for (int r = 0; r < 3; ++r)
        for (int v = 0; v < 7; ++v)
            fxMatrixParams[r][v] = apvts.getRawParameterValue("route_" + fxRowIds[r] + "_" + vNames[v]);

    fxDriveParam       = apvts.getRawParameterValue("fx_drive");
    fxToneParam        = apvts.getRawParameterValue("fx_tone");
    fxDriveMixParam    = apvts.getRawParameterValue("fx_drive_mix");

    fxDelayTimeSyncParam = apvts.getRawParameterValue("fx_delay_time_sync");
    fxDelayFdbkParam   = apvts.getRawParameterValue("fx_delay_fdbk");
    fxDelayMixParam    = apvts.getRawParameterValue("fx_delay_mix");
    fxDelayPingPongParam = apvts.getRawParameterValue("fx_delay_pingpong");

    fxReverbSizeParam  = apvts.getRawParameterValue("fx_reverb_size");
    fxReverbDampParam  = apvts.getRawParameterValue("fx_reverb_damp");
    fxReverbMixParam   = apvts.getRawParameterValue("fx_reverb_mix");
    fxReverbModeParam  = apvts.getRawParameterValue("fx_reverb_mode");

    fxFilterCutoffParam = apvts.getRawParameterValue("fx_filter_cutoff");
    fxFilterResParam    = apvts.getRawParameterValue("fx_filter_res");
    fxFilterOnParam     = apvts.getRawParameterValue("fx_filter_on");
    fxFilterHpParam     = apvts.getRawParameterValue("fx_filter_hp");
    fxFilter24dbParam   = apvts.getRawParameterValue("fx_filter_24db");

    global12BitParam   = apvts.getRawParameterValue("global_12bit");
    seqBpmParam        = apvts.getRawParameterValue("seq_bpm");
    seqLengthParam     = apvts.getRawParameterValue("seq_length");

    for (int t = 0; t < 8; ++t)
    {
        if (t < 7)
        {
            manualTrigger[t].store(false);
            manualTriggerAccent[t].store(false);
        }
        for (int s = 0; s < 64; ++s)
            stepPattern[t][s].store(false);
    }
}

void Super606AudioProcessor::triggerVoice(int voiceIndex, bool accented)
{
    if (voiceIndex >= 0 && voiceIndex < 7)
    {
        manualTriggerAccent[voiceIndex].store(accented);
        manualTrigger[voiceIndex].store(true);
    }
}

void Super606AudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    sampleRate_ = sampleRate;
    seqSampleCounter_ = 0.0;
    lastTimelineStep_ = -1;

    bassDrum.init(sampleRate);
    clap.init(sampleRate);
    hiHat.init(sampleRate);
    snare.init(sampleRate, 0x6063u);
    lowTom.init(sampleRate, 0x6061u);
    highTom.init(sampleRate, 0x6062u);

    stereoDelay.init(sampleRate);
    plateReverb.init(sampleRate);
    masterFilter.reset();
    vintageSampler.reset();
}

void Super606AudioProcessor::releaseResources()
{
    bassDrum.stop();
    clap.stop();
    hiHat.stop();
    snare.stop();
    lowTom.stop();
    highTom.stop();
}

bool Super606AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return (layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
    || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo());
}

void Super606AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const int totalNumSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    auto* channelLeft  = buffer.getWritePointer(0);
    auto* channelRight = (numChannels > 1) ? buffer.getWritePointer(1) : nullptr;

    auto midiIterator = midiMessages.findNextSamplePosition(0);

    double bpm = seqBpmParam != nullptr ? static_cast<double>(seqBpmParam->load()) : 120.0;
    bool dawPlaying = false;
    double currentPpq = 0.0;
    bool hasPpq = false;

    // 1. Read Host Transport & Timeline Position
    if (auto* playHead = getPlayHead())
    {
        if (auto posOpt = playHead->getPosition())
        {
            if (auto b = posOpt->getBpm())
                bpm = *b;

            dawPlaying = posOpt->getIsPlaying();

            if (auto ppqOpt = posOpt->getPpqPosition())
            {
                currentPpq = *ppqOpt;
                hasPpq = true;
            }
        }
    }
    isDawPlaying.store(dawPlaying);

    const double samplesPer16th = (sampleRate_ * 60.0) / (std::max(20.0, bpm) * 4.0);
    int lengthIndex = seqLengthParam != nullptr ? static_cast<int>(seqLengthParam->load()) : 0;
    const int maxSteps = (lengthIndex + 1) * 16;

    auto fireVoice = [this](int voiceIndex, bool accented)
    {
        activeVoiceAccentGain[voiceIndex] = accented ? 1.88f : 1.0f;

        if (voiceIndex == 0)
        {
            const bool is608 = kickMode608Param != nullptr && kickMode608Param->load() > 0.5f;
            const float trans = kickTransientParam != nullptr ? kickTransientParam->load() * (accented ? 1.45f : 1.0f) : 0.30f;
            const float tune  = kickTuneParam != nullptr ? kickTuneParam->load() : 2.22f;
            const float decay = kickDecayParam != nullptr ? kickDecayParam->load() : 0.80f;
            bassDrum.trigger(trans, is608 ? decay : decay * 0.45f, tune, 0.0f);
        }
        else if (voiceIndex == 1)
        {
            const float snappy = snareSnappyParam != nullptr ? snareSnappyParam->load() * (accented ? 1.35f : 1.0f) : 0.75f;
            snare.trigger(snareDecayParam->load(), snarePitchParam->load(), snappy, snareColorParam->load());
        }
        else if (voiceIndex == 2) clap.trigger(clapDecayParam->load(), clapPitchParam->load(), clapNoiseParam->load());
        else if (voiceIndex == 3) hiHat.trigger(SynthDrums606::kClosedHatSpec, hatsChDecayParam->load(), hatsPitchParam->load());
        else if (voiceIndex == 4) hiHat.trigger(SynthDrums606::kOpenHatSpec, hatsOhDecayParam->load(), hatsPitchParam->load());
        else if (voiceIndex == 5) lowTom.trigger(SynthDrums606::kLowTomSpec, ltomDecayParam->load(), ltomPitchParam->load());
        else if (voiceIndex == 6) highTom.trigger(SynthDrums606::kHighTomSpec, htomDecayParam->load(), htomPitchParam->load());
    };

        bool anySolo = false;
        for (int v = 0; v < 7; ++v)
        {
            if (mixerSoloParams[v] != nullptr && mixerSoloParams[v]->load() > 0.5f)
            {
                anySolo = true;
                break;
            }
        }

        bool voiceAudible[7];
        float voiceGain[7];
        float panL[7], panR[7];

        for (int v = 0; v < 7; ++v)
        {
            bool isMuted  = mixerMuteParams[v] != nullptr && mixerMuteParams[v]->load() > 0.5f;
            bool isSoloed = mixerSoloParams[v] != nullptr && mixerSoloParams[v]->load() > 0.5f;
            voiceAudible[v] = anySolo ? isSoloed : !isMuted;

            voiceGain[v] = mixerVolParams[v] != nullptr ? mixerVolParams[v]->load() : 1.0f;
            float panVal = voicePanParams[v] != nullptr ? voicePanParams[v]->load() : 0.0f;

            float angle = (panVal + 1.0f) * 0.25f * juce::MathConstants<float>::pi;
            panL[v] = std::cos(angle);
            panR[v] = std::sin(angle);
        }

        const float kickHeat = kickHeatParam != nullptr ? kickHeatParam->load() : 0.0f;

        bool routeDrive[7], routeDelay[7], routeReverb[7];
        for (int v = 0; v < 7; ++v)
        {
            routeDrive[v]  = (fxMatrixParams[0][v] == nullptr || fxMatrixParams[0][v]->load() > 0.5f);
            routeDelay[v]  = (fxMatrixParams[1][v] == nullptr || fxMatrixParams[1][v]->load() > 0.5f);
            routeReverb[v] = (fxMatrixParams[2][v] == nullptr || fxMatrixParams[2][v]->load() > 0.5f);
        }

        std::vector<float> dryBufL(totalNumSamples, 0.0f);
        std::vector<float> dryBufR(totalNumSamples, 0.0f);
        std::vector<float> driveBufL(totalNumSamples, 0.0f);
        std::vector<float> driveBufR(totalNumSamples, 0.0f);
        std::vector<float> delayBufL(totalNumSamples, 0.0f);
        std::vector<float> delayBufR(totalNumSamples, 0.0f);
        std::vector<float> revBufL(totalNumSamples, 0.0f);
        std::vector<float> revBufR(totalNumSamples, 0.0f);

        for (int sample = 0; sample < totalNumSamples; ++sample)
        {
            // 2. MIDI Real-Time Transport Commands (Start / Stop / Continue)
            for (; midiIterator != midiMessages.end() && (*midiIterator).samplePosition == sample; ++midiIterator)
            {
                auto message = (*midiIterator).getMessage();

                if (message.isMidiStart())
                {
                    seqIsPlaying.store(true);
                    currentSeqStep.store(0);
                    seqSampleCounter_ = 0.0;
                    lastTimelineStep_ = -1;
                }
                else if (message.isMidiStop())
                {
                    seqIsPlaying.store(false);
                }
                else if (message.isMidiContinue())
                {
                    seqIsPlaying.store(true);
                }
                else if (message.isNoteOn())
                {
                    const int note = message.getNoteNumber();
                    bool accented = message.getVelocity() > 100;
                    if (note == 35 || note == 36)      fireVoice(0, accented);
                    else if (note == 38 || note == 40) fireVoice(1, accented);
                    else if (note == 39)               fireVoice(2, accented);
                    else if (note == 42 || note == 44) fireVoice(3, accented);
                    else if (note == 46)               fireVoice(4, accented);
                    else if (note == 41 || note == 45) fireVoice(5, accented);
                    else if (note == 47 || note == 48 || note == 50) fireVoice(6, accented);
                }
            }

            // 3. Sequencer Clock (Locked to DAW Timeline or Internal Clock)
            if (dawPlaying && hasPpq)
            {
                // Sample-accurate song position lock to DAW grid
                double sampleFraction = static_cast<double>(sample) / static_cast<double>(totalNumSamples);
                double exactPpq = currentPpq + (sampleFraction * totalNumSamples / sampleRate_) * (bpm / 60.0);
                int step = static_cast<int>(std::floor(exactPpq * 4.0));
                if (step < 0) step = 0;
                step = step % maxSteps;

                if (step != lastTimelineStep_)
                {
                    lastTimelineStep_ = step;
                    currentSeqStep.store(step);

                    bool accented = stepPattern[0][step].load();
                    for (int track = 0; track < 7; ++track)
                    {
                        if (stepPattern[track + 1][step].load())
                            fireVoice(track, accented);
                    }
                }
            }
            else if (seqIsPlaying.load())
            {
                // Internal Clock (Standalone or unlinked playback)
                seqSampleCounter_ += 1.0;
                if (seqSampleCounter_ >= samplesPer16th)
                {
                    seqSampleCounter_ -= samplesPer16th;
                    int nextStep = (currentSeqStep.load() + 1) % maxSteps;
                    currentSeqStep.store(nextStep);

                    bool accented = stepPattern[0][nextStep].load();
                    for (int track = 0; track < 7; ++track)
                    {
                        if (stepPattern[track + 1][nextStep].load())
                            fireVoice(track, accented);
                    }
                }
            }
            else
            {
                lastTimelineStep_ = -1;
                seqSampleCounter_ = 0.0;
            }

            // 4. Manual UI Triggers
            for (int track = 0; track < 7; ++track)
            {
                if (manualTrigger[track].exchange(false))
                {
                    bool acc = manualTriggerAccent[track].exchange(false);
                    fireVoice(track, acc);
                }
            }

            // 5. Synthesize voices
            float v[7];
            v[0] = bassDrum.process();

            if (kickHeat > 0.001f)
            {
                float kGain = 1.0f + kickHeat * 3.5f;
                v[0] = std::tanh(v[0] * kGain) / std::sqrt(kGain);
            }

            v[1] = snare.process();
            v[2] = clap.process();
            v[3] = hiHat.process();
            v[4] = 0.0f;
            v[5] = lowTom.process();
            v[6] = highTom.process();

            float dryL = 0.0f, dryR = 0.0f;
            float drvL = 0.0f, drvR = 0.0f;
            float dlyL = 0.0f, dlyR = 0.0f;
            float revL = 0.0f, revR = 0.0f;

            for (int i = 0; i < 7; ++i)
            {
                if (!voiceAudible[i]) continue;

                float sigL = v[i] * voiceGain[i] * activeVoiceAccentGain[i] * panL[i];
                float sigR = v[i] * voiceGain[i] * activeVoiceAccentGain[i] * panR[i];

                dryL += sigL;
                dryR += sigR;

                if (routeDrive[i])  { drvL += sigL; drvR += sigR; }
                if (routeDelay[i])  { dlyL += sigL; dlyR += sigR; }
                if (routeReverb[i]) { revL += sigL; revR += sigR; }
            }

            dryBufL[sample] = dryL; dryBufR[sample] = dryR;
            driveBufL[sample] = drvL; driveBufR[sample] = drvR;
            delayBufL[sample] = dlyL; delayBufR[sample] = dlyR;
            revBufL[sample] = revL; revBufR[sample] = revR;
        }

        // --- 1. OVERDRIVE ---
        const float driveAmount = fxDriveParam != nullptr ? fxDriveParam->load() : 0.0f;
        const float driveTone   = fxToneParam != nullptr ? fxToneParam->load() : 0.5f;
        const float driveMix    = fxDriveMixParam != nullptr ? fxDriveMixParam->load() : 0.0f;

        if (driveMix > 0.001f)
        {
            for (int sample = 0; sample < totalNumSamples; ++sample)
            {
                float l = driveBufL[sample], r = driveBufR[sample];
                float preGain = 1.0f + driveAmount * 9.0f;
                float satL = std::tanh(l * preGain) + 0.08f * std::tanh(l * preGain * 2.0f) * (1.0f - driveTone);
                float satR = std::tanh(r * preGain) + 0.08f * std::tanh(r * preGain * 2.0f) * (1.0f - driveTone);

                float alphaLow = 0.08f + (1.0f - driveTone) * 0.40f;
                driveToneLowZ += alphaLow * ((satL + satR) * 0.5f - driveToneLowZ);
                float alphaHigh = 0.15f + driveTone * 0.75f;
                driveToneHighZ += alphaHigh * ((satL + satR) * 0.5f - driveToneHighZ);

                satL = satL * 0.6f + driveToneLowZ * 0.4f * (1.0f - driveTone) + driveToneHighZ * 0.3f * driveTone;
                satR = satR * 0.6f + driveToneLowZ * 0.4f * (1.0f - driveTone) + driveToneHighZ * 0.3f * driveTone;

                driveBufL[sample] = (satL - l) * driveMix;
                driveBufR[sample] = (satR - r) * driveMix;
            }
        }
        else
        {
            std::fill(driveBufL.begin(), driveBufL.end(), 0.0f);
            std::fill(driveBufR.begin(), driveBufR.end(), 0.0f);
        }

        // --- 2. PING-PONG DELAY ---
        static const float beatDivisions[8] = { 0.125f, 0.16667f, 0.25f, 0.33333f, 0.5f, 0.75f, 1.0f, 2.0f };
        const int syncIndex = fxDelayTimeSyncParam != nullptr ? juce::jlimit(0, 7, static_cast<int>(fxDelayTimeSyncParam->load())) : 4;
        const float delayTimeSec = (beatDivisions[syncIndex] * 60.0f) / std::max(20.0f, static_cast<float>(bpm));
        const float delayFdbk    = fxDelayFdbkParam != nullptr ? fxDelayFdbkParam->load() : 0.35f;
        const float delayMix     = fxDelayMixParam != nullptr ? fxDelayMixParam->load() : 0.0f;
        const bool delayPP       = fxDelayPingPongParam != nullptr && fxDelayPingPongParam->load() > 0.5f;

        if (delayMix > 0.001f)
        {
            for (int sample = 0; sample < totalNumSamples; ++sample)
            {
                float l = delayBufL[sample], r = delayBufR[sample];
                stereoDelay.process(l, r, delayTimeSec, delayFdbk, 1.0f, delayPP);
                delayBufL[sample] = l * delayMix;
                delayBufR[sample] = r * delayMix;
            }
        }
        else
        {
            std::fill(delayBufL.begin(), delayBufL.end(), 0.0f);
            std::fill(delayBufR.begin(), delayBufR.end(), 0.0f);
        }

        // --- 3. DATTORRO REVERB ---
        const float revSize = fxReverbSizeParam != nullptr ? fxReverbSizeParam->load() : 0.35f;
        const float revDamp = fxReverbDampParam != nullptr ? fxReverbDampParam->load() : 0.60f;
        const float revMix  = fxReverbMixParam != nullptr ? fxReverbMixParam->load() : 0.0f;
        const bool isHall   = fxReverbModeParam != nullptr && fxReverbModeParam->load() > 0.5f;

        if (revMix > 0.001f)
        {
            for (int sample = 0; sample < totalNumSamples; ++sample)
            {
                float l = 0.0f, r = 0.0f;
                plateReverb.process(revBufL[sample], revBufR[sample], l, r, revSize, revDamp, 1.0f, isHall);
                revBufL[sample] = l * revMix;
                revBufR[sample] = r * revMix;
            }
        }
        else
        {
            std::fill(revBufL.begin(), revBufL.end(), 0.0f);
            std::fill(revBufR.begin(), revBufR.end(), 0.0f);
        }

        // --- 4. MASTER FILTER & VOLUME ---
        const float masterVol = masterVolParam != nullptr ? masterVolParam->load() : 1.0f;
        const bool filterOn   = fxFilterOnParam != nullptr && fxFilterOnParam->load() > 0.5f;
        const bool filterHp   = fxFilterHpParam != nullptr && fxFilterHpParam->load() > 0.5f;
        const bool filter24db = fxFilter24dbParam != nullptr && fxFilter24dbParam->load() > 0.5f;
        const float fCutoff   = fxFilterCutoffParam != nullptr ? fxFilterCutoffParam->load() : 20000.0f;
        const float fRes      = fxFilterResParam != nullptr ? fxFilterResParam->load() : 0.0f;

        for (int sample = 0; sample < totalNumSamples; ++sample)
        {
            float outL = (dryBufL[sample] + driveBufL[sample] + delayBufL[sample] + revBufL[sample]) * masterVol;
            float outR = (dryBufR[sample] + driveBufR[sample] + delayBufR[sample] + revBufR[sample]) * masterVol;

            if (filterOn)
                masterFilter.process(outL, outR, fCutoff, fRes, filterHp, filter24db, sampleRate_);

            channelLeft[sample]  = outL;
            if (channelRight != nullptr)
                channelRight[sample] = outR;
        }

        // --- 5. 12-BIT SP-1200 SAMPLER ENGINE ---
        const bool is12Bit = global12BitParam != nullptr && global12BitParam->load() > 0.5f;
        if (is12Bit)
        {
            for (int sample = 0; sample < totalNumSamples; ++sample)
            {
                float l = channelLeft[sample];
                float r = (channelRight != nullptr) ? channelRight[sample] : l;

                vintageSampler.process(l, r, sampleRate_);

                channelLeft[sample] = l;
                if (channelRight != nullptr)
                    channelRight[sample] = r;
            }
        }
}

juce::AudioProcessorEditor* Super606AudioProcessor::createEditor()
{
    return new Super606AudioProcessorEditor(*this);
}

void Super606AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    std::unique_ptr<juce::XmlElement> xml = std::make_unique<juce::XmlElement>("SIMPLE606_STATE");

    auto* paramsXml = xml->createNewChildElement("PARAMETERS");
    for (auto* param : getParameters())
    {
        if (auto* p = dynamic_cast<juce::RangedAudioParameter*>(param))
        {
            auto* pXml = paramsXml->createNewChildElement("PARAM");
            pXml->setAttribute("id", p->getParameterID());
            pXml->setAttribute("value", static_cast<double>(p->convertFrom0to1(p->getValue())));
        }
    }

    auto* seqXml = xml->createNewChildElement("SEQUENCER");
    for (int t = 0; t < 8; ++t)
    {
        juce::String pat;
        for (int s = 0; s < 64; ++s)
            pat += stepPattern[t][s].load() ? "1" : "0";
        seqXml->setAttribute("track_" + juce::String(t), pat);
    }

    copyXmlToBinary(*xml, destData);
}

void Super606AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml == nullptr)
        xml = juce::parseXML(juce::String::createStringFromData(static_cast<const char*>(data), sizeInBytes));

    if (xml != nullptr)
    {
        if (auto* paramsXml = xml->getChildByName("PARAMETERS"))
        {
            for (auto* pXml : paramsXml->getChildIterator())
            {
                if (pXml->hasTagName("PARAM"))
                {
                    auto paramId = pXml->getStringAttribute("id");
                    float val = static_cast<float>(pXml->getDoubleAttribute("value"));
                    if (auto* param = apvts.getParameter(paramId))
                    {
                        param->setValueNotifyingHost(param->convertTo0to1(val));
                    }
                }
            }
        }

        if (auto* seqXml = xml->getChildByName("SEQUENCER"))
        {
            for (int t = 0; t < 8; ++t)
            {
                juce::String pat = seqXml->getStringAttribute("track_" + juce::String(t), "0");
                for (int s = 0; s < 64; ++s)
                    stepPattern[t][s].store(s < pat.length() && pat[s] == '1');
            }
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Super606AudioProcessor();
}
