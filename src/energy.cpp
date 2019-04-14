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
#include <TPaveLabel.h>
using namespace std;


string FWHM(TH1D* &hist)
{
    int bin1 = hist->FindFirstBinAbove(hist->GetMaximum()/2);
    int bin2 = hist->FindLastBinAbove(hist->GetMaximum()/2);
    string f = to_string( hist->GetBinCenter(bin2) - hist->GetBinCenter(bin1));
    return  f.substr(0,4);
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
            float de=0;
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
        float de=0;
        float primEn = 0;
        multimap<float,float> energyPair;
        while(file.read((char*)&primEn,sizeof (primEn)))
        {
            file.read((char*)&de, sizeof (de));
            energyPair.insert(pair<float,float>(de,primEn));
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
            enHist[i] = new TH1D("value", ("Energy distribution " + particleName[k]).c_str() ,  100,2,9);
        }
        for(unsigned int i = 0; i < enName.size(); i++)
        {
            auto enVec = specData.at(particleName[k]);
            for(auto el : enVec[i])
                enHist[i]->Fill(log10(el));

        }

//        for(unsigned int j = 0; j < enName.size(); j++)
//        {
//            for (int i = 1;i <= 100;i++)
//            {
//                int sum = enHist[j]->GetBinContent(i);
//                double cen = enHist[j]->GetBinCenter(i);
//                if( k == 0 || j == 3)
//                    enHist[j]->SetBinContent(i,sum/10000./cen);
//                else
//                    enHist[j]->SetBinContent(i,sum/15000./cen);
//            }
//        }

        TCanvas c ("test", "test");

        enHist[0]->SetLineColor(kBlack);
        enHist[0]->Draw();
        enHist[0]->GetXaxis()->SetTitle("lg E_{ex} , lg MeV");


        enHist[0]->GetYaxis()->SetTitle("NOE");

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
        gStyle->SetOptFit(11);

        c.SetGridy();
        //c.SetLogx();
        //c.SetLogy();
        c.Print(("/home/xayc/CERN/data/graph/"+particleName[k]+"/specEN_"+particleName[k]+".pdf").c_str());
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
        c.SetGrid();
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
        double avg_alphaN = avgAlpha;
        double avg_alphaSp = avgAlpha;

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
                        alpha = log10(de*avg_alphaN)*p1+p0;
                        vecAlpha.push_back(alpha);
                    }
                }
                avg_alphaN = accumulate(vecAlpha.begin(),vecAlpha.end(),0.0)/vecAlpha.size();
                if(j==1)
                {
                    for(auto &vecde : it->second)
                    {
                        for(auto &de : vecde)
                            vecEn.push_back(de*avg_alphaN);
                        vecEnNew.push_back(vecEn);
                        vecEn.clear();

                    }
                }
            }
            if(itsp!=specSpData.end())
            {
                vector<double> vecAlphaSp;
                double alphaSp;
                for(auto &vecdesp : itsp->second)
                {

                    alphaSp = log10(vecdesp.first*avg_alphaSp)*p1+p0;
                    vecAlphaSp.push_back(alphaSp);

                }
                avg_alphaSp = accumulate(vecAlphaSp.begin(),vecAlphaSp.end(),0.0)/vecAlphaSp.size();
                if(j==1)
                {
                    for(auto &vecde : itsp->second)
                    {
                        vecEnSp.insert(pair<double,double>(vecde.first*avg_alphaSp,vecde.second)); /// second???
                    }
                }
            }
        }
        TH1D *enHist[enName.size()];
        for (size_t i = 0; i < enName.size(); i++)
        {
            vector<double> results;
            enHist[i] = new TH1D("value", ("Energy spectra for E_{2} " + particleName[k] +" "+ enName[i]).c_str() ,  50,3,i+6);
            for(auto &en : vecEnNew[i])
                enHist[i]->Fill(log10(en));
            enHist[i]->GetXaxis()->SetTitle("lg E_{2}, lg MeV ");
            enHist[i]->GetYaxis()->SetTitle("NOE ");
            enHist[i]->Draw();
            TPaveText *pt = new TPaveText(0.2,0.7,0.4,0.85,"NDC");
            pt->SetTextSize(0.04);
            pt->SetFillColor(0);
            pt->SetTextAlign(12);
            pt->AddText(("FWHM = " + FWHM(enHist[i])).c_str());
            pt->AddText(("#sigma = "+ to_string(stod(FWHM(enHist[i]))/2.35)).c_str());
            pt->Draw();
            c.SetGridx();
            c.Print(("/home/xayc/CERN/data/graph/"+particleName[k]+"/specEN2_"+particleName[k]+"_"+enName[i]+".pdf").c_str());
            int maxbin = enHist[i]->GetMaximumBin();
            double max = enHist[i]->GetBinCenter(maxbin);
            results.push_back(max);
            results.push_back(stod(FWHM(enHist[i])));
            results.push_back(stod(FWHM(enHist[i]))/2.35*2.3*100);
            result_EnergyTable.push_back(results);
        }
        c.Clear();
        TH1D *sumHist = new TH1D("value", ("Energy spectra for E_{2} " + particleName[k]).c_str() ,  90,3,9);
        for(int i = 0; i < 4; i++)
        for(auto &en : vecEnNew[i])
            sumHist->Fill(log10(en));
        sumHist->GetXaxis()->SetTitle("lg E_{2}, lg MeV ");
        sumHist->GetYaxis()->SetTitle("NOE ");
        sumHist->Draw();

        c.SetGridx();
        c.Print(("/home/xayc/CERN/data/graph/"+particleName[k]+"/specEN2_"+particleName[k]+".pdf").c_str());

        TH1D *enHist2 = new TH1D("value", ("Energy spectra for " + particleName[k]).c_str() ,  100,2,9);
        for(auto &en : vecEnSp)
        {
            enHist2->Fill(log10(en.first),pow(en.second,-1.6));
        }
        TH1D *enHist3 = new TH1D("value", ("Energy spectra  " + particleName[k]).c_str() ,  100,2,9);
        for(auto &en : specSpData[particleName[k]])
        {
            enHist3->Fill(log10(en.first),pow(en.second,-1.6));
        }
        c.Clear();

        enHist2->SetLineColor(kBlack);
        gROOT->SetStyle("Plain");
        enHist2->SetMarkerStyle(8);
        enHist2->Draw("HIST");
        enHist2->GetXaxis()->SetTitle("lg E_{ex} , lg MeV");
        enHist2->GetYaxis()->SetTitle("NOE with W = E^{- #gamma}");

        enHist3->SetLineStyle(2);
        enHist3->Draw("HIST SAME");
        TLegend *legend = new TLegend(0.65,0.7,0.999,0.999);
        legend->AddEntry(enHist2,"Reconstructed energy spectrum");
        legend->AddEntry(enHist3,"Simulated energy spectrum");
        legend->Draw();
        c.SetLogy();
        c.SetGridx();
        c.Print(("/home/xayc/CERN/data/graph/"+particleName[k]+"/specEN_"+particleName[k]+"_sp.pdf").c_str());

    }
}



void RootProccessing::ZRCSpectrum(string path)
{
    path = path + "specZRC_";
    vector<string> zName = enName;
    zName.push_back("sp");
    TH1D *pheHist = new TH1D("value","Z distribution of He and p 100 TeV" ,  80,0.5,3);

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
                if(zName[i] == "100TeV" && (particleName[k]=="he"|| particleName[k]=='p'))
                    pheHist->Fill(zwn);
                zHist->Fill(zwn);
                rcHist->Fill(rc/silicMIP);
            }
            file.close();
            TCanvas c ( "test", "test" );
            zHist->GetXaxis()->SetTitle("Z");
            zHist->GetYaxis()->SetTitle("NOE ");
            zHist->Draw();
            TPaveText *pt = new TPaveText(0.2,0.7,0.4,0.85,"NDC");
            pt->SetTextSize(0.04);
            pt->SetFillColor(0);
            pt->SetTextAlign(12);
            pt->AddText(("FWHM = " + FWHM(zHist)).c_str());
            pt->AddText(("#sigma = "+ to_string(stod(FWHM(zHist))/2.35)).c_str());

            pt->Draw();
            c.SetGridx();
            c.Print(("/home/xayc/CERN/data/graph/"+particleName[k]+"/specZ_"+particleName[k]+"_"+zName[i]+".pdf").c_str());
            rcHist->GetXaxis()->SetTitle("RC, MIP");
            rcHist->GetYaxis()->SetTitle("NOE ");
            rcHist->Draw();
            pt = new TPaveText(0.6,0.6,0.8,0.75,"NDC");
            pt->AddText(("FWHM = " + FWHM(rcHist)).c_str());
            pt->AddText(("#sigma = "+ to_string(stod(FWHM(rcHist))/2.35)).c_str());
            pt->Draw();
            c.SetGrid();
            c.Print(("/home/xayc/CERN/data/graph/"+particleName[k]+"/specRC_"+particleName[k]+"_"+zName[i]+".pdf").c_str());
            if( zName[i]!="sp")
            {
                vector<double> results;
                int maxbin = zHist->GetMaximumBin();
                double max = zHist->GetBinCenter(maxbin);
                results.push_back(max);
                results.push_back(stod(FWHM(zHist)));
                results.push_back(stod(FWHM(zHist))/2.35);
                maxbin = rcHist->GetMaximumBin();
                max = rcHist->GetBinCenter(maxbin);
                results.push_back(max);
                results.push_back(stod(FWHM(rcHist)));
                results.push_back(stod(FWHM(rcHist))/2.35);
                result_ZRCTable.push_back(results);
            }
        }
    }
    TCanvas c ( "test", "test" );
    pheHist->GetXaxis()->SetTitle("Z");
    pheHist->GetYaxis()->SetTitle("NOE ");
    pheHist->Draw();
    c.SetGrid();
    c.Print("/home/xayc/CERN/data/graph/specZ_he_p.pdf");


}

RootProccessing::~RootProccessing()
{
    ofstream file("/home/xayc/CERN/data/table.dat");
    for(size_t i = 0; i < 4 ; i ++)
    {
        for(int j = 0 ; j < 4; j++)
        {
            file << j+5 <<"&"<<'\t';
            for(auto &n : result_EnergyTable[j+i*4])
            {
                file << fixed << setprecision(2)<< n <<'&'<< '\t';
            }
            for(auto &n : result_ZRCTable[j+i*4])
            {
                file << fixed << setprecision(2)<<n <<'&'<< '\t';
            }
            file<<"\\\\"<<endl;
        }
        file<<endl;
    }
    file.close();
    result_EnergyTable.clear();
    result_ZRCTable.clear();
}
