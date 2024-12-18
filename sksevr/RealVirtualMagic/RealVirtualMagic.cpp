#include "RealVirtualMagic.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <numeric>
#include <algorithm>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;
using namespace lsl;

namespace RealVirtualMagic
{
	bool IXRInitialized = false;
	bool eventMarkersInitialized = false;
	bool IXRIsInitializing = false;
	bool eventMarkersIsInitializing = false;

	// Use pointers for stream_inlet and stream_outlet
	stream_inlet* IXRStream = nullptr;
	stream_outlet* eventOutlet = nullptr;

	int creationCounter = 0;
	int creationCounterMax = 9999999; // LSL is re-initialized every 10s if it hadnt found a stream after 5s. set this to a lower number to give up at some point


	void InitializeIXRStream()
	{
		IXRIsInitializing = true;
		LOG_ERR("Resolving LSL streams");
		std::vector<stream_info> results = resolve_stream("type", "IXR-metric", 1, 5.0); // this line determines the LSL stream that we are searching. it aborts after 5s
		if (results.empty()) {
			LOG_ERR("No LSL streams found!");
		}
		else {
			std::string lslstring = results[0].as_xml();
			LOG_ERR("Here is what was resolved: %s", lslstring.c_str());

			LOG_ERR("Creating stream inlet");
			IXRStream = new stream_inlet(results[0]);  // Create a new stream_inlet with the resolved stream_info

			LOG_ERR("--------------- IXR Suite connected!!! ---------------");
			IXRInitialized = true;
		}
		IXRIsInitializing = false;
	}

	void CreateEventStream()
	{
		eventMarkersIsInitializing = true;
		LOG_ERR("Creating event marker stream outlet");
		// Marker streams are irregular and usually have channel format string
		stream_info info("RVM-events", "Markers", 1, IRREGULAR_RATE, cf_string, "RVM_Events_01");
		eventOutlet = new stream_outlet(info);  // Create a new stream_outlet with the defined stream_info
		eventMarkersIsInitializing = false;
		eventMarkersInitialized = true;
		LOG_ERR("--------------- Marker stream created!!! ---------------");
	}

	void CreateSystem()
	{
		if (creationCounter < creationCounterMax)
		{
			creationCounter++;
			LOG_ERR("Creating LSL system %i / %i", creationCounter, creationCounterMax);
			
			if (!IXRInitialized && !IXRIsInitializing)
			{
				std::thread t1(InitializeIXRStream);
				t1.detach();
			}

			if (!eventMarkersInitialized && !eventMarkersIsInitializing)
			{
				std::thread t2(CreateEventStream);
				t2.detach();
			}
		}

		// wait for 10s to make sure there are no two initialization processes going on
		Sleep(10000);
	}


	double GetFocusValue()
	{
		if (!IXRInitialized) {
			LOG_ERR("IXRStream is not initialized. Attempting to initialize.");
			CreateSystem();
			return 0.0;
		}

		// Create a buffer for the incoming sample
		std::vector<double> sample;

		// Pull the next sample from the IXRStream
		double timestamp = IXRStream->pull_sample(sample,5.0); // this will block if no stream is available at the moment and continue as soon as it is
		

		if (sample.empty() || sample[0]==0.0){
			delete IXRStream;
			IXRStream = nullptr;
			IXRInitialized = false;
			LOG_ERR("--------------- No sample available. IXRStream stream inlet closed!!! ---------------");
			return 0.0;
		}

		// Returning the first value in the sample
		LOG("LSL-timestamp: %f, sample[0]: %f", timestamp, sample[0]);
		return sample[0];
	}

	void WriteEventMarker(const std::string& eventType)
	{
		//board->insert_marker(eventType);
		LOG("marker: %s", eventType.c_str());

		// Send the event marker via LSL
		if (!eventMarkersInitialized) {
			LOG("LSL event marker stream is not initialized. Attempting to initialize.");
			CreateSystem();
			return;
		}
		std::vector<std::string> sample(1);
		sample[0] = eventType;
		eventOutlet->push_sample(sample);
	}
}
