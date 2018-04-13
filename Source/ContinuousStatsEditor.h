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

#ifndef CONTINUOUS_STATS_EDITOR_H_INCLUDED
#define CONTINUOUS_STATS_EDITOR_H_INCLUDED

#include <EditorHeaders.h>
#include "ContinuousStats.h"

class ContinuousStatsEditor : public GenericEditor, public ComboBox::Listener, public Label::Listener
{
public:
    ContinuousStatsEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors = false);
    ~ContinuousStatsEditor();

    // implements ComboBox::Listener
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

    // implements Label::Listener
    void labelTextChanged(Label* labelThatHasChanged) override;

    void saveCustomParameters(XmlElement* xml) override;
    void loadCustomParameters(XmlElement* xml) override;

private:
    // utility for label listening
    // ouputs whether the label contained a valid input; if so, it is stored in *result.
    static bool updateFloatLabel(Label* labelThatHasChanged,
        float minValue, float maxValue, float defaultValue, float* result);

    // Attempt to parse an input string into a float between min and max, inclusive.
    // Returns false if no integer could be parsed.
    static bool parseInput(String& in, float min, float max, float* out);

    // UI elements
    // Row 1
    ScopedPointer<ComboBox>      statBox;

    // Row 2
    ScopedPointer<Label> timeConstLabel;
    ScopedPointer<Label> timeConstEditable;
    ScopedPointer<Label> timeConstUnit;

    // constants
    const String APPLY_TO_CHAN_TOOLTIP = "Turn processing of selected channels on or off";
    const String TIME_CONST_TOOLTIP = "Time for the influence of a single sample to decay to 36.8% (1/e) of its initial value (larger = smoother, smaller = more local)";

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContinuousStatsEditor);
};

#endif // CONTINUOUS_STATS_EDITOR_H_INCLUDED