#include "XYPadComponent.h"

constexpr float CircleRadius = 12.0f;
constexpr float CircleShadowOffset = 2.0f;

XYPadComponent::XYPadComponent()
{
}

void XYPadComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(bounds);
    
    g.setColour(juce::Colours::white);
    g.drawRect(bounds, 2.0f);
    
    auto center = getCircleCenter();
    
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillEllipse(center.x - CircleRadius + CircleShadowOffset, 
                  center.y - CircleRadius + CircleShadowOffset, 
                  CircleRadius * 2, CircleRadius * 2);
    
    g.setColour(juce::Colours::cyan);
    g.fillEllipse(center.x - CircleRadius, center.y - CircleRadius, 
                  CircleRadius * 2, CircleRadius * 2);
    
    g.setColour(juce::Colours::white);
    g.drawEllipse(center.x - CircleRadius, center.y - CircleRadius, 
                  CircleRadius * 2, CircleRadius * 2, 2.0f);
}

void XYPadComponent::resized()
{
}

void XYPadComponent::mouseDown(const juce::MouseEvent& event)
{
    updatePositionFromMouse(event);
}

void XYPadComponent::mouseDrag(const juce::MouseEvent& event)
{
    updatePositionFromMouse(event);
}

void XYPadComponent::updatePositionFromMouse(const juce::MouseEvent& event)
{
    auto bounds = getLocalBounds().toFloat();
    auto mousePos = event.position;
    
    float x = (mousePos.x - bounds.getX()) / bounds.getWidth();
    float y = 1.0f - ((mousePos.y - bounds.getY()) / bounds.getHeight());
    
    x = std::clamp(x, 0.0f, 1.0f);
    y = std::clamp(y, 0.0f, 1.0f);
    
    mXPosition = x;
    mYPosition = y;
    
    repaint();
    
    if (onPositionChanged)
    {
        onPositionChanged(mXPosition, abs(1.f - mYPosition));
    }
}

void XYPadComponent::setPosition(float x, float y)
{
    mXPosition = std::clamp(x, 0.0f, 1.0f);
    mYPosition = std::clamp(y, 0.0f, 1.0f);
    repaint();
}

juce::Point<float> XYPadComponent::getCircleCenter() const
{
    auto bounds = getLocalBounds().toFloat();
    
    const float x = bounds.getX() + (mXPosition * bounds.getWidth());
    const float y = bounds.getY() + ((1.0f - mYPosition) * bounds.getHeight());
    
    return {x, y};
}
