/*CANBUS Motor Setup*/
CAN_message_t msgR;
struct CAN_filter_t defaultMask;
uint32_t ID_offset = 0x140;
uint32_t Motor_ID1 = 2;
uint32_t Motor_ID2 = 3;
int CAN_ID = 0;
double Gear_ratio = 6;
int Stop_button = 0;
String mode = "start";
double milli_time_current_double = 0;
Motor_Control m1(Motor_ID1, CAN_ID, Gear_ratio);
Motor_Control m2(Motor_ID2, CAN_ID, Gear_ratio);
double Pgain = 7.5;
double Igain = 0.7;
double Dgain = 0;
double MaxPIDout = 10;

/*Mode Setup*/
int assist_mode = 1;

/*Sensors Setup*/
IMU imu;

/*Time intervals Setup*/
double Fsample = 500;
unsigned long current_time = 0;
unsigned long previous_time = 0;
unsigned long Tinterval_microsecond = (unsigned long)(1000000 / Fsample);

/*Commads Setup*/
double Cur_command_L = 0;
double Cur_command_R = 0;
int current_limitation = 20;

void setup()
{
  delay(3000);
  /*Serial initialization*/
  Serial.begin(115200);
  Serial4.begin(115200);

  /*Canbus initialization*/
  initial_CAN();

  delay(500);

  /*Imu initialization*/
  imu.Gain_E = 3;
  imu.Gain_F = 1;
  imu.delaypoint = 0;

  /*Motors initialization*/
  m1.init_motor();
  delay(100);
  m2.init_motor();
  delay(100);
  reset_motor_angle();

  /*Control/IMU initialization*/
  CurrentControlSetup();

}

void loop()
{
  /*Motors initialization*/
  while (stopFlag)
  {
  };
  CurrentControl();
}

void CurrentControlSetup()
{
  imu.INIT();
  delay(500);
  imu.INIT_MEAN();
  current_time = micros();
  previous_time = current_time;
}

void CurrentControl()
{

  imu.READ();
  current_time = micros();

  if (current_time - previous_time > Tinterval_microsecond)
  {
    if (Stop_button)
    {
      Cur_command_L = 0;
      Cur_command_R = 0;
    }
    else
    {
      Compute_Cur_Commands();
    }
    Cur_limitation();
 
    m1.send_current_command(Cur_command_L);
    receive_CAN_data();
    m2.send_current_command(Cur_command_R);
    receive_CAN_data();

    previous_time = current_time;
    relTime += Tinterval_microsecond / 1000;
  }

}

void Compute_Cur_Commands()
{

  if (assist_mode == 1) 
  {
    mode = "Walking (IMU)";
    Cur_command_L = -imu.walking[0] / 2.2; //this is for bilateral walking assistance_left leg
    Cur_command_R = +imu.walking[1] / 2.2; //this is for bilateral walking assistance_right leg

  }

  else if (assist_mode == 2)
  {
    mode = "Stair Ascending (IMU)";
    Cur_command_L = imu.stair_ascending[0] / 2.2; //this is for bilateral walking assistance_left leg
    Cur_command_R = (-1) * imu.stair_ascending[1] / 2.2; //this is for bilateral walking assistance_right leg
  }

  else if (assist_mode == 3)
  {
    mode = "Squatting (IMU)";

    if ((((imu.RTAVx + imu.LTAVx) / 2) < -30) && (((imu.RTx + imu.LTx) / 2) > -100))
    {
      Cur_command_L = 0;
      Cur_command_R = 0;
    }
    else
    {
      Cur_command_L = (-1) * 10 * 0.002 * imu.Gain_E * imu.SquatTorque / 2.2;
      Cur_command_R =  10 * 0.002 * imu.Gain_E * imu.SquatTorque / 2.2;
    }

    if (Cur_command_L < 0)
    {
      Cur_command_L = 0;
    }

    if (Cur_command_R > 0)
    {
      Cur_command_R = 0;
    }
  }

  else if (assist_mode == 100)
  {
    mode = "Stop";
    Cur_command_L = 0;
    Cur_command_R = 0;
  }
}

void Cur_limitation()
{
  //************* Current limitation *************//
  Cur_command_L = min(current_limitation, Cur_command_L);
  Cur_command_L = max(-current_limitation, Cur_command_L);
  Cur_command_R = min(current_limitation, Cur_command_R);
  Cur_command_R = max(-current_limitation, Cur_command_R);

  torque_command_L = Cur_command_L * 2.2;
  torque_command_R = Cur_command_R * 2.2;
}

void initial_CAN()
{
  //initial CAN Bus
  Can0.begin(1000000, defaultMask, 1, 1);
  delay(3000);
  pinMode(28, OUTPUT);
  digitalWrite(28, LOW);
  Serial.println("Can bus setup done...");
}

void receive_CAN_data()
{
  while (Can0.available() > 0)
  {
    Can0.read(msgR);
    if (msgR.id == (ID_offset + Motor_ID1))
    {
      m1.DataExplanation(msgR);
    }
    else if (msgR.id == (ID_offset + Motor_ID2))
    {
      m2.DataExplanation(msgR);
    }
  }
}


