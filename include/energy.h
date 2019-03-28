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
    vector<double> logEn={5, 6, 7, 8};
    vector<double> enPrimary = {1e5, 1e6 ,1e7, 1e8};
    map<string,vector<vector<double>>> specData;
    map<string,multimap<double,double>> specSpData;
    double silicMIP = 0.18;

public:
    RootProccessing();
    ~RootProccessing(){}
    void ParsingFileEn (string);
    void EnergyDist( );
    void EnergyRecovery();
    void EnergySpecSp(string);
    void EnergyCoef();
    void ZRCSpectrum(string);
    static void ReadZ(ifstream &, vector<double> &);

};
