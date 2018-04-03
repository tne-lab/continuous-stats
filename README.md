# Continuous Statistics Plugin
Plugin for the Open Ephys GUI to compute the exponentially weighted sliding mean or standard deviation of incoming continuous data.

## Installing:

* All platforms: Copy contents of "Source" into a new folder in your "Source/Plugins" called "ContinuousStats".
* Windows: Copy contents of "Builds/VisualStudio2013" into a new folder in your "Builds/VisualStudio2013/Plugins" called "ContinuousStats". Then build as usual in Visual Studio.
* Linux: Use `make` to compile as usual.
* Mac: Not currently implemented, but should be easy to setup as a standard plugin in XCode.

## Testing:

There is a test file in "Test" that allows you to compare the output of the plugin to the expected output (as implemented in MATLAB). See its header for more information.
