/*
------------------------------------------------------------------

This file is part of a plugin for the Open Ephys GUI
Copyright (C) 2018 Translational NeuroEngineering Laboratory, MGH

------------------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "ContinuousStatsEditor.h"
#include <string> // stof

ContinuousStatsEditor::ContinuousStatsEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors)
    : GenericEditor(parentNode, useDefaultParameterEditors)
{
    desiredWidth = 155;
    ContinuousStats* processor = static_cast<ContinuousStats*>(parentNode);

    const int TEXT_HT = 18;
    const int LEFT_EDGE = 12;
    /* -------------- Top row ------------- */
    int xPos = LEFT_EDGE;
    int yPos = 40;

    statBox = new ComboBox("stat");
    statBox->addItem("MEAN", MEAN);
    statBox->addItem("STD DEV", STDDEV);
    statBox->setSelectedId(processor->currStat, dontSendNotification);
    statBox->setBounds(xPos + 15, yPos, 90, 20);
    statBox->addListener(this);
    addAndMakeVisible(statBox);

    /* ------------- Bottom row (time const) ---------- */
    yPos += 35;

    timeConstLabel = new Label("timeConstL", "Time constant:");
    timeConstLabel->setBounds(xPos, yPos, 120, TEXT_HT + 2);
    timeConstLabel->setFont(Font("Small Text", 14, Font::plain));
    timeConstLabel->setColour(Label::textColourId, Colours::darkgrey);
    timeConstLabel->setTooltip(TIME_CONST_TOOLTIP);
    addAndMakeVisible(timeConstLabel);

    yPos += 20;

    timeConstEditable = new Label("timeConstE");
    timeConstEditable->setEditable(true);
    timeConstEditable->setBounds(xPos += 5, yPos, 60, TEXT_HT);
    timeConstEditable->setText(String(processor->timeConstMs), dontSendNotification);
    timeConstEditable->setColour(Label::backgroundColourId, Colours::grey);
    timeConstEditable->setColour(Label::textColourId, Colours::white);
    timeConstEditable->setTooltip(TIME_CONST_TOOLTIP);
    timeConstEditable->addListener(this);
    addAndMakeVisible(timeConstEditable);

    timeConstUnit = new Label("timeConstU", "ms");
    timeConstUnit->setBounds(xPos += 70, yPos, 40, TEXT_HT);
    timeConstUnit->setFont(Font("Small Text", 14, Font::plain));
    timeConstUnit->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(timeConstUnit);
}

ContinuousStatsEditor::~ContinuousStatsEditor() {}

void ContinuousStatsEditor::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == statBox)
        getProcessor()->setParameter(STAT, static_cast<float>(statBox->getSelectedId()));
}

void ContinuousStatsEditor::labelTextChanged(Label* labelThatHasChanged)
{
    if (labelThatHasChanged == timeConstEditable)
    {
        ContinuousStats* processor = static_cast<ContinuousStats*>(getProcessor());

        float newVal;
        bool success = updateFloatLabel(labelThatHasChanged, 0.01F, FLT_MAX, static_cast<float>(processor->timeConstMs), &newVal);

        if (success)
            processor->setParameter(TIME_CONST, newVal);
    }
}

void ContinuousStatsEditor::saveCustomParameters(XmlElement* xml)
{
    xml->setAttribute("Type", "ContinuousStatsEditor");

    ContinuousStats* processor = static_cast<ContinuousStats*>(getProcessor());
    XmlElement* paramValues = xml->createNewChildElement("VALUES");
    
    paramValues->setAttribute("currStat", processor->currStat);
    paramValues->setAttribute("timeConstMs", processor->timeConstMs);
}

void ContinuousStatsEditor::loadCustomParameters(XmlElement* xml)
{
    forEachXmlChildElementWithTagName(*xml, xmlNode, "VALUES")
    {
        statBox->setSelectedId(xmlNode->getIntAttribute("currStat", statBox->getSelectedId()));
        timeConstEditable->setText(xmlNode->getStringAttribute("timeConstMs", timeConstEditable->getText()), sendNotificationAsync);
    }
}

// static utilities

bool ContinuousStatsEditor::updateFloatLabel(Label* labelThatHasChanged,
    float minValue, float maxValue, float defaultValue, float* result)
{
    String& input = labelThatHasChanged->getText();
    bool valid = parseInput(input, minValue, maxValue, result);
    if (!valid)
        labelThatHasChanged->setText(String(defaultValue), dontSendNotification);
    else
        labelThatHasChanged->setText(String(*result), dontSendNotification);

    return valid;
}

bool ContinuousStatsEditor::parseInput(String& in, float min, float max, float* out)
{
    float parsedFloat;
    try
    {
        parsedFloat = std::stof(in.toRawUTF8());
    }
    catch (...)
    {
        return false;
    }

    if (parsedFloat < min)
        *out = min;
    else if (parsedFloat > max)
        *out = max;
    else
        *out = parsedFloat;

    return true;
}