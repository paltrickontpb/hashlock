#ifndef Proximus_h
#define Proximus_h

#include "Arduino.h"

class Proximus
{
  public:
    Proximus(int cipherType);
    void begin();
    void setPin(int key);
    void setCipher(int cipherType);
    void encrypt(String ctext, int key);
    void decrypt(String ctext, int key);
  private:
    int _key;
    int _cipherType;
    String _ctext;
};

#endif