// This set of algorithms is not 100% realiable, some assumptions have been made, especially when
// generating the random binary message and carrier wave.

#include <iostream>
#include <math.h>
#include <vector>
#include <fstream>
#include <algorithm>


// Carrier frequency in Hertz - Fc or F
#define carrierFrequency 100.f // 100.f to TDM

// 10000 Hz = 10000 samples per second
#define samplingFrequency 10000.f // 10000.f to TDM

// Bits per second
#define samplingFrequencyMessage 10.f

// Signal sampling duration in seconds
#define samplingDuration 1.f // 0.02f to TDM


// Using carrierFrequency and samplingDuration defines, it returns a time vector where Ts = 1/carrierFrequency (interval between samples)
std::vector <float> timeSampling()
{
    std::vector< float> timeVector;

    float sampleStepSize = 1.f / samplingFrequency;

    float sampleInterval = 0.f;

    while(sampleInterval <= samplingDuration)
    {
        timeVector.push_back(sampleInterval);

        sampleInterval = sampleInterval + sampleStepSize;
    }

    return timeVector;


}

// Generate a binary message signal of samplingFrequencyMessage bits per second
std::vector< unsigned short int> randomBinaryMessageGenerator()
{

    std::vector<unsigned short int> binaryMessage;

    // Se 10 Hz = 10 bits por segundo
    float step = 1.f / samplingFrequencyMessage;
    float sample = 0.0f;

    srand(time(0));

    while(sample < samplingDuration)
    {
        binaryMessage.push_back(rand() % 2);

        sample = sample + step;
    }

    binaryMessage.pop_back();
    return binaryMessage;

}

// Generate a unrealiable carrier wave at the frequency defined by carrierFrequency, timeVector and voltage passed by reference
std::vector<float> carrierSignalGenerator(float amplitudeVoltage, std::vector <float> timeVector, float delta = 0)
{
    // C(t) = Ac cos (2*pi*fc*t), Ac=3, fc=100 Hz, t=time.


    std::vector< float> carrierWave;

    for(int timeStep = 0; timeStep <= timeVector.size(); timeStep++)
    {
        carrierWave.push_back(amplitudeVoltage * std::sin(2.f * 3.1415f * carrierFrequency * timeVector[timeStep] + delta));
    }

    return carrierWave;


//    for(int k = 0; k <= samplesTime.size(); k++)
//    {
//        carrierWave.push_back(std::make_tuple(samplesTime[k], (amplitudeVoltage * std::sin(2.f * 3.14f * carrierFrequency * samplesTime[k]))));
//    }
//
//    return carrierWave;

}

// Applies the Amplitude Shift Keying technique, using the previous generated binary message and carrier wave
std::vector<float> amplitudeShiftKeying(std::vector< unsigned short int > binaryMessage,
                                        std::vector< float> carrierWave)
{
    std::vector< float> ASK_ModulatedWave;

    int sizeOfBlock = (int)round(carrierWave.size() / binaryMessage.size());

    int numberOfBlocks = (int)round(carrierWave.size() / sizeOfBlock);




    int holder = 0;
    for(int block = 0; block <= numberOfBlocks; block++)
    {

        for(int k = holder; k < (holder + sizeOfBlock); k++ )
        {
            ASK_ModulatedWave.push_back(binaryMessage[block] * carrierWave[k]);
        }

        holder = holder + sizeOfBlock;


    }

    return ASK_ModulatedWave;
}


// Compute the Time Division Multiplexation with only 3 analog input signals
std::vector <float> timeDivisionMultiplexor()
{

    std::vector <float> common_TimeVector = timeSampling();

    std::vector <float> linear_TDM_DataStreamVector;

    std::vector <float> first_InputSignal   = carrierSignalGenerator(5.f, common_TimeVector, 3.f * 3.1415f);
    std::vector <float> second_InputSignal  = carrierSignalGenerator(3.f, common_TimeVector);
    std::vector <float> third_InputSignal   = carrierSignalGenerator(1.f, common_TimeVector, 3.1415f);


    for(int timeSlot = 0; timeSlot < second_InputSignal.size(); timeSlot++)
    {
        //linear_TDM_DataStreamVectorAxis.push_back(timeSlot);
        linear_TDM_DataStreamVector.push_back(first_InputSignal[timeSlot]);
        linear_TDM_DataStreamVector.push_back(second_InputSignal[timeSlot]);
        linear_TDM_DataStreamVector.push_back(third_InputSignal[timeSlot]);
        //linear_TDM_DataStreamVector.push_back(0.f);
    }

    return linear_TDM_DataStreamVector;

}


// Export to .txt function
void export2File(std::vector <float> input)
{
    std::ofstream outFile("Modulation_Output.txt");
    // the important part
    for (const auto &e : input) outFile << e << "\n";
}

int main()
{

    std::vector <float> timeVector                    = timeSampling();
    std::vector <float> carrierWave                   = carrierSignalGenerator(3.f, timeVector);
    std::vector <unsigned short int> binaryMessage    = randomBinaryMessageGenerator();
    std::vector <float> ASK_ModulatedWave             = amplitudeShiftKeying(binaryMessage, carrierWave);

    //std::vector <float> TDM_Result = timeDivisionMultiplexor();
    
    export2File(ASK_ModulatedWave);

    return 0;
}
