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
using namespace std;

int main()
{
    TApplication tapp("Test", 0, nullptr);

    string particleName = "p";
    string fileName = "specEN_";
    vector<string> enFiles;
    enFiles.push_back("100GeV");
    enFiles.push_back("1TeV");
    enFiles.push_back("10TeV");
    enFiles.push_back("100TeV");
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

    enHist[0]->SetLineColor(kRed);
    enHist[0]->Draw();
    enHist[0]->GetXaxis()->SetTitle("dE, MeV");
    enHist[0]->GetYaxis()->SetTitle("Number of events");

    enHist[1]->SetLineColor(kMagenta);
    enHist[1]->Draw("SAME");
    enHist[2]->SetLineColor(kBlue);
    enHist[2]->Draw("SAME");
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
    return 0;
}
