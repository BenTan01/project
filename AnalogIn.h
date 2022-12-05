
#ifndef ANALOGIN_H_
#define ANALOGIN_H_

class AnalogIn{
    private:
      unsigned int number;

    public:
      AnalogIn();
      AnalogIn(unsigned int n);
      virtual  unsigned int getNumber(){return number;}
      virtual void setNumber(unsigned int n);
      virtual int readAdcSample();
      virtual ~AnalogIn();
};

#endif /* ANALOGIN_H_ */
