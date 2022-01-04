// This class module sends data to usb serial port for data logging
//

class TermData{ 
private:
  float dBm2W(float dbm){
    return(pow(10.0,(dbm-30)/10.0));
  }
  float dBm2mW(float dbm){
    return(pow(10.0,dbm/10.0));
  }
  float dBm2uW(float dbm){
    return(pow(10.0,(dbm+30)/10.0));
  }
  float dBm2nW(float dbm){
    return(pow(10.0,(dbm+60)/10.0));
  }
  float dBm2pW(float dbm){
    return(pow(10.0,(dbm+90)/10.0));
  }
  double ShowPwr(boolean Fwd,float v,float dbm,float p){
    double w;
    int m=0;
    Serial.print(Fwd?"Vfwd = ":"Vref = ");
    Serial.print(v);
    Serial.print(" dBm= ");
    Serial.print(dbm);
    if(dbm<-60){
      w=dBm2pW(dbm);
      Serial.print(" pW= ");
      m=-12;
    } else if (dbm<-30){
      w=dBm2nW(dbm);
      Serial.print(" nW= ");
      m=-9;
    } else if (dbm<0){
      w=dBm2uW(dbm);
      Serial.print(" uW= ");
      m=-6;
    } else if (dbm<30){
      w=dBm2mW(dbm);
      Serial.print(" mW= ");
      m=-3;
    } else {
      w=dBm2W(dbm);
      Serial.print("  W= ");
      m=0;
    }
    Serial.print(w);
    /////////////////////
    // This shouldn't be here !!!!!!!!!!!!!
    if(Fwd){
      PowF.SetMult(m);
      PowF.ShowValue(w);
    } else {
      PowR.SetMult(m);
      PowR.ShowValue(w);
    }
    w=pow(10.0,(20+dbm-p)/10.0);//  range 0-100% for meter display
    Serial.print(" range= ");
    Serial.print(pow(10.0,p/10.0));
    Serial.print("mW meter= ");
    Serial.println(w);
    return(w);
  }
public:
  void Setup(unsigned long b){
    Serial.begin(b);
    if(!Serial)delay(1000); // wait a second if no terminal
  }
  void Welcome(char * fn){
    Serial.println(fn);
    Serial.println("Monitor Dallas Temperature IC, Voltage and RF Power.");
    Serial.println("Display on TFT screen and log to USB serial port.");
    Serial.println("mW power meter, max 10W");
    Serial.println("Type a|b to change frequency range.");
    Serial.println("Type c|d to adjust calibration.");
  }
  void ShowTempVusb(float t,float v){
    Serial.print(" T= ");
    Serial.print(t);
    Serial.print("'C  V= ");
    Serial.println(v);
  }
  double ShowFwdPwr(float v,float dbm,float p){// p= max range for meter
    return(ShowPwr(true,v,dbm,p)); // return meter display 0-100%
  }
  double ShowRefPwr(float v,float dbm,float p){
    return(ShowPwr(false,v,dbm,p));
  }
  TermData(){}
  ~TermData(){}
};
