#include <iostream>
#include <fstream>
#include <string>
#include <energy.h>
using namespace std;

int main()
{
    string path = "/home/xayc/CERN/data/spectrums/";
    RootProccessing obj;
//    obj.ParsingFileEn(path);
//    obj.EnergyDist();
//    obj.EnergyRecovery();
//    obj.EnergyCoef();
//    obj.EnergySpectrum(path);
    obj.ZRCSpectrum(path);
    return 0;
}
