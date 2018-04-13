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

#ifndef CONTINUOUS_STATS_H_INCLUDED
#define CONTINUOUS_STATS_H_INCLUDED

#include <ProcessorHeaders.h>

/*
 * Calculates the exponentially-weighted moving average or standard deviation of
 * the incoming continuous channels. The time constant controls the decay rate.
 *
 * @see GenericProcessor
 */

// parameter indices
enum Param
{
    STAT,
    TIME_CONST
};

// statistic option indices
enum Statistic
{
    MEAN = 1,
    STDDEV,
};

class ContinuousStats : public GenericProcessor
{
    friend class ContinuousStatsEditor;

public:
    ContinuousStats();
    ~ContinuousStats();

    bool hasEditor() const { return true; }
    AudioProcessorEditor* createEditor() override;

    void process(AudioSampleBuffer& continuousBuffer) override;

    bool disable() override;

    void setParameter(int parameterIndex, float newValue) override;

    // handle changing number of channels
    void updateSettings() override;
         
private:
    // which statistic is currently being calculated
    Statistic currStat;

    // time constant in milliseconds
    double timeConstMs;

    // state
    Array<double> currMean;
    Array<double> currVar;
    Array<bool> startingRunningMean; // flag to treat the first sample differently

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContinuousStats);
};

#endif // CONTINUOUS_STATS_H_INCLUDED