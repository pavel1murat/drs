//

int plot_a() {

  c = new TCanvas;
  c -> Divide (2,2);

  c -> cd(1) ;
 char* fn = "hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_02_02_00_13000.hist";

  TH1F* h = gh1(fn,"mcrystal","evt_0/dy2");

  h->Draw();

  c -> cd(2);
  char* gn = "hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_02_02_00_31500.hist";

  TH1F* h1 = gh1(gn,"mcrystal","evt_0/dy2");

  h1->Draw();

  c -> cd(3);
  char* tn = "hist/mcrystal/mcrystal_uniform_lyso_20_20_20_sipm_02_02_00_1000.hist";

  TH1F* h2 = gh1(tn,"mcrystal","evt_0/dy2");

  h2->Draw();




  return 0;
}
