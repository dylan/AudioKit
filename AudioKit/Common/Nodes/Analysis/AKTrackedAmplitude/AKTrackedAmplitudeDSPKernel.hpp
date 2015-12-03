//
//  AKTrackedAmplitudeDSPKernel.hpp
//  AudioKit
//
//  Autogenerated by scripts by Aurelius Prochazka. Do not edit directly.
//  Copyright (c) 2015 Aurelius Prochazka. All rights reserved.
//

#ifndef AKTrackedAmplitudeDSPKernel_hpp
#define AKTrackedAmplitudeDSPKernel_hpp

#import "AKDSPKernel.hpp"
#import "AKParameterRamper.hpp"

extern "C" {
#include "soundpipe.h"
}

enum {
    halfPowerPointAddress = 0
};

class AKTrackedAmplitudeDSPKernel : public AKDSPKernel {
public:
    // MARK: Member Functions

    AKTrackedAmplitudeDSPKernel() {}

    void init(int channelCount, double inSampleRate) {
        channels = channelCount;

        sampleRate = float(inSampleRate);

        sp_create(&sp);
        sp_rms_create(&rms);
        sp_rms_init(sp, rms);
        rms->ihp = 10;
    }

    void destroy() {
        sp_rms_destroy(&rms);
        sp_destroy(&sp);
    }
    
    void reset() {
    }

    void setParameter(AUParameterAddress address, AUValue value) {
        switch (address) {
            case halfPowerPointAddress:
                halfPowerPointRamper.set(clamp(value, (float)0, (float)20000));
                break;

        }
    }

    AUValue getParameter(AUParameterAddress address) {
        switch (address) {
            case halfPowerPointAddress:
                return halfPowerPointRamper.goal();

            default: return 0.0f;
        }
    }

    void startRamp(AUParameterAddress address, AUValue value, AUAudioFrameCount duration) override {
        switch (address) {
            case halfPowerPointAddress:
                halfPowerPointRamper.startRamp(clamp(value, (float)0, (float)20000), duration);
                break;

        }
    }

    void setBuffers(AudioBufferList *inBufferList, AudioBufferList *outBufferList) {
        inBufferListPtr = inBufferList;
        outBufferListPtr = outBufferList;
    }

    void process(AUAudioFrameCount frameCount, AUAudioFrameCount bufferOffset) override {
        // For each sample.
        for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
            double halfPowerPoint = double(halfPowerPointRamper.getStep());

            int frameOffset = int(frameIndex + bufferOffset);

            rms->ihp = (float)halfPowerPoint;

            for (int channel = 0; channel < channels; ++channel) {
                float *in  = (float *)inBufferListPtr->mBuffers[channel].mData  + frameOffset;
                float *out = (float *)outBufferListPtr->mBuffers[channel].mData + frameOffset;

                sp_rms_compute(sp, rms, in, out);
            }
        }
    }

    // MARK: Member Variables

private:

    int channels = 2;
    float sampleRate = 44100.0;

    AudioBufferList *inBufferListPtr = nullptr;
    AudioBufferList *outBufferListPtr = nullptr;

    sp_data *sp;
    sp_rms *rms;

public:
    AKParameterRamper halfPowerPointRamper = 10;
};

#endif /* AKTrackedAmplitudeDSPKernel_hpp */