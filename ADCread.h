// This class module reads power values from log power chip
//
#define FWD_PWR1 14 // A0
#define FWD_PWR2 15 // A1
#define REF_PWR1 16 // A2
#define REF_PWR2 17 // A3
#define VSUP     18 // A4

#define PMAX 12
#define PMAXFLOOR -40

#define NCAL 11
const char cal[NCAL][11] = {"1.8MHz","3.5MHz","7MHz","10MHz","14MHz","21MHz","28MHz","50MHz","70MHz","144MHz","432MHz"};
// dB coupling corrections 
float Offset[NCAL] =  { 33.5,39,45.2,47.8,51.1,54.3,57.1,61.8,64.5,69.6,69.0};
class ADCread{
private:
  struct AD {
    float Reading;
    float Volts;
  };
  byte CalIndex;
  boolean RefMode=false;
  float RefDivider = 1023 *3.3/(15+3.3); 
  float Vsup=5;
  float Vf=0,Vr=0,fRatio=2,rRatio=2;// Gain of buffer amplifier 
#define Vref 1.1
  float AvFilter(float OldReading,float NewReading,float nReadings){
    return((nReadings*OldReading+NewReading)/(nReadings+1));
  }
  AD getNreadingsADC(int n,int chan){
    int i,r=0;
    float fr;
    AD X;
    analogRead(chan);// Flush and forget
    for(i=0;i<n;i++)r+=analogRead(chan);
    X.Reading=(float)r/(float)n;// Value = 0 to 1023
    X.Volts=X.Reading*(RefMode?Vref:Vsup)/(chan==VSUP?RefDivider:1023);
    return(X);
  }
  float V2dBm(float v){
    float d;
    //d=v*40.0 - 90.0;//   0dB coupling
    //d=v*40.0 - 65.0;// -25dB coupling
      d=v*40.0 - Offset[CalIndex];
    if(d>pMax[pMaxPtr])pMax[pMaxPtr]=d;
    return(d);
  }
  float pMax[PMAX];
  int pMaxPtr=0;
  float pMaxPrevious;
  void ClearPMax(){
    int i;
    for (i=0;i<PMAX;i++) pMax[i]=PMAXFLOOR;
    pMaxPrevious=PMAXFLOOR;
    pMaxPtr=0;
  }
  void SetRefInternal(boolean x){
#define DELAY_REF 10
    analogReference(x?INTERNAL:DEFAULT);
    RefMode=x;
    delay(DELAY_REF);
    analogRead(VSUP);
    delay(DELAY_REF);
  }
  void GetInitCal(){
    int i;
    SaveCal(CalIndex=EEPROM.read(0));
    //  Preload EEPROM values
    //  for(i=0;i<NCAL;i++) EEPROM.put(4+(i*sizeof(float)),Offset[i]);
    for(i=0;i<NCAL;i++) {
      EEPROM.get(4+(i*sizeof(float)),Offset[i]);
      if(Offset[i]>100)Offset[i]=100;
      if(Offset[i]<0)Offset[i]=0;
    }
  }
public:
  void SaveCal(byte c){
    byte NewCalIndex;
    NewCalIndex=((c&0x80)?0:((c>=NCAL)?NCAL-1:c));// constrain to range
    if(NewCalIndex!=CalIndex)EEPROM.update(0,CalIndex=NewCalIndex);
  }
  byte GetCalIndex(){
    return(CalIndex);
  }
  float GetPMax(){
    return((pMax[pMaxPtr]>pMaxPrevious)?pMax[pMaxPtr]:pMaxPrevious);
  }
  void TogglePMax(){// Rollover Pmax for next time interval
    int i;
    pMax[pMaxPtr=(++pMaxPtr%PMAX)]=PMAXFLOOR;
    pMaxPrevious=PMAXFLOOR;
    for (i=0;i<PMAX;i++)if(pMax[i]>pMaxPrevious)pMaxPrevious=pMax[i];
  }
  void GetADCratio(){ // Calculate ratio between internal reference and Vsup
    AD X;
    int n=16;
    SetRefInternal(false);
    X=getNreadingsADC(n,VSUP);
    RefDivider=AvFilter(RefDivider,X.Reading,8);
    SetRefInternal(true);
    X=getNreadingsADC(n,VSUP);
    Vsup=X.Volts;
  }
  float getVsupplyUSB(){
    return(Vsup);
  }
#define THRESHOLD_ADC1 600
#define THRESHOLD_ADC2 1000
  void ReadVoltages(){
    int n=16;
    AD XF1,XF2;
    AD XR1,XR2;
    boolean FwdOver1=false,RefOver1=false;
    boolean FwdOver2=false,RefOver2=false;
    SetRefInternal(true);
    
    XF2 = getNreadingsADC(n,FWD_PWR2);
    FwdOver2 = XF2.Reading > THRESHOLD_ADC2 ;
    if(!FwdOver2 ) {
      XF1=getNreadingsADC(n,FWD_PWR1);
      FwdOver1 = XF1.Reading>THRESHOLD_ADC1;
      if((!FwdOver1) & (XF1.Reading > ( THRESHOLD_ADC1 / 3 ) ) )
        fRatio=AvFilter(fRatio,XF1.Reading/XF2.Reading,8);
      Vf=FwdOver1?XF2.Volts:XF1.Volts/fRatio;
    }
  
    XR2 = getNreadingsADC(n,REF_PWR2);
    RefOver2 = XR2.Reading > THRESHOLD_ADC2 ; 
    if(!RefOver2) {
      XR1=getNreadingsADC(n,REF_PWR1);
      RefOver1 = XR1.Reading>THRESHOLD_ADC1;
      if((!RefOver1)&(XR1.Reading > ( THRESHOLD_ADC1 / 3 ) ) )
        rRatio=AvFilter(rRatio,XR1.Reading/XR2.Reading,8);
      Vr=RefOver1?XR2.Volts:XR1.Volts/rRatio;
    }
    
    if(FwdOver2|RefOver2){
      SetRefInternal(false);
      if(FwdOver2){
        XF1=getNreadingsADC(n,FWD_PWR1);
        XF2=getNreadingsADC(n,FWD_PWR2);
        FwdOver1 = XF1.Reading>THRESHOLD_ADC1;
        if((!FwdOver1)&(XF1.Reading > ( THRESHOLD_ADC1 / 3 ) ) )
          fRatio=AvFilter(fRatio,XF1.Reading/XF2.Reading,8);
        Vf=FwdOver1?XF2.Volts:XF1.Volts/fRatio;
      }
      if(RefOver2){
        XR1=getNreadingsADC(n,REF_PWR1);
        XR2=getNreadingsADC(n,REF_PWR2);
        RefOver1 = XR1.Reading>THRESHOLD_ADC1;
        if((!RefOver1)& ( XR1.Reading > ( THRESHOLD_ADC1 / 3 ) ) )
          rRatio=AvFilter(rRatio,XR1.Reading/XR2.Reading,8);
        Vr=RefOver1?XR2.Volts:XR1.Volts/rRatio;
      }
    }
  }
  float GetFwdVolts(){
    return(Vf);
  }
  float GetRefVolts(){
    return(Vr);
  }
  float GetFwddBm(){
    return(V2dBm(Vf));
  }
  float GetRefdBm(){
    return(V2dBm(Vr));
  }
  float GetSWR(){
    float pp,s;
    pp=2*(Vf-Vr);
    if (pp>0)pp=-pp;
    s=pow(10.0,pp);
    if(s<0.99){
      return ((1+s)/(1-s));
    } else return(200);
  }
  ADCread(){
    ClearPMax();
    GetInitCal();
  }
  ~ADCread(){}
};
