#pragma once

#if defined(WIN32)
#ifdef WHALETEQ_API

#else
#define WHALETEQ_API extern "C" __declspec(dllimport)
#endif
#elif defined(LINUX)
#define WHALETEQ_API extern "C" __attribute__ ((visibility ("default")))
#endif

#define DefaultAmplitude	100.0
#define MaxAmplitude		2000.0
#define MinAmplitude		-2000.0

#define DefaultFrequency		10.0
#define	MaxFrequency			500.0
#define MinFrequency			0.05
#define	MaxFrequencyPulseWave	5.0

#define DefaultDCOffset		0
#define MaxDCOffset			300
#define MinDCOffset			-300
#define MaxDCOffset_100E	150
#define MinDCOffset_100E	-150
#define MaxDCOffsetVarMode	1000
#define MinDCOffsetVarMode	-1000

#define DefaultPulseWidth	100
#define MaxPulseWidth		300
#define MinPulseWidth		2

#define DefaultScanSineStartFreq	0.5
#define DefaultScanSineStopFreq		150
#define MaxScanSineFreq				500.0
#define MinScanSineFreq				0.5
#define MinScanSineStopFreq			0.01
#define DefaultScanSineDuration		30
#define MaxScanSineDuration			180
#define MinScanSineDuration			10

typedef enum _OutputFunctionEEG_E {
    EEG_Output_Off = 0,
    EEG_Output_Sine,
    EEG_Output_Triangle,
    EEG_Output_Square,
    EEG_Output_RectanglePulse,
    EEG_Output_TrianglePulse,
    EEG_Output_Hysteresis,
    EEG_Output_PlayWaveform
} OutputFunctionEEG_E;

typedef enum _OutputLead_E {
    Lead_Ch1 = 0,
    Lead_Ch2,
    Lead_Ch3,
    Lead_Ch4,
    Lead_Ch5,
    Lead_Ch6,
    Lead_Ch7,
    Lead_Ch8,
    Lead_Ch9
    //Channel 10~22 are shorted together
} OutputLead_E;

typedef enum _BaselineTest_E {
    Baseline_Off = 0,
    Baseline_50Hz,
    Baseline_60Hz,
    Baseline_80Hz,
    Baseline_100Hz
} BaselineTest_E;

/// \fn ConnectedCallback
/// \brief Called when the device is connected or disconnected
/// @param[in] connected    true if connected; otherwise, it's false
typedef void (*ConnectedCallback) (bool connected);

//Parameter - time
//	unit: second
//	time from the output signal changed(function, amplitude, frequency ...)
//Parameter - amp
//	unit: millivolt(mV)
typedef void (*TimeAmplitudeCallback) (double time, double amp);

//Parameter - freq
//	unit: hertz
//	frequency value changed during "Baseline Reset Test" or "Frequency Scanning"
typedef void (*SyncFreqCallback) (double freq);

//Parameter - func
//	main function changed during "Frequency Scanning"
typedef void (*SyncMainFuncCallback)(int func);

///
/// Initialization
///
/// During initialization, it will try to connect a device. If a device is found,
/// the cb function will be called. After then, if a device is disconnected,
/// the cb will be called again to notify the disconnection event. 
/// @param[in] cb  a callback function to notify the connection or disconnection event
///
WHALETEQ_API
bool
Init (
    ConnectedCallback cb
);

///
/// Connect the device
///
/// @param[in] portNumber            Device COM port number; -1 means the port number is automatically selected
/// @param[in] millisecondsTimeout   Connection timeout; the number of milliseconds to connect, or -1 to wait indefinitely.
/// @return true if the device is connected; false if the time-out interval elapsed and 
///         the device is still not connected
///
WHALETEQ_API
bool
Connect (
    unsigned int portNumber,
    unsigned int millisecondsTimeout
);

WHALETEQ_API
bool
IsEEG100E (
    void
);

WHALETEQ_API
void
ResetSEEG (
    void
);

WHALETEQ_API
void
Close (
    void
);

WHALETEQ_API
bool
IsConnected (
    void
);

WHALETEQ_API
char*
GetSerialNumber (
    void
);

WHALETEQ_API
char*
GetFWVersion (
    void
);

WHALETEQ_API
char*
GetHWVersion (
    void
);

//About 0.006 second(6ms) interval between each callback be triggered
WHALETEQ_API
void
RegisterTimeAmpCB (
    TimeAmplitudeCallback cb
);

//The "TimeAmplitudeCB cb" would be triggered during outputting waveform with pacing signal
WHALETEQ_API
void
RegisterPacingTimeAmpCB (
    TimeAmplitudeCallback cb
);

WHALETEQ_API
void
RegisterSyncFreqCB (
    SyncFreqCallback cb
);

WHALETEQ_API
void
RegisterSyncMainFuncCB (
    SyncMainFuncCallback cb
);

//Return value:
//	0:	succeed
//	-1:	SEEG not be initialized
WHALETEQ_API
int
SetOutputFunc (
    OutputFunctionEEG_E func
);

WHALETEQ_API
OutputFunctionEEG_E
GetOutputFunc (
    void
);

//Parameter - amp:
//	value range: -2000 ~ 2000
//	unit: microvolt(uV)
//Return value:
//	0: succeed
//	-1: SEEG not be initialized
//	-2: value out of range
//  -3: "Baseline Reset Test" is working
//Default value after InitSEEG(): 100uV
WHALETEQ_API
int
SetAmplitude (
    double amp
);

WHALETEQ_API
double
GetAmplitude (
    void
);

//Parameter - freq:
//	value range: 0.05 ~ 500
//               0.05 ~ 3 when Output as Pulse Waveforms
//	unit: hertz
//Return value:
//	0: succeed
//	-1: SEEG not be initialized
//	-2: value out of range
//	-3: "Frequency Scan" is working
//  -4: "Baseline Reset Test" is working
//Default value after InitSEEG(): 10Hz
WHALETEQ_API
int
SetFrequency (
    double freq
);

WHALETEQ_API
double
GetFrequency (
    void
);

//Parameter - setting:
//	value range: -300 ~ 300 - DCOffset Variable mode Off
//				 -150 ~ 150 - DCOffset Variable mode Off for 100E
//               -1000 ~ 1000 - DCOffset Variable mode On
//	unit: millivolt(mV)
//Return value:
//	0: succeed
//	-1: SEEG not be initialized
//	-2: value out of range
//Default value after InitSEEG(): 0mV
WHALETEQ_API
int
SetDCOffset (
    int dcOffset
);

WHALETEQ_API
int
GetDCOffset (
    void
);

//Return value:
//	0: succeed
//	-1: SEEG not be initialized
//Default value after InitSEEG(): false(Off)
WHALETEQ_API
int
SetDCOffsetVariableMode (
    bool OnOff
);

WHALETEQ_API
bool
GetDCOffsetVariableMode (
    void
);


//Return value:
//	0: succeed
//	-1: SEEG not be initialized
//Default value after InitSEEG(): Lead_Ch1 - On; all others - Off;
WHALETEQ_API
int
SetOutputLead (
    OutputLead_E lead,
    bool OnOff
);

WHALETEQ_API
bool
GetOutputLead (
    OutputLead_E lead
);

//Parameter - width:
//	value range: 2 ~ 300
//	unit: milisecond(ms)
//Return value:
//	0: succeed
//	-1: SEEG not be initialized
//	-2: value out of range
//Default value after InitSEEG(): 100ms
WHALETEQ_API
int
SetPulseWidth (
    double width
);

WHALETEQ_API
double
GetPulseWidth (
    void
);

//Return value:
//	0: succeed
//	-1: SEEG not be initialized
//Default value after InitSEEG(): true(On - 620k&/4.7nF shorted)
WHALETEQ_API
int
SetInputImpedanceMode (
    bool OnOff
);

WHALETEQ_API
bool
GetInputImpedanceMode (
    void
);

//Return value:
//	0: succeed
//	-1: SEEG not be initialized
//	-2: current main output function not be "Output_Sine"
//Default value after InitSEEG(): Baseline_Off
//"Output Function" must be set to "Output_Sine" before setting as "non - Baseline_Off" value
//After setting as "non - Baseline_Off" value, "Amplitude" would be locked to "2.5mV"
//and "Frequency" would be locked as "50/60/80/100" Hz.
WHALETEQ_API
int
SetBaselineResetTest (
    BaselineTest_E mode
);

WHALETEQ_API
BaselineTest_E
GetBaselineResetTest (
    void
);

//Parameter - startFreq:
//	value range: 0.5 ~ 500
//	unit: hertz
//	Default value after InitSEEG(): 0.5Hz
//Parameter - stopFreq:
//	value range: 0.05 ~ 500
//	unit: hertz
//	Default value after InitSEEG(): 150Hz
//Parameter - duration:
//	value range: 10 ~ 180
//	unit: millisecond(ms)
//	Default value after InitSEEG(): 30ms
//Return value:
//	0: succeed
//	-1: SEEG not be initialized
//	-2: startFreq out of range
//	-3: stopFreq out of range
//	-4: duration out of range
//"Output Function" would be set to "Output_Sine" when setting "OnOff" as true,
//then "Frequency" would be locked(SetFrequency() return -3) for periodly scanning change.
//"Output Function" would be set to "Output_Off" when setting "OnOff" as false.
WHALETEQ_API
int
SetFreqScanSine (
    bool OnOff,
    double startFreq,
    double stopFreq,
    int duration
);

WHALETEQ_API
bool
GetFreqScanSine (
    void
);

WHALETEQ_API
double
GetFreqScanSineStartFreq (
    void
);

WHALETEQ_API
double
GetFreqScanSineStopFreq (
    void
);

WHALETEQ_API
int
GetFreqScanSineDuration (
    void
);


//Return value:
// >0: succeed, the number of sample data loaded
// -1: Open file failed
// -2: Can't solve sample rate in Line 1
// -3: Can't solve sample number in Line 2
// -4: Raw data file is too large to fit the memory
WHALETEQ_API
int
LoadEEGtxt (
    wchar_t* file
);

WHALETEQ_API
int
LoadEEGbin (
    wchar_t* file
);

//Return value:
// >0: succeed, the number of sample data loaded
// -1: Open file failed
// -2: Can't solve signal corresponding to signalIdx
// -4: Raw data file is too large to fit the memory
WHALETEQ_API
int
LoadEEGedf (
    wchar_t* file,
    int signalIdx
);

WHALETEQ_API
char*
GetSDKVersion (
    void
);
