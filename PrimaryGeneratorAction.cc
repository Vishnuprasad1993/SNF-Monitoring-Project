#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "Randomize.hh"

#include "G4UnitsTable.hh"


namespace G4_BREMS
{
	PrimaryGeneratorAction::PrimaryGeneratorAction() {
		// set up particle gun
		G4int nParticles = 1;
		fParticleGun = new G4ParticleGun(nParticles);

		// define particle properties
		//const G4String& particleName = "opticalphoton";
		const G4String& particleName = "e+";


		// Find the particle (e+) in the G4Particle table
		G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
		G4ParticleDefinition* particle
			= particleTable->FindParticle(particleName);

		// Set the particle type to the particle gun
		fParticleGun->SetParticleDefinition(particle);

		
		G4ThreeVector momentumDirection = G4ThreeVector(1, 0, 0);
		//G4ThreeVector momentumDirection = G4ThreeVector(0, 1, 0);

		//G4cout << "Particle direction: " << momentumDirection << G4endl;

		fParticleGun->SetParticleMomentumDirection(momentumDirection);

		G4double energy = 10.0 * MeV;
		//G4cout << "Particle energy: " << G4BestUnit(energy, "Energy") << G4endl;

		fParticleGun->SetParticleEnergy(energy);

	}

	PrimaryGeneratorAction::~PrimaryGeneratorAction() {
		delete fParticleGun;
	}

	void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
	{
		
		G4ThreeVector position = G4ThreeVector(-200.0 * mm, 0 * mm, 0 * mm);
		
		fParticleGun->SetParticlePosition(position);

		fParticleGun->GeneratePrimaryVertex(event);
	}
}