#include "PhysicsList.hh"

// Standard EM Physics
#include "G4EmStandardPhysics.hh"
#include "G4LossTableManager.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"

// Different EM Physics options
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmStandardPhysicsWVI.hh"
#include "G4EmStandardPhysicsSS.hh"
#include "G4EmStandardPhysicsGS.hh"

// Optical Physics
#include "G4OpticalPhysics.hh"
#include "G4OpticalParameters.hh"

//#include "QGSP_BERT.hh"



namespace G4_BREMS
{
    PhysicsList::PhysicsList() : G4VModularPhysicsList()
    {

        
        
        G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();

        auto opticalParams = G4OpticalParameters::Instance();
        opticalParams->SetWLSTimeProfile("delta");
        opticalParams->SetProcessActivation("OpWLS", true);
        
        RegisterPhysics(opticalPhysics);



       
    }

    

    void PhysicsList::ConstructParticle()
    {
        G4VModularPhysicsList::ConstructParticle();
    }

    void PhysicsList::ConstructProcess()
    {
        G4VModularPhysicsList::ConstructProcess();
    }


}