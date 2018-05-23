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

#include "ContinuousStats.h"
#include "ContinuousStatsEditor.h"
#include <cmath> // sqrt

ContinuousStats::ContinuousStats()
    : GenericProcessor  ("Continuous Stats")
    , currStat          (MEAN)
    , timeConstMs       (1000.0)
{
    setProcessorType(PROCESSOR_TYPE_FILTER);
}

ContinuousStats::~ContinuousStats() {}

AudioProcessorEditor* ContinuousStats::createEditor()
{
    editor = new ContinuousStatsEditor(this);
    return editor;
}

void ContinuousStats::process(AudioSampleBuffer& continuousBuffer)
{
    int nChannels = continuousBuffer.getNumChannels();
    Statistic stat = currStat;

    Array<int> activeChannels = editor->getActiveChannels();
    int numActiveChannels = activeChannels.size();
    for (int i = 0; i < numActiveChannels; ++i)
    {
        int chan = activeChannels[i];
        int nSamples = getNumSamples(chan);

        double samplesPerMs = getDataChannel(chan)->getSampleRate() / 1000.0;
        double mean, var;

        // compute running mean and variance
        // algorithm references: Wikipedia and Finch, Tony. "Incremental calculation of weighted mean and variance" (PDF). University of Cambridge.
        int samp = 0;
        if (startingRunningMean[chan])
        {
            mean = continuousBuffer.getSample(chan, samp);
            var = 0.0;
            continuousBuffer.setSample(chan, samp, static_cast<float>(stat == MEAN ? mean : std::sqrt(var)));
            startingRunningMean.set(chan, false);
            ++samp;
        }
        else 
        {
            mean = currMean[chan];
            var = currVar[chan];
        }

        // calculate exponential weighting algorithm values
        double timeConstSamp = timeConstMs * samplesPerMs;
        double alpha = -std::expm1(-1 / timeConstSamp);
        double delta;

        for (; samp < nSamples; ++samp)
        {
            delta = continuousBuffer.getSample(chan, samp) - mean;
            mean += alpha * delta;
            var = (1 - alpha) * (var + alpha * std::pow(delta, 2));
            continuousBuffer.setSample(chan, samp, static_cast<float>(stat == MEAN ? mean : std::sqrt(var)));
        }

        // save for next buffer
        currMean.set(chan, mean);
        currVar.set(chan, var);
    }
}

bool ContinuousStats::disable()
{
    // reset state
    for (int i = 0; i < startingRunningMean.size(); ++i)
    {
        startingRunningMean.set(i, true);
    }
    return true;
}

void ContinuousStats::setParameter(int parameterIndex, float newValue)
{
    switch (parameterIndex)
    {
    case STAT:
        currStat = static_cast<Statistic>(static_cast<int>(newValue));
        break;

    case TIME_CONST:
        timeConstMs = newValue;
        break;
    }
}

void ContinuousStats::updateSettings()
{
    int numInputsChange = getNumInputs() - currMean.size();

    if (numInputsChange > 0)
    {
        currMean.insertMultiple(-1, 0.0, numInputsChange);
        currVar.insertMultiple(-1, 0.0, numInputsChange);
        startingRunningMean.insertMultiple(-1, true, numInputsChange);
    }
    else if (numInputsChange < 0)
    {
        currMean.removeLast(-numInputsChange);
        currVar.removeLast(-numInputsChange);
        startingRunningMean.removeLast(-numInputsChange);
    }
}
