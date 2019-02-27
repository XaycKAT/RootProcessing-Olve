#pragma once
#include<string>
#include<vector>
using namespace std;
class EnergyProccessing
{
private:
    vector<string> enFiles = {"100GeV", "1TeV", "10TeV", "100TeV"};


public:
    EnergyProccessing();
    ~EnergyProccessing(){}
    void EnergyDist(string);
    void EnergyRecovery();

};
