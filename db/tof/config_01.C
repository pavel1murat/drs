//------------------------------------------------------------------------------
//  2012-04-26 P.Murat : configuration script itself
//-----------------------------------------------------------------------------
#include "drs/ana/scripts/modules.hh"


//-----------------------------------------------------------------------------
void PetAnaModule_init_run(int RunNumber) {
}


//-----------------------------------------------------------------------------
void PetRecoModule_init_run(int RunNumber) {
}



//-----------------------------------------------------------------------------
void Pet2AnaModule_init_run(int RunNumber) {
  
  m_pet2 = (TPet2AnaModule*) g.x->GetModule("Pet2Ana");

  TDrsFitAlgNew* alg = m_pet2->GetAlgorithm(0);

  TTofDetector* tof = TPetDetector::Instance()->GetTofDetector();

  if      (RunNumber == 12) {
    m_pet2->SetTimeChannel(0,0);
    m_pet2->SetTimeChannel(1,1);

    alg->SetUsedChannelNumber(0,0);
    alg->SetUsedChannelNumber(1,1);

    tof->GetData(0)->SetMinQ(50.);
    tof->GetData(1)->SetMinQ(50.);
  }
  else if (RunNumber == 13) {
    m_pet2->SetTimeChannel(0,0);
    m_pet2->SetTimeChannel(1,1);

    alg->SetUsedChannelNumber(0,0);
    alg->SetUsedChannelNumber(1,1);

    tof->GetData(0)->SetMinQ(105.);
    tof->GetData(1)->SetMinQ(105.);
  }
  else if (RunNumber == 14) {
    printf(">>> init_run: RunNumber = 14\n");
    
    m_pet2->SetTimeChannel(0,0);
    m_pet2->SetTimeChannel(1,1);

    alg->SetUsedChannelNumber(0,0);
    alg->SetUsedChannelNumber(1,1);

    tof->GetData(0)->SetMinQ(40.);
    tof->GetData(1)->SetMinQ(40.);
    printf(">>> init_run: exit RunNumber = 14\n");
  }
  else if (RunNumber == 22) {
    m_pet2->SetTimeChannel(0,0);
    m_pet2->SetTimeChannel(1,1);

    alg->SetUsedChannelNumber(0,0);
    alg->SetUsedChannelNumber(1,1);
  }
  else if (RunNumber == 23) {
    m_pet2->SetTimeChannel(0,0);
    m_pet2->SetTimeChannel(1,1);

    alg->SetUsedChannelNumber(0,0);
    alg->SetUsedChannelNumber(1,1);
  }
  else if (RunNumber == 24) {
    m_pet2->SetTimeChannel(0,0);
    m_pet2->SetTimeChannel(1,1);

    alg->SetUsedChannelNumber(0,0);
    alg->SetUsedChannelNumber(1,1);
  }
  else if (RunNumber == 25) {
    m_pet2->SetTimeChannel(0,11);
    m_pet2->SetTimeChannel(1,23);

    alg->SetUsedChannelNumber(0,11);
    alg->SetUsedChannelNumber(1,23);
  }
  else if (RunNumber == 26) {
    m_pet2->SetTimeChannel(0,11);
    m_pet2->SetTimeChannel(1,23);

    alg->SetUsedChannelNumber(0,11);
    alg->SetUsedChannelNumber(1,23);
  }
  else if (RunNumber == 27) {
    tof->GetData(0)->SetMinQ(90.);
    tof->GetData(1)->SetMinQ(95.);
  }
  else if (RunNumber == 28) {
    tof->GetData(0)->SetMinQ(90.);
    tof->GetData(1)->SetMinQ(95.);
  }
  else if (RunNumber == 29) {
    tof->GetData(0)->SetMinQ(90.);
    tof->GetData(1)->SetMinQ(95.);
  }
  else if (RunNumber == 30) {
    tof->GetData(0)->SetMinQ(90.);
    tof->GetData(1)->SetMinQ(95.);
  }
  else if (RunNumber == 31) {
    tof->GetData(0)->SetMinQ(90.);
    tof->GetData(1)->SetMinQ(95.);
  }
  else if (RunNumber == 32) {
    tof->GetData(0)->SetMinQ(90.);
    tof->GetData(1)->SetMinQ(95.);
  }
  else if (RunNumber == 33) {
    tof->GetData(0)->SetMinQ(90.);
    tof->GetData(1)->SetMinQ(95.);
  }
  else if (RunNumber == 34) {
    tof->GetData(0)->SetMinQ(90.);
    tof->GetData(1)->SetMinQ(95.);
  }
  else if (RunNumber == 99) {
    m_pet2->GetClusterFinder()->SetMinCell(20.);
    m_pet2->GetClusterFinder()->SetMaxCell(90.);
  }

  return ;
}

