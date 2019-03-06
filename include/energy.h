#pragma once
#include<string>
#include<vector>
#include<map>
using namespace std;
class EnergyProccessing
{
private:
    vector<string> enName = {"100GeV", "1TeV", "10TeV", "100TeV"};
    vector<string> particleName = {"p","c", "he", "fe"};
    vector<double> logEn={-1, 0, 1, 2};
    vector<double> enPrimary={0.1, 1, 10, 100};
    map<string,vector<vector<double>>> specData;

public:
    EnergyProccessing();
    ~EnergyProccessing(){}
    void ParsingFileEn (string);
    void EnergyDist();
    void EnergyRecovery();
    void EnergySpectrum();
    void EnergyCoef();

};
