#include "config.h"

namespace RealVirtualMagic 
{
		
	//Config parameters
	int logging = 1;
	int useDebugger = false;

	// game parameters
	float minMagickaRate = -3;
	float maxMagickaRate = 7;
	float minSpellpower =  -100;
	float maxSpellpower = 100;
	float unstableMagicThreshold = -10;
	float unstableMagicDamage = -10;

	unsigned short hapticFrequency = 15;
	int hapticStrength = 1;

	//Other parameters
	bool leftHandedMode = false;

	void loadConfig()
	{
		std::string	runtimeDirectory = GetRuntimeDirectory();

		if (!runtimeDirectory.empty())
		{
			std::string filepath = runtimeDirectory + "Data\\SKSE\\Plugins\\RealVirtualMagic.ini";

			std::ifstream file(filepath);

			if (!file.is_open())
			{
				transform(filepath.begin(), filepath.end(), filepath.begin(), ::tolower);
				file.open(filepath);
			}

			_MESSAGE("Loading config file.");
			if (file.is_open())
			{
				std::string line;
				std::string currentSetting;
				while (std::getline(file, line))
				{
					//trim(line);
					skipComments(line);
					trim(line);
					if (line.length() > 0)
					{
						if (line.substr(0, 1) == "[")
						{
							//newsetting
							currentSetting = line;
						}
						else
						{							
							std::string variableName;
							int variableValue = GetConfigSettingsValue(line, variableName);
							if (variableName == "logging")
							{
								logging = variableValue;
							}
							else if (variableName == "hapticFrequency")
							{
								hapticFrequency = variableValue;
							}
							else if (variableName == "hapticStrength")
							{
								hapticStrength = variableValue;
							}
							else if (variableName == "minMagickaRate")
							{
								minMagickaRate = GetConfigSettingsFloatValue(line, variableName);
							}
							else if (variableName == "maxMagickaRate")
							{
								maxMagickaRate = GetConfigSettingsFloatValue(line, variableName);
							}
							else if (variableName == "minSpellpower")
							{
								minSpellpower = GetConfigSettingsFloatValue(line, variableName);
							}
							else if (variableName == "maxSpellpower")
							{
								maxSpellpower = GetConfigSettingsFloatValue(line, variableName);
							}
							else if (variableName == "unstableMagicDamage")
							{
								unstableMagicDamage = GetConfigSettingsFloatValue(line, variableName);
							}
							else if (variableName == "unstableMagicThreshold")
							{
								unstableMagicThreshold = GetConfigSettingsFloatValue(line, variableName);
							}
							else if (variableName == "useDebugger")
							{
								useDebugger = variableValue;
							}
							else
							{
								_MESSAGE("Unknown ini key: %s", variableName.c_str());
							}
						}
					}
				}
			}
			_MESSAGE("Config file is loaded successfully.");
			return;
		}

		_MESSAGE("Config file is not loaded.");
		return;
	}

	void Log(const int msgLogLevel, const char* fmt, ...)
	{
		if (msgLogLevel > logging)
		{
			return;
		}

		va_list args;
		char logBuffer[4096];

		va_start(args, fmt);
		vsprintf_s(logBuffer, sizeof(logBuffer), fmt, args);
		va_end(args);

		_MESSAGE(logBuffer);
	}
}