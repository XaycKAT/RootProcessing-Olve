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
RootProccessing::RootProccessing()
{

}

void RootProccessing::ParsingFileEn(string path)
{
    path = path + "specEN_";
    for (unsigned int k = 0; k < particleName.size(); k++)
    {
        vector<vector<double>> envec;
        for (unsigned int i = 0; i < enName.size(); i++)
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
            file.close();
        }
        specData.insert(pair<string,vector<vector<double>>>(particleName[k],envec));

    }

}
void RootProccessing::EnergyDist()
{
    // TApplication tapp("Test", 0, nullptr);

    for(unsigned int k = 0; k < particleName.size(); k++)
    {
        TH1D *enHist[enName.size()];
        for (unsigned int i = 0; i < enName.size(); i++)
        {
            enHist[i] = new TH1D("value", ("Energy distribution " + particleName[k]).c_str() ,  100,0,10);
        }
        for(unsigned int i = 0; i < enName.size(); i++)
        {
            auto enVec = specData.at(particleName[k]);
            for(auto el : enVec[i])
                enHist[i]->Fill(log10(el));

        }

        for(unsigned int j = 0; j < enName.size(); j++)
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
        enHist[0]->GetXaxis()->SetTitle("lg dE , lg MeV");
        enHist[0]->GetYaxis()->SetTitle("Normalized NOE");

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
        c1.Print(("/home/xayc/CERN/data/graph/"+particleName[k]+"/specEN_"+particleName[k]+".pdf").c_str());
    }
    //tapp.Run();
}
void RootProccessing::EnergyRecovery()
{
    //TApplication tapp("Test", 0, nullptr);

    for(int k = 0; k < 4 ; k++)
    {

        vector<double> enEmissionX;
        vector<double> enEmissionY;
        for(int i = 0; i < 4; i++)
        {
            double en=0;
            int j=0;

            auto enVec = specData.at(particleName[k]);
            for(auto el : enVec[i])
            {
                en+=log10(el);
                j++;

            }

            en/=static_cast<double>(j);
            enEmissionX.push_back(en);
            enEmissionY.push_back(logEn[i]);
        }

        
        TGraph *gr = new TGraph (enEmissionX.size(), enEmissionX.data(), enEmissionY.data());
        TCanvas c ("test", "test");
        gr->SetTitle(("Energy Recovery "+particleName[k]).c_str());
        gr->GetXaxis()->SetTitle("lg dE, lg MeV ");
        gr->GetYaxis()->SetTitle("lg E_{0}, lg MeV ");
        gr->SetMarkerStyle(8);
        gr->Fit("pol1");
        gROOT->SetStyle("Plain");
        gStyle->SetOptFit(11);
        gr->Draw("AP");

        c.Update();
        c.SetGridx();
        c.Modified();
        c.Print(("~/CERN/data/graph/"+particleName[k]+"/enRecovery_"+particleName[k]+".pdf").c_str());
        //tapp.Run();
    }

}
void RootProccessing::EnergyCoef()
{
    for(int k = 0; k < 4 ; k++)
    {
        vector<double> coef;
        for(int i = 0; i < 4; i++)
        {
            double d1=0;
            double d2=0;

            auto enVec = specData.at(particleName[k]);
            for(auto el : enVec[i])
            {
                d1 += pow(el/enPrimary[i],2.6);
                d2 += pow(el/enPrimary[i],1.6);
            }

            coef.push_back(d1/d2);

        }


        TGraph *gr = new TGraph (coef.size(), logEn.data(), coef.data());
        TCanvas c ("test", "test");
        //c.SetLogx();
        c.SetGridy();
        gr->SetTitle(("Artificial coefficient "+particleName[k]).c_str());
        gr->SetMarkerStyle(8);
        gr->GetXaxis()->SetTitle("lg E, lg MeV");
        gr->GetYaxis()->SetTitle("E (dN/dE)");
        gROOT->SetStyle("Plain");
        gr->Fit("pol1");
        gStyle->SetOptFit(11);
        gr->Draw("AP");
        c.Update();
        c.Print(("~/CERN/data/graph/"+particleName[k]+"/coef_"+particleName[k] +".pdf").c_str());
        coef.clear();
    }
}
void RootProccessing::EnergySpectrum(string path)
{
    path = path + "specEN_";
    for(unsigned int k = 0; k < particleName.size(); k++)
    {
        ifstream file((path+particleName[k]+"_sp.dat").c_str(), ifstream::binary);
        if (!file.is_open())
        {
            cout << "file can not be opened, or it does not exist " << endl;
            exit(EXIT_FAILURE);
        }
        TH1D *enHist = new TH1D("value", ("Energy distribution of uniform " + particleName[k]).c_str() ,  100,2,8);
        double de=0;
        while(file.read((char*)&de,sizeof (de)))
            enHist->Fill(log10(de));

        for (int i = 1;i <= 100;i++)
        {
            int sum = enHist->GetBinContent(i);
            double cen = enHist->GetBinCenter(i);
            enHist->SetBinContent(i,sum/10000./cen);
        }
        TCanvas c1 ("test", "test");

        enHist->SetLineColor(kBlack);
        enHist->Draw();
        enHist->GetXaxis()->SetTitle("lg dE , lg MeV");
        enHist->GetYaxis()->SetTitle("Normalized NOE");
        c1.SetGridy();
        c1.Print(("/home/xayc/CERN/data/graph/"+particleName[k]+"/specEN_"+particleName[k]+"_sp.pdf").c_str());

    }

}

void RootProccessing::ReadZ(ifstream &file, vector<double> &vec)
{
    for(int i = 0; i < 7; i ++)
    {
        double z = 0;
        file.read((char*)&z,sizeof (z));
        vec.push_back(z);
    }
}
void RootProccessing::ZRCSpectrum(string path)
{
    path = path + "specZRC_";
    vector<string> zName = enName;
    zName.push_back("sp");
    for(unsigned int k = 0; k < particleName.size(); k++)
    {
        for (unsigned int i = 0; i < zName.size(); i++)
        {
            ifstream file((path+particleName[k]+'_'+zName[i]+".dat").c_str(), ifstream::binary);
            if (!file.is_open())
            {
                cout << "file can not be opened, or it does not exist " << endl;
                exit(EXIT_FAILURE);
            }
            TH1D *zHist;
            TH1D *zwHist;
            TH1D *rcHist;
            if( k == 0 || k == 1)
            {
                zHist = new TH1D("value", ("Z distribution of " + particleName[k]+" " +zName[i]).c_str() ,  60,0,4);
                zwHist = new TH1D("value", ("Z distribution of " + particleName[k]+" " +zName[i]).c_str() ,  60,0,4);
                rcHist = new TH1D("value", ("RC distribution of " + particleName[k]+" " +zName[i]).c_str() ,  60,0,4);

            }
            if( k == 2)
            {
                zHist = new TH1D("value", ("Z distribution of " + particleName[k]+" " +zName[i]).c_str() ,  60,4,8);
                zwHist = new TH1D("value", ("Z distribution of " + particleName[k]+" " +zName[i]).c_str() ,  60,4,8);
                rcHist = new TH1D("value", ("RC distribution of " + particleName[k]+" " +zName[i]).c_str() ,  60,4,8);

            }
            if( k == 3)
            {
                zHist = new TH1D("value", ("Z distribution of " + particleName[k]+" " +zName[i]).c_str() ,  60,20,50);
                zwHist = new TH1D("value", ("Z distribution of " + particleName[k]+" " +zName[i]).c_str() ,  60,20,50);
                rcHist = new TH1D("value", ("RC distribution of " + particleName[k]+" " +zName[i]).c_str() ,  100,100,150);

            }
            double zwn = 0;
            double zwon = 0;
            double rc = 0;
            while(file.read((char*)&zwn,sizeof (zwn)))
            {
                file.read((char*)&zwon,sizeof (zwon));
                file.read((char*)&rc,sizeof (rc));
                zHist->Fill(zwn);
                zwHist->Fill(zwon);
                rcHist->Fill(rc);
            }
            TCanvas c ( "test", "test" );
            zHist->SetLineStyle(2);
            zHist->GetXaxis()->SetTitle("Z");
            zHist->Draw();
            zwHist->Draw("SAME");
            TLegend *legend = new TLegend(0.78,0.7,0.98,0.95);
            legend->AddEntry(zHist, "Z value with projection");
            legend->AddEntry(zwHist, "Z value without projection");
            legend->Draw();
            c.SetGridx();
            c.SaveAs(("/home/xayc/CERN/data/graph/"+particleName[k]+"/specZ_"+particleName[k]+"_"+zName[i]+".pdf").c_str());
            TCanvas c1 ( "test", "test" );
            rcHist->GetXaxis()->SetTitle("RC, MeV");
            rcHist->Draw();
            c1.SetGrid();
            c1.SaveAs(("/home/xayc/CERN/data/graph/"+particleName[k]+"/specRC_"+particleName[k]+"_"+zName[i]+".pdf").c_str());


        }
    }
}
