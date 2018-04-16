# Continuous Statistics Plugin
Plugin for the Open Ephys GUI to compute the exponentially weighted sliding mean or standard deviation of incoming continuous data.

## Installing:

* All platforms: Copy contents of "Source" into a new folder in your "Source/Plugins" called "ContinuousStats".
* Windows: Copy contents of "Builds/VisualStudio2013" into a new folder in your "Builds/VisualStudio2013/Plugins" called "ContinuousStats". In Visual Studio, with the Plugins solution open, right-click the solution, go to Add->Existing Project, then select the ContinuousStats.vcxproj file you just copied to add the new plugin. Then build as usual in Visual Studio.
* Linux: Use `make` to compile as usual.
* Mac: Not currently implemented, but should be easy to setup as a standard plugin in XCode.

## Usage:

Use the drop-down box to select mean or standard deviation. The "time constant" controls how rapidly the sliding statistics decay; it
equals how long it takes for the influence of a sample to drop to 1/e of its original value.

Channels can be included/excluded from processing by selecting/deselecting them in the "Param" section the drawer.

## Testing:

There is a test file in "Test" that allows you to compare the output of the plugin to the expected output (as implemented in MATLAB). See its header for more information.
