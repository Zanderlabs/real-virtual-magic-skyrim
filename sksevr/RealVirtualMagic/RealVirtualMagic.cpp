#include "RealVirtualMagic.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <numeric>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

namespace RealVirtualMagic
{
	bool systemInitialized = false;
	float TOLERANCE = 0.00001f;

	BoardShim* board;
	json board_descr;
	int sampling_rate;
	int fft_len;
	std::vector<int> eegChannels;
	std::vector<int> gyroChannels;
	std::vector<double> brain_power_history;
	std::vector<double> brain_power_calib_history;

	int nullPackages;
	int initializations = 0;

	void CreateSystem()
	{
		if (!systemInitialized && initializations < 3)
		{
			initializations += 1;

			LOG_ERR("Mind reading initialization %i of 3...", initializations);

			brain_power_history = std::vector<double>();
			brain_power_calib_history = std::vector<double>();

			BoardShim::set_log_file("BrainflogLog.txt");
			BoardShim::enable_dev_board_logger();

			struct BrainFlowInputParams params;

			LOG_ERR("boardID = %i", boardID);

			if (boardID == -1)
			{
				LOG_ERR("Using synthetic board for testing!");
			}
			else if (boardID == 22)
			{
				LOG_ERR("Using Muse 2!");
				std::string serialPort = "COM" + std::to_string(serialPortID);
				LOG_ERR("Serial port = %s", serialPort.c_str());
				params.serial_port = serialPort;
			}
			else if (boardID == 21)
			{
				LOG_ERR("Using Muse S!");
				std::string serialPort = "COM" + std::to_string(serialPortID);
				LOG_ERR("Serial port = %s", serialPort.c_str());
				params.serial_port = serialPort;
			}
			else if (boardID == 38)
			{
				LOG_ERR("Using Muse 2 with regular bluetooth!");
			}
			else if (boardID == 39)
			{
				LOG_ERR("Using Muse S with regular bluetooth!");
			}
			else
			{
				LOG_ERR("Unknown board ID!");
				return;
			}

			LOG_ERR("Creating board...");

			params.timeout = timeout;
			board = new BoardShim(boardID, params);
			LOG_ERR("Board created!");

			try
			{


				board_descr = BoardShim::get_board_descr(boardID);
				std::string board_desc_str = board_descr.dump();
				LOG_ERR("Board description:");
				LOG_ERR("%s", board_desc_str.c_str());


				LOG_ERR("Preparing session...");
				board->prepare_session();
				LOG_ERR("Session running!");

				LOG_ERR("Starting stream...");
				board->start_stream();
				LOG_ERR("Stream running!");

#ifdef _WIN32
				Sleep(1500);
#else
				sleep(1.5);
#endif

				sampling_rate = (int)board_descr["sampling_rate"];
				fft_len = DataFilter::get_nearest_power_of_two(sampling_rate);
				std::vector<int> eeg_channels = board_descr["eeg_channels"];
				eegChannels = eeg_channels;
				std::vector<int> gyro_channels = board_descr["gyro_channels"];
				gyroChannels = gyro_channels;

				LOG_ERR("Getting test data batch values...");
				BrainFlowArray<double, 2> data = board->get_board_data();

				if (logBrainData)
				{
					DataFilter::write_file(data, "brainData.tsv", "w");
				}

				for (int i = 0; i < eegChannels.size(); i++)
				{
					LOG_ERR("channel %i", i);
					int channel = eegChannels[i];
					// optional - detrend
					DataFilter::detrend(
						data.get_address(channel), data.get_size(1), (int)DetrendOperations::LINEAR);
					//std::cout << "Data after detrend:" << std::endl << data << std::endl;
					int datasize = data.get_size(1);
					std::pair<double*, double*> psd = DataFilter::get_psd_welch(data.get_address(channel),
						data.get_size(1), fft_len, fft_len / 2, sampling_rate, (int)WindowFunctions::HANNING);
					// calc band power
					double band_power_delta = DataFilter::get_band_power(psd, fft_len / 2 + 1, 2.0, 4.0);
					double band_power_theta = DataFilter::get_band_power(psd, fft_len / 2 + 1, 4.0, 7.0);
					double band_power_alpha = DataFilter::get_band_power(psd, fft_len / 2 + 1, 7.0, 13.0);
					double band_power_beta = DataFilter::get_band_power(psd, fft_len / 2 + 1, 14.0, 30.0);
					double band_power_gamma = DataFilter::get_band_power(psd, fft_len / 2 + 1, 30.0, 60.0);
					double band_power_line = DataFilter::get_band_power(psd, fft_len / 2 + 1, 49.5, 50.5);
					//std::cout << "alpha/beta:" << band_power_alpha / band_power_beta << std::endl;
					LOG_ERR("delta: %f, theta: %f, alpha: %f, beta: %f, gamma: %f, line: %f", band_power_delta, band_power_theta, band_power_beta, band_power_gamma, band_power_line);
					LOG_ERR("alpha/theta ratio: %f, line/gamma ratio: %f, beta / (theta + alpha): %f", band_power_alpha / band_power_theta, band_power_line / band_power_gamma, band_power_beta / (band_power_theta + band_power_alpha));
					delete[] psd.first;
					delete[] psd.second;
				}

#ifdef _WIN32
				Sleep(1500);
#else
				sleep(1);
#endif

				nullPackages = 0;
				systemInitialized = true;
				LOG_ERR("Mind reading initialization done!");

			}
			catch (const BrainFlowException& err)
			{
				BoardShim::log_message((int)LogLevels::LEVEL_ERROR, err.what());
				if (board->is_prepared())
				{
					board->release_session();
				}
				LOG_ERR("Initialization failed. exitCode: %d - error: %s", err.exit_code, err.what());
			}

		}

	}

	double GetBrainData()
	{
		double res = 0;
		if (systemInitialized)
		{
			try
			{
				BrainFlowArray<double, 2> data = board->get_board_data();

				if (logBrainData)
				{
					DataFilter::write_file(data, "brainData.tsv", "a");
				}

				int databatchlength = data.get_size(1);
				LOG("Got %i brain data batch values",databatchlength);
				if (databatchlength >= fft_len)
				{
					nullPackages = 0;
					// GYRO

					float sumchannel = 0;
					for (int i = 0; i < gyroChannels.size(); i++)
					{
						//LOG("channel %i", i);
						int channel = gyroChannels[i];

						for (int j = 0; j < databatchlength; j++)
						{
							sumchannel += abs(data.at(gyroChannels[i], j));
						}

					}
					float headmovement = sumchannel / (databatchlength * gyroChannels.size());
					LOG("head movement: %f", headmovement);

					// EEG

					vector<double> posterior_alpha = vector<double>();
					vector<double> anterior_theta = vector<double>();
					vector<double> all_chan_alpha_beta_ratios = vector<double>();
					vector<double> all_chan_engagement_index = vector<double>();
					vector<double> all_chan_lineratios = vector<double>();

					for (int i = 0; i < eegChannels.size(); i++)
					{
						LOG("channel %i", i);
						int channel = eegChannels[i];
						// optional - detrend
						DataFilter::detrend(
							data.get_address(channel), databatchlength, (int)DetrendOperations::LINEAR);
						//std::cout << "Data after detrend:" << std::endl << data << std::endl;
						std::pair<double*, double*> psd = DataFilter::get_psd_welch(data.get_address(channel),
							databatchlength, fft_len, fft_len / 2, sampling_rate, (int)WindowFunctions::HANNING);
						// calc band power
						double band_power_delta = DataFilter::get_band_power(psd, fft_len / 2 + 1, 2.0, 4.0);
						double band_power_theta = DataFilter::get_band_power(psd, fft_len / 2 + 1, 4.0, 7.0);
						double band_power_alpha = DataFilter::get_band_power(psd, fft_len / 2 + 1, 7.0, 13.0);
						double band_power_beta = DataFilter::get_band_power(psd, fft_len / 2 + 1, 14.0, 30.0);
						double band_power_gamma = DataFilter::get_band_power(psd, fft_len / 2 + 1, 30.0, 60.0);
						double band_power_line = DataFilter::get_band_power(psd, fft_len / 2 + 1, 49.5, 50.5);
						double band_power_belowline = DataFilter::get_band_power(psd, fft_len / 2 + 1, 48, 49.5);
						double band_power_aboveline = DataFilter::get_band_power(psd, fft_len / 2 + 1, 50.5, 52);
						//std::cout << "alpha/beta:" << band_power_alpha / band_power_beta << std::endl;
						LOG("delta: %f, theta: %f, alpha: %f, beta: %f, gamma: %f, line: %f", band_power_delta, band_power_theta, band_power_beta, band_power_gamma, band_power_line);
						LOG("alpha/theta ratio: %f, line/gamma ratio: %f, beta / (theta + alpha): %f", band_power_alpha / band_power_theta, band_power_line / ((band_power_belowline + band_power_aboveline) / 2), band_power_beta / (band_power_theta + band_power_alpha));

						all_chan_alpha_beta_ratios.push_back(band_power_alpha / band_power_theta);
						all_chan_engagement_index.push_back(band_power_beta / (band_power_theta + band_power_alpha));
						all_chan_lineratios.push_back(band_power_line / ((band_power_belowline + band_power_aboveline) / 2));

						if (i == 1 || i == 4)
						{
							posterior_alpha.push_back(band_power_alpha);
						}
						else
						{
							anterior_theta.push_back(band_power_theta);
						}

						delete[] psd.first;
						delete[] psd.second;
					}

					double sumall = std::accumulate(all_chan_alpha_beta_ratios.begin(), all_chan_alpha_beta_ratios.end(), 0.0);
					double meanall = sumall / all_chan_alpha_beta_ratios.size();

					LOG("Mean raw brain power all channels: %f", meanall);

					double brainpower = log(std::accumulate(posterior_alpha.begin(), posterior_alpha.end(), 0.0) / std::accumulate(anterior_theta.begin(), anterior_theta.end(), 0.0));

					LOG("Posterior alpha / anterior theta brain power: %f", brainpower);

					double sumline = std::accumulate(all_chan_lineratios.begin(), all_chan_lineratios.end(), 0.0);
					double meanline = sumline / all_chan_lineratios.size();
					LOG("Mean line ratios: %f", meanline);
					double sumengagement = std::accumulate(all_chan_engagement_index.begin(), all_chan_engagement_index.end(), 0.0);
					double meanengagement = sumengagement / all_chan_engagement_index.size();
					LOG("Mean engagement index: %f", meanengagement);

					// store for calibration
					brain_power_calib_history.push_back(brainpower);

					if (brain_power_calib_history.size() > historyLengthCalibration)
					{
						brain_power_calib_history.erase(brain_power_calib_history.begin());
					}

					// compute calib mean and var

					double sum_calib = 0;
					for (int i = 0; i < brain_power_calib_history.size(); i++)
					{
						sum_calib += brain_power_calib_history.at(i);
					}

					double mean_calib = sum_calib / brain_power_calib_history.size();

					double std_calib = 0;
					for (int i = 0; i < brain_power_calib_history.size(); i++)
					{
						std_calib += (brain_power_calib_history.at(i) - mean_calib) * (brain_power_calib_history.at(i) - mean_calib); // summing up
					}
					std_calib /= brain_power_calib_history.size(); // compute var
					std_calib = sqrt(std_calib); // this is std

					LOG("calib mean: %f, calib std: %f", mean_calib, std_calib);

					// detect outliers
					if (brain_power_calib_history.size() > round(historyLengthCalibration/5))
					{
						bool outlier_detected = false;
						for (int i = 0; i < brain_power_calib_history.size(); i++)
						{
							if ((brain_power_calib_history.at(i) >= mean_calib + 3 * std_calib) || (brain_power_calib_history.at(i) <= mean_calib - 3 * std_calib))
							{
								LOG("Erasing value %f at %i", brain_power_calib_history.at(i), i);
								brain_power_calib_history.erase(brain_power_calib_history.begin()+i);
								outlier_detected = true;
							}
						}
						if (outlier_detected)
						{
							double sum_calib = 0;
							for (int i = 0; i < brain_power_calib_history.size(); i++)
							{
								sum_calib += brain_power_calib_history.at(i);
							}

							double mean_calib = sum_calib / brain_power_calib_history.size();

							double std_calib = 0;
							for (int i = 0; i < brain_power_calib_history.size(); i++)
							{
								std_calib += (brain_power_calib_history.at(i) - mean_calib) * (brain_power_calib_history.at(i) - mean_calib); // summing up
							}
							std_calib /= brain_power_calib_history.size(); // compute var
							std_calib = sqrt(std_calib); // this is std

							LOG("calib mean: %f, calib std: %f", mean_calib, std_calib);
						}
					}
					
					// scale & clamp
					/*
					double meanoldscale = mean * brainScale;

					if (meanoldscale < brainOffset)
					{
						LOG("current brain power at lower bound: %f, correcting to %f, which will be 0", meanoldscale, brainOffset);
						meanoldscale = brainOffset;
					}
					else if (meanoldscale > 1 + brainOffset)
					{
						LOG("current brain power at upper bound: %f, correcting to %f, which will be 1", meanoldscale, 1 + brainOffset);
						meanoldscale = 1 + brainOffset;
					}

					meanoldscale -= brainOffset;
					LOG("brain power by regular scale: %f", meanoldscale);
					//LOG_ERR("current brain power after correction: %f", mean);

					*/

					// calibrated brainpower
					if (std_calib != 0)
					{
						brainpower = ((brainpower - mean_calib) / std_calib); // z scored
						brainpower /= brainScale*2; // scaled to the sigma threshold
						brainpower += brainOffset; // recentered
					}
					else
					{
						LOG("std is zero!");
					}
					LOG("scaled brain power: %f", brainpower);

					// add headmovement
					if (headmovement < 4)
					{
						brainpower += 0.15;
					}
					else if (headmovement < 8)
					{
						brainpower += 0.1;
					}
					else if (headmovement < 12)
					{
						brainpower += 0.05;
					}
					else if (headmovement > 20 && headmovement < 25)
					{
						brainpower -= 0.1;
					}
					else if (headmovement > 25)
					{
						brainpower -= 0.25;
					}
					else if (headmovement > 30)
					{
						brainpower -= 0.5;
					}
					else if (headmovement > 35)
					{
						brainpower -= 1;
					}

					LOG("current brain power incl headmovement: %f", brainpower);

					// clamp
					if (brainpower < 0)
					{
						LOG("current brain power incl headmovement at lower bound: %f, correcting to 0", brainpower);
						brainpower = 0;
					}
					else if (brainpower > 1 && headmovement < 10)
					{
						LOG("current brain power incl headmovement at upper bound: %f, correcting to 1", brainpower);
						brainpower = 1;
					}
					else if (brainpower > 1 && headmovement >= 10)
					{
						LOG("current brain power incl headmovement at upper bound: %f, correcting to 0.3 due to strong headmovement!", brainpower);
						brainpower = 0.3;
					}

					brain_power_history.push_back(brainpower);

					if (brain_power_history.size() > historyLength)
					{
						brain_power_history.erase(brain_power_history.begin());
					}

					double weighted_sum_history = 0;
					for (int i = 0; i < brain_power_history.size(); i++)
					{
						//LOG_ERR("brain power %i: %f", i, brain_power_history.at(i)* (i + 1));
						weighted_sum_history += brain_power_history.at(i) * (i + 1);
						//LOG_ERR("weighted sum history: %f", weighted_sum_history);
					}

					double meanweightfactor = 0;
					for (int i = 1; i <= brain_power_history.size(); i++)
					{
						meanweightfactor += (double)i;
					}
					//LOG_ERR("mean weight factor: %f", meanweightfactor);

					weighted_sum_history = weighted_sum_history / meanweightfactor;

					//double sum_history = std::accumulate(brain_power_history.begin(), brain_power_history.end(), 0.0);
					//double mean_history = sum_history / brain_power_history.size();

					float clamped_brain_power = weighted_sum_history;// *2.5 - 0.9;

					// clamp
					if (clamped_brain_power < 0)
					{
						LOG("weighted brain power at lower bound: %f, correcting to 0", clamped_brain_power);
						clamped_brain_power = 0;
					}
					else if (clamped_brain_power > 1)
					{
						LOG("weighted brain power at upper bound: %f, correcting to 1", clamped_brain_power);
						clamped_brain_power = 1;
					}

					res = clamped_brain_power;
				}
				else
				{
					LOG_ERR("Data batch length insufficient! Is: %i, needs: %i", databatchlength, fft_len * 2);
					nullPackages++;

					if (nullPackages >= 10)
					{
						LOG_ERR("Board lost!");

						if (board->is_prepared())
						{
							LOG_ERR("Is prepared, releasing session...");
							board->release_session();
						}
						else
						{
							LOG_ERR("Is not prepared...");
						}
						systemInitialized = false;
						LOG_ERR("Re-initializing...");
						CreateSystem();
					}
				}

			}
			catch (const BrainFlowException& err)
			{
				BoardShim::log_message((int)LogLevels::LEVEL_ERROR, err.what());
				if (board->is_prepared())
				{
					board->release_session();
				}
				systemInitialized = false;
				LOG_ERR("Board broken! Re-initializing... exitCode: %d - error: %s", err.exit_code, err.what());
				CreateSystem();
			}

		}
		else
		{
			LOG_ERR("Board uninitialized! Re-initializing...");
			CreateSystem();
		}

		return res;

	}

	void WriteEventMarker(float eventType)
	{
		board->insert_marker(eventType);
		LOG("marker: %f", eventType);
	}


}
