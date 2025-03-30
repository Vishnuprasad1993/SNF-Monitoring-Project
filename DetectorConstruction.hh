
#ifndef G4_BREMS_DETECTOR_CONSTRUCTION_H
#define G4_BREMS_DETECTOR_CONSTRUCTION_H 1

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VSensitiveDetector.hh"
#include "SteppingAction.hh"

namespace G4_BREMS {
    class DetectorConstruction : public G4VUserDetectorConstruction {
    public:
        
        DetectorConstruction();  // Add constructor
        ~DetectorConstruction() override;  // Add destructor

        G4VPhysicalVolume* Construct() override;
        void ConstructSDandField() override;

        
        G4LogicalVolume* GetTileVolume() const { return fTileVolume; }
        G4LogicalVolume* GetFiberVolume() const { return fFiberCoreVolume; }
        G4LogicalVolume* GetCladVolume() const { return fFiberCladVolume; }
        G4LogicalVolume* GetSipmVolume() const { return fSipmVolume; }
        
        

    private:
        
        
        G4LogicalVolume* fTileVolume;
        G4LogicalVolume* fFiberCoreVolume;
        G4LogicalVolume* fFiberCladVolume;
        G4LogicalVolume* fSipmVolume;

        G4VSensitiveDetector* fTileSD;
        G4VSensitiveDetector* fFiberCoreSD;
        G4VSensitiveDetector* fFiberCladSD;
        G4VSensitiveDetector* fSipmSD;
        
        

    };
}

#endif


