// This class module displays thermometer image on TFT display
//

class ShowReadingTFT{ // show text readings on screen
private:
  int len;
  int xs=0,xn=0,xd=0,y=0;
  char UnitChar=' ';
  int UnitMult=0;
  char Units[4]="";
  boolean UnitsChanged=false;
  void ShowValues(char*s,int c){
    tft.setBackgroundColor(COLOR_BLACK);
    tft.setFont(Terminal12x16);
    tft.fillRectangle(xn,y,xd,y+16,COLOR_BLACK);
    tft.drawText(xn,y,s,c);
    if(UnitsChanged==true){
      UnitsChanged=false;
      tft.fillRectangle(xd+1,y,xd+36,y+16,COLOR_BLACK);
      tft.drawText(xd+1,y,Units,c);
    }
  }
  void SetUnits(char u,int mult){
    char OldUnits[4]="";
    strcpy(OldUnits,Units);
    UnitMult=mult;
    switch(UnitChar=u){
    case 'C':
      strcpy(Units,"C");
      break;
    case 'd':
      strcpy(Units,"dBm");
      break;
    case 'V':
      if (mult==-12)strcpy(Units,"pV");
      if (mult==-9)strcpy(Units,"nV");
      if (mult==-6)strcpy(Units,"uV");
      if (mult==-3)strcpy(Units,"mV");
      if (mult==0)strcpy(Units,"V");
      break;
    case 'W':
      if (mult==-12)strcpy(Units,"pW");
      if (mult==-9)strcpy(Units,"nW");
      if (mult==-6)strcpy(Units,"uW");
      if (mult==-3)strcpy(Units,"mW");
      if (mult==0)strcpy(Units,"W");
      break;
    default:
      strcpy(Units,"");
      break;
    }
    UnitsChanged= (0==strcmp(Units,OldUnits))?false:true;
  }
public:
  void SetLocation(int x1,int y1){
    xs=x1;
    y=y1;
  }
  void SetUnits(char u){
    SetUnits(u,0);
  }
  void SetMult(int m){
    SetUnits(UnitChar,m);
  }
  void ShowLabel(char * t,int col){
    tft.setBackgroundColor(COLOR_BLACK);
    tft.setFont(Terminal12x16);
    tft.drawText(xs,y, t,col);
    xn=xs+12*strlen(t);
    xd=xn+12*7-1;
  }
  void ShowLabel(char * t){
    ShowLabel(t,COLOR_WHITE);
  }
  // set mult then show value
  void ShowValue(int n,int col){
    char nstr[10];
    dtostrf(n,6,0,nstr);
    ShowValues(nstr,col);
  }
  void ShowValue(int n){
    ShowValue(n,COLOR_WHITE);
  }
  void ShowValue(float n,int col){
    char nstr[10];
    dtostrf(n,6,1,nstr);
    ShowValues(nstr,col);  
  }
  void ShowValue(float n){
    ShowValue(n,COLOR_WHITE);
  }
  void ShowValue(double n,int col){
    char nstr[10];
    dtostrf(n,6,2,nstr);
    ShowValues(nstr,col);   
  }
  void ShowValue(double n){
    ShowValue(n,COLOR_WHITE);
  }
  ShowReadingTFT(){}
  ~ShowReadingTFT(){}
};
