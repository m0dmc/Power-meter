// This class module displays an analog meter image on TFT display
//
class MeterTFT{
private:
  int xBase=0,yBase=0,xSize=100,ySize=100;
  int colBorder=COLOR_BLUEVIOLET,colScreen=COLOR_SKYBLUE;
  int colNeedle=COLOR_RED,colText=COLOR_WHITE,colFrame=COLOR_WHITE;
  int xNeedlePivot=0,yNeedlePivot=0,xNeedleTop=0,yNeedleTop=0;
  float xNeedleScale;
  void DrawNeedle(int Point){
    int xNew=xBase+xNeedleScale+Point*xNeedleScale/10;
    if(xNew!=xNeedleTop){
      //   tft.drawLine(xNeedlePivot,yNeedlePivot,xNeedleTop,yNeedleTop,colScreen);
      //   tft.drawLine(xNeedlePivot,yNeedlePivot,xNeedleTop=xNew,yNeedleTop,colNeedle);
      tft.fillTriangle(xNeedlePivot+2,yNeedlePivot,
		       xNeedlePivot-2,yNeedlePivot,
		       xNeedleTop,yNeedleTop,colScreen);
      tft.fillTriangle(xNeedlePivot+2,yNeedlePivot,
		       xNeedlePivot-2,yNeedlePivot,
		       xNeedleTop=xNew,yNeedleTop,colNeedle);
    }
  }
public:
  void NewText(char *t){
    tft.setFont(Terminal6x8);
    tft.setBackgroundColor(colBorder);
    tft.fillRectangle(xBase+4,yBase+ySize-9,xBase+xSize-4,yBase+ySize-2,colBorder);
    tft.drawText(xBase+4,yBase+ySize-9,t,colText);
  }
  void Setup(int xb,int yb,int xs,int ys,char * t,int dir){
    xBase=xb;
    yBase=yb;
    xSize=xs;
    ySize=ys;
    xNeedlePivot=xBase+xSize/2;
    xNeedleScale=xs/12.0;
    xNeedleTop=xNeedlePivot;// temporary init value
    yNeedlePivot=yBase+ySize-14;
    yNeedleTop=yBase+(ySize-10)/10.0;
    tft.fillRectangle(xBase+1,yBase+1,xBase+xSize-1,yBase+ySize-1,colBorder);
    tft.drawRectangle(xBase,yBase,xBase+xSize,yBase+ySize,colFrame);// Outer Frame
    tft.fillRectangle(xBase+3,yBase+3,xBase+xSize-3,yBase+ySize-12,colScreen);
    tft.drawRectangle(xBase+2,yBase+2,xBase+xSize-2,yBase+ySize-11,colFrame);
    tft.fillCircle(xNeedlePivot,yNeedlePivot+2,2,colFrame);
    switch(dir){// Direction Arrow
    case 1:
      tft.fillTriangle(xb+5,yb+45,xb+5,yb+55,xb+35,yb+50,COLOR_BLUEVIOLET);
      break;
    case 2:
      tft.fillTriangle(xb+xs-5,yb+45,xb+xs-5,yb+55,xb+xs-35,yb+50,COLOR_BLUEVIOLET);
      break;
    default:
      break;
    }
    NewText(t);
    DrawNeedle(0);
  }
  void Setup(int xb,int yb,int xs,int ys,char * t){
    Setup(xb,yb,xs,ys,t,0);
  }
  void Setup(int xb,int yb,int xs,int ys){
    Setup(xb,yb,xs,ys,"",0);
  }
  void Clear(int xb,int yb,int xs,int ys){
    tft.fillRectangle(xb,yb,xb+xs,yb+ys,COLOR_BLACK);
  }
  void Clear(){
    Clear(xBase,yBase,xSize,ySize);
  }
  void NewReading(int p){
    DrawNeedle(p<-2?-2:(p>102?102:p));
  }
  MeterTFT(){}
  ~MeterTFT(){}
};
