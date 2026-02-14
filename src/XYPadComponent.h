#pragma once

#include <JuceHeader.h>
#include <functional>

class XYPadComponent : public juce::Component
{
public:
    XYPadComponent();
    ~XYPadComponent() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    
    void setPosition(float x, float y);
    float getXPosition() const { return mXPosition; }
    float getYPosition() const { return mYPosition; }
    
    std::function<void(float, float)> onPositionChanged;

private:
    float mXPosition = 0.0f;
    float mYPosition = 0.0f;
    
    void updatePositionFromMouse(const juce::MouseEvent& event);
    inline juce::Point<float> getCircleCenter() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XYPadComponent)
};
