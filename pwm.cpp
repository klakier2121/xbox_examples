#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>

#define MSEC(a) usleep(1000*a)

using namespace std;

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void analogUnexport(int pin);
void analogWrite(int p, int value);
void analogExport(int pin);
void analogEnable(int _enable, int _pin);

char chTxt[100];
string sStart = "start:";
string sAnalog = "RT:";

int main()
{
    FILE *in;
    string sBuf;
    size_t pos, pos2;
    string value[2];
    char buff[1024];
    int val;

    if(!(in = popen("nohup xboxdrv", "r")))
    {
        return 1;
    }
    analogExport(0);
    analogEnable(1, 0);

    while(fgets(buff, sizeof(buff), in)!=NULL)
    {
        sBuf = buff;

        //while pressed start button, close process
        pos2 = sBuf.find(sStart);
        if(pos2 != string::npos)
        {
            value[1] = sBuf.substr(pos2+sStart.length(), 1);
            if(value[1] == "1")
                break;
        }

        pos = sBuf.find(sAnalog);
        if(pos == string::npos) continue; //output xboxdrv junk and find string

        value[0] = sBuf.substr(pos+sAnalog.length(), 3);

        val = atoi(value[0].c_str());
        analogWrite(0, val);
        cout << val << endl;


    }
    analogUnexport(0);

    //Fading led
    /*
      for (int fadeValue = 0 ; fadeValue <= 100; fadeValue += 1) {
        analogWrite(0, fadeValue);
    	cout << fadeValue << endl;
        MSEC(30);
      }

      for (int fadeValue = 100 ; fadeValue >= 0; fadeValue -= 1) {
        analogWrite(0, fadeValue);
    	cout << fadeValue << endl;
        MSEC(30);
      }

      */

    return 0;
}
void analogUnexport(int pin)
{
    sprintf(chTxt, "echo %i > /sys/class/pwm/pwmchip0/unexport", pin);
    system(chTxt);
}
void analogExport(int pin)
{
    sprintf(chTxt, "echo %i > /sys/class/pwm/pwmchip0/export", pin);
    system(chTxt);
}
void analogEnable(int _enable, int _pin)
{
    sprintf(chTxt, "echo %i > /sys/class/pwm/pwmchip0/pwm%i/enable", _enable, _pin);
    system(chTxt);
}
void analogWrite(int p, int value)
{
    value = map(value, 0, 255, 0, 100); //convert 0-255 to 0-100

    sprintf(chTxt, "echo 1000000 > /sys/class/pwm/pwmchip0/pwm%i/period", p);
    system(chTxt);

    sprintf(chTxt, "echo %i > /sys/class/pwm/pwmchip0/pwm%i/duty_cycle", value*10000, p);
    system(chTxt);

}
