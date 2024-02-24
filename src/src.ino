#include <PS2X_lib.h>
int IN1 = 32;           //同正
int IN2 = 34;           //同负
int pinNum;             //  控制引脚号
int ctrlVal;            //  电机运行控制
int angle;
boolean AIdrive;        //辅助驾驶
boolean shoot;          //发射测试
#include <Servo.h>  //舵机控制
Servo myservo;      //转向舵机
Servo ctservo;      //发射舵机
#include <PS2X_lib.h> //PS2X库函数
PS2X ps2x; //创建一个PS2X对象
#define PS2_DAT        5  //DI
#define PS2_CMD        6  //DO
#define PS2_SEL        7  //CS
#define PS2_CLK        8  //CLK
unsigned char PS2_LX,PS2_LY,PS2_RX,PS2_RY;  //左摇杆X轴坐标，左摇杆Y轴坐标，右摇杆X轴坐标，右摇杆Y轴坐标
float      distance;                        //超声测距模块的距离
const int  echo=22;                          //echO接D3脚
const int  trig=24;                          //echO接D2脚
int LEDpin = 23;                             //独立控制LED的nano板信号输出口
int source;                                  //定义LED输入信号
long previousTime=0;                        //控制当前时间

int music_00 = 48;                // 音乐口，低电平为启动
int music_01 = 49;
int music_02 = 50;
int music_03 = 51;
int music_04 = 52;


int lastLine = 0;
int line = 5;
unsigned int period = 0;
unsigned int lastTime = 0;





void setup()   
{//电机控制接口
  pinMode(IN1, OUTPUT);
  digitalWrite(IN1,0);
  pinMode(IN2, OUTPUT);
  digitalWrite(IN2,0);
  pinMode(echo,INPUT);                       //设置echo为输入脚
  pinMode(trig,OUTPUT);                      //设置trig为输出脚
  pinMode(LEDpin,OUTPUT);
  pinMode(music_00,OUTPUT);
  pinMode(music_01,OUTPUT);
  pinMode(music_02,OUTPUT);
  pinMode(music_03,OUTPUT);
  pinMode(music_04,OUTPUT);
  BGM(0);
  //手柄操控器
  char error;
  Serial.begin(9600);        
  Serial.println("PS2初始化..."); 
  error=ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);//初始化ps2，如果初始化成功返回byte值0
  if(!error)Serial.println("PS2初始化成功！");
  else Serial.println("PS2初始化失败！");

  AIdrive = false;            //初始化辅助驾驶
  shoot = false;              //初始化射击判断代码
  source = LOW;
  digitalWrite(LEDpin,source);
  
  myservo.attach(9);
  myservo.write(90);
  ctservo.attach(10);
  ctservo.write(0);
}


void loop()
{
    ps2x.read_gamepad(false, 0); //read controller and set large motor to spin at 'vibrate' speed
    PS2_LX=ps2x.Analog(PSS_LX);  //读取拍摄ps2左摇杆X坐标并赋值给PS2_LX
    PS2_LY=ps2x.Analog(PSS_LY);  //读取拍摄ps2左摇杆Y坐标并赋值给PS2_LY
    PS2_RX=ps2x.Analog(PSS_RX);  //读取拍摄ps2右摇杆X坐标并赋值给PS2_RX
    PS2_RY=ps2x.Analog(PSS_RY);  //读取拍摄ps2右摇杆Y坐标并赋值给PS2_RY

//    串口发送左右摇杆坐标状态
    Serial.print("PS2_LX:");
    Serial.print(PS2_LX);
    Serial.print("   PS2_LY:");
    Serial.print(PS2_LY);
    Serial.print("   PS2_RX:");
    Serial.print(PS2_RX);
    Serial.print("   PS2_RY:");
    Serial.print(PS2_RY);

    Serial.print("   Key State:");
    if(ps2x.Button(PSB_TRIANGLE))          Serial.println("PSB_TRIANGLE being pressed");
    else if(ps2x.Button(PSB_CROSS))        Serial.println("PSB_CROSS being pressed");
    else if(ps2x.Button(PSB_CIRCLE))       Serial.println("PSB_CIRC being pressed");
    else if(ps2x.Button(PSB_SQUARE))       Serial.println("PSB_SQUARE being pressed");
    else if(ps2x.Button(PSB_PAD_UP))       Serial.println("PSB_PAD_UP being pressed");
    else if(ps2x.Button(PSB_PAD_DOWN))     Serial.println("PSB_PAD_DOWN being pressed");
    else if(ps2x.Button(PSB_PAD_RIGHT))    Serial.println("PSB_PAD_RIGHT being pressed");
    else if(ps2x.Button(PSB_PAD_LEFT))     Serial.println("PSB_PAD_LEFT being pressed");
    else if(ps2x.Button(PSB_R1))           Serial.println("PSB_R1 being pressed");
    else if(ps2x.Button(PSB_R2))           Serial.println("PSB_R2");
    else if(ps2x.Button(PSB_SELECT))       Serial.println("PSB_SELECT being pressed");
    else if(ps2x.Button(PSB_START))        Serial.println("PSB_START being pressed");
    else if(ps2x.Button(PSB_L1))           Serial.println("PSB_L1 being pressed");
    else if (ps2x.Button(PSB_L2))          Serial.println("PSB_L2 being pressed");
    else Serial.println(" All empty");
    //ps2x.ButtonPressed()是ps2按键被按下的触发函数，返回boolean值
    if(ps2x.ButtonPressed(PSB_CIRCLE)) Serial.println("PSB_CIRCLE have been pressed");

    if(ps2x.ButtonPressed(PSB_TRIANGLE)){
      AIdrive = !AIdrive;     //辅助驾驶切换
      if(source == HIGH){source = LOW;} //给LED引脚一个低电平
      else {source = HIGH;}
      digitalWrite(LEDpin,source);
     }
     if(ps2x.ButtonPressed(PSB_SQUARE)){
      shoot = !shoot;
     }
     if(ps2x.ButtonPressed(PSB_CROSS)){
      line = music_04;            //放音乐
     }
     if(!AIdrive){
     turn();
     }
     else if(AIdrive){
      AIturn();
     }
     //执行方法
     read();
     shootMethod();
     setBGM();
}




void read(){     //运行电机
if(PS2_LY <= 126){//左手柄调控向前速度
            //0.016最大值254
  if(velocity != 158)
  {
  analogWrite(IN2, velocity);
  digitalWrite(IN1, 0);
  }
    else{
    analogWrite(IN2, velocity);
    digitalWrite(IN1, 0);
    line = music_00;      //最大速度音乐
  }
  Serial.print("toward velocity:");
  Serial.println(velocity);
  }
if(PS2_LY >= 130){//左手柄调控向后速度
  int velocity =int(float(0.01) * pow((PS2_LY - 130),2));
  line = music_03;
  analogWrite(IN1, velocity);
  digitalWrite(IN2, 0);
  Serial.print("backward velocity");
  Serial.println(velocity);
}
if(PS2_LY == 128 || PS2_LY == 127 || PS2_LY == 129){
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 0);
}
}

void turn() {
  Serial.println("turn模式");
  if(0 <= PS2_RX <= 255)
  angle = 50 + (PS2_RX * 0.352);
  myservo.write(angle);
  if(angle > 130){ BGM(line = music_01); }
  else if(angle < 50){ line = music_01; }
}

void AIturn(){
  unsigned long currentTime = micros();
  if(currentTime - previousTime > 20000){
 digitalWrite(trig,LOW);
 delayMicroseconds(20);
 digitalWrite(trig,HIGH);
 delayMicroseconds(20);
 digitalWrite(trig,LOW);                     //发一个20US的高脉冲去触发Trig
 distance  = pulseIn(echo,HIGH);            //计数接收高电平时间
 distance  = distance*0.017;            //计算距离 1：声速：340M/S  2：实际距离1/2声速距离 3：计数时钟为1US
 Serial.print("AIdistance:");
 Serial.println(distance);
 if(PS2_LY <= 126){
 if(20 > distance > 11){
  myservo.write(65);
 }
 else if(distance < 4 && distance > 20){
  myservo.write(115);
 }
 else if(4 <= distance <= 11){
  myservo.write(90);
 }
  }
 else if(PS2_LY >= 130)  {
   if(20 > distance > 11){
  myservo.write(120);
 }
 else if(distance < 4 && distance > 20){
  myservo.write(60);
 }
 else if(4 <= distance <= 11){
  myservo.write(90);
     }
   }
   previousTime = currentTime;
 }
}

void BGM(int pin){
  switch(pin){
    case 48 :
    digitalWrite(music_00,LOW);
    digitalWrite(music_01,HIGH);
    digitalWrite(music_02,HIGH);
    digitalWrite(music_03,HIGH);
    digitalWrite(music_04,HIGH);
    Serial.println("BGM1***");
    break;
    
    case 49 :
    digitalWrite(music_00,HIGH);
    digitalWrite(music_01,LOW);
    digitalWrite(music_02,HIGH);
    digitalWrite(music_03,HIGH);
    digitalWrite(music_04,HIGH);
        Serial.println("BGM2***");
    break;

    case 50 :
    digitalWrite(music_00,HIGH);
    digitalWrite(music_01,HIGH);
    digitalWrite(music_02,LOW);
    digitalWrite(music_03,HIGH);
    digitalWrite(music_04,HIGH);
        Serial.println("BGM3***");
    break;

    case 51 :
    digitalWrite(music_00,HIGH);
    digitalWrite(music_01,HIGH);
    digitalWrite(music_02,HIGH);
    digitalWrite(music_03,LOW);
    digitalWrite(music_04,HIGH);
        Serial.println("BGM4***");
    break;

    case 52 :
    digitalWrite(music_00,HIGH);
    digitalWrite(music_01,HIGH);
    digitalWrite(music_02,HIGH);
    digitalWrite(music_03,HIGH);
    digitalWrite(music_04,LOW);
        Serial.println("BGM5***");
    break;

    case 0 :
    digitalWrite(music_00,HIGH);
    digitalWrite(music_01,HIGH);
    digitalWrite(music_02,HIGH);
    digitalWrite(music_03,HIGH);
    digitalWrite(music_04,HIGH);
        Serial.println("noBGM!!!!!!***");
  }
}


void shootMethod(){
    if(shoot){
      ctservo.write(90);
       line = music_02;
    }
    else{
      ctservo.write(0);
    }
  }

void setBGM(){
  if(line > lastLine){
    setPeriod(line);
    BGM(line);
    lastLine = line;
    lastTime = millis();
  }
  else if(millis() - lastTime > period && line != lastLine){
    setPeriod(line);
    BGM(line);
    lastLine = line;
    lastTime = millis();
  }
  else if(millis() - lastTime > period && line == lastLine){
    BGM(0);
  }
}

void setPeriod(int line){
  switch(line){
    case 48 :
    period = 2000;
    break;
    case 49 :
    period = 4000;
    break;
    case 50 :
    period = 1000;
    break;
    case 51 :
    period = 3000;
    break;
    case 52 :
    period = 243000;
    break;
  }
}
