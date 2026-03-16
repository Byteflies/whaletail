def TestProcedure():
        for lead in range(0,2):
            if 0 != sdk.SetOutputLead(lead, 1):
                print('SetOutputLead(lead,1) failed')
                return -1
            print('lead is',lead)

            if 0 != sdk.SetOutputFunc(1):
                print('SetOutputFunc(1) failed')
                return -1
            print('Output Sine')
            time.sleep(5)

            pacingAmp=c_double(100)
            if 0 != sdk.SetPacingAmplitude(pacingAmp):
                print('SetPacingAmplitude(100.0) failed')
                return -1
            sdk.GetPacingAmplitude.restype = c_double
            print('Pacing Amp',sdk.GetPacingAmplitude())
            time.sleep(10)

            pacingAmp=c_double(-100)
            if 0 != sdk.SetPacingAmplitude(pacingAmp):
                print('SetPacingAmplitude(-100.0) failed')
                return -1
            print('Pacing Amp',sdk.GetPacingAmplitude())
            time.sleep(10)

            pacingAmp=c_double(0)
            if 0 != sdk.SetPacingAmplitude(pacingAmp):
                print('SetPacingAmplitude(0) failed')
                return -1

            if 0 != sdk.SetOutputFunc(0):
                print('SetOutputFunc(0) failed')
                return -1

            if 0 != sdk.SetOutputLead(lead, 0):
                print('SetOutputLead(lead,0) failed')
                return -1

            time.sleep(3)
    
from ctypes import *
import time
sdk = cdll.LoadLibrary("../../SDK/libsecgx64.so")
if 0 == sdk.InitSECG():
    print('InstSECG() failed')
else:
    print('InitSECG() succeed')
    TestProcedure()
