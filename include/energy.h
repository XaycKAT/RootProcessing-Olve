#pragma once
#include<string>
#include<vector>
#include<map>
using namespace std;
class RootProccessing
{
private:
    vector<string> enName = {"100GeV", "1TeV", "10TeV", "100TeV"};
    vector<string> particleName = {"p", "he", "c", "fe"};
    vector<double> logEn={2, 3, 4, 5};
    vector<double> enPrimary = {100, 1000,10000, 100000};
    map<string,vector<vector<double>>> specData;

public:
    RootProccessing();
    ~RootProccessing(){}
    void ParsingFileEn (string);
    void EnergyDist();
    void EnergyRecovery();
    void EnergySpectrum(string);
    void EnergyCoef();
    void ZRCSpectrum(string);
    static void ReadZ(ifstream &, vector<double> &);

};
