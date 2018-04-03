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
    , currStat          (Statistic::mean)
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

    for (int chan = 0; chan < nChannels; ++chan)
    {
        int nSamples = getNumSamples(chan);

        // "+CH" button
        if (!shouldProcessChannel[chan] || nSamples == 0)
            continue;

        double samplesPerMs = getDataChannel(chan)->getSampleRate() / 1000.0;
        double mean, var;

        // compute running mean and variance
        // algorithm references: Wikipedia and Finch, Tony. "Incremental calculation of weighted mean and variance" (PDF). University of Cambridge.
        int samp = 0;
        if (startingRunningMean[chan])
        {
            mean = continuousBuffer.getSample(chan, samp);
            var = 0.0;
            continuousBuffer.setSample(chan, samp, static_cast<float>(stat == Statistic::mean ? mean : std::sqrt(var)));
            startingRunningMean.set(chan, false);
            ++samp;
        }
        else {
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
            continuousBuffer.setSample(chan, samp, static_cast<float>(stat == Statistic::mean ? mean : std::sqrt(var)));
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
        startingRunningMean.set(i, true);

    return true;
}

void ContinuousStats::setParameter(int parameterIndex, float newValue)
{
    switch (parameterIndex)
    {
    case Param::stat:
        currStat = static_cast<Statistic>(static_cast<int>(newValue));
        break;

    case Param::timeConst:
        timeConstMs = newValue;
        break;

    case Param::enabledState:
        startingRunningMean.set(currentChannel, true);
        shouldProcessChannel.set(currentChannel, newValue == 0 ? false : true);
        break;
    }
}

void ContinuousStats::updateSettings()
{
    int numInputsChange = getNumInputs() - shouldProcessChannel.size();

    if (numInputsChange > 0)
    {
        shouldProcessChannel.insertMultiple(-1, true, numInputsChange);
        currMean.insertMultiple(-1, 0.0, numInputsChange);
        currVar.insertMultiple(-1, 0.0, numInputsChange);
        startingRunningMean.insertMultiple(-1, true, numInputsChange);
    }
    else if (numInputsChange < 0)
    {
        shouldProcessChannel.removeLast(-numInputsChange);
        currMean.removeLast(-numInputsChange);
        currVar.removeLast(-numInputsChange);
        startingRunningMean.removeLast(-numInputsChange);
    }
}

void ContinuousStats::saveCustomChannelParametersToXml(XmlElement* channelElement, int channelNumber, InfoObjectCommon::InfoObjectType channelType)
{
    if (channelType == InfoObjectCommon::DATA_CHANNEL)
    {
        XmlElement* channelParams = channelElement->createNewChildElement("PARAMETERS");
        channelParams->setAttribute("shouldProcess", shouldProcessChannel[channelNumber]);
    }
}

void ContinuousStats::loadCustomChannelParametersFromXml(XmlElement* channelElement, InfoObjectCommon::InfoObjectType channelType)
{
    int channelNum = channelElement->getIntAttribute("number");

    forEachXmlChildElement(*channelElement, subnode)
    {
        if (subnode->hasTagName("PARAMETERS"))
        {
            shouldProcessChannel.set(channelNum, subnode->getBoolAttribute("shouldProcess", true));
        }
    }
}
