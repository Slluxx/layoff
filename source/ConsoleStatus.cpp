#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ConsoleStatus.hpp"

namespace layoff::console
{
	ConsoleStatus_t Status;
	static time_t LastUpdate = 0;

	static const time_t MinRefreshWaitSecs = 5;
		
	void RequestStatusUpdate()
	{
		LastUpdate = 0;
	}
	
	static inline void IpAddrToString(u32 i, char* buf)
	{
		snprintf(buf, 16 ,"%i.%i.%i.%i",
			i & 0xFF,			(i >> 8) & 0xFF, 
			(i >> 16) & 0xFF,	(i >> 24) & 0xFF);
	}
	
	static inline void DoStatusUpdate()
	{
		psmGetBatteryChargePercentage(&Status.BatteryLevel);
		psmGetChargerType(&Status.chargerType);
		
		Status.connectionStatus = NifmInternetConnectionStatus_ConnectingUnknown1;
		if (R_FAILED(nifmGetInternetConnectionStatus(&Status.connectionType, &Status.ConnectionStrenght, &Status.connectionStatus)))
			Status.IpAddress = 0;
		else {
			nifmIsWirelessCommunicationEnabled(&Status.WirelessEnabled);
			nifmGetCurrentIpAddress(&Status.IpAddress);
			IpAddrToString(Status.IpAddress, Status.IpStr);
		}

		lblGetCurrentBrightnessSetting(&Status.BrightnessLevel);
		lblIsAutoBrightnessControlEnabled(&Status.AutoBrightness);	

		timeGetCurrentTime(TimeType_UserSystemClock, &Status.Timestamp);
		timeToCalendarTimeWithMyRule(Status.Timestamp, &Status.DateTime, nullptr);
	}
	
	void UpdateStatus()
	{
		if (time(NULL) - LastUpdate <= MinRefreshWaitSecs) return;
		LastUpdate = time(NULL);		
		
		DoStatusUpdate();
	}	
}
