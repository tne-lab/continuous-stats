function [rms_err_mean, rms_err_std] = test_continuous_stats_run(raw_data, mean_data, std_data, time_const, Fs)
% test_continuous_stats_run.m: Compare the output of the Continuous Stats plugin with
% the expected results. Requires raw input, mean and std deviation data all recorded
% over the same period.
%
% Syntax: test_continuous_stats_run(in_data, out_data, stat_type, time_const, Fs);
%
% Plots the actual output against the expected for mean and std, and outputs the root-mean-square error for each.
%
% Created in Apr. 2018 by Ethan Blackwood (ethanbblackwood@gmail.com)

assert(isvector(raw_data), 'raw_data must be a vector');
assert(isvector(mean_data), 'mean_data must be a vector');
assert(isvector(std_data), 'std_data must be a vector');
N = length(raw_data);
assert(length(mean_data) == N, 'raw_data and mean_data must have the same length');
assert(length(std_data) == N, 'raw_data and std_data must have the same length');
raw_data = raw_data(:);
mean_data = mean_data(:);
std_data = std_data(:);

assert(isscalar(time_const) && time_const > 0, 'time_const must be a positive scalar');
assert(isscalar(Fs) && Fs > 0, 'Fs must be a positive scalar');

% setup exponential moving average/std/var
% see: https://en.wikipedia.org/wiki/Moving_average#Exponentially_weighted_moving_variance_and_standard_deviation
time_const_samp = time_const * Fs / 1000;
alpha = -expm1(-1/time_const_samp);

% calculate ground truth mean
mean_gt = mean_data;
mean_gt(2:end) = filter(alpha, [1 alpha-1], raw_data(2:end), (1-alpha)*mean_gt(1));

% calculate ground truth std deviation
std_gt = std_data;
delta = raw_data(2:end) - mean_gt(1:end-1);
std_gt(2:end) = sqrt(filter(1-alpha, [1 alpha-1], alpha*delta.^2, (1-alpha)*std_gt(1)^2));

% calc errors
rms_err_mean = rms(mean_data - mean_gt);
rms_err_std = rms(std_data - std_gt);

% plot
figure;
plot((1:N)/Fs, mean_data, 'r');
hold on;
plot((1:N)/Fs, mean_gt, 'b');
title(['Continuous Stats mean error with time const = ' num2str(time_const), ' ms']);
legend('Continuous Stats mean output', 'Expected mean output');
xlabel('Time (s)');

figure;
plot((1:N)/Fs, std_data, 'r');
hold on;
plot((1:N)/Fs, std_gt, 'b');
title(['Continuous Stats std dev error with time const = ' num2str(time_const), ' ms']);
legend('Continuous Stats std dev output', 'Expected std dev output');
xlabel('Time (s)');

end

