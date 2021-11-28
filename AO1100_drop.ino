//-------------------------
// Airon Robotics 
// Yonguk Cho
//-------------------------

#include <Servo.h>  //Servo Library 추가
#define SRV 11      //Servo pin num 선언

Servo ServoS;         //Servo 클래스로 ServoS 객체 생성
int Srv_Stop = 1500;  //Servo center value for stop
int Srv_FSpd = 1700;  //Servo Forward value for speed
int Srv_BSpd = 1300;  //Servo Backward value for speed 

int i_stop;
int i_Fgo;  
int i_Hgo;

#define inPWM 7 //pwm signal input num 선언

int pwm; //pwm signal 저장 변수
bool toggle = false; //Forward/Backward 변수
int i_cnt = 0; //동작지연을 위한 카운터 변수
int i_cnt1 = 0; //동작지연을 위한 카운터 변수 

void setup() 
{
  Serial.begin(9600); //9600bps로 시리어포트 Open
 
 /*digital I/O pin을 사용하려면 먼저 pinMode() 함수로 각각의 핀을 입력 또는 출력 모드로
 사용하겠다고 먼저 선언해야 한다.
 일반적으로 아두이노에 스위치를 연결하는 경우, 외부에 pull-up 저항을 연결해 주는것이 보통이다. 
 하지만 ATmega168/328의 각 I/O 핀에는 pull-up 회로가 들어있다. 이를 사용한다고 선언해야 한다.
 이는 스위치의 경우 채털링에 의한 오류를 방지하기 위히 사용한다.
 */
 
  pinMode(inPWM,INPUT); //pwm을 "INPUT" 선언, 입력의 경우 "INPUT"를 선언 
  pinMode(2,INPUT_PULLUP); //전진 스위치, 내부 pull up 저항 사용을 선언
  pinMode(3,INPUT_PULLUP); //후진 스위치, 내부 pull up 저항 사용을 선언
  pinMode(4,INPUT_PULLUP); //내부 pull up 저항 사용을 선언
  pinMode(5,INPUT_PULLUP); //전외선 근접센서, 내부 pull up 저항 사용을 선언
 
  pinMode(SRV, OUTPUT); //Servo(출력이므로) 제어를 "OUTPUT"으로 선언
  ServoS.attach(SRV);
} 

void loop() 
{
  pwm = pulseIn(inPWM,HIGH);  //pwm 변수에 PWM pulse의 Length를 저장. 
  //Serial.print(pwm);        //Serial Port로 pwm 값을 출력
  //Serial.println("--");     //Serial Port로 개행문자와 함께 "--"를 출력
 
  if (pwm >= 1700){     //입력된 PWM이 1700보다 크다면 서보를 Forward 밯향으로 회전하기 위한 구문
    toggle = true;      //toggle boolean 변수를 true로 설정
    i_cnt1+=1;          //동작시간(회수)를 계산하기 위한 변수(i_cnt1)을 증가
  } 
  else if (pwm <= 1300){ //입력된 PWM이 1300보다 작다면 서보 정지
    toggle = false;     //toggle boolean 변수를 false로 설정
    act_manual();       //서보가 동작하지 않을 시 버튼 입력을 기다리는 함수
    i_cnt1=0;           //서보동작하지 않는다면 칸운터를 계수하지 않는다.
  }
 
  if ( toggle == true ) {     //PW이 1700보다 크다면...... 
    i_stop = digitalRead(5);  //근접센서의 값을 읽는다.
    if ( (i_stop == 0)&&(i_cnt1>=20)) { //근접센서의 값에 Low이고 i_cnt1이 10보다 크거나 같다면 서보를 정지 시킨다. 
      ServoS.write(Srv_Stop); 
      if ( i_cnt1 == 30 ) { ServoS.write(Srv_BSpd); delay(100); };
    } 
    else { // i_stop이 High이고 i_cnt1이 10보다 작다면 서보 전진(Forward)
      ServoS.write(Srv_FSpd); 
    }
  }
} 

void act_manual()
{
  //pwm이 1300보다 작아서 이 함수가 계속 실행 중일 때는 전진 스위치, 
  //후진 스위치, 근접센서의 값을 계속 읽는다.
  i_Fgo = digitalRead(2);   //전전 스위치
  i_Hgo = digitalRead(3);   //후진 스위친
  i_stop = digitalRead(5);  //근접센서
  Serial.print(i_Fgo); Serial.print(":"); Serial.print(i_Hgo); Serial.print(":"); Serial.println(i_stop);
  
  /*전진 스위치는 1번 누르면 다음 bottle위치까지 회전한다.
  그러나 후진 스위친는 1번 누르면 절반 정도면 회전한다. */ 
  
  if (i_stop == 0) { //근전센서가 LOW가 되면 서보정지(STOP)
    ServoS.write(Srv_Stop); 
  }; 
 
  if ( i_Fgo == 0) { //전진 스위치가 LOW가 되면 서보전진(FORWARD), 근접센서가 LOW가 될때 까지...
    ServoS.write(Srv_FSpd);
  }; 
 
  //후진은 카운터 계수 만큼만 후진 시키위 위한 i_cnt를 1로 한다.
  if ( i_Hgo == 0) { i_cnt = 1; };
  if ( i_cnt >= 1 ){ //후진 버튼이 눌렸을 때, 서보는 후진한다. i_cnt를 증가시키면서...
    ServoS.write(Srv_BSpd);
    i_cnt += 1;
    if ( i_cnt >= 10 ) { //i_cnt가 10보다 크면 서보는 정지되며 i_cnt를 0으로 한다.
      ServoS.write(Srv_Stop);
      i_cnt = 0; 
    }; 
  }
}
