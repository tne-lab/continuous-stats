function rms_err = test_continuous_stats_run(in_data, out_data, stat_type, time_const, Fs)
% test_continuous_stats_run.m: Compare the output of the Continuous Stats plugin with
% the expected results.
%
% Assumes the recording starts at (very close to) the time when the plugin began processing. Starting
% a recording while acquisition is already running will not work because the plugin will maintain its
% state from before the recording.
%
% Syntax: test_continuous_stats_run(in_data, out_data, stat_type, time_const, Fs);
%
% Inputs:
%     in_data:    continuous data of input to plugin as a Matlab vector.
%     out_data:   continuous data of plugin output as a Matlab vector. Must be the same length as in_data.
%     stat_type:  'mean', 'std', or 'var' (the statistic that was calculated)
%     time_const: time constant used on the plugin (in ms)
%     Fs          sampling rate of input
%
% Plots the actual output against the expected and outputs the root-mean-square error.
%
% Created in Apr. 2018 by Ethan Blackwood (ethanbblackwood@gmail.com)

assert(isvector(in_data), 'in_data must be a vector');
assert(isvector(out_data), 'out_data must be a vector');
N = length(in_data);
assert(length(out_data) == N, 'in_data and out_data must have the same length');
in_data = in_data(:);
out_data = out_data(:);

assert(isscalar(time_const) && time_const > 0, 'time_const must be a positive scalar');
assert(isscalar(Fs) && Fs > 0, 'Fs must be a positive scalar');

% setup exponential moving average/std/var
% see: https://en.wikipedia.org/wiki/Moving_average#Exponentially_weighted_moving_variance_and_standard_deviation
time_const_samp = time_const * Fs / 1000;
decay_rate = exp(-1/time_const_samp);
mean_gt = filter(1-decay_rate, [1 -decay_rate], in_data, decay_rate*in_data(1));

switch(stat_type)
    case 'mean'
        out_gt = mean_gt;
    case {'std', 'var'}
        var_gt = zeros(N, 1);
        delta = in_data(2:end) - mean_gt(1:end-1);
        var_gt(2:end) = filter(decay_rate, [1 -decay_rate], (1-decay_rate)*delta.^2);
        if strcmp(stat_type, 'var')
            out_gt = var_gt;
        else
            out_gt = sqrt(var_gt);
        end
    otherwise
        error('Unrecognized stat_type "%s"', stat_type);
end

rms_err = rms(out_data - out_gt);

figure;
plot((1:N)/Fs, out_data, 'r');
hold on;
plot((1:N)/Fs, out_gt, 'b');

legend('Continuous Stats output', 'Expected output');
xlabel('Time (s)');

end

