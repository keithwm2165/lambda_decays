// This code will help verify if the polarization found in lambda frame should be below the mixed frame.

// The detector has a bias that preferentially observes negative (or positive) hellicity lambdas. This bias may influence the results of boosting to the lambda frame due to the preference of the boost being related to the lambda which is only observed through the pions. Isaac can average this effect and have no bias, but once we boost, we are then saying these are the lambdas but these lambdas ????...... ask isaac to be sure. 


// Either way this code checks this detector effect by generating a random distrubition of lambda particles in their own rest frame, boosting them to the lab frame and making a cut on the pions by excepting only those with greater than 0.2 GeV momentum perpendicular to the beam direction?? ask Isaac.

// Finally, the beta direction is generated as  

// This code will help verify if the polarization found in lambda frame should be below the mixed frame.
#include <TLorentzVector.h>
#include <TVector3.h>
#include <TRandom.h>
#include <TProfile.h>
#include <TStopwatch.h>
#include <iostream> 
#include <TCanvas.h>
using namespace std;

float isaac (TVector3 *mom_proton); // Pass the 4 vector, boost it to the lambda frame, construct the spin 4 vector, dot it with the angular momentum
float lambda (TVector3 *mom_proton, TVector3 *b_b);	// Pass it the 4 vector, boost it to the lambda frame, construct the spin vector, boost the angular momentum to the lambda frame and dot them
float lab (TVector3 *mom_proton, TVector3 *b_b);	// Pass it the 4 vector, construct the spin vector and dot it
void Analyzepions ()
{
	TStopwatch timer;
	timer.Start();
	const int i_max = 50000;
	float pi = 3.14159265358979323846;
	TProfile *pols = new TProfile ("polarization", "polarization", 3, 0, 3);
	float beta = 0.85;
	TRandom *rand = new TRandom(0);
	float proton_decay_param = 0.101; //PDG value
	float proton_mass =0.94; //GeV	
	float pion_mass = 0.14;
	int neg_pions = 0;
	int neg_pions_cut = 0;
	TCanvas *c1 = new TCanvas("Rapidity practice","multipads",900,700);	
	for(int i = 0; i <= i_max; i++)
	{
		for(float phi = 0.0; phi < 2.*pi; phi += pi/1000.)
		{
			float PhiLam= rand->Uniform(0,2*pi);
			float ThetaLam = rand->Uniform(-1, 1);
			float SThetaLam = TMath::Sqrt(1-ThetaLam*ThetaLam);
			// Lambda frame first
			TVector3 mom_proton(proton_decay_param*ThetaLam*TMath::Cos(PhiLam), proton_decay_param*SThetaLam*TMath::Sin(PhiLam), proton_decay_param*ThetaLam);
			//TVector3 momproton(1., 2., 3.);

			//cout << momproton.x() << endl;

			TVector3 mom_pion(-1*(mom_proton));
			TVector3 b_b(beta*TMath::Cos(phi), beta*TMath::Sin(phi), 0.0);
			//cout << mom_proton.z() << endl;
			//TLorentzVector Protonfourmomentum (1., 2. ,3. ,4. );
			//			cout << "yes" << endl;
			TLorentzVector mom_pion_four(mom_pion, TMath::Sqrt(pion_mass*pion_mass+mom_pion.Mag2()));
			
			mom_pion_four.Boost(b_b);	// lab frame now
			if (mom_pion_four.Perp() < 0.2) 
			{
				neg_pions_cut+=1;
				//cout << mom_pion_four.Perp() << endl;
				continue;
			}
			neg_pions+=1;
			pols->Fill(0.5,isaac(&mom_proton));
			pols->Fill(1.5,lambda(&mom_proton, &b_b));
			pols->Fill(2.5,lab(&mom_proton, &b_b));

		}
		if (i%1000 == 0 )
		{
			cout << (float)(i)/(float)(i_max)*100. << "%" << endl;
		}
	}
	cout << neg_pions << " pions not cut " << (timer.RealTime())/(float)(neg_pions) << " ms/pion" << endl;
	pols->Draw();
	c1->SaveAs("neg_pion.pdf");
}


float isaac (TVector3 *mom_proton)	// pass the momentum 3 in lambda frame and then construct the spin 4 vector?
{
	float polarization = 0.0;
	TVector3 spin(0.5*mom_proton->Unit());
	TVector3 angular(0.6,0.8, 0.5);
	polarization = angular.Dot(spin)/(angular.Mag()*spin.Mag());
	return polarization;
}


float lambda (TVector3 *mom_proton, TVector3 *b_b) // both are in lab frame, need to boost both?
{
	float polarization = 0.0;
	TVector3 spin(0.5*mom_proton->Unit());
	TVector3 angular(0.6,0.8, 0.5);
	TLorentzVector angular_four(angular, 0.0);
	angular_four.Boost(-1*(*b_b));
	polarization = spin.Dot(angular_four.Vect())/((angular_four.Vect()).Mag()*spin.Mag());
	return polarization;
}

float lab (TVector3 *mom_proton, TVector3 *b_b) // both are in lab frame, boost spin to lab frame, construct the spin 4-vector, boost back to lab frame, dot product
{
	float polarization = 0.0;
	TLorentzVector spin_four(0.5*mom_proton->Unit(), 0.0);
	spin_four.Boost(*b_b);
	TVector3 spin(spin_four.Vect());
	TVector3 angular(0.6,0.8, 0.5);
	polarization = spin.Dot(angular)/(spin.Mag()*angular.Mag());
	return polarization;
}

