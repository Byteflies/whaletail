from ctypes import *
from enum import Enum, auto
import time

class OutputFunction_E (Enum):
    Output_Off = 0
    Output_Sine = auto()
    Output_Triangle = auto()
    Output_Square = auto()
    Output_RectanglePulse = auto()
    Output_TrianglePulse = auto()
    Output_Exponential = auto()
    Output_ECG2_27 = auto()
    Output_IEC227W = auto()
    Output_IEC251W = auto()
    Output_JJG1041 = auto()
    Output_JJG1041_HR = auto()
    Output_JJG_Hysteresis = auto()
    Output_ECG_File = auto()
    Output_InputDynamicRange = auto()
    Output_ANE20000 = auto()

def TestMisc():
    print('TestMisc -----------------------------------------------------')

    sdk.GetFWVersion.restype = c_char_p
    print('FirmWare Version is: ', sdk.GetFWVersion())

    sdk.GetSDKVersion.restype = c_char_p
    print('SDK Version is: ', sdk.GetSDKVersion())

    sdk.GetSerialNo.restype = c_char_p
    print('Device Serial Number is: ', sdk.GetSerialNo())


def TestDCOffset():
    print('TestDCOffset -----------------------------------------------------')
    #DC Offset Related APIs
    DCOffset = sdk.GetDCOffset()
    print('Initial DC Offset: ', DCOffset, ' mV') 

    
    DCVarMode = sdk.GetDCOffsetVariableMode()
    if 0 != DCVarMode:
        print('Initial DC Offset Variable Mode: On') 
    else:
        print('Initial DC Offset Variable Mode: Off') 
   
    DCComMode = sdk.GetDCOffsetCommoneMode()
    if 0 != DCComMode:
        print('Initial DC Offset Common Mode: On') 
    else:
        print('Initial DC Offset Common Mode: Off') 


    #Under "Non-Variable Mode", only "300mV" and "-300mV" 2 value can be set
    DCOffset = 300
    sdk.SetDCOffset(DCOffset)
    sdk.SetOutputFunc(OutputFunction_E.Output_Sine.value)
    print('Output Sine signal with 300mV DC offset')
    time.sleep(5)


    DCOffset = -300
    sdk.SetDCOffset(DCOffset)
    print('Output Sine signal with -300mV DC offset')
    time.sleep(5)

    #Under "Variable Mode", between "-1000mV"  and "1000mV" integer value van be set
    sdk.SetDCOffsetVariableMode(1)
    DCVarMode = sdk.GetDCOffsetVariableMode()
    if 0 != DCVarMode:
        print('Turn On DC Offset Variable Mode.')

    DCOffset = 150
    sdk.SetDCOffset(DCOffset) #Output Sine signal with 150mV DC offset
    print('Output Sine signal with 150mV DC offset')
    time.sleep(5)
    
    DCOffset = -150
    sdk.SetDCOffset(DCOffset) #Output Sine signal with -150mV DC offset
    print('Output Sine signal with -150mV DC offset')
    time.sleep(5)

    
    sdk.SetOutputFunc(OutputFunction_E.Output_Off.value)
    sdk.SetDCOffset(0)
    sdk.SetDCOffsetVariableMode(0) #Common Mode only work without Variable Mode

    sdk.SetDCOffsetCommoneMode(1)
    DCComMode = sdk.GetDCOffsetCommoneMode()
    if 0 != DCComMode:
        print('Turn On DC Offset Common Mode.')

    DCOffset = 300
    sdk.SetDCOffset(DCOffset)
    sdk.SetOutputFunc(OutputFunction_E.Output_Sine.value)
    print('Output Sine signal with 300mV DC offset in common mode')
    time.sleep(5)


def TestImpedance():
    print('TestImpedance -----------------------------------------------------')

    ImpedMode = sdk.GetInputImpedanceMode()
    if 0 != ImpedMode:
        print('Initial Input Impedance Mode: On (without 620K Ohm impedance)') 
    else:
        print('Initial Input Impedance Mode: Off (with 620K Ohm impedance)') 

    amp = c_double(5)
    sdk.SetAmplitude(amp)

    freq = c_double(10)
    sdk.SetFrequency(freq)

    sdk.SetOutputFunc(OutputFunction_E.Output_Sine.value)
    print('Output 5mV 10Hz Sine without 620K Ohm impedance signal to measure Reference Vpp')
    time.sleep(5)
                                                            
    sdk.SetOutputFunc(OutputFunction_E.Output_Off.value)
    sdk.SetInputImpedanceMode(0)
    ImpedMode = sdk.GetInputImpedanceMode()
    if 0 == ImpedMode:
        print('Turn off Input Impedance Mode: with 620K Ohm impedance') 
    sdk.SetOutputFunc(OutputFunction_E.Output_Sine.value)
    print('Output 5mV 10Hz Sine with 620K Ohm impedance signal to measure Comparison Vpp')
    time.sleep(5)
    

def TestPulseWidth():
    print('TestPulseWidth -----------------------------------------------------')

    sdk.GetPulseWidth.restype = c_double
    pulseWidth = sdk.GetPulseWidth()
    print('Initial Pulse Widht: ', pulseWidth, ' ms')


    sdk.SetOutputFunc(OutputFunction_E.Output_RectanglePulse.value)
    print('Output ', OutputFunction_E(sdk.GetOutputFunc()), ' signal with ', pulseWidth, ' ms Pulse Width')
    time.sleep(5)

    sdk.SetOutputFunc(OutputFunction_E.Output_Off.value)
    sdk.SetOutputFunc(OutputFunction_E.Output_TrianglePulse.value)
    print('Output ', OutputFunction_E(sdk.GetOutputFunc()), ' signal with ', pulseWidth, ' ms Pulse Width')
    time.sleep(5)

    sdk.SetOutputFunc(OutputFunction_E.Output_Off.value)
    pulseWidth = c_double(200)
    if 0 == sdk.SetPulseWidth(pulseWidth):
         print('Set Pulse Width to ', sdk.GetPulseWidth())

    sdk.SetOutputFunc(OutputFunction_E.Output_RectanglePulse.value)
    print('Output ', OutputFunction_E(sdk.GetOutputFunc()), ' signal with ', sdk.GetPulseWidth(), ' ms Pulse Width')
    time.sleep(5)

    
sdk = cdll.LoadLibrary("../../SDK/libsecgx64.so")
if 0 == sdk.InitSECG():
    print('InstSECG() failed')
else:
    print('InitSECG() succeed')

    TestMisc()

    TestDCOffset()
    sdk.ResetSECG()

    TestImpedance()
    sdk.ResetSECG()

    TestPulseWidth()
