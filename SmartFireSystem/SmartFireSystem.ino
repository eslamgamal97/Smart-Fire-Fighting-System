#define temp_sensor A2
#define human_sensor 7
#define pwmFan  3
#define dirFan 2
#define pwmMotor  5
#define dirMotor 4
#include <PID_v1.h>
#define debug 0
///########################## PID Fan ####################

//Define Variables we'll be connecting to
double SetpFan, Input, OutputFan;
//Specify the links and initial tuning parameters.
double Kp1 = 10.0, Ki1 = 1.5, Kd1 = 0.05;
PID fanPID(&Input, &OutputFan, &SetpFan, Kp1, Ki1, Kd1, DIRECT);
///-----------------------------------------------------

///########################## PID Motor ####################

//Define Variables we'll be connecting to
double SetpMotor, OutputMotor;

//Specify the links and initial tuning parameters.
double Kp2 = 4, Ki2 = 1.0, Kd2 = 0.1;
PID motorPID(&Input, &OutputMotor, &SetpMotor, Kp2, Ki2, Kd2, DIRECT);

///-----------------------------------------------------

///#############  Declaring variables ###########################333
double offset_t = 0;    //room temperature
double cur_temp = 0;    //current temperature
bool cur_pir;           // PIR Reading 
char state_flag = 0;    // State of the system
char humanWas=0;       // if there is a human or not
bool fire_flag = 0;    // flag for fire causing
bool air_cool = 0;     //flag for air cooling

void setup() {
  Serial.begin(115200);

  /// ######################## Pins mode #######################
  // put your setup code here, to run once:
  pinMode(temp_sensor, INPUT);
  pinMode(human_sensor, INPUT);
  pinMode(pwmFan, OUTPUT);
  pinMode(dirFan, OUTPUT);
  pinMode(pwmMotor, OUTPUT);
  pinMode(dirMotor, OUTPUT);
  //-------------------------------------------------------------

  //################## take temp init value and setpoints for pid ############
  for (int i = 0 ; i < 10; i++)
  {
    offset_t += analogRead(temp_sensor);
    delay(4);
  }
  offset_t /= 10;
  offset_t /= 2.048;
  SetpFan = offset_t+5;
  SetpMotor = offset_t + 20;
  Serial.print("Room Temperature = ");
  Serial.println(offset_t);
  //-----------------------------------------------------------------------

  ///#####################  PID ON #########################

  //turn the PID on
  fanPID.SetMode(AUTOMATIC);
  fanPID.SetSampleTime(100);
  fanPID.SetOutputLimits(-250, 0);
  motorPID.SetSampleTime(100);
  motorPID.SetOutputLimits(-250, 0);
  motorPID.SetMode(AUTOMATIC);
  ///--------------------------------------------------------

}

void loop() {
   while(state_flag == 0)  // here everything is fine no fire 
   {
      humanWas = 0;
      fire_flag = 0;
      cur_temp = analogRead(temp_sensor) / 2.048;
      
      //turn on FAN
      fanPID.Compute();
      analogWrite(pwmFan, -OutputFan);
      
      if(air_cool == 0)
      {
        Serial.println("everything is fine here");
        air_cool = 1;
      }
      if(cur_temp > offset_t + 8) // condition for occuring fire  
      {
        state_flag = 1;
        break;
      }
   }
   while(state_flag == 1)  //here is fire
   {
      air_cool = 0;
      if(humanWas != 2)
      {
        humanWas = 0;
      }
      cur_temp = analogRead(temp_sensor) / 2.048;
      cur_pir = digitalRead(human_sensor);
      if(cur_pir == 0 && cur_temp > offset_t+8)
      {
          if(fire_flag == 0)
          {
             if(humanWas != 2)
             {
                Serial.println("Home on fire & no human");
                fire_flag = 1;
             }
          }  
      }
      else if (cur_temp < offset_t+3)
      {
        state_flag = 0;
        humanWas = 0;
        break;
      }
      else if(cur_pir == 1 && cur_temp > offset_t+3)    //there is a condition for the case is a human in fire and need a help
      {
         state_flag = 2;
         break;
      }
  }
  while(state_flag == 2)  //here is a human in fire and need a help
  {
      air_cool = 0;
      fire_flag = 0;
      cur_temp = analogRead(temp_sensor) / 2.048;
      cur_pir = digitalRead(human_sensor);
      if(cur_pir == 0 && cur_temp > offset_t+3)
      {
        Serial.println("Keep calm ! now human is safe but there is still a fire");
        humanWas = 2;  
        state_flag = 1;
      }
      else if(cur_pir == 1)
      {
        if(humanWas ==0)
        {
           Serial.println("DANGER!! there is a human in fire at home");
           humanWas = 1;
        }
      }
      else if(cur_temp < offset_t+3)
      {
        Serial.println("now fire is finished and human is in safe");
        state_flag = 0;
      }
  }
}
