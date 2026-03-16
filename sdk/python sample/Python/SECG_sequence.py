from ctypes import *
from enum import Enum, auto
import time

class OutputLead_E (Enum):
    Lead_RA = 0
    Lead_LA = auto ()
    Lead_LL = auto ()
    Lead_V1 = auto ()
    Lead_V2 = auto ()
    Lead_V3 = auto ()
    Lead_V4 = auto ()
    Lead_V5 = auto ()
    Lead_V6 = auto ()

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


def printLeads():
    if 0 != sdk.GetOutputLead(OutputLead_E.Lead_RA.value):
        print(OutputLead_E.Lead_RA.name,'  ON')
    else:
        print(OutputLead_E.Lead_RA.name,'  OFF')

    if 0 != sdk.GetOutputLead(OutputLead_E.Lead_LA.value):
        print(OutputLead_E.Lead_LA.name,'  ON')
    else:
        print(OutputLead_E.Lead_LA.name,'  OFF')

    if 0 != sdk.GetOutputLead(OutputLead_E.Lead_LL.value):
        print(OutputLead_E.Lead_LL.name,'  ON')
    else:
        print(OutputLead_E.Lead_LL.name,'  OFF')


def testprocedure():
    for lead in range(0, 3):   #Loop RA/LA/LL
        print('### Test Iteration: ', lead, '###')

        #Reset All Setting to Default
        sdk.ResetSECG()


        #Set Output Lead
        if 0 != sdk.SetOutputLead(lead, 1):
            print('SetOutputLead(', lead, '1) failed')
            return -1
        printLeads()

        #Set Output Function
        outFunc = lead + 1
        if 0 != sdk.SetOutputFunc(outFunc):
            print('SetOutputFunc(', outFunc, ' 1) failed')
            return -1
        print('Output Function: ', OutputFunction_E(outFunc).name)

        #Set Amplitude : 0.5 / 1.0 / 1.5 mV
        amp = c_double(outFunc * 0.5)
        if 0 != sdk.SetAmplitude(amp):
            print('SetAmplitude(', amp, ') failed')
            return -1
        sdk.GetAmplitude.restype = c_double
        print('Amplitude is: ', sdk.GetAmplitude(), ' mV')

        #Set Frequency : 1.0 / 2.0 / 3.0 Hz
        freq = c_double(outFunc)
        if 0 != sdk.SetFrequency(freq):
            print('SetFrequency(', freq, ') failed')
            return -1
        sdk.GetFrequency.restype = c_double
        print('Frequency is: ', sdk.GetFrequency(), ' Hz')

        print('---------------------- Output Signal for 5 seconds ------------------------')
        time.sleep(5)


sdk = cdll.LoadLibrary("../../SDK/libsecgx64.so")
if 0 == sdk.InitSECG():
    print('InitSECG failed!')
else:
    print('--- InitSECG succeeded! ---')
    testprocedure()

