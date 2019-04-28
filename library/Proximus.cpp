#include "Arduino.h"
#include "Proximus.h"

Proximus::Proximus(int cipherType)
{
    _cipherType = cipherType;
}

Proximus::begin(){
    setCipher();
}