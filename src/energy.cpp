#include <energy.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <numeric>

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

        ifstream file((path+particleName[k]+"_sp.dat").c_str(), ifstream::binary);
        if (!file.is_open())
        {
            cout << "file can not be opened, or it does not exist " << endl;
            exit(EXIT_FAILURE);
        }
        double de=0;
        float primEn = 0;
        multimap<double,double> energyPair;
        while(file.read((char*)&primEn,sizeof (primEn)))
        {
            file.read((char*)&de, sizeof (de));
            energyPair.insert(pair<double,double>(de,primEn));
        }
        specSpData[particleName[k]]=energyPair;

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
        enHist[0]->GetXaxis()->SetTitle("lg E_{ex} , lg MeV");


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
        gROOT->SetStyle("Plain");

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
        gr->GetXaxis()->SetTitle("lg E_{ex}, lg MeV ");
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

    for(size_t k = 0; k < particleName.size() ; k++)
    {
        double avgAlpha=0;

        vector<double> coef;
        auto &vecdE = specData.at(particleName[k]);
        for(int i = 0; i < 4; i++)
        {
            double d1=0;
            double d2=0;

            for(auto &de : vecdE[i])
            {
                d1 += pow(de/enPrimary[i],2.6);
                d2 += pow(de/enPrimary[i],1.6);

            }
            coef.push_back(d2/d1);

        }

        avgAlpha = accumulate(coef.begin(),coef.end(),0.0)/coef.size();

        TGraph *gr = new TGraph (coef.size(), logEn.data(), coef.data());
        TCanvas c ("test", "test");
        //c.SetLogx();
        c.SetGridy();
        gROOT->SetStyle("Plain");
        gr->SetMarkerStyle(8);
        gr->SetTitle(("Artificial coefficient "+particleName[k]).c_str());
        gr->GetXaxis()->SetTitle("lg E, lg MeV");
        gr->GetYaxis()->SetTitle("#alpha");
        gr->Fit("pol1");
        gStyle->SetOptFit(11);
        gr->Draw("AP");
        c.Print(("~/CERN/data/graph/"+particleName[k]+"/coef_"+particleName[k] +".pdf").c_str());
        coef.clear();
        double p0,p1;
        p0 = gr->GetFunction("pol1")->GetParameter("p0");
        p1 = gr->GetFunction("pol1")->GetParameter("p1");

        vector<vector<double>> vecEnNew;
        vector<double> vecEn;
        map<double,double> vecEnSp;
        for(int j=0; j<2; j++)
        {
            auto it = specData.find(particleName[k]);
            auto itsp = specSpData.find(particleName[k]);
            if(it!=specData.end())
            {
                vector<double> vecAlpha;
                double alpha;
                for(auto &vecde : it->second)
                {
                    for(auto &de : vecde)
                    {
                        alpha = log10(de*avgAlpha)*p1+p0;
                        vecAlpha.push_back(alpha);
                    }
                }
                avgAlpha = accumulate(vecAlpha.begin(),vecAlpha.end(),0.0)/vecAlpha.size();
                if(j==1)
                {
                    for(auto &vecde : it->second)
                    {
                        for(auto &de : vecde)
                            vecEn.push_back(de*avgAlpha);
                    }
                    vecEnNew.push_back(vecEn);
                }
            }
            vector<double> vecAlphaSp;
            double alphaSp;
            for(auto &vecdesp : itsp->second)
            {

                alphaSp = log10(vecdesp.first*avgAlpha)*p1+p0;
                vecAlphaSp.push_back(alphaSp);

            }
            avgAlpha = accumulate(vecAlphaSp.begin(),vecAlphaSp.end(),0.0)/vecAlphaSp.size();
            if(j==1)
            {
                for(auto &vecde : itsp->second)
                {
                    vecEnSp.insert(pair<double,double>(vecde.first*avgAlpha,vecde.second));
                }
            }
        }
        TH1D *enHist = new TH1D("value", ("Energy distribution E_{2} " + particleName[k]).c_str() ,  70,2,9);
        for(auto &vecen : vecEnNew)
        {
            for(auto &en : vecen)
                enHist->Fill(log10(en));
        }
        TCanvas c1("test");
        enHist->GetXaxis()->SetTitle("lg E, lg MeV ");
        enHist->GetYaxis()->SetTitle("NOE ");
        enHist->Draw();

        c1.SetGridx();
        c1.Print(("/home/xayc/CERN/data/graph/"+particleName[k]+"/specEN2_"+particleName[k]+".pdf").c_str());
        TH1D *enHist2 = new TH1D("value", ("Energy distribution E_{sp2} " + particleName[k]).c_str() ,  70,2,9);
        for(auto &en : vecEnSp)
        {
            enHist2->Fill(log10(en.first),pow(en.second,-1.6));
        }
        TH1D *enHist3 = new TH1D("value", ("Energy distribution of uniform " + particleName[k]).c_str() ,  100,2,8);
        for(auto &en : specSpData[particleName[k]])
        {
            enHist3->Fill(log10(en.first),pow(en.second,-1.6));
        }
        TCanvas c2 ("test", "test");

        enHist2->SetLineColor(kBlack);
        gROOT->SetStyle("Plain");
        enHist2->SetMarkerStyle(8);
        enHist2->Draw("HIST");
        enHist2->GetXaxis()->SetTitle("lg E_{ex} , lg MeV");
        enHist2->GetYaxis()->SetTitle("NOE with W = E^{- #gamma}");

        enHist3->SetLineStyle(2);
        enHist3->Draw("HIST SAME");
        TLegend *legend = new TLegend(0.78,0.7,0.98,0.95);
        legend->AddEntry(enHist2,"Distribution of E_{sp 2}");
        legend->AddEntry(enHist3,"Distribution of E_{sp}");
        legend->Draw();
        c2.SetLogy();
        c2.SetGridy();
        c2.Print(("/home/xayc/CERN/data/graph/"+particleName[k]+"/specEN_"+particleName[k]+"_sp.pdf").c_str());

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
            TH1D *rcHist;
            if( k == 0 )
            {
                zHist = new TH1D("value", ("Z distribution of " + particleName[k]+" " +zName[i]).c_str() ,  60,0,2);
                rcHist = new TH1D("value", ("RC distribution of " + particleName[k]+" " +zName[i]).c_str() ,  60,0,3);

            }
            if( k == 1)
            {
                zHist = new TH1D("value", ("Z distribution of " + particleName[k]+" " +zName[i]).c_str() ,  60,1,3);
                rcHist = new TH1D("value", ("RC distribution of " + particleName[k]+" " +zName[i]).c_str() ,  60,0,10);

            }
            if( k == 2)
            {
                zHist = new TH1D("value", ("Z distribution of " + particleName[k]+" " +zName[i]).c_str() ,  60,5,7);
                rcHist = new TH1D("value", ("RC distribution of " + particleName[k]+" " +zName[i]).c_str() ,  60,25,50);

            }
            if( k == 3)
            {
                zHist = new TH1D("value", ("Z distribution of " + particleName[k]+" " +zName[i]).c_str() ,  60,24,28);
                rcHist = new TH1D("value", ("RC distribution of " + particleName[k]+" " +zName[i]).c_str() ,  100,600,800);

            }
            double zwn = 0;
            double zwon = 0;
            double rc = 0;
            while(file.read((char*)&zwn,sizeof (zwn)))
            {
                file.read((char*)&zwon,sizeof (zwon));
                file.read((char*)&rc,sizeof (rc));
                zHist->Fill(zwn);
                //zwHist->Fill(zwon);
                rcHist->Fill(rc/silicMIP);
            }
            file.close();
            TCanvas c ( "test", "test" );
            zHist->GetXaxis()->SetTitle("Z");
            zHist->Draw();

            c.SetGridx();
            c.SaveAs(("/home/xayc/CERN/data/graph/"+particleName[k]+"/specZ_"+particleName[k]+"_"+zName[i]+".pdf").c_str());
            TCanvas c1 ( "test", "test" );
            rcHist->GetXaxis()->SetTitle("RC, MIP");
            rcHist->Draw();
            c1.SetGrid();
            c1.SaveAs(("/home/xayc/CERN/data/graph/"+particleName[k]+"/specRC_"+particleName[k]+"_"+zName[i]+".pdf").c_str());
        }
    }
}
