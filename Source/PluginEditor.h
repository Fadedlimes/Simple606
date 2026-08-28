#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class Super606LookAndFeel : public juce::LookAndFeel_V4
{
public:
    juce::Colour mainGlowColour { 0xffc084fc };
    juce::Colour mainDarkGlowColour { 0xffa855f7 };
    juce::Colour accentTrackColour { 0xfff97316 };
    bool isTransPrideTheme = false;

    Super606LookAndFeel()
    {
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xff111215));
        setColour(juce::Label::textColourId, juce::Colour(0xff111215));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider&) override
                          {
                              auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
                              const float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f - 14.0f;
                              const float centreX = bounds.getCentreX();
                              const float centreY = bounds.getCentreY();

                              const int numTicks = 11;
                              for (int i = 0; i < numTicks; ++i)
                              {
                                  const float angle = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * (static_cast<float>(i) / (numTicks - 1));
                                  const float innerR = radius + 3.5f;
                                  const float outerR = radius + (i == 0 || i == numTicks - 1 || i == numTicks / 2 ? 8.5f : 5.5f);

                                  const float x1 = centreX + innerR * std::sin(angle);
                                  const float y1 = centreY - innerR * std::cos(angle);
                                  const float x2 = centreX + outerR * std::sin(angle);
                                  const float y2 = centreY - outerR * std::cos(angle);

                                  g.setColour(juce::Colour(0xff52565e));
                                  g.drawLine(x1, y1, x2, y2, 1.2f);
                              }

                              g.setColour(juce::Colour(0x40000000));
                              g.fillEllipse(centreX - radius + 1.0f, centreY - radius + 3.0f, radius * 2.0f, radius * 2.0f);

                              juce::ColourGradient bezelGrad(juce::Colour(0xfffafbfc), centreX - radius, centreY - radius,
                                                             juce::Colour(0xff757982), centreX + radius, centreY + radius, false);
                              g.setGradientFill(bezelGrad);
                              g.fillEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);

                              g.setColour(juce::Colour(0xff3a3d45));
                              g.drawEllipse(centreX - radius + 1.0f, centreY - radius + 1.0f, (radius - 1.0f) * 2.0f, (radius - 1.0f) * 2.0f, 1.0f);

                              const float innerR = radius - 2.5f;
                              juce::ColourGradient dialGrad(juce::Colour(0xffeaecf0), centreX - innerR * 0.4f, centreY - innerR * 0.4f,
                                                            juce::Colour(0xffa2a6af), centreX + innerR * 0.8f, centreY + innerR * 0.8f, true);
                              g.setGradientFill(dialGrad);
                              g.fillEllipse(centreX - innerR, centreY - innerR, innerR * 2.0f, innerR * 2.0f);

                              const float capR = innerR * 0.60f;
                              juce::ColourGradient capGrad(juce::Colour(0xff989ca5), centreX - capR * 0.2f, centreY - capR * 0.2f,
                                                           juce::Colour(0xffd4d7dc), centreX + capR * 0.5f, centreY + capR * 0.5f, true);
                              g.setGradientFill(capGrad);
                              g.fillEllipse(centreX - capR, centreY - capR, capR * 2.0f, capR * 2.0f);

                              const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
                              juce::Path p;
                              p.addRectangle(-1.2f, -innerR + 2.0f, 2.4f, innerR * 0.65f);
                              g.setColour(juce::Colour(0xff23262c));
                              g.fillPath(p, juce::AffineTransform::rotation(angle).translated(centreX, centreY));
                          }

                          void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                                float sliderPos, float, float, juce::Slider::SliderStyle, juce::Slider&) override
                                                {
                                                    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
                                                    const float trackW = 4.0f;
                                                    const float trackX = bounds.getCentreX() - trackW * 0.5f;

                                                    g.setColour(juce::Colour(0xff2b2e35));
                                                    g.fillRoundedRectangle(trackX, bounds.getY() + 4.0f, trackW, bounds.getHeight() - 8.0f, 2.0f);

                                                    const float thumbW = 28.0f;
                                                    const float thumbH = 14.0f;
                                                    auto thumbRect = juce::Rectangle<float>(bounds.getCentreX() - thumbW * 0.5f, sliderPos - thumbH * 0.5f, thumbW, thumbH);

                                                    g.setColour(juce::Colour(0x50000000));
                                                    g.fillRoundedRectangle(thumbRect.translated(0.0f, 2.0f), 2.0f);

                                                    juce::ColourGradient thumbGrad(juce::Colour(0xfff0f2f6), thumbRect.getX(), thumbRect.getY(),
                                                                                   juce::Colour(0xff8c909a), thumbRect.getX(), thumbRect.getBottom(), false);
                                                    g.setGradientFill(thumbGrad);
                                                    g.fillRoundedRectangle(thumbRect, 2.0f);

                                                    g.setColour(juce::Colour(0xff181a1f));
                                                    g.drawRoundedRectangle(thumbRect, 2.0f, 1.0f);

                                                    g.setColour(juce::Colour(0xffffffff));
                                                    g.drawLine(thumbRect.getX() + 3.0f, thumbRect.getCentreY(), thumbRect.getRight() - 3.0f, thumbRect.getCentreY(), 1.5f);
                                                }

                                                void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                                                      bool, bool) override
                                                                      {
                                                                          auto bounds = button.getLocalBounds().toFloat();
                                                                          const bool isOn = button.getToggleState();
                                                                          const juce::String text = button.getButtonText();

                                                                          const float boxSize = 18.0f;

                                                                          if (text.isEmpty())
                                                                          {
                                                                              auto btnBox = bounds.reduced(1.5f);
                                                                              drawBox(g, btnBox, isOn, button.getComponentID());
                                                                          }
                                                                          else if (bounds.getWidth() > bounds.getHeight() * 1.35f)
                                                                          {
                                                                              auto btnBox = juce::Rectangle<float>(bounds.getX() + 2.0f, bounds.getCentreY() - boxSize * 0.5f, boxSize, boxSize);
                                                                              drawBox(g, btnBox, isOn, button.getComponentID());

                                                                              auto textRect = bounds.withTrimmedLeft(btnBox.getRight() + 3.0f);
                                                                              g.setColour(isOn ? mainDarkGlowColour : juce::Colour(0xff111215));
                                                                              g.setFont(juce::Font(10.0f, juce::Font::bold));
                                                                              g.drawText(text, textRect, juce::Justification::centredLeft);
                                                                          }
                                                                          else
                                                                          {
                                                                              auto btnBox = juce::Rectangle<float>(bounds.getCentreX() - boxSize * 0.5f, bounds.getY() + 1.0f, boxSize, boxSize);
                                                                              drawBox(g, btnBox, isOn, button.getComponentID());

                                                                              auto textRect = juce::Rectangle<float>(bounds.getX(), btnBox.getBottom() + 1.0f, bounds.getWidth(), 14.0f);
                                                                              g.setColour(isOn ? mainDarkGlowColour : juce::Colour(0xff111215));
                                                                              g.setFont(juce::Font(9.5f, juce::Font::bold));
                                                                              g.drawText(text, textRect, juce::Justification::centred);
                                                                          }
                                                                      }

                                                                      void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                                                                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
                                                                                          {
                                                                                              const juce::String txt = button.getButtonText();
                                                                                              if (txt == "M" || txt == "S")
                                                                                              {
                                                                                                  g.setFont(juce::Font(11.0f, juce::Font::bold));
                                                                                                  g.setColour(button.getToggleState() ? juce::Colour(0xff111215) : juce::Colour(0xfff0f2f5));
                                                                                                  g.drawText(txt, button.getLocalBounds(), juce::Justification::centred, false);
                                                                                              }
                                                                                              else
                                                                                              {
                                                                                                  LookAndFeel_V4::drawButtonText(g, button, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
                                                                                              }
                                                                                          }

                                                                                          void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                                                                                    const juce::Colour&, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
                                                                                                                    {
                                                                                                                        auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
                                                                                                                        const juce::String txt = button.getButtonText();
                                                                                                                        const bool isTab = (txt == "DRUMS" || txt == "PADS" || txt == "SEQUENCER" || txt == "MIXER" || txt == "FX RACK" || txt == "SETTINGS");
                                                                                                                        const bool isMute = (txt == "M");
                                                                                                                        const bool isSolo = (txt == "S");
                                                                                                                        const bool isPad  = button.getComponentID() == "drumpad";

                                                                                                                        if (isTab)
                                                                                                                        {
                                                                                                                            juce::ColourGradient tabGrad(shouldDrawButtonAsHighlighted ? juce::Colour(0xffeaedf3) : juce::Colour(0xffd4d8e0),
                                                                                                                                                         bounds.getX(), bounds.getY(),
                                                                                                                                                         juce::Colour(0xffb8bcc6), bounds.getX(), bounds.getBottom(), false);
                                                                                                                            g.setGradientFill(tabGrad);
                                                                                                                            g.fillRoundedRectangle(bounds, 4.0f);
                                                                                                                            g.setColour(juce::Colour(0xff757984));
                                                                                                                            g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
                                                                                                                        }
                                                                                                                        else if (isPad)
                                                                                                                        {
                                                                                                                            if (shouldDrawButtonAsDown)
                                                                                                                            {
                                                                                                                                g.setColour(mainGlowColour);
                                                                                                                                g.fillRoundedRectangle(bounds, 6.0f);
                                                                                                                                g.setColour(juce::Colour(0xffffffff));
                                                                                                                                g.drawRoundedRectangle(bounds, 6.0f, 2.0f);
                                                                                                                            }
                                                                                                                            else
                                                                                                                            {
                                                                                                                                juce::ColourGradient padGrad(juce::Colour(0xff3f424b), bounds.getX(), bounds.getY(),
                                                                                                                                                             juce::Colour(0xff22242a), bounds.getX(), bounds.getBottom(), false);
                                                                                                                                g.setGradientFill(padGrad);
                                                                                                                                g.fillRoundedRectangle(bounds, 6.0f);
                                                                                                                                g.setColour(juce::Colour(0xff606470));
                                                                                                                                g.drawRoundedRectangle(bounds, 6.0f, 1.2f);
                                                                                                                            }
                                                                                                                        }
                                                                                                                        else if (isMute || isSolo)
                                                                                                                        {
                                                                                                                            bool isOn = button.getToggleState();
                                                                                                                            juce::Colour col = isMute ? juce::Colour(0xffef4444) : juce::Colour(0xffeab308);
                                                                                                                            if (isOn)
                                                                                                                            {
                                                                                                                                g.setColour(col);
                                                                                                                                g.fillRoundedRectangle(bounds, 3.0f);
                                                                                                                            }
                                                                                                                            else
                                                                                                                            {
                                                                                                                                g.setColour(juce::Colour(0xff3f424a));
                                                                                                                                g.fillRoundedRectangle(bounds, 3.0f);
                                                                                                                            }
                                                                                                                            g.setColour(juce::Colour(0xff181a1f));
                                                                                                                            g.drawRoundedRectangle(bounds, 3.0f, 1.0f);
                                                                                                                        }
                                                                                                                        else
                                                                                                                        {
                                                                                                                            if (shouldDrawButtonAsDown)
                                                                                                                            {
                                                                                                                                g.setColour(mainGlowColour);
                                                                                                                                g.fillRoundedRectangle(bounds, 4.0f);
                                                                                                                                g.setColour(mainDarkGlowColour);
                                                                                                                                g.drawRoundedRectangle(bounds, 4.0f, 1.2f);
                                                                                                                            }
                                                                                                                            else
                                                                                                                            {
                                                                                                                                juce::ColourGradient bGrad(shouldDrawButtonAsHighlighted ? juce::Colour(0xff555963) : juce::Colour(0xff444750),
                                                                                                                                                           bounds.getX(), bounds.getY(),
                                                                                                                                                           juce::Colour(0xff292b31), bounds.getX(), bounds.getBottom(), false);
                                                                                                                                g.setGradientFill(bGrad);
                                                                                                                                g.fillRoundedRectangle(bounds, 4.0f);
                                                                                                                                g.setColour(juce::Colour(0xff181a1f));
                                                                                                                                g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
                                                                                                                            }
                                                                                                                        }
                                                                                                                    }

private:
    void drawBox(juce::Graphics& g, juce::Rectangle<float> btnBox, bool isOn, const juce::String& compId)
    {
        g.setColour(juce::Colour(0xff4a4d56));
        g.fillRoundedRectangle(btnBox, 3.5f);

        auto innerBox = btnBox.reduced(1.5f);

        if (isOn)
        {
            bool isAccent = (compId == "accent");

            if (isTransPrideTheme && !isAccent)
            {
                g.setColour(juce::Colour(0x665bcefa));
                g.fillRoundedRectangle(btnBox.expanded(2.5f), 5.0f);

                juce::ColourGradient prideGlow(juce::Colour(0xff5bcefa), innerBox.getX(), innerBox.getY(),
                                               juce::Colour(0xfff5a9b8), innerBox.getX(), innerBox.getBottom(), false);
                g.setGradientFill(prideGlow);
                g.fillRoundedRectangle(innerBox, 2.5f);

                g.setColour(juce::Colour(0xffffffff));
                g.fillEllipse(innerBox.getCentreX() - 2.0f, innerBox.getCentreY() - 2.0f, 4.0f, 4.0f);
            }
            else
            {
                juce::Colour glowColor = isAccent ? accentTrackColour : mainGlowColour;

                g.setColour(glowColor.withAlpha(0.4f));
                g.fillRoundedRectangle(btnBox.expanded(2.5f), 5.0f);

                juce::ColourGradient glow(juce::Colours::white, innerBox.getX(), innerBox.getY(), glowColor, innerBox.getX(), innerBox.getBottom(), false);
                g.setGradientFill(glow);
                g.fillRoundedRectangle(innerBox, 2.5f);

                g.setColour(juce::Colour(0xffffffff));
                g.fillEllipse(innerBox.getCentreX() - 2.0f, innerBox.getCentreY() - 2.0f, 4.0f, 4.0f);
            }
        }
        else
        {
            juce::ColourGradient offGrad(juce::Colour(0xff6e727c), innerBox.getX(), innerBox.getY(),
                                         juce::Colour(0xff3f424a), innerBox.getX(), innerBox.getBottom(), false);
            g.setGradientFill(offGrad);
            g.fillRoundedRectangle(innerBox, 2.5f);
        }

        g.setColour(juce::Colour(0xff1e2025));
        g.drawRoundedRectangle(innerBox, 2.5f, 1.0f);
    }
};

class Super606AudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    Super606AudioProcessorEditor(Super606AudioProcessor&);
    ~Super606AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    void timerCallback() override;

    struct Section
    {
        juce::String name;
        juce::Rectangle<int> bounds;
    };

    void setupKnob(juce::Slider& slider, juce::Label& label, const juce::String& text,
                   std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                   const juce::String& paramID);

    void setupFader(juce::Slider& slider, juce::Label& label, const juce::String& text,
                    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                    const juce::String& paramID);

    void layoutSection(juce::Rectangle<int> area,
                       const std::vector<std::pair<juce::Slider*, juce::Label*>>& knobs);

    void updateThemeColors();

    Super606AudioProcessor& audioProcessor;
    Super606LookAndFeel customLookAndFeel;

    // Tabs: DRUMS -> PADS -> SEQUENCER -> MIXER -> FX RACK -> SETTINGS
    enum class Tab { Drums, Pads, Sequencer, Mixer, FX, Settings };
    Tab currentTab = Tab::Drums;
    juce::TextButton drumsTabBtn, padsTabBtn, seqTabBtn, mixerTabBtn, fxTabBtn, settingsTabBtn;

    // --- TAB 1: DRUMS ---
    juce::Slider kickTransientSlider, kickDecaySlider, kickTuneSlider, kickHeatSlider, kickPanSlider;
    juce::Label  kickTransientLabel,  kickDecayLabel,  kickTuneLabel,  kickHeatLabel,  kickPanLabel;
    juce::ToggleButton kick608Toggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> kickTransientAtt, kickDecayAtt, kickTuneAtt, kickHeatAtt, kickPanAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> kick608Att;

    juce::Slider snareDecaySlider, snarePitchSlider, snareSnappySlider, snareColorSlider, snarePanSlider;
    juce::Label  snareDecayLabel,  snarePitchLabel,  snareSnappyLabel,  snareColorLabel,  snarePanLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> snareDecayAtt, snarePitchAtt, snareSnappyAtt, snareColorAtt, snarePanAtt;

    juce::Slider clapDecaySlider, clapPitchSlider, clapNoiseSlider, clapPanSlider;
    juce::Label  clapDecayLabel,  clapPitchLabel,  clapNoiseLabel,  clapPanLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> clapDecayAtt, clapPitchAtt, clapNoiseAtt, clapPanAtt;

    juce::Slider hatsChDecaySlider, hatsOhDecaySlider, hatsPitchSlider, hatsPanSlider;
    juce::Label  hatsChDecayLabel,  hatsOhDecayLabel,  hatsPitchLabel,  hatsPanLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hatsChDecayAtt, hatsOhDecayAtt, hatsPitchAtt, hatsPanAtt;

    juce::Slider ltomDecaySlider, ltomPitchSlider, ltomPanSlider;
    juce::Label  ltomDecayLabel,  ltomPitchLabel,  ltomPanLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ltomDecayAtt, ltomPitchAtt, ltomPanAtt;

    juce::Slider htomDecaySlider, htomPitchSlider, htomPanSlider;
    juce::Label  htomDecayLabel,  htomPitchLabel,  htomPanLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> htomDecayAtt, htomPitchAtt, htomPanAtt;

    // --- TAB 2: PADS ---
    juce::TextButton drumPads[7];

    // --- TAB 3: SEQUENCER ---
    int seqCurrentPage = 0;
    bool seqPageClipboard[8][16] = { { false } };
    juce::TextButton pageBtns[4];
    juce::ComboBox   seqLengthBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> seqLengthAtt;

    juce::TextButton transportToggleBtn, clearBtn, copyPageBtn, pastePageBtn;
    juce::Slider bpmSlider;
    juce::Label  bpmLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bpmAtt;

    juce::TextButton seqMuteBtns[7], seqSoloBtns[7];
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> seqMuteAtts[7], seqSoloAtts[7];

    juce::ToggleButton stepButtons[8][16];
    juce::Label trackLabels[8];

    // --- TAB 4: MIXER ---
    juce::Slider mixerFaders[7], masterFader;
    juce::Label  mixerFaderLabels[7], masterFaderLabel;
    juce::Slider mixerPanDials[7];
    juce::Label  mixerPanLabels[7];
    juce::TextButton mixerMuteBtns[7], mixerSoloBtns[7];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixerFaderAtts[7], masterFaderAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixerPanAtts[7];
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mixerMuteAtts[7], mixerSoloAtts[7];

    // --- TAB 5: FX MATRIX ---
    juce::ToggleButton fxMatrixToggles[3][7];
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> fxMatrixAtts[3][7];
    juce::Label fxRowLabels[3];
    juce::Label fxColLabels[7];

    juce::Slider fxDriveSlider, fxToneSlider, fxDriveMixSlider;
    juce::Label  fxDriveLabel,  fxToneLabel,  fxDriveMixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fxDriveAtt, fxToneAtt, fxDriveMixAtt;

    juce::Slider fxDelayTimeSlider, fxDelayFdbkSlider, fxDelayMixSlider;
    juce::Label  fxDelayTimeLabel,  fxDelayFdbkLabel,  fxDelayMixLabel;
    juce::ToggleButton fxDelayPingPongToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fxDelayTimeAtt, fxDelayFdbkAtt, fxDelayMixAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> fxDelayPingPongAtt;

    juce::Slider fxReverbSizeSlider, fxReverbDampSlider, fxReverbMixSlider;
    juce::Label  fxReverbSizeLabel,  fxReverbDampLabel,  fxReverbMixLabel;
    juce::ToggleButton fxReverbModeToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fxReverbSizeAtt, fxReverbDampAtt, fxReverbMixAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> fxReverbModeAtt;

    juce::Slider fxFilterCutoffSlider, fxFilterResSlider;
    juce::Label  fxFilterCutoffLabel,  fxFilterResLabel;
    juce::ToggleButton fxFilterOnToggle, fxFilterHpToggle, fxFilter24dbToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fxFilterCutoffAtt, fxFilterResAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> fxFilterOnAtt, fxFilterHpAtt, fxFilter24dbAtt;

    // --- TAB 6: SETTINGS ---
    juce::ComboBox themeColorBox, accentColorBox;
    juce::Label themeColorLabel, accentColorLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> themeColorAtt, accentColorAtt;

    juce::ToggleButton transPrideToggle, global12BitToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> transPrideAtt, global12BitAtt;

    juce::TextButton savePresetBtn, loadPresetBtn;
    std::unique_ptr<juce::FileChooser> fileChooser;

    std::vector<Section> sections;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Super606AudioProcessorEditor)
};
