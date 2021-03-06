// This code will help verify if the polarization found in lambda frame should be below the mixed frame.

// The detector has a bias that preferentially observes negative (or positive) hellicity lambdas. This bias may influence the results of boosting to the lambda frame due to the preference of the boost being related to the lambda which is only observed through the pions. Isaac can average this effect and have no bias, but once we boost, we are then saying these are the lambdas but these lambdas ????...... ask isaac to be sure. 


// Either way this code checks this detector effect by generating a random distrubition of lambda particles in their own rest frame, boosting them to the lab frame and making a cut on the pions by excepting only those with greater than 0.2 GeV momentum perpendicular to the beam direction?? ask Isaac.

// Finally, the beta direction is generated as  

// This code will help verify if the polarization found in lambda frame should be below the mixed frame.

// 7/28/16 to add histogram of dn dphi and check random values.
// Also need to add to isaac's code the check of <L> and  <L'>

#include <TLorentzVector.h>
#include <TVector3.h>
#include <TRandom.h>
#include <TProfile.h>
#include <TStopwatch.h>
#include <iostream> 
#include <TCanvas.h>
#include <TH1F.h>
#include <fstream>
using namespace std;
// lorentz vector for angular momentum direction should be global and constant in lab frame
const TLorentzVector angular(0.0,1.0, 0.0, 0.0);
// function for determining the dot product of spin in lambda frame and angular momentum direction in lab frame. 
float isaac (TVector3 *mom_proton); // Pass the 3 vector, construct the spin 4 vector, dot it with the angular momentum
// function for determining the dot product of the spin in lambda frame and angular momentum in lambda frame.
float lambda (TVector3 *mom_proton, TVector3 *angular_boost);	// Pass it the 3 vector, construct the spin 4 vector, boost the angular momentum to the lambda frame and dot them
// function for determining the dot product of spin in lab frame and angular momentum direction in lab frame
float lab (TVector3 *mom_proton, TVector3 *b_b);	// Pass it the 3 vector, construct the spin 4 vector, boost it to lab frame and dot it
// function for boosting angular momentum direction is separate so that running average of l hat prime can be found.
TVector3 lboost (TVector3 *b_b); // function boosts and returns Lprime
// function for filling and calculating all values not in other fucntions above.
void AnalyzePions_logging_2 ()
{
	ofstream log;
	log.open("phi.dat");
	log << " lhat dot lprime      lprime.y     lprime.x      lprime.z " << endl;
// timer for determining efficiency
	TStopwatch timer;
	timer.Start();
// running sum vector for determining average L hat prime versus L hat
	TVector3 Lhatprime(0.0,0.0,0.0);
// number of lambdas
	const int i_max = 5000;
// declare pi
	float pi = 3.14159265358979323846;
// Default profiles for simulation
	TProfile *pols = new TProfile ("polarization", "polarization", 2, 0, 2);
	TProfile *pols_1 = new TProfile ("polarization without cuts", "polarization without cuts", 2, 0, 2);
	TH1F *dndphi = new TH1F ("hits per phi", "hits per phi", 2000, 0 , 2.*pi);
// profiles for pols versus phi for each histogram, before and after cut.
	TProfile *pols_phi_isaac = new TProfile ("polarization versus phi isaac", "polarization versus phi isaac", 2000, 0, 2.*pi);
	TProfile *pols_phi_lambda = new TProfile ("polarization versus phi lambda", "polarization versus phi lambda", 2000, 0, 2.*pi);
	TProfile *pols_phi_lab = new TProfile ("polarization versus phi lab", "polarization versus phi lab", 2000, 0, 2.*pi);
	TProfile *pols_phi_isaac_cuts = new TProfile ("polarization versus phi isaac cuts", "polarization versus phi isaac cuts", 2000, 0, 2.*pi);
	TProfile *pols_phi_lambda_cuts = new TProfile ("polarization versus phi lambda cuts", "polarization versus phi lambda cuts", 2000, 0, 2.*pi);
	TProfile *pols_phi_lab_cuts = new TProfile ("polarization versus phi lab cuts", "polarization versus phi lab cuts", 2000, 0, 2.*pi);
// parameter for lambda boost (laab frame lambda perp velocity).
	float beta = 0.85;
// random generator for momentum of lambda daughters and lambda velocity. 
	TRandom *rand = new TRandom(0);
// amount of momentum of proton and pion in lambda rest frame
	float proton_decay_param = 0.101; //PDG value
	float proton_mass =0.94; //GeV	
	float pion_mass = 0.14;
// ints for counting to log code
	int neg_pions = 0;
	int neg_pions_cut = 0;
// canvas for savings pdfs'
	TCanvas *c1 = new TCanvas("Rapidity practice","multipads",900,700);	
// to edit for each lambda need a  dn dphi (easy)
// to edit for L prime hat somehow add in an average dot product.
	for(int i = 0; i < i_max; i++)
	{

		for(float phi = 0.0; phi < 2.*pi; phi += pi/1000.)
		{
			//log << phi << endl;
			dndphi->Fill(phi);
			float PhiLam= rand->Uniform(0,2*pi);
			float ThetaLam = rand->Uniform(-1, 1);
			float SThetaLam = TMath::Sqrt(1-ThetaLam*ThetaLam);
			// Lambda frame first
			TVector3 mom_proton(proton_decay_param*ThetaLam*TMath::Cos(PhiLam), proton_decay_param*SThetaLam*TMath::Sin(PhiLam), proton_decay_param*ThetaLam);
			
			TVector3 mom_pion(-1*(mom_proton));
			TVector3 b_b(beta*TMath::Cos(phi), beta*TMath::Sin(phi), 0.0);
			
			TLorentzVector mom_pion_four(mom_pion, TMath::Sqrt(pion_mass*pion_mass+mom_pion.Mag2()));
			
			mom_pion_four.Boost(b_b);	// lab frame now
			

			TVector3 angular_boost(lboost(&b_b));

			Lhatprime+= angular_boost;


//			pols_1->Fill(0.5,isaac(&mom_proton));
//			pols_1->Fill(1.5,lambda(&mom_proton, &angular_boost));
//			pols_1->Fill(2.5,lab(&mom_proton, &b_b));
			pols_1->Fill(0.01,isaac(&mom_proton));
			pols_1->Fill(1.01,lambda(&mom_proton, &angular_boost));
			pols_phi_isaac->Fill(phi, isaac(&mom_proton));
			pols_phi_lambda->Fill(phi, lambda(&mom_proton, &angular_boost));
			pols_phi_lab->Fill(phi, lab(&mom_proton, &b_b));

			if (mom_pion_four.Perp() < 0.2) 
			{
				neg_pions_cut+=1;
				
				continue;
			}
			neg_pions+=1;
			pols_phi_isaac_cuts->Fill(phi, isaac(&mom_proton));
			pols_phi_lambda_cuts->Fill(phi,lambda(&mom_proton, &angular_boost));
			pols_phi_lab_cuts->Fill(phi, lab(&mom_proton, &b_b));
			
//			pols->Fill(0.5,isaac(&mom_proton));
//			pols->Fill(1.5,lambda(&mom_proton, &angular_boost));
//			pols->Fill(2.5,lab(&mom_proton, &b_b));
			pols->Fill(0.01,isaac(&mom_proton));
			pols->Fill(1.01,lambda(&mom_proton, &angular_boost));


		}
		if (i%1000 == 0 )
		{
			cout << (float)(i)/(float)(i_max)*100. << "%" << endl;
		}
	}

	log << neg_pions << " pions not cut " << (timer.RealTime())/(float)(neg_pions) << " ms/pion" << endl;
	pols->Draw();
	c1->SaveAs("polarization_dependence_beta_0.85_cut_0.2.pdf");
	pols_1->Draw();
	c1->SaveAs("polarization_dependence_beta_0.85.pdf");
	dndphi->Draw();
	c1->SaveAs("dndphi_AnalyzePions.pdf");
	pols_phi_isaac->Draw();
	c1->SaveAs("isaac_pols_vs_phi.pdf");
	pols_phi_isaac_cuts->Draw();
	c1->SaveAs("isaac_pols_vs_phi_cuts.pdf");
	pols_phi_lambda->Draw();
	c1->SaveAs("lambda_pols_vs_phi.pdf");
	pols_phi_lambda_cuts->Draw();
	c1->SaveAs("lambda_pols_vs_phi_cuts.pdf");
	pols_phi_lab->Draw();
	c1->SaveAs("lab_pols_vs_phi.pdf");
	pols_phi_lab_cuts->Draw();
	c1->SaveAs("lab_pols_vs_phi_cuts.pdf");


//	cout << (angular.Vect()).Dot(Lhatprime)/(Lhatprime.Mag()) << endl;
	log << (angular.Vect()).Dot(Lhatprime)/(Lhatprime.Mag()) << " " << Lhatprime.Y() << " " << Lhatprime.X() << " " << Lhatprime.Z() << endl;
	log.close();
	
}	


float isaac (TVector3 *mom_proton)	// pass the momentum 3 in lambda frame and then construct the spin 4 vector?
{
	float polarization = 0.0;
	TVector3 spin(0.5*mom_proton->Unit());
	polarization = (angular.Vect()).Dot(spin)/((angular.Vect()).Mag()*spin.Mag());
	return polarization;
}


float lambda (TVector3 *mom_proton, TVector3 *angular_boost) // both are in lab frame, need to boost both?
{
	float polarization = 0.0;
	TVector3 spin(0.5*mom_proton->Unit());
//	TVector3 angular(0.0,1.0, 0.0);
//	TLorentzVector angular_four(angular, 0.0);
//	angular_four.Boost(-1*(*b_b));
	polarization = spin.Dot(*angular_boost)/((angular_boost->Mag())*spin.Mag());
	return polarization;
}

float lab (TVector3 *mom_proton, TVector3 *b_b) // both are in lab frame, boost spin to lab frame, construct the spin 4-vector, boost back to lab frame, dot product
{
	float polarization = 0.0;
	TLorentzVector spin_four(0.5*mom_proton->Unit(), 0.0);
	spin_four.Boost(*b_b);
	TVector3 spin(spin_four.Vect());
//	TVector3 angular(0.0,1.0, 0.0);
	polarization = spin.Dot(angular.Vect())/(spin.Mag()*((angular.Vect()).Mag()));
	return polarization;
}

TVector3 lboost(TVector3 *b_b)
{
	TLorentzVector angular_2(angular);
	angular_2.Boost(*b_b);
	return(angular_2.Vect());
}
