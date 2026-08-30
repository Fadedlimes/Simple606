#include "PluginProcessor.h"
#include "PluginEditor.h"

Super606AudioProcessorEditor::Super606AudioProcessorEditor(Super606AudioProcessor& p)
: AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&customLookAndFeel);
    setOpaque(true);
    setWantsKeyboardFocus(true);

    // --- Top Tab Navigation ---
    auto setupTabBtn = [this](juce::TextButton& btn, const juce::String& text, Tab tab)
    {
        btn.setButtonText(text);
        btn.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff111215));
        btn.onClick = [this, tab]()
        {
            currentTab = tab;
            resized();
            repaint();
        };
        addAndMakeVisible(btn);
    };

    setupTabBtn(drumsTabBtn,    "DRUMS", Tab::Drums);
    setupTabBtn(padsTabBtn,     "PADS", Tab::Pads);
    setupTabBtn(seqTabBtn,      "SEQUENCER", Tab::Sequencer);
    setupTabBtn(mixerTabBtn,    "MIXER", Tab::Mixer);
    setupTabBtn(fxTabBtn,       "FX RACK", Tab::FX);
    setupTabBtn(settingsTabBtn, "SETTINGS", Tab::Settings);

    // --- DRUMS TAB CONTROLS ---
    setupKnob(kickTransientSlider, kickTransientLabel, "TRANS", kickTransientAtt, "kick_transient");
    setupKnob(kickDecaySlider,     kickDecayLabel,     "DECAY", kickDecayAtt,     "kick_decay");
    setupKnob(kickTuneSlider,      kickTuneLabel,      "TUNE",  kickTuneAtt,      "kick_tune");
    setupKnob(kickHeatSlider,      kickHeatLabel,      "HEAT",  kickHeatAtt,      "kick_heat");
    setupKnob(kickPanSlider,       kickPanLabel,       "PAN",   kickPanAtt,       "pan_bd");
    kick608Toggle.setButtonText("608 XL");
    addAndMakeVisible(kick608Toggle);
    kick608Att = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "kick_608_mode", kick608Toggle);

    setupKnob(snareDecaySlider,    snareDecayLabel,    "DECAY",  snareDecayAtt,   "snare_decay");
    setupKnob(snarePitchSlider,    snarePitchLabel,    "TUNE",   snarePitchAtt,   "snare_pitch");
    setupKnob(snareSnappySlider,   snareSnappyLabel,   "SNAPPY", snareSnappyAtt,  "snare_snappy");
    setupKnob(snareColorSlider,    snareColorLabel,    "COLOR",  snareColorAtt,   "snare_color");
    setupKnob(snarePanSlider,      snarePanLabel,      "PAN",    snarePanAtt,     "pan_sn");

    setupKnob(clapDecaySlider,     clapDecayLabel,     "DECAY", clapDecayAtt,     "clap_decay");
    setupKnob(clapPitchSlider,     clapPitchLabel,     "TUNE",  clapPitchAtt,     "clap_pitch");
    setupKnob(clapNoiseSlider,     clapNoiseLabel,     "NOISE", clapNoiseAtt,     "clap_noise");
    setupKnob(clapPanSlider,       clapPanLabel,       "PAN",   clapPanAtt,       "pan_cl");

    setupKnob(hatsChDecaySlider,   hatsChDecayLabel,   "CH DEC", hatsChDecayAtt,  "hats_ch_decay");
    setupKnob(hatsOhDecaySlider,   hatsOhDecayLabel,   "OH DEC", hatsOhDecayAtt,  "hats_oh_decay");
    setupKnob(hatsPitchSlider,     hatsPitchLabel,     "TUNE",   hatsPitchAtt,    "hats_pitch");
    setupKnob(hatsPanSlider,       hatsPanLabel,       "PAN",    hatsPanAtt,      "pan_ch");

    setupKnob(cyDecaySlider,       cyDecayLabel,       "DECAY",  cyDecayAtt,      "cy_decay");
    setupKnob(cyPitchSlider,       cyPitchLabel,       "TUNE",   cyPitchAtt,      "cy_pitch");
    setupKnob(cyPanSlider,         cyPanLabel,         "PAN",    cyPanAtt,        "pan_cy");

    setupKnob(ltomDecaySlider,     ltomDecayLabel,     "DECAY", ltomDecayAtt,     "ltom_decay");
    setupKnob(ltomPitchSlider,     ltomPitchLabel,     "TUNE",  ltomPitchAtt,     "ltom_pitch");
    setupKnob(ltomPanSlider,       ltomPanLabel,       "PAN",   ltomPanAtt,       "pan_lt");

    setupKnob(htomDecaySlider,     htomDecayLabel,     "DECAY", htomDecayAtt,     "htom_decay");
    setupKnob(htomPitchSlider,     htomPitchLabel,     "TUNE",  htomPitchAtt,     "htom_pitch");
    setupKnob(htomPanSlider,       htomPanLabel,       "PAN",   htomPanAtt,       "pan_ht");

    // --- PADS TAB (8 Drum Pads) ---
    const juce::String padTitles[8] = {
        "BASS DRUM\n[A / 1]",
        "SNARE\n[S / 2]",
        "CLAP\n[D / 3]",
        "CLOSED HAT\n[F / 4]",
        "OPEN HAT\n[G / 5]",
        "CYMBAL\n[Y / 6]",
        "LOW TOM\n[H / 7]",
        "HIGH TOM\n[J / 8]"
    };

    for (int v = 0; v < 8; ++v)
    {
        drumPads[v].setButtonText(padTitles[v]);
        drumPads[v].setComponentID("drumpad");
        drumPads[v].setColour(juce::TextButton::textColourOffId, juce::Colour(0xffe9d5ff));
        drumPads[v].setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        drumPads[v].onClick = [this, v]() { audioProcessor.triggerVoice(v, true); };
        addAndMakeVisible(drumPads[v]);
    }

    // --- MIXER TAB (8 Channels + Master) ---
    const juce::String vNames[8] = { "bd", "sn", "cl", "ch", "oh", "cy", "lt", "ht" };
    const juce::String vTitles[8] = { "BD", "SN", "CL", "CH", "OH", "CY", "LT", "HT" };

    for (int v = 0; v < 8; ++v)
    {
        setupFader(mixerFaders[v], mixerFaderLabels[v], vTitles[v], mixerFaderAtts[v], "vol_" + vNames[v]);
        setupKnob(mixerPanDials[v], mixerPanLabels[v], "PAN", mixerPanAtts[v], "pan_" + vNames[v]);

        mixerMuteBtns[v].setButtonText("M");
        mixerMuteBtns[v].setClickingTogglesState(true);
        addAndMakeVisible(mixerMuteBtns[v]);
        mixerMuteAtts[v] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "mute_" + vNames[v], mixerMuteBtns[v]);

        mixerSoloBtns[v].setButtonText("S");
        mixerSoloBtns[v].setClickingTogglesState(true);
        addAndMakeVisible(mixerSoloBtns[v]);
        mixerSoloAtts[v] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "solo_" + vNames[v], mixerSoloBtns[v]);
    }
    setupFader(masterFader, masterFaderLabel, "MASTER", masterFaderAtt, "vol_master");

    // --- SEQUENCER TAB (9 Tracks) ---
    transportToggleBtn.setButtonText("START");
    transportToggleBtn.setComponentID("transport");
    transportToggleBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff22c55e));
    transportToggleBtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    transportToggleBtn.onClick = [this]()
    {
        bool playing = !(audioProcessor.seqIsPlaying.load() || audioProcessor.isDawPlaying.load());
        audioProcessor.seqIsPlaying.store(playing);
        transportToggleBtn.setButtonText(playing ? "STOP" : "START");
        transportToggleBtn.setColour(juce::TextButton::buttonColourId, playing ? juce::Colour(0xffef4444) : juce::Colour(0xff22c55e));
        transportToggleBtn.repaint();
    };
    addAndMakeVisible(transportToggleBtn);

    clearBtn.setButtonText("CLEAR");
    clearBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff6b7280));
    clearBtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    clearBtn.onClick = [this]()
    {
        for (int t = 0; t < 9; ++t)
            for (int s = 0; s < 64; ++s)
                audioProcessor.stepPattern[t][s].store(false);
        resized();
    };
    addAndMakeVisible(clearBtn);

    copyPageBtn.setButtonText("COPY");
    copyPageBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff4b5563));
    copyPageBtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    copyPageBtn.onClick = [this]()
    {
        const int pageOffset = seqCurrentPage * 16;
        for (int t = 0; t < 9; ++t)
            for (int s = 0; s < 16; ++s)
                seqPageClipboard[t][s] = audioProcessor.stepPattern[t][pageOffset + s].load();
    };
    addAndMakeVisible(copyPageBtn);

    pastePageBtn.setButtonText("PASTE");
    pastePageBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff4b5563));
    pastePageBtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    pastePageBtn.onClick = [this]()
    {
        const int pageOffset = seqCurrentPage * 16;
        for (int t = 0; t < 9; ++t)
            for (int s = 0; s < 16; ++s)
                audioProcessor.stepPattern[t][pageOffset + s].store(seqPageClipboard[t][s]);
        resized();
        repaint();
    };
    addAndMakeVisible(pastePageBtn);

    const juce::String pageNames[4] = { "1", "2", "3", "4" };
    for (int p = 0; p < 4; ++p)
    {
        pageBtns[p].setButtonText(pageNames[p]);
        pageBtns[p].onClick = [this, p]()
        {
            seqCurrentPage = p;
            resized();
            repaint();
        };
        addAndMakeVisible(pageBtns[p]);
    }

    seqLengthBox.addItemList({ "16 STEPS", "32 STEPS", "48 STEPS", "64 STEPS" }, 1);
    addAndMakeVisible(seqLengthBox);
    seqLengthAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "seq_length", seqLengthBox);

    setupKnob(bpmSlider, bpmLabel, "TEMPO", bpmAtt, "seq_bpm");

    // Track Mute & Solo on Sequencer (8 drum voices)
    for (int v = 0; v < 8; ++v)
    {
        seqMuteBtns[v].setButtonText("M");
        seqMuteBtns[v].setClickingTogglesState(true);
        addAndMakeVisible(seqMuteBtns[v]);
        seqMuteAtts[v] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "mute_" + vNames[v], seqMuteBtns[v]);

        seqSoloBtns[v].setButtonText("S");
        seqSoloBtns[v].setClickingTogglesState(true);
        addAndMakeVisible(seqSoloBtns[v]);
        seqSoloAtts[v] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "solo_" + vNames[v], seqSoloBtns[v]);
    }

    const juce::String trackNames[9] = { "ACC", "BD", "SN", "CL", "CH", "OH", "CY", "LT", "HT" };
    for (int t = 0; t < 9; ++t)
    {
        trackLabels[t].setText(trackNames[t], juce::dontSendNotification);
        trackLabels[t].setFont(juce::Font(11.0f, juce::Font::bold));
        trackLabels[t].setColour(juce::Label::textColourId, t == 0 ? customLookAndFeel.accentTrackColour : juce::Colour(0xff111215));
        trackLabels[t].setJustificationType(juce::Justification::centredRight);
        addAndMakeVisible(trackLabels[t]);

        for (int s = 0; s < 16; ++s)
        {
            auto& btn = stepButtons[t][s];
            btn.setButtonText("");
            if (t == 0) btn.setComponentID("accent");
            addAndMakeVisible(btn);
        }
    }

    // --- FX TAB MATRIX (3 Rows x 8 Columns) ---
    const juce::String fxRowTitles[3] = { "DRIVE", "DELAY", "REVERB" };
    const juce::String fxRowIds[3]    = { "drive", "delay", "reverb" };

    for (int r = 0; r < 3; ++r)
    {
        fxRowLabels[r].setText(fxRowTitles[r], juce::dontSendNotification);
        fxRowLabels[r].setFont(juce::Font(10.5f, juce::Font::bold));
        fxRowLabels[r].setColour(juce::Label::textColourId, juce::Colour(0xff111215));
        fxRowLabels[r].setJustificationType(juce::Justification::centredRight);
        addAndMakeVisible(fxRowLabels[r]);

        for (int v = 0; v < 8; ++v)
        {
            auto& btn = fxMatrixToggles[r][v];
            btn.setButtonText("");
            addAndMakeVisible(btn);
            juce::String id = "route_" + fxRowIds[r] + "_" + vNames[v];
            fxMatrixAtts[r][v] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, id, btn);
        }
    }

    for (int v = 0; v < 8; ++v)
    {
        fxColLabels[v].setText(vTitles[v], juce::dontSendNotification);
        fxColLabels[v].setFont(juce::Font(10.5f, juce::Font::bold));
        fxColLabels[v].setColour(juce::Label::textColourId, juce::Colour(0xff111215));
        fxColLabels[v].setJustificationType(juce::Justification::centred);
        addAndMakeVisible(fxColLabels[v]);
    }

    setupKnob(fxDriveSlider,    fxDriveLabel,    "DRIVE", fxDriveAtt,    "fx_drive");
    setupKnob(fxToneSlider,     fxToneLabel,     "TONE",  fxToneAtt,     "fx_tone");
    setupKnob(fxDriveMixSlider, fxDriveMixLabel, "MIX",   fxDriveMixAtt, "fx_drive_mix");

    setupKnob(fxDelayTimeSlider, fxDelayTimeLabel, "TIME", fxDelayTimeAtt, "fx_delay_time_sync");
    setupKnob(fxDelayFdbkSlider, fxDelayFdbkLabel, "FDBK", fxDelayFdbkAtt, "fx_delay_fdbk");
    setupKnob(fxDelayMixSlider,  fxDelayMixLabel,  "MIX",  fxDelayMixAtt,  "fx_delay_mix");
    fxDelayPingPongToggle.setButtonText("PING PONG");
    addAndMakeVisible(fxDelayPingPongToggle);
    fxDelayPingPongAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "fx_delay_pingpong", fxDelayPingPongToggle);

    setupKnob(fxReverbSizeSlider, fxReverbSizeLabel, "SIZE", fxReverbSizeAtt, "fx_reverb_size");
    setupKnob(fxReverbDampSlider, fxReverbDampLabel, "DAMP", fxReverbDampAtt, "fx_reverb_damp");
    setupKnob(fxReverbMixSlider,  fxReverbMixLabel,  "MIX",  fxReverbMixAtt,  "fx_reverb_mix");
    fxReverbModeToggle.setButtonText("HALL");
    addAndMakeVisible(fxReverbModeToggle);
    fxReverbModeAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "fx_reverb_mode", fxReverbModeToggle);

    setupKnob(fxFilterCutoffSlider, fxFilterCutoffLabel, "CUTOFF", fxFilterCutoffAtt, "fx_filter_cutoff");
    setupKnob(fxFilterResSlider,    fxFilterResLabel,    "RES",    fxFilterResAtt,    "fx_filter_res");

    fxFilterOnToggle.setButtonText("ON");
    addAndMakeVisible(fxFilterOnToggle);
    fxFilterOnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "fx_filter_on", fxFilterOnToggle);

    fxFilterHpToggle.setButtonText("HP");
    addAndMakeVisible(fxFilterHpToggle);
    fxFilterHpAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "fx_filter_hp", fxFilterHpToggle);

    fxFilter24dbToggle.setButtonText("24dB");
    addAndMakeVisible(fxFilter24dbToggle);
    fxFilter24dbAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "fx_filter_24db", fxFilter24dbToggle);

    // --- SETTINGS TAB ---
    themeColorBox.addItemList({
        "NEON VIOLET",
        "ELECTRIC BLUE",
        "TR RED",
        "SUNSET YELLOW",
        "AMBER CRT",
        "ACID GREEN",
        "CYBER TEAL",
        "HOT PINK",
        "TRANS PRIDE"
    }, 1);
    addAndMakeVisible(themeColorBox);
    themeColorAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "ui_theme_color", themeColorBox);
    themeColorLabel.setText("THEME COLOR", juce::dontSendNotification);
    themeColorLabel.setFont(juce::Font(10.5f, juce::Font::bold));
    addAndMakeVisible(themeColorLabel);

    accentColorBox.addItemList({ "ORANGE", "YELLOW", "RED", "WHITE", "PURPLE" }, 1);
    addAndMakeVisible(accentColorBox);
    accentColorAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "ui_accent_color", accentColorBox);
    accentColorLabel.setText("ACCENT TRACK COLOR", juce::dontSendNotification);
    accentColorLabel.setFont(juce::Font(10.5f, juce::Font::bold));
    addAndMakeVisible(accentColorLabel);

    global12BitToggle.setButtonText("12-BIT SAMPLER CRUNCH");
    addAndMakeVisible(global12BitToggle);
    global12BitAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "global_12bit", global12BitToggle);

    transPrideToggle.setButtonText("TRANS PRIDE ACCENT");
    addAndMakeVisible(transPrideToggle);
    transPrideAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "ui_trans_pride", transPrideToggle);

    savePresetBtn.setButtonText("SAVE PRESET");
    savePresetBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff4b5563));
    savePresetBtn.onClick = [this]()
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Save Simple606 Preset",
            juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("Preset.simple606"),
                                                          "*.simple606");

        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting,
                                 [this](const juce::FileChooser& fc)
                                 {
                                     auto file = fc.getResult();
                                     if (file != juce::File())
                                     {
                                         juce::MemoryBlock mb;
                                         audioProcessor.getStateInformation(mb);
                                         std::unique_ptr<juce::XmlElement> xml(juce::AudioProcessor::getXmlFromBinary(mb.getData(), static_cast<int>(mb.getSize())));
                                         if (xml != nullptr)
                                             file.replaceWithText(xml->toString());
                                         else
                                             file.replaceWithData(mb.getData(), mb.getSize());
                                     }
                                 });
    };
    addAndMakeVisible(savePresetBtn);

    loadPresetBtn.setButtonText("LOAD PRESET");
    loadPresetBtn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff4b5563));
    loadPresetBtn.onClick = [this]()
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Load Simple606 Preset",
            juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
                                                          "*.simple606");

        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                                 [this](const juce::FileChooser& fc)
                                 {
                                     auto file = fc.getResult();
                                     if (file.existsAsFile())
                                     {
                                         juce::MemoryBlock mb;
                                         file.loadFileAsData(mb);
                                         audioProcessor.setStateInformation(mb.getData(), static_cast<int>(mb.getSize()));
                                         updateThemeColors();
                                         resized();
                                         repaint();
                                     }
                                 });
    };
    addAndMakeVisible(loadPresetBtn);

    setResizable(true, true);
    setResizeLimits(880, 380, 1920, 1080);
    setSize(960, 420);

    startTimerHz(30);
}

Super606AudioProcessorEditor::~Super606AudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void Super606AudioProcessorEditor::updateThemeColors()
{
    auto* themeParam = audioProcessor.apvts.getRawParameterValue("ui_theme_color");
    int themeIdx = themeParam != nullptr ? static_cast<int>(themeParam->load()) : 0;

    customLookAndFeel.isTransPrideTheme = (themeIdx == 8);

    switch (themeIdx)
    {
        case 1:
            customLookAndFeel.mainGlowColour = juce::Colour(0xff60a5fa);
            customLookAndFeel.mainDarkGlowColour = juce::Colour(0xff2563eb);
            break;
        case 2:
            customLookAndFeel.mainGlowColour = juce::Colour(0xffff3b30);
            customLookAndFeel.mainDarkGlowColour = juce::Colour(0xffdc2626);
            break;
        case 3:
            customLookAndFeel.mainGlowColour = juce::Colour(0xfffacc15);
            customLookAndFeel.mainDarkGlowColour = juce::Colour(0xffd97706);
            break;
        case 4:
            customLookAndFeel.mainGlowColour = juce::Colour(0xfffb923c);
            customLookAndFeel.mainDarkGlowColour = juce::Colour(0xffea580c);
            break;
        case 5:
            customLookAndFeel.mainGlowColour = juce::Colour(0xff4ade80);
            customLookAndFeel.mainDarkGlowColour = juce::Colour(0xff16a34a);
            break;
        case 6:
            customLookAndFeel.mainGlowColour = juce::Colour(0xff2dd4bf);
            customLookAndFeel.mainDarkGlowColour = juce::Colour(0xff0d9488);
            break;
        case 7:
            customLookAndFeel.mainGlowColour = juce::Colour(0xfff472b6);
            customLookAndFeel.mainDarkGlowColour = juce::Colour(0xffdb2777);
            break;
        case 8:
            customLookAndFeel.mainGlowColour = juce::Colour(0xff5bcefa);
            customLookAndFeel.mainDarkGlowColour = juce::Colour(0xfff5a9b8);
            break;
        default:
            customLookAndFeel.mainGlowColour = juce::Colour(0xffc084fc);
            customLookAndFeel.mainDarkGlowColour = juce::Colour(0xffa855f7);
            break;
    }

    auto* accParam = audioProcessor.apvts.getRawParameterValue("ui_accent_color");
    int accIdx = accParam != nullptr ? static_cast<int>(accParam->load()) : 0;
    switch (accIdx)
    {
        case 1: customLookAndFeel.accentTrackColour = juce::Colour(0xffeab308); break;
        case 2: customLookAndFeel.accentTrackColour = juce::Colour(0xffef4444); break;
        case 3: customLookAndFeel.accentTrackColour = juce::Colour(0xffffffff); break;
        case 4: customLookAndFeel.accentTrackColour = juce::Colour(0xffc084fc); break;
        default: customLookAndFeel.accentTrackColour = juce::Colour(0xfff97316); break;
    }

    trackLabels[0].setColour(juce::Label::textColourId, customLookAndFeel.accentTrackColour);
}

void Super606AudioProcessorEditor::timerCallback()
{
    updateThemeColors();

    bool isPlaying = audioProcessor.seqIsPlaying.load() || audioProcessor.isDawPlaying.load();
    transportToggleBtn.setButtonText(isPlaying ? "STOP" : "START");
    transportToggleBtn.setColour(juce::TextButton::buttonColourId, isPlaying ? juce::Colour(0xffef4444) : juce::Colour(0xff22c55e));
    transportToggleBtn.repaint();

    if (currentTab == Tab::Sequencer)
        repaint();
}

bool Super606AudioProcessorEditor::keyPressed(const juce::KeyPress& key)
{
    auto keyChar = juce::CharacterFunctions::toLowerCase(key.getTextCharacter());
    int voiceToTrigger = -1;

    if      (keyChar == 'a' || keyChar == '1' || keyChar == 'z') voiceToTrigger = 0; // BD
    else if (keyChar == 's' || keyChar == '2' || keyChar == 'x') voiceToTrigger = 1; // SN
    else if (keyChar == 'd' || keyChar == '3' || keyChar == 'c') voiceToTrigger = 2; // CL
    else if (keyChar == 'f' || keyChar == '4' || keyChar == 'v') voiceToTrigger = 3; // CH
    else if (keyChar == 'g' || keyChar == '5' || keyChar == 'b') voiceToTrigger = 4; // OH
    else if (keyChar == 'y' || keyChar == '6' || keyChar == 'n') voiceToTrigger = 5; // CY
    else if (keyChar == 'h' || keyChar == '7' || keyChar == 'm') voiceToTrigger = 6; // LT
    else if (keyChar == 'j' || keyChar == '8' || keyChar == ',') voiceToTrigger = 7; // HT
    else if (key.isKeyCurrentlyDown(juce::KeyPress::spaceKey))
    {
        bool playing = !(audioProcessor.seqIsPlaying.load() || audioProcessor.isDawPlaying.load());
        audioProcessor.seqIsPlaying.store(playing);
        transportToggleBtn.setButtonText(playing ? "STOP" : "START");
        transportToggleBtn.setColour(juce::TextButton::buttonColourId, playing ? juce::Colour(0xffef4444) : juce::Colour(0xff22c55e));
        transportToggleBtn.repaint();
        return true;
    }

    if (voiceToTrigger >= 0)
    {
        audioProcessor.triggerVoice(voiceToTrigger, true);
        drumPads[voiceToTrigger].setState(juce::Button::buttonDown);
        juce::Timer::callAfterDelay(120, [this, voiceToTrigger]()
        {
            drumPads[voiceToTrigger].setState(juce::Button::buttonNormal);
        });
        return true;
    }

    return false;
}

void Super606AudioProcessorEditor::setupKnob(juce::Slider& slider, juce::Label& label, const juce::String& text,
                                             std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                                             const juce::String& paramID)
{
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 14);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xff111215));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font(10.5f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, juce::Colour(0xff111215));
    addAndMakeVisible(label);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, paramID, slider);
}

void Super606AudioProcessorEditor::setupFader(juce::Slider& slider, juce::Label& label, const juce::String& text,
                                              std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                                              const juce::String& paramID)
{
    slider.setSliderStyle(juce::Slider::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 14);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xff111215));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font(10.5f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, juce::Colour(0xff111215));
    addAndMakeVisible(label);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, paramID, slider);
}

void Super606AudioProcessorEditor::layoutSection(juce::Rectangle<int> area,
                                                 const std::vector<std::pair<juce::Slider*, juce::Label*>>& knobs)
{
    area.removeFromTop(24);
    const int count = static_cast<int>(knobs.size());
    if (count == 0) return;
    const int knobWidth = area.getWidth() / count;

    for (const auto& k : knobs)
    {
        auto slot = area.removeFromLeft(knobWidth);
        int maxKnobHeight = juce::jmin(slot.getWidth() + 12, slot.getHeight() - 15);
        auto knobSlot = slot.removeFromTop(maxKnobHeight);
        k.first->setBounds(knobSlot.reduced(2, 0));
        k.second->setBounds(slot.removeFromTop(15));
    }
}

void Super606AudioProcessorEditor::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();

    // 1. Brushed Aluminum Faceplate
    juce::ColourGradient silverGrad(juce::Colour(0xffe2e5ea), 0.0f, 0.0f,
                                    juce::Colour(0xffb2b6bf), 0.0f, bounds.getBottom(), false);
    silverGrad.addColour(0.48, juce::Colour(0xffcfd3db));
    silverGrad.addColour(0.52, juce::Colour(0xffc4c8d1));
    g.setGradientFill(silverGrad);
    g.fillAll();

    // 2. Top Header Bar
    g.setColour(juce::Colour(0xff7a7e87));
    g.drawLine(0.0f, 40.0f, bounds.getRight(), 40.0f, 1.0f);

    juce::Rectangle<float> activeTabLine;
    if (currentTab == Tab::Drums) activeTabLine = drumsTabBtn.getBounds().toFloat();
    else if (currentTab == Tab::Pads) activeTabLine = padsTabBtn.getBounds().toFloat();
    else if (currentTab == Tab::Sequencer) activeTabLine = seqTabBtn.getBounds().toFloat();
    else if (currentTab == Tab::Mixer) activeTabLine = mixerTabBtn.getBounds().toFloat();
    else if (currentTab == Tab::FX) activeTabLine = fxTabBtn.getBounds().toFloat();
    else if (currentTab == Tab::Settings) activeTabLine = settingsTabBtn.getBounds().toFloat();

    if (customLookAndFeel.isTransPrideTheme)
    {
        juce::ColourGradient pride(juce::Colour(0xff5bcefa), activeTabLine.getX(), 0.0f,
                                   juce::Colour(0xff5bcefa), activeTabLine.getRight(), 0.0f, false);
        pride.addColour(0.25, juce::Colour(0xfff5a9b8));
        pride.addColour(0.50, juce::Colour(0xffffffff));
        pride.addColour(0.75, juce::Colour(0xfff5a9b8));
        g.setGradientFill(pride);
        g.fillRect(activeTabLine.getX(), 37.0f, activeTabLine.getWidth(), 3.0f);
    }
    else
    {
        g.setColour(customLookAndFeel.mainDarkGlowColour);
        g.fillRect(activeTabLine.getX(), 37.0f, activeTabLine.getWidth(), 3.0f);
    }

    // Title
    g.setColour(juce::Colour(0xff111215));
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawText("SIMPLE 606", 16, 8, 110, 24, juce::Justification::left);

    // 3. Draw Capsule Headers
    for (const auto& sec : sections)
    {
        auto b = sec.bounds;
        auto headerRect = b.removeFromTop(20).toFloat().reduced(4.0f, 1.0f);

        g.setColour(juce::Colour(0x35ffffff));
        g.fillRoundedRectangle(headerRect, 8.0f);

        g.setColour(juce::Colour(0xff686d77));
        g.drawRoundedRectangle(headerRect, 8.0f, 1.2f);

        g.setColour(juce::Colour(0xff111215));
        g.setFont(juce::Font(10.5f, juce::Font::bold));
        g.drawText(sec.name, headerRect, juce::Justification::centred);
    }

    // 4. Trans Pride Special Gradient on Kick TRANS label
    auto* transPrideParam = audioProcessor.apvts.getRawParameterValue("ui_trans_pride");
    if (currentTab == Tab::Drums && ((transPrideParam != nullptr && transPrideParam->load() > 0.5f) || customLookAndFeel.isTransPrideTheme))
    {
        auto tRect = kickTransientLabel.getBounds().toFloat();
        juce::ColourGradient pride(juce::Colour(0xff5bcefa), tRect.getX(), 0.0f,
                                   juce::Colour(0xff5bcefa), tRect.getRight(), 0.0f, false);
        pride.addColour(0.25, juce::Colour(0xfff5a9b8));
        pride.addColour(0.50, juce::Colour(0xffffffff));
        pride.addColour(0.75, juce::Colour(0xfff5a9b8));

        g.setGradientFill(pride);
        g.fillRoundedRectangle(tRect.reduced(6.0f, 1.0f), 3.0f);
        g.setColour(juce::Colour(0xff111215));
        g.setFont(juce::Font(10.5f, juce::Font::bold));
        g.drawText("TRANS", tRect, juce::Justification::centred);
    }

    // 5. Sequencer Playhead Highlight
    if (currentTab == Tab::Sequencer)
    {
        const int activeStep = audioProcessor.currentSeqStep.load();
        const int pageOffset = seqCurrentPage * 16;
        if (activeStep >= pageOffset && activeStep < pageOffset + 16)
        {
            int visibleStep = activeStep - pageOffset;
            auto firstBtn = stepButtons[0][visibleStep].getBounds();
            auto lastBtn  = stepButtons[8][visibleStep].getBounds();

            auto colRect = juce::Rectangle<float>(static_cast<float>(firstBtn.getX() - 3),
                                                  static_cast<float>(firstBtn.getY() - 8),
                                                  static_cast<float>(firstBtn.getWidth() + 6),
                                                  static_cast<float>((lastBtn.getBottom() - firstBtn.getY()) + 16));

            g.setColour(customLookAndFeel.mainGlowColour.withAlpha(0.25f));
            g.fillRoundedRectangle(colRect, 5.0f);

            g.setColour(customLookAndFeel.mainGlowColour);
            g.drawRoundedRectangle(colRect, 5.0f, 2.0f);

            g.setColour(juce::Colour(0xffffffff));
            g.fillEllipse(colRect.getCentreX() - 3.0f, colRect.getY() + 2.0f, 6.0f, 6.0f);
        }
    }
}

void Super606AudioProcessorEditor::resized()
{
    sections.clear();
    const int marginX = 14;
    const int topBarH = 48;
    const int availW = getWidth() - (marginX * 2);
    const int availH = getHeight() - topBarH - 12;

    // Navigation Tabs: DRUMS -> PADS -> SEQUENCER -> MIXER -> FX RACK -> SETTINGS
    drumsTabBtn.setBounds(130, 8, 70, 26);
    padsTabBtn.setBounds(205, 8, 65, 26);
    seqTabBtn.setBounds(275, 8, 90, 26);
    mixerTabBtn.setBounds(370, 8, 65, 26);
    fxTabBtn.setBounds(440, 8, 75, 26);
    settingsTabBtn.setBounds(520, 8, 80, 26);

    auto setDrumsVisible = [this](bool v)
    {
        kickTransientSlider.setVisible(v); kickTransientLabel.setVisible(v);
        kickDecaySlider.setVisible(v); kickDecayLabel.setVisible(v);
        kickTuneSlider.setVisible(v); kickTuneLabel.setVisible(v);
        kickHeatSlider.setVisible(v); kickHeatLabel.setVisible(v);
        kickPanSlider.setVisible(v);  kickPanLabel.setVisible(v);
        kick608Toggle.setVisible(v);

        snareDecaySlider.setVisible(v); snareDecayLabel.setVisible(v);
        snarePitchSlider.setVisible(v); snarePitchLabel.setVisible(v);
        snareSnappySlider.setVisible(v); snareSnappyLabel.setVisible(v);
        snareColorSlider.setVisible(v); snareColorLabel.setVisible(v);
        snarePanSlider.setVisible(v);   snarePanLabel.setVisible(v);

        clapDecaySlider.setVisible(v); clapDecayLabel.setVisible(v);
        clapPitchSlider.setVisible(v); clapPitchLabel.setVisible(v);
        clapNoiseSlider.setVisible(v); clapNoiseLabel.setVisible(v);
        clapPanSlider.setVisible(v);   clapPanLabel.setVisible(v);

        hatsChDecaySlider.setVisible(v); hatsChDecayLabel.setVisible(v);
        hatsOhDecaySlider.setVisible(v); hatsOhDecayLabel.setVisible(v);
        hatsPitchSlider.setVisible(v); hatsPitchLabel.setVisible(v);
        hatsPanSlider.setVisible(v);   hatsPanLabel.setVisible(v);

        cyDecaySlider.setVisible(v); cyDecayLabel.setVisible(v);
        cyPitchSlider.setVisible(v); cyPitchLabel.setVisible(v);
        cyPanSlider.setVisible(v);   cyPanLabel.setVisible(v);

        ltomDecaySlider.setVisible(v); ltomDecayLabel.setVisible(v);
        ltomPitchSlider.setVisible(v); ltomPitchLabel.setVisible(v);
        ltomPanSlider.setVisible(v);   ltomPanLabel.setVisible(v);

        htomDecaySlider.setVisible(v); htomDecayLabel.setVisible(v);
        htomPitchSlider.setVisible(v); htomPitchLabel.setVisible(v);
        htomPanSlider.setVisible(v);   htomPanLabel.setVisible(v);
    };

    auto setPadsVisible = [this](bool v)
    {
        for (int p = 0; p < 8; ++p) drumPads[p].setVisible(v);
    };

        auto setSeqVisible = [this](bool v)
        {
            transportToggleBtn.setVisible(v); clearBtn.setVisible(v);
            copyPageBtn.setVisible(v); pastePageBtn.setVisible(v);
            for (int p = 0; p < 4; ++p) pageBtns[p].setVisible(v);
            seqLengthBox.setVisible(v);
            bpmSlider.setVisible(v); bpmLabel.setVisible(v);
            for (int vIdx = 0; vIdx < 8; ++vIdx)
            {
                seqMuteBtns[vIdx].setVisible(v);
                seqSoloBtns[vIdx].setVisible(v);
            }
            for (int t = 0; t < 9; ++t)
            {
                trackLabels[t].setVisible(v);
                for (int s = 0; s < 16; ++s) stepButtons[t][s].setVisible(v);
            }
        };

        auto setMixerVisible = [this](bool v)
        {
            for (int vIdx = 0; vIdx < 8; ++vIdx)
            {
                mixerFaders[vIdx].setVisible(v); mixerFaderLabels[vIdx].setVisible(v);
                mixerPanDials[vIdx].setVisible(v); mixerPanLabels[vIdx].setVisible(v);
                mixerMuteBtns[vIdx].setVisible(v); mixerSoloBtns[vIdx].setVisible(v);
            }
            masterFader.setVisible(v); masterFaderLabel.setVisible(v);
        };

        auto setFxVisible = [this](bool v)
        {
            for (int r = 0; r < 3; ++r)
            {
                fxRowLabels[r].setVisible(v);
                for (int col = 0; col < 8; ++col) fxMatrixToggles[r][col].setVisible(v);
            }
            for (int col = 0; col < 8; ++col) fxColLabels[col].setVisible(v);

            fxDriveSlider.setVisible(v); fxDriveLabel.setVisible(v);
            fxToneSlider.setVisible(v);  fxToneLabel.setVisible(v);
            fxDriveMixSlider.setVisible(v); fxDriveMixLabel.setVisible(v);

            fxDelayTimeSlider.setVisible(v); fxDelayTimeLabel.setVisible(v);
            fxDelayFdbkSlider.setVisible(v); fxDelayFdbkLabel.setVisible(v);
            fxDelayMixSlider.setVisible(v);  fxDelayMixLabel.setVisible(v);
            fxDelayPingPongToggle.setVisible(v);

            fxReverbSizeSlider.setVisible(v); fxReverbSizeLabel.setVisible(v);
            fxReverbDampSlider.setVisible(v); fxReverbDampLabel.setVisible(v);
            fxReverbMixSlider.setVisible(v);  fxReverbMixLabel.setVisible(v);
            fxReverbModeToggle.setVisible(v);

            fxFilterCutoffSlider.setVisible(v); fxFilterCutoffLabel.setVisible(v);
            fxFilterResSlider.setVisible(v);    fxFilterResLabel.setVisible(v);
            fxFilterOnToggle.setVisible(v);
            fxFilterHpToggle.setVisible(v);
            fxFilter24dbToggle.setVisible(v);
        };

        auto setSettingsVisible = [this](bool v)
        {
            themeColorBox.setVisible(v); themeColorLabel.setVisible(v);
            accentColorBox.setVisible(v); accentColorLabel.setVisible(v);
            transPrideToggle.setVisible(v); global12BitToggle.setVisible(v);
            savePresetBtn.setVisible(v); loadPresetBtn.setVisible(v);
        };

        setDrumsVisible(currentTab == Tab::Drums);
        setPadsVisible(currentTab == Tab::Pads);
        setSeqVisible(currentTab == Tab::Sequencer);
        setMixerVisible(currentTab == Tab::Mixer);
        setFxVisible(currentTab == Tab::FX);
        setSettingsVisible(currentTab == Tab::Settings);

        // ==========================================
        // TAB 1: DRUMS (Row 1: BD, SN, CL | Row 2: HATS, CY, LT, HT)
        // ==========================================
        if (currentTab == Tab::Drums)
        {
            const int tierH = (availH - 10) / 2;
            const int topY = topBarH;

            // Row 1
            const int kickW  = static_cast<int>(availW * 0.38f);
            const int snareW = static_cast<int>(availW * 0.35f);
            const int clapW  = availW - kickW - snareW;

            int x = marginX;

            auto kickBounds = juce::Rectangle<int>(x, topY, kickW, tierH);
            sections.push_back({ "BASS DRUM", kickBounds });
            auto kickArea = kickBounds;
            auto toggleArea = kickArea.removeFromLeft(48);
            toggleArea.removeFromTop(24);
            kick608Toggle.setBounds(toggleArea.reduced(2, 8));
            layoutSection(kickArea, { {&kickTransientSlider, &kickTransientLabel}, {&kickDecaySlider, &kickDecayLabel}, {&kickTuneSlider, &kickTuneLabel}, {&kickHeatSlider, &kickHeatLabel}, {&kickPanSlider, &kickPanLabel} });
            x += kickW;

            auto snareBounds = juce::Rectangle<int>(x, topY, snareW, tierH);
            sections.push_back({ "SNARE", snareBounds });
            layoutSection(snareBounds, { {&snareDecaySlider, &snareDecayLabel}, {&snarePitchSlider, &snarePitchLabel}, {&snareSnappySlider, &snareSnappyLabel}, {&snareColorSlider, &snareColorLabel}, {&snarePanSlider, &snarePanLabel} });
            x += snareW;

            auto clapBounds = juce::Rectangle<int>(x, topY, clapW, tierH);
            sections.push_back({ "CLAP", clapBounds });
            layoutSection(clapBounds, { {&clapDecaySlider, &clapDecayLabel}, {&clapPitchSlider, &clapPitchLabel}, {&clapNoiseSlider, &clapNoiseLabel}, {&clapPanSlider, &clapPanLabel} });

            // Row 2
            const int botY = topY + tierH + 10;
            const int hatsW = static_cast<int>(availW * 0.28f);
            const int cyW   = static_cast<int>(availW * 0.24f);
            const int ltomW = static_cast<int>(availW * 0.24f);
            const int htomW = availW - hatsW - cyW - ltomW;

            x = marginX;

            auto hatsBounds = juce::Rectangle<int>(x, botY, hatsW, tierH);
            sections.push_back({ "HI-HATS", hatsBounds });
            layoutSection(hatsBounds, { {&hatsChDecaySlider, &hatsChDecayLabel}, {&hatsOhDecaySlider, &hatsOhDecayLabel}, {&hatsPitchSlider, &hatsPitchLabel}, {&hatsPanSlider, &hatsPanLabel} });
            x += hatsW;

            auto cyBounds = juce::Rectangle<int>(x, botY, cyW, tierH);
            sections.push_back({ "CYMBAL", cyBounds });
            layoutSection(cyBounds, { {&cyDecaySlider, &cyDecayLabel}, {&cyPitchSlider, &cyPitchLabel}, {&cyPanSlider, &cyPanLabel} });
            x += cyW;

            auto ltomBounds = juce::Rectangle<int>(x, botY, ltomW, tierH);
            sections.push_back({ "LOW TOM", ltomBounds });
            layoutSection(ltomBounds, { {&ltomDecaySlider, &ltomDecayLabel}, {&ltomPitchSlider, &ltomPitchLabel}, {&ltomPanSlider, &ltomPanLabel} });
            x += ltomW;

            auto htomBounds = juce::Rectangle<int>(x, botY, htomW, tierH);
            sections.push_back({ "HIGH TOM", htomBounds });
            layoutSection(htomBounds, { {&htomDecaySlider, &htomDecayLabel}, {&htomPitchSlider, &htomPitchLabel}, {&htomPanSlider, &htomPanLabel} });
        }
        // ==========================================
        // TAB 2: PADS (8 Drum Machine Pads)
        // ==========================================
        else if (currentTab == Tab::Pads)
        {
            auto padsBounds = juce::Rectangle<int>(marginX, topBarH, availW, availH);
            sections.push_back({ "DRUM TRIGGER PADS", padsBounds });

            auto pArea = padsBounds;
            pArea.removeFromTop(28);

            const int padW = pArea.getWidth() / 8;
            for (int p = 0; p < 8; ++p)
                drumPads[p].setBounds(pArea.removeFromLeft(padW).reduced(4, 12));
        }
        // ==========================================
        // TAB 3: SEQUENCER (9 Tracks: ACC + 8 Voices)
        // ==========================================
        else if (currentTab == Tab::Sequencer)
        {
            const int transportH = 50;
            auto transportArea = juce::Rectangle<int>(marginX, topBarH, availW, transportH);

            auto bpmArea = transportArea.removeFromRight(110);
            bpmLabel.setBounds(bpmArea.removeFromTop(14));
            bpmSlider.setBounds(bpmArea);

            transportArea.removeFromRight(16);

            transportToggleBtn.setBounds(transportArea.removeFromLeft(80).reduced(2, 8));
            clearBtn.setBounds(transportArea.removeFromLeft(68).reduced(2, 8));

            transportArea.removeFromLeft(10);
            copyPageBtn.setBounds(transportArea.removeFromLeft(75).reduced(2, 8));
            pastePageBtn.setBounds(transportArea.removeFromLeft(75).reduced(2, 8));

            transportArea.removeFromLeft(14);
            for (int p = 0; p < 4; ++p)
            {
                pageBtns[p].setBounds(transportArea.removeFromLeft(38).reduced(2, 8));
                pageBtns[p].setColour(juce::TextButton::buttonColourId, seqCurrentPage == p ? customLookAndFeel.mainGlowColour : juce::Colour(0xffd4d8e0));
            }

            transportArea.removeFromLeft(14);
            seqLengthBox.setBounds(transportArea.removeFromLeft(115).reduced(2, 10));

            const int gridY = topBarH + transportH + 4;
            const int gridH = availH - transportH - 4;
            const int rowH = gridH / 9;
            const int trackHeaderW = 82;
            const int stepW = (availW - trackHeaderW - 16) / 16;
            const int pageOffset = seqCurrentPage * 16;

            for (int t = 0; t < 9; ++t)
            {
                const int y = gridY + (t * rowH);
                auto headerArea = juce::Rectangle<int>(marginX, y, trackHeaderW, rowH);

                if (t == 0)
                {
                    trackLabels[t].setBounds(headerArea);
                }
                else
                {
                    int vIdx = t - 1;
                    trackLabels[t].setBounds(headerArea.removeFromLeft(34));
                    seqMuteBtns[vIdx].setBounds(headerArea.removeFromLeft(22).reduced(1, 2));
                    seqSoloBtns[vIdx].setBounds(headerArea.removeFromLeft(22).reduced(1, 2));
                }

                for (int s = 0; s < 16; ++s)
                {
                    int groupOffset = (s / 4) * 4;
                    int stepX = marginX + trackHeaderW + 4 + (s * stepW) + groupOffset;
                    auto& btn = stepButtons[t][s];
                    btn.setBounds(stepX, y + 2, stepW - 2, rowH - 4);

                    int actualStep = pageOffset + s;
                    btn.setToggleState(audioProcessor.stepPattern[t][actualStep].load(), juce::dontSendNotification);
                    btn.onClick = [this, t, actualStep, s]()
                    {
                        audioProcessor.stepPattern[t][actualStep].store(stepButtons[t][s].getToggleState());
                    };
                }
            }
        }
        // ==========================================
        // TAB 4: MIXER (8 Voice Channels + Master)
        // ==========================================
        else if (currentTab == Tab::Mixer)
        {
            auto mixerBounds = juce::Rectangle<int>(marginX, topBarH, availW, availH);
            sections.push_back({ "CONSOLE MIXER", mixerBounds });

            auto mArea = mixerBounds;
            mArea.removeFromTop(26);

            const int numStrips = 9;
            const int stripW = mArea.getWidth() / numStrips;

            for (int v = 0; v < 8; ++v)
            {
                auto strip = mArea.removeFromLeft(stripW);
                mixerPanLabels[v].setBounds(strip.removeFromTop(14));
                mixerPanDials[v].setBounds(strip.removeFromTop(44).reduced(4, 0));

                auto btnRow = strip.removeFromBottom(22);
                mixerMuteBtns[v].setBounds(btnRow.removeFromLeft(btnRow.getWidth() / 2).reduced(1, 1));
                mixerSoloBtns[v].setBounds(btnRow.reduced(1, 1));

                mixerFaderLabels[v].setBounds(strip.removeFromBottom(14));
                mixerFaders[v].setBounds(strip.reduced(3, 2));
            }

            auto masterStrip = mArea;
            masterStrip.removeFromTop(58);
            masterFaderLabel.setBounds(masterStrip.removeFromBottom(14));
            masterFader.setBounds(masterStrip.reduced(4, 2));
        }
        // ==========================================
        // TAB 5: FX RACK & MATRIX (3 x 8 Routing Matrix)
        // ==========================================
        else if (currentTab == Tab::FX)
        {
            const int matrixH = 94;
            auto matrixBounds = juce::Rectangle<int>(marginX, topBarH, availW, matrixH);
            sections.push_back({ "FX SENDS MATRIX", matrixBounds });

            auto mArea = matrixBounds;
            mArea.removeFromTop(20);

            const int rowLabelW = 65;
            const int colW = (mArea.getWidth() - rowLabelW) / 8;

            auto colHeaderArea = mArea.removeFromTop(14);
            colHeaderArea.removeFromLeft(rowLabelW);
            for (int c = 0; c < 8; ++c)
                fxColLabels[c].setBounds(colHeaderArea.removeFromLeft(colW));

            const int mRowH = mArea.getHeight() / 3;
            for (int r = 0; r < 3; ++r)
            {
                auto rowArea = mArea.removeFromTop(mRowH);
                fxRowLabels[r].setBounds(rowArea.removeFromLeft(rowLabelW).reduced(2, 0));

                for (int c = 0; c < 8; ++c)
                {
                    auto cell = rowArea.removeFromLeft(colW);
                    fxMatrixToggles[r][c].setBounds(cell.reduced((cell.getWidth() - 18) / 2, (cell.getHeight() - 18) / 2));
                }
            }

            const int fxGridY = topBarH + matrixH + 8;
            const int fxGridH = availH - matrixH - 8;
            const int sectionW = availW / 4;
            int x = marginX;

            auto layoutFxUnit = [this](juce::Rectangle<int> bounds, const juce::String& title,
                                       const std::vector<std::pair<juce::Slider*, juce::Label*>>& knobs,
                                       const std::vector<juce::Component*>& toggles = {})
            {
                sections.push_back({ title, bounds });
                auto area = bounds;
                if (!toggles.empty())
                {
                    auto toggleArea = area.removeFromBottom(34);
                    const int numTog = static_cast<int>(toggles.size());
                    const int tW = toggleArea.getWidth() / numTog;
                    for (auto* tog : toggles)
                        tog->setBounds(toggleArea.removeFromLeft(tW).reduced(3, 4));
                }
                layoutSection(area, knobs);
            };

            auto odBounds = juce::Rectangle<int>(x, fxGridY, sectionW, fxGridH);
            layoutFxUnit(odBounds, "ANALOG OVERDRIVE", { {&fxDriveSlider, &fxDriveLabel}, {&fxToneSlider, &fxToneLabel}, {&fxDriveMixSlider, &fxDriveMixLabel} });
            x += sectionW;

            auto delayBounds = juce::Rectangle<int>(x, fxGridY, sectionW, fxGridH);
            layoutFxUnit(delayBounds, "STEREO DELAY", { {&fxDelayTimeSlider, &fxDelayTimeLabel}, {&fxDelayFdbkSlider, &fxDelayFdbkLabel}, {&fxDelayMixSlider, &fxDelayMixLabel} }, { &fxDelayPingPongToggle });
            x += sectionW;

            auto revBounds = juce::Rectangle<int>(x, fxGridY, sectionW, fxGridH);
            layoutFxUnit(revBounds, "DATTORRO REVERB", { {&fxReverbSizeSlider, &fxReverbSizeLabel}, {&fxReverbDampSlider, &fxReverbDampLabel}, {&fxReverbMixSlider, &fxReverbMixLabel} }, { &fxReverbModeToggle });
            x += sectionW;

            auto fltBounds = juce::Rectangle<int>(x, fxGridY, sectionW, fxGridH);
            layoutFxUnit(fltBounds, "MASTER FILTER", { {&fxFilterCutoffSlider, &fxFilterCutoffLabel}, {&fxFilterResSlider, &fxFilterResLabel} }, { &fxFilterOnToggle, &fxFilterHpToggle, &fxFilter24dbToggle });
        }
        // ==========================================
        // TAB 6: SETTINGS
        // ==========================================
        else if (currentTab == Tab::Settings)
        {
            const int secW = availW / 3;
            int x = marginX;

            auto colorBounds = juce::Rectangle<int>(x, topBarH, secW, availH);
            sections.push_back({ "UI COLOR THEMES", colorBounds });
            auto cArea = colorBounds;
            cArea.removeFromTop(30);

            themeColorLabel.setBounds(cArea.removeFromTop(16).reduced(12, 0));
            themeColorBox.setBounds(cArea.removeFromTop(28).reduced(12, 2));

            cArea.removeFromTop(12);
            accentColorLabel.setBounds(cArea.removeFromTop(16).reduced(12, 0));
            accentColorBox.setBounds(cArea.removeFromTop(28).reduced(12, 2));

            cArea.removeFromTop(12);
            transPrideToggle.setBounds(cArea.removeFromTop(30).reduced(12, 2));
            x += secW;

            auto presetBounds = juce::Rectangle<int>(x, topBarH, secW, availH);
            sections.push_back({ "PRESET MANAGEMENT", presetBounds });
            auto pArea = presetBounds;
            pArea.removeFromTop(36);

            savePresetBtn.setBounds(pArea.removeFromTop(34).reduced(24, 2));
            pArea.removeFromTop(12);
            loadPresetBtn.setBounds(pArea.removeFromTop(34).reduced(24, 2));
            x += secW;

            auto vntgBounds = juce::Rectangle<int>(x, topBarH, secW, availH);
            sections.push_back({ "VINTAGE AUDIO ENGINE", vntgBounds });
            auto vArea = vntgBounds;
            vArea.removeFromTop(36);

            global12BitToggle.setBounds(vArea.removeFromTop(34).reduced(18, 2));
        }
}
