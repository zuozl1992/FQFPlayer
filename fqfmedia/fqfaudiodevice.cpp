#include "fqfaudiodevice.h"
#include "qtfqfaudiodevice.h"

FQFAudioDevice::FQFAudioDevice(){}

FQFAudioDevice::~FQFAudioDevice(){}

FQFAudioDevice * FQFAudioDevice::getObject()
{
    static QtFQFAudioDevice play;
    return &play;
}
