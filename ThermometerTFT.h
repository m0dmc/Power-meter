// This class module displays thermometer image on TFT display
//

class ThermometerTFT{
private:
  int xBase=0,yBase=0,xSize=0,ySize=0;
  int xScale=0,yScale=0,xWidth=0,yHeight=0;
  int LastOffset=0,colourL=COLOR_RED,colourG=COLOR_WHITE;
  void DrawLiquid(int point){
    int Offset=yHeight*point/100;
    if(Offset!=LastOffset){
      tft.fillRectangle(xScale,yScale-LastOffset,
			xWidth,yScale-Offset,
			(Offset<LastOffset)?COLOR_BLACK:colourL);
      LastOffset=Offset;
    }
  }
public:
  void Setup(int xb,int yb,int xs,int ys){
    xBase=xb;
    yBase=yb;
    xSize=xs;
    ySize=ys;
    int x2=xSize/2;
    int x4=xSize/4;
    int x8=xSize/8;
    tft.fillCircle(xBase+x2,yBase+ySize-x2,x2,colourG);// bulb
    tft.fillCircle(xBase+x2,yBase+x4,x4,colourG);      // top
    tft.fillRectangle(xBase+x4,yBase+x4,xBase+3*x4,yBase+ySize-x2,colourG);
    tft.fillCircle(xBase+x2,yBase+ySize-x2,x4,colourL);// bulb liquid
    xScale=xBase+3*x8;
    xWidth=xBase+5*x8;
    yScale=yBase+ySize-3*x4;
    yHeight=ySize-xSize;
    LastOffset=yHeight;
    DrawLiquid(0);
  }
  void Clear(int xb,int yb,int xs,int ys){
    tft.fillRectangle(xb,yb,xb+xs,yb+ys,COLOR_BLACK);
  }
  void Clear(){
    Clear(xBase,yBase,xSize,ySize);
  }
  void NewReading(int p){
    DrawLiquid((p<0?0:(p>100?100:p)));
  }
  ThermometerTFT(){}
  ~ThermometerTFT(){}
};
