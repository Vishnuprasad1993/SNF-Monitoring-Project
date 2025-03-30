# SNF-Monitoring-Project

GEANT4 Simulation for the Antineutrino Detector

Studying positron interactions from the inverse beta decay events and 

1) PrimaryGeneratorAction
           The details regarding the particle type (e+), energy, direction, and source position.
2) PhysicsList
           Positron annihilation produce 2 gamma rays, which causes scintillation in the plastic scintillator tiles. Considered only optical photons for simulation.
3) Actioninit
4) DetectorConstruction
           Detector Components Used
               1) Plastic Scintillator Tiles 2) Wavelength Shifting (WLS) Fibers 3) Sipms
           Arranged plastic scintillator tiles as bottom layer and top layer (90 deg rotation). Each layer has 4 tiles in 2x2 manner. Developed an 8 layered detector setup by placing bottom and top layers at 
           appropriate distances in z direction. Introduced grooves in each layers, placed Fiber Core and Fiber Cladding inside the grooves, and Sipms at the end of each fibers.
5) SteppingAction
           Track optical photon hits
6) RunAction
           plot histograms



