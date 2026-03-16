// WhaleTeqSECG_SDK.h
#pragma once

#ifdef WHALETEQSECG_SDK_EXPORTS
#  if defined(LINUX_GCC) || defined(__APPLE__)
#    define WHALETEQSECG_SDK_API __attribute__ ((visibility ("default")))
#  else
#    define WHALETEQSECG_SDK_API __declspec(dllexport) 
#  endif
#else
#  if defined(LINUX_GCC) || defined(ANDROID) || defined(__APPLE__)
#    define WHALETEQSECG_SDK_API
#  else
#    define WHALETEQSECG_SDK_API __declspec(dllimport) 
#  endif
#endif

extern "C"{
        WHALETEQSECG_SDK_API void SpecifyCOMPort(int portNum);

	WHALETEQSECG_SDK_API bool InitSECG();
	WHALETEQSECG_SDK_API void ResetSECG();
	WHALETEQSECG_SDK_API void CloseSECG();

	WHALETEQSECG_SDK_API bool TriggerDeviceChanged();

	WHALETEQSECG_SDK_API char* GetUSBPort();

	WHALETEQSECG_SDK_API char* GetSerialNo();

	WHALETEQSECG_SDK_API char* GetHWInfo();
	WHALETEQSECG_SDK_API char* GetFWVersion();
	WHALETEQSECG_SDK_API char* GetHWVersion();

	WHALETEQSECG_SDK_API unsigned char* GetLicenseKey();
	WHALETEQSECG_SDK_API void WriteLicenseKey(unsigned char** B, int len, int offset);

	WHALETEQSECG_SDK_API int GetAD();

	//Parameter - time
	//	unit: second
	//	time from the output signal changed(function, amplitude, frequency ...)
	//Parameter - amp
	//	unit: microvolt
	typedef void (*TimeAmplitudeCB)(double time, double amp);

	//About 0.006 second(6ms) interval between each callback be triggered
	WHALETEQSECG_SDK_API void RegisterTimeAmpCB(TimeAmplitudeCB cb);

	WHALETEQSECG_SDK_API void RegisterPacingTimeAmpCB(TimeAmplitudeCB cb);


	enum OutputFunction_E {
		Output_Off	= 0,
		Output_Sine,
		Output_Triangle,
		Output_Square,
		Output_RectanglePulse,
		Output_TrianglePulse,
		Output_Exponential,
		Output_ECG2_27,//add specialwaveform 0713
		Output_IEC227W,
		Output_IEC251W,
		Output_JJG1041,
		Output_JJG1041_HR,
		Output_JJG_Hysteresis,
		Output_ECG_File

	};
	
	enum SpecialWave227_E{
		IEC227W_3A_A1 = 0,
		IEC227W_3B_A2,
		IEC227W_3C_A3,
		IEC227W_3D_A4,
		IEC227W_4A_B1,
		IEC227W_4A_B1x2,
		IEC227W_4A_B1d2,
		IEC227W_4B_B2,
		IEC227W_4B_B2x2,
		IEC227W_4B_B2d2,

	};
	enum SpecialWave251_E{
		IEC251W_1 = 0,
		IEC251W_2,
		IEC251W_3,
		IEC251W_4,
		IEC251W_6,
		IEC251W_10,
	};

	//Return value:
	// 0:  succeed
	// -1: Mainfunction not set to IEC227W
	// Call after set main func to Output_IEC227w
	WHALETEQSECG_SDK_API int SetSpecialWave227(SpecialWave227_E spec);
	WHALETEQSECG_SDK_API SpecialWave227_E GetSpecialWave227();
	//Return value:
	// 0:  succeed
	// -1: Mainfunction not set to IEC251W
	// Call after set main func to Output_IEC251w
	WHALETEQSECG_SDK_API int SetSpecialWave251(SpecialWave251_E spec);
	WHALETEQSECG_SDK_API SpecialWave251_E GetSpecialWave251();
	//Return value:
	//	0:	succeed
	//	-1:	SECG not be initialized
	WHALETEQSECG_SDK_API int SetOutputFunc(OutputFunction_E func);
	WHALETEQSECG_SDK_API OutputFunction_E GetOutputFunc();

	//Parameter - amp:
	//	value range: -10 ~ 10
	//	unit: microvolt(mV)
	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//	-2: value out of range
	//  -3: "Baseline Reset Test" is working
	//Default value after InitSECG(): 1mV
	WHALETEQSECG_SDK_API int SetAmplitude(double amp);
	WHALETEQSECG_SDK_API double GetAmplitude();
	#define DefaultAmplitude	1.0
	#define MaxAmplitude		10.0
	#define MinAmplitude		-10.0

	//Parameter - freq:
	//	value range: 0.05 ~ 500
	//               0.05 ~ 3 when Output as Pulse Waveforms
	//	unit: hertz
	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//	-2: value out of range
	//	-3: "Frequency Scan" is working
	//  -4: "Baseline Reset Test" is working
	//Default value after InitSECG(): 1Hz
	WHALETEQSECG_SDK_API int SetFrequency(double freq);
	WHALETEQSECG_SDK_API double GetFrequency();
	#define DefaultFrequency		1.0
	#define	MaxFrequency			500.0
	#define MinFrequency			0.05
	#define	MaxFrequencyPulseWave	3.0

	//Parameter - setting:
	//	value range: -300 ~ 300 - DCOffset Variable mode Off
	//               -1000 ~ 1000 - DCOffset Variable mode On
	//	unit: microvolt(mV)
	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//	-2: value out of range
	//Default value after InitSECG(): 0mV
	WHALETEQSECG_SDK_API int SetDCOffset(int setting);
	WHALETEQSECG_SDK_API int GetDCOffset();
	#define DefaultDCOffset		0
	#define MaxDCOffset			300
	#define MinDCOffset			-300
	#define MaxDCOffsetVarMode	1000
	#define MinDCOffsetVarMode	-1000

	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//Default value after InitSECG(): false(Off)
	WHALETEQSECG_SDK_API int SetDCOffsetVariableMode(bool OnOff);
	WHALETEQSECG_SDK_API bool GetDCOffsetVariableMode();

	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//Default value after InitSECG(): false(Off)
	WHALETEQSECG_SDK_API int SetDCOffsetCommoneMode(bool OnOff);
	WHALETEQSECG_SDK_API bool GetDCOffsetCommoneMode();

	enum OutputLead_E {
		Lead_RA		= 0,	//(R)
		Lead_LA,			//(L)
		Lead_LL,			//(F)
		Lead_V1,
		Lead_V2,
		Lead_V3,
		Lead_V4,
		Lead_V5,
		Lead_V6
	};
	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//Default value after InitSECG(): Lead_RA - On; all others - Off;
	WHALETEQSECG_SDK_API int SetOutputLead(OutputLead_E lead, bool OnOff);
	WHALETEQSECG_SDK_API bool GetOutputLead(OutputLead_E lead);

	//Parameter - width:
	//	value range: 2 ~ 300
	//	unit: milisecond(ms)
	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//	-2: value out of range
	//Default value after InitSECG(): 100ms
	WHALETEQSECG_SDK_API int SetPulseWidth(double width);
	WHALETEQSECG_SDK_API double GetPulseWidth();
	#define DefaultPulseWidth	100
	#define MaxPulseWidth		300
	#define MinPulseWidth		2

	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//Default value after InitSECG(): true(On - 620k&/4.7nF shorted)
	WHALETEQSECG_SDK_API int SetInputImpedanceMode(bool OnOff);
	WHALETEQSECG_SDK_API bool GetInputImpedanceMode();

	enum PacingPulse_E {
		SinglePulse	= 0,
		DoublePulse_150ms,
		DoublePulse_250ms
	};
	//Return value:
	//	0:	succeed
	//	-1:	SECG not be initialized
	//Default value after InitSECG(): SinglePulse
	WHALETEQSECG_SDK_API int SetPacingPulseMode(PacingPulse_E mode);
	WHALETEQSECG_SDK_API PacingPulse_E GetPacingPulseMode();

	//Parameter - time:
	//	value range: 5 ~ 200
	//	unit: milisecond(ms)
	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//	-2: value out of range
	//Default value after InitSECG(): 100ms
	WHALETEQSECG_SDK_API int SetQRSDuration(double time);
	WHALETEQSECG_SDK_API double GetQRSDuration();
	#define DefaultQRSDuration	100
	#define MaxQRSDuration	200
	#define MinQRSDuration	5

	//Parameter - amp:
	//	value range: 0 ~ 2.5
	//	unit: microvolt(mV)
	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//	-2: value out of range
	//Default value after InitSECG(): 0.2mV
	WHALETEQSECG_SDK_API int SetTWave(double amp);
	WHALETEQSECG_SDK_API double GetTWave();
	#define DefaultTWave	0.2
	#define MaxTWave		2.5
	#define MinTWave		0

	//Parameter - amp:
	//	value range: -700 ~ 700
	//	unit: microvolt(mV)
	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//	-2: value out of range
	//Default value after InitSECG(): 0mV
	WHALETEQSECG_SDK_API int SetPacingAmplitude(double amp);
	WHALETEQSECG_SDK_API double GetPacingAmplitude();
	#define DefaultPacingAmplitude	0
	#define MaxPacingAmplitude		700
	#define MinPacingAmplitude		-700

	//Parameter - time:
	//	value range: 0.1 ~ 2
	//	unit: milisecond(ms)
	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//	-2: value out of range
	//Default value after InitSECG(): 2ms
	WHALETEQSECG_SDK_API int SetPacingDuration(double time);
	WHALETEQSECG_SDK_API double GetPacingDuration();
	#define DefaultPacingDuration	2.0
	#define MaxPacingDuration		3.0
	#define MinPacingDuration		0.1

	//Parameter - BPM:
	//	value range: 10 ~ 300
	//	unit: beats per minute(BPM)
	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//	-2: value out of range
	//Default value after InitSECG(): 60BPM
	WHALETEQSECG_SDK_API int SetPacingRate(unsigned BPM);
	WHALETEQSECG_SDK_API unsigned GetPacingRate();
	#define DefaultPacingRate	60
	#define MaxPacingRate		300
	#define MinPacingRate		10

	//Parameter - time:
	//	value range: 0 ~ 100
	//	unit: milisecond(ms)
	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//	-2: value out of range
	//Default value after InitSECG(): 0ms
	WHALETEQSECG_SDK_API int SetOvershootTimeConstant(unsigned time);
	WHALETEQSECG_SDK_API unsigned GetOvershootTimeConstant();
	#define DefaultOvershootTimeConstant	0
	#define MaxOvershootTimeConstant		100
	#define MinOvershootTimeConstant		0

	//Parameter - freq
	//	unit: hertz
	//	frequency value changed during "Baseline Reset Test" or "Frequency Scanning"
	typedef void (*SyncFreqCB)(double freq);

	WHALETEQSECG_SDK_API void RegisterSyncFreqCB(SyncFreqCB cb);

	enum BaselineReset_E {
		Baseline_Off	= 0,
		Baseline_50Hz,
		Baseline_60Hz,
		Baseline_80Hz,
		Baseline_100Hz
	};
	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//	-2: current main output function not be "Output_Sine"
	//Default value after InitSECG(): Baseline_Off
	//"Output Function" must be set to "Output_Sine" before setting as "non - Baseline_Off" value
	//After setting as "non - Baseline_Off" value, "Amplitude" would be locked to "2.5mV"
	//and "Frequency" would be locked as "50/60/80/100" Hz.
	WHALETEQSECG_SDK_API int SetBaselineResetTest(BaselineReset_E mode);
	WHALETEQSECG_SDK_API BaselineReset_E GetBaselineResetTest();

	enum ECG2_27_NoiseFreq_E {
		Noise_None	= 0,
		Noise_50Hz,
		Noise_60Hz
	};
	//Parameter - freq:
	//	Default value after InitSECG(): Noise_None
	//Parameter - amp:
	//	value range: 0.01 ~ 0.2
	//	unit: microvolt(mV)
	//	Default value after InitSECG(): 0.1mV
	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//	-2: current main output function not be "Output_ECG2_27"
	//	-3: amp out of range 
	WHALETEQSECG_SDK_API int SetECG2_27_Noise(ECG2_27_NoiseFreq_E freq, double amp);
	WHALETEQSECG_SDK_API ECG2_27_NoiseFreq_E GetECG2_27_NoiseFreq();
	WHALETEQSECG_SDK_API double GetECG2_27_NoiseAmp();
	#define DefaultECG2_27_NoiseAmp	0.1
	#define MaxECG2_27_NoiseAmp		0.2
	#define MinECG2_27_NoiseAmp		0.01
 

	//Parameter - enableDriftTest
	//    true, enable AAMI EC 13 Drift test; adds 4mVpp 0.1Hz triangle waveform to the ECG signal 
	//    false, disable AAMI EC 13 Drift test
	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	WHALETEQSECG_SDK_API int SetECG2_27_AAMIDriftTest (bool enableDriftTest);
	WHALETEQSECG_SDK_API bool GetECG2_27_AAMIDriftTest ();

	//Parameter - amp:
	//	value range: 0 ~ 5
	//	unit: microvolt(mV)
	//	Default value after InitSECG(): 1mV
	//Parameter - freq:
	//	value range: 10 ~ 50
	//	unit: hertz
	//	Default value after InitSECG(): 40Hz
	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//	-2: current main output function not be "Output_Square"
	//	-3: amp out of range
	//	-4: freq out of range
	WHALETEQSECG_SDK_API int SetDynamicRangeTest(bool OnOff, double amp, double freq);
	WHALETEQSECG_SDK_API bool GetDynamicRangeTest();
	WHALETEQSECG_SDK_API double GetDynamicRangeTestAmp();
	WHALETEQSECG_SDK_API double GetDynamicRangeTestFreq();
	#define DefaultDynamicRangeTestAmp	1.0
	#define MaxDynamicRangeTestAmp		5.0
	#define MinDynamicRangeTestAmp		0
	#define DefaultDynamicRangeTestFreq	40
	#define MaxDynamicRangeTestFreq		50
	#define MinDynamicRangeTestFreq		10

	//Parameter - func
	//	main function changed during "Frequency Scanning"
	typedef void (*SyncMainFuncCB)(OutputFunction_E func);

	WHALETEQSECG_SDK_API void RegisterSyncMainFuncCB(SyncMainFuncCB cb);

	//Parameter - startFreq:
	//	value range: 0.67 ~ 500
	//	unit: hertz
	//	Default value after InitSECG(): 0.67Hz
	//Parameter - stopFreq:
	//	value range: 0.05 ~ 500
	//	unit: hertz
	//	Default value after InitSECG(): 150Hz
	//Parameter - duration:
	//	value range: 10 ~ 180
	//	unit: milisecond(ms)
	//	Default value after InitSECG(): 30ms
	//Return value:
	//	0: succeed
	//	-1: SECG not be initialized
	//	-2: startFreq out of range
	//	-3: stopFreq out of range
	//	-4: duration out of range
	//"Output Function" would be set to "Output_Sine" when setting "OnOff" as true,
	//then "Frequency" would be locked(SetFrequency() return -3) for periodly scanning change.
	//"Output Function" would be set to "Output_Off" when setting "OnOff" as false.
	WHALETEQSECG_SDK_API int SetFreqScanSine(bool OnOff, double startFreq, 
											 double stopFreq, int duration);
	WHALETEQSECG_SDK_API bool GetFreqScanSine();
	WHALETEQSECG_SDK_API double GetFreqScanSineStartFreq();
	WHALETEQSECG_SDK_API double GetFreqScanSineStopFreq();
	WHALETEQSECG_SDK_API int GetFreqScanSineDuration();
	#define DefaultScanSineStartFreq	0.67
	#define DefaultScanSineStopFreq		150
	#define MaxScanSineFreq				500.0
	#define MinScanSineFreq				0.67
	#define DefaultScanSineDuration		30
	#define MaxScanSineDuration			180
	#define MinScanSineDuration			10

	//Start Frequency as 3BPM(0.05Hz)
	//Stop Frequency as 30BPM(0.5Hz)
	//Duration as 30ms
	//"Output Function" would be set to "Output_ECG2_27" when setting "OnOff" as true,
	//then "Frequency" would be locked(SetFrequency() return -3) for periodly scanning change.
	//"Output Function" would be set to "Output_Off" when setting "OnOff" as false.
	WHALETEQSECG_SDK_API int SetFreqScanECG(bool OnOff);
	WHALETEQSECG_SDK_API bool GetFreqScanECG();

	//Return value:
	// >0: succeed, the number of sample data loaded
	// -1: Open file failed
	// -2: Can't solve sample rate in Line 1
	// -3: Can't solve sample number in Line 2
    // -4: Raw data file is too large to fit the memory
	WHALETEQSECG_SDK_API int LoadECGtxt(wchar_t* file);
	WHALETEQSECG_SDK_API int LoadECGbin(wchar_t* file);

	WHALETEQSECG_SDK_API char* GetSDKVersion();
}







