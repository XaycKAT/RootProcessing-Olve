#include <iostream>
#include <fstream>
#include <string>
#include <energy.h>
using namespace std;

int main()
{
    string path = "/home/xayc/CERN/data/spectrums/specEN_";
    EnergyProccessing obj;
    obj.ParsingFileEn(path);
    //obj.EnergyDist();
    //obj.EnergyRecovery();
    obj.EnergyCoef();

    return 0;
}
