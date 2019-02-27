#include<energy.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <TTree.h>
#include <TF1.h>
#include <TApplication.h>
#include "TH2.h"
#include "TCanvas.h"
#include <TGraph2D.h>
#include <TLegendEntry.h>
#include <TLegend.h>
#include <TGraph.h>
using namespace std;
EnergyProccessing::EnergyProccessing()
{

}

void EnergyProccessing::EnergyDist(string particleName)
{
    TApplication tapp("Test", 0, nullptr);

    //string particleName = "he";
    string fileName = "specEN_";

    TH1D *enHist[4];
    for (int i = 0; i< 4; i++)
    {
        enHist[i] = new TH1D("value", ("Energy distribution " + particleName).c_str() ,  100,0,10);
    }
    for(int i = 0; i < 4; i++)
    {
        string name = ("/home/xayc/CERN/data/spectrums/specEN_"+particleName+'_'+enFiles[i]+".dat").c_str();
        ifstream file(name, ifstream::binary);
        if (!file.is_open())
        {
            cout << name << "file can not be opened, or it does not exist " << endl;
            exit(EXIT_FAILURE);
        }
        double en;
        while(file.read((char*)&en,sizeof (en)))
        {
            enHist[i]->Fill(log10(en));
        }

    }

    for(int j = 0; j < 4; j++)
    {
        for (int i = 1;i <= 100;i++)
        {
            int sum = enHist[j]->GetBinContent(i);
            double cen = enHist[j]->GetBinCenter(i);
            if( j == 0 || j == 1)
                enHist[j]->SetBinContent(i,sum/4000./cen);
            else
                enHist[j]->SetBinContent(i,sum/3000./cen);
        }
    }

    TCanvas c1 ("test", "test");

    enHist[0]->SetLineColor(kBlack);
    enHist[0]->Draw();
    enHist[0]->GetXaxis()->SetTitle("lg(dE), lg(MeV)");
    enHist[0]->GetYaxis()->SetTitle("Number of events");

    enHist[1]->SetLineStyle(2);
    enHist[1]->SetLineColor(kBlack);
    enHist[1]->Draw("SAME");

    enHist[2]->SetLineStyle(3);
    enHist[2]->SetLineColor(kBlack);
    enHist[2]->Draw("SAME");

    enHist[3]->SetLineStyle(5);
    enHist[3]->SetLineColor(kBlack);
    enHist[3]->Draw("SAME");
    TLegend *legend = new TLegend(0.78,0.7,0.98,0.95);
    legend->AddEntry(enHist[0],enFiles[0].c_str());
    legend->AddEntry(enHist[1],enFiles[1].c_str());
    legend->AddEntry(enHist[2],enFiles[2].c_str());
    legend->AddEntry(enHist[3],enFiles[3].c_str());
    legend->Draw();
    c1.SetGridy();
    //c1.SetLogx();
    //c1.SetLogy();
    c1.Print(("/home/xayc/CERN/data/graph/specEN_"+particleName+".pdf").c_str());

    //tapp.Run();
}

void EnergyProccessing::EnergyRecovery()
{
    TApplication tapp("Test", 0, nullptr);

    vector<double> enEmissionX;
    vector<double> enEmissionY;
    vector<double> enPrimary={-1, 0, 1, 2};
    vector<string> particleName = {"p","c", "he", "fe"};
    for(int k =0; k<4; k++)
    {
        for(int i = 0; i < 4; i++)
        {
            string name = ("/home/xayc/CERN/data/spectrums/specEN_"+particleName[k]+'_'+enFiles[i]+".dat").c_str();
            ifstream file(name, ifstream::binary);
            if (!file.is_open())
            {
                cout << name << "file can not be opened, or it does not exist " << endl;
                exit(EXIT_FAILURE);
            }
            double en=0;
            double de=0;
            int j=0;
            while(file.read((char*)&de,sizeof (de)))
            {
                en+=log10(de);
                j++;
            }
            en/=static_cast<double>(j);
            enEmissionX.push_back(en);
            enEmissionY.push_back(enPrimary[i]);
        }
    }
    TGraph *gr = new TGraph (enEmissionX.size(), enEmissionX.data(), enEmissionY.data());
    TCanvas c ("test", "test");
    gr->SetMarkerStyle(8);
    gr->SetTitle("Energy Recovery");
    gr->GetXaxis()->SetTitle("lg(dE), lg(MeV)");
    gr->GetYaxis()->SetTitle("lg(E_0), lg(MeV)");
    gr->Fit("pol1");
    gr->Draw("AP");
    c.SetGridx();
    c.Print("~/CERN/data/graph/enRecovery.pdf");
    //tapp.Run();

}
