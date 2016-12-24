#include "Arduino.h"

template <typename T>
class MessageValidator
{
  const uint32_t timeout;
  uint32_t timeout_cntr;
  T replacementValue;  
  T currentValue;
  T* dst_ptr;
  public:
  MessageValidator(uint32_t _timeout, T _replacementValue, T* _dst_ptr): timeout(_timeout), currentValue(_replacementValue), replacementValue(_replacementValue), dst_ptr(_dst_ptr)
  {}
  void set(T value)
  {
      currentValue = value;
      timeout_cntr = millis();
  }
  void update()
  {
    if(millis()>(timeout_cntr+timeout))
    {
      currentValue = replacementValue;  
    }  
    if(dst_ptr != 0)
      *dst_ptr=currentValue;
  }
  T get()
  {
    update();
    return currentValue;
  }
};
