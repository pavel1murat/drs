//
void plot_drs_example_hist() {

  const char* fn = "event-1332364438.drs_example.hist";

  TCanvas* c0 = new_slide("c0","c0",2,2,1000,800); 

  TPad* p1 = (TPad*) c0->GetPrimitive("p1");

  p1->cd(1); plot_hist(fn,"PetAna","event_0/first_cell_0");
  p1->cd(2); plot_hist(fn,"PetAna","event_0/first_cell_1");
  p1->cd(3); plot_hist(fn,"PetAna","event_0/first_cell_2");
  p1->cd(4); plot_hist(fn,"PetAna","event_0/first_cell_3");

  c0->Print("first_cell.png");

  TCanvas* c1 = new_slide("c1","c1",2,2,1000,800); 
  p1          = (TPad*) c1->GetPrimitive("p1");

  p1->cd(1); plot_hist(fn,"PetAna","event_0/trig_pulse_0");
  p1->cd(2); plot_hist(fn,"PetAna","event_0/trig_pulse_1");
  p1->cd(3); plot_hist(fn,"PetAna","event_0/trig_pulse_2");
  p1->cd(4); plot_hist(fn,"PetAna","event_0/trig_pulse_3");

  c1->Print("c1_trig_pulse.png");
  //  c1->Print("c1_trig_pulse.eps");

  TCanvas* c2 = new_slide("c2","c2",2,2,1000,800); 

  p1 = (TPad*) c2->GetPrimitive("p1");

  p1->cd(1); plot_hist(fn,"PetAna","event_0/last_trig_pulse_0");
  p1->cd(2); plot_hist(fn,"PetAna","event_0/last_trig_pulse_1");
  p1->cd(3); plot_hist(fn,"PetAna","event_0/last_trig_pulse_2");
  p1->cd(4); plot_hist(fn,"PetAna","event_0/last_trig_pulse_3");

  c2->Print("c2_last_trig_pulse.png");
  //  c1->Print("c2_last_trig_pulse.eps");


  TCanvas* c;
  
  char name[100], fname[100], histname[100];
  int ich;

  for (int ig=0; ig<4; ig++) {

    sprintf(name,"wave_form_group_%i",ig);
    c  = new_slide(name,name,4,2,1200,800);
    p1 = (TPad*) c->GetPrimitive("p1");

    for (int i=0; i<8; i++) {
      p1->cd(i+1);
      ich = 8*ig+i;
      sprintf(histname,"event_0/wave_form_%i",ich);
      plot_hist(fn,"PetAna",histname);
    }

    sprintf(fname,"wave_form_group_%i.png",ig);
    c->Print(fname);
//     sprintf(fname,"wave_form_group_%i.eps",ig);
//     c->Print(fname);
  }

}
