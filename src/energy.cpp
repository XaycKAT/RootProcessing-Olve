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
#include <TStyle.h>
#include <TGraph2D.h>
#include <TLegendEntry.h>
#include <TLegend.h>
#include <TGraph.h>
#include <TPaveStats.h>
#include <TROOT.h>
using namespace std;
EnergyProccessing::EnergyProccessing()
{

}

void EnergyProccessing::ParsingFileEn(string path)
{
    for (int k = 0; k < 4; k++)
    {
        vector<vector<double>> envec;
        for (int i = 0; i < 4; i++)
        {
            ifstream file((path+particleName[k]+'_'+enName[i]+".dat").c_str(), ifstream::binary);
            if (!file.is_open())
            {
                cout << "file can not be opened, or it does not exist " << endl;
                exit(EXIT_FAILURE);
            }
            double de=0;
            vector<double> vecde;
            while(file.read((char*)&de,sizeof (de)))
            {
                vecde.push_back(de);
            }
            envec.push_back(vecde);
        }
        specData.insert(pair<string,vector<vector<double>>>(particleName[k],envec));

    }
}
void EnergyProccessing::EnergyDist()
{
    TApplication tapp("Test", 0, nullptr);

    for(int k = 0; k < particleName.size(); k++)
    {
        TH1D *enHist[4];
        for (int i = 0; i< 4; i++)
        {
            enHist[i] = new TH1D("value", ("Energy distribution " + particleName[k]).c_str() ,  100,0,10);
        }
        for(int i = 0; i < 4; i++)
        {
            auto enVec = specData.at(particleName[k]);
            for(auto el : enVec[i])
                enHist[i]->Fill(log10(el));

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
        legend->AddEntry(enHist[0],enName[0].c_str());
        legend->AddEntry(enHist[1],enName[1].c_str());
        legend->AddEntry(enHist[2],enName[2].c_str());
        legend->AddEntry(enHist[3],enName[3].c_str());
        legend->Draw();
        c1.SetGridy();
        //c1.SetLogx();
        //c1.SetLogy();
        c1.Print(("/home/xayc/CERN/data/graph/specEN_"+particleName[k]+".pdf").c_str());
    }
    //tapp.Run();
}

void EnergyProccessing::EnergyRecovery()
{
    TApplication tapp("Test", 0, nullptr);

    for(int k = 0; k < 4 ; k++)
    {

        vector<double> enEmissionX;
        vector<double> enEmissionY;
        for(int i = 0; i < 4; i++)
        {
            double en=0;
            double d1=0;
            double d2=0;
            int j=0;

            auto enVec = specData.at(particleName[k]);
            for(auto el : enVec[i])
            {
                en+=log10(el);
                j++;
                d1 += pow(el/enPrimary[i],2.6);
                d2 += pow(el/enPrimary[i],1.6);
            }

            en/=static_cast<double>(j);
            enEmissionX.push_back(en);
            enEmissionY.push_back(logEn[i]);
        }


        TGraph *gr = new TGraph (enEmissionX.size(), enEmissionX.data(), enEmissionY.data());
        TCanvas c ("test", "test");
        gr->SetTitle(("Energy Recovery "+particleName[k]).c_str());
        gr->GetXaxis()->SetTitle("lg(dE), lg(MeV)");
        gr->GetYaxis()->SetTitle("lg(E_0), lg(MeV)");
        gr->SetMarkerStyle(8);
        gr->Fit("pol1");
        gr->Draw("AP");
        gROOT->SetStyle("Plain");
        gStyle->SetOptFit(11);
        c.SetGridx();
        c.Print(("~/CERN/data/graph/enRecovery_"+particleName[k]+".pdf").c_str());


        //tapp.Run();
    }

}

void EnergyProccessing::EnergyCoef()
{
    for(int k = 0; k < 4 ; k++)
    {
        vector<double> coef;
        for(int i = 0; i < 4; i++)
        {
            double d1=0;
            double d2=0;
            int j=0;

            auto enVec = specData.at(particleName[k]);
            for(auto el : enVec[i])
            {
                j++;
                d1 += pow(el/enPrimary[i],2.6);
                d2 += pow(el/enPrimary[i],1.6);
            }

            coef.push_back(1./d2*d1);

        }


        TGraph *gr1 = new TGraph (coef.size(), enPrimary.data(), coef.data());
        TCanvas c1 ("test", "test");
        gROOT->SetStyle("Plain");
        gStyle->SetOptFit(11);
        gr1->Draw("APL");
        gr1->SetMarkerStyle(8);
        gr1->SetTitle(("Artificial coefficient "+particleName[k]).c_str());
        gr1->GetXaxis()->SetTitle("E, TeV");
        gr1->GetYaxis()->SetTitle("E (dN/dE)");
        c1.SetLogx();
        c1.SetGridy();
        c1.Print(("~/CERN/data/graph/coef_"+particleName[k] +".pdf").c_str());
        coef.clear();
    }
}


