// ============================================================================
// SimGhostParticles.cpp
// Contains all routines for searching for and creating ghost particles.
// Also contains routine to correct particle positions/velocities to keep 
// them contained in simulation bounding box.
// ============================================================================


#include <cstdlib>
#include <math.h>
#include <map>
#include <string>
#include "Precision.h"
#include "Constants.h"
#include "Debug.h"
#include "Exception.h"
#include "Sph.h"
#include "SphSimulation.h"
#include "SphParticle.h"
using namespace std;


static const FLOAT ghost_range = 1.1;


//=============================================================================
//  SphSimulation::CheckBoundaries
/// Check all particles to see if any have crossed the simulation bounding 
/// box.  If so, then move the particles to their new location on the other 
/// side of the periodic box.
//=============================================================================
template <int ndim>
void Simulation<ndim>::CheckBoundaries(void)
{
  int i;                            // Particle counter
  SphParticle<ndim> *part;          // Pointer to SPH particle data


  // --------------------------------------------------------------------------
  for (i=0; i<sph->Nsph; i++) {
    part = &sph->sphdata[i];

    if (part->r[0] < simbox.boxmin[0])
      if (simbox.x_boundary_lhs == "periodic") part->r[0] += simbox.boxsize[0];
    if (part->r[0] > simbox.boxmax[0])
      if (simbox.x_boundary_rhs == "periodic") part->r[0] -= simbox.boxsize[0];

    if (ndim >= 2 && part->r[1] < simbox.boxmin[1])
      if (simbox.y_boundary_lhs == "periodic") part->r[1] += simbox.boxsize[1];
    if (ndim >= 2 && part->r[1] > simbox.boxmax[1])
      if (simbox.y_boundary_rhs == "periodic") part->r[1] -= simbox.boxsize[1];

    if (ndim == 3 && part->r[2] < simbox.boxmin[2])
      if (simbox.z_boundary_lhs == "periodic") part->r[2] += simbox.boxsize[2];
    if (ndim == 3 && part->r[2] > simbox.boxmax[2])
      if (simbox.z_boundary_rhs == "periodic") part->r[2] -= simbox.boxsize[2];

  }
  // --------------------------------------------------------------------------

  return;
}



//=============================================================================
//  SphSimulation::SearchGhostParticles
/// Search domain to create any required ghost particles near any boundaries.
/// Currently only searches to create periodic or mirror ghost particles.
//=============================================================================
template <int ndim>
void Simulation<ndim>::SearchGhostParticles(void)
{
  int i;
  int k;
  FLOAT kernrange = sph->kernp->kernrange*sph->kernfac;
  SphParticle<ndim>* sphdata = sph->sphdata;

  // Set all relevant particle counters
  sph->Nghost    = 0;
  sph->Nghostmax = sph->Nsphmax - sph->Nsph;
  sph->Ntot      = sph->Nsph;

  // If all boundaries are open, immediately return to main loop
  if (simbox.x_boundary_lhs == "open" && simbox.x_boundary_rhs == "open" &&
      simbox.y_boundary_lhs == "open" && simbox.y_boundary_rhs == "open" &&
      simbox.z_boundary_lhs == "open" && simbox.z_boundary_rhs == "open")
    return;

  debug2("[SphSimulation::SearchGhostParticles]");

  // Create ghost particles in x-dimension
  // --------------------------------------------------------------------------
  if ((simbox.x_boundary_lhs == "open" && 
       simbox.x_boundary_rhs == "open") == 0) {
    for (i=0; i<sph->Ntot; i++) {
      if (sphdata[i].r[0] < simbox.boxmin[0] + 
	  ghost_range*kernrange*sphdata[i].h) {
	if (simbox.x_boundary_lhs == "periodic")
	  CreateGhostParticle(i,0,sphdata[i].r[0] + simbox.boxsize[0],
			      sphdata[i].v[0],
			      sphdata[i].r[0] - simbox.boxmin[0]);
	if (simbox.x_boundary_lhs == "mirror")
	  CreateGhostParticle(i,0,2.0*simbox.boxmin[0] - 
			      sphdata[i].r[0],-sphdata[i].v[0],
			      sphdata[i].r[0] - simbox.boxmin[0]);
      }
      if (sphdata[i].r[0] > simbox.boxmax[0] - 
	  ghost_range*kernrange*sphdata[i].h) {
	if (simbox.x_boundary_rhs == "periodic")
	  CreateGhostParticle(i,0,sphdata[i].r[0] - simbox.boxsize[0],
			      sphdata[i].v[0],
			      simbox.boxmax[0] - sphdata[i].r[0]);
	if (simbox.x_boundary_rhs == "mirror")
	  CreateGhostParticle(i,0,2.0*simbox.boxmax[0] - 
			      sphdata[i].r[0],-sphdata[i].v[0],
			      simbox.boxmax[0] - sphdata[i].r[0]);
      }
    }
    sph->Ntot = sph->Nsph + sph->Nghost;
  }


  // Create ghost particles in y-dimension
  // --------------------------------------------------------------------------
  if (ndim >= 2 && (simbox.y_boundary_lhs == "open" && 
		    simbox.y_boundary_rhs == "open") == 0) {
    for (i=0; i<sph->Ntot; i++) {
      if (sphdata[i].r[1] < simbox.boxmin[1] + 
	  ghost_range*kernrange*sphdata[i].h) {
	if (simbox.y_boundary_lhs == "periodic")
	  CreateGhostParticle(i,1,sphdata[i].r[1] + simbox.boxsize[1],
			      sphdata[i].v[1],
			      sphdata[i].r[1] - simbox.boxmin[1]);
	if (simbox.y_boundary_lhs == "mirror")
	  CreateGhostParticle(i,1,2.0*simbox.boxmin[1] - 
			      sphdata[i].r[1],-sphdata[i].v[1],
			      sphdata[i].r[1] - simbox.boxmin[1]);
      }
      if (sphdata[i].r[1] > simbox.boxmax[1] - 
	  ghost_range*kernrange*sphdata[i].h) {
	if (simbox.y_boundary_rhs == "periodic")
	  CreateGhostParticle(i,1,sphdata[i].r[1] - simbox.boxsize[1],
			      sphdata[i].v[1],
			      simbox.boxmax[1] - sphdata[i].r[1]);
	if (simbox.y_boundary_rhs == "mirror")
	  CreateGhostParticle(i,1,2.0*simbox.boxmax[1] - 
			      sphdata[i].r[1],-sphdata[i].v[1],
			      simbox.boxmax[1] - sphdata[i].r[1]);
      }
    }
    sph->Ntot = sph->Nsph + sph->Nghost;
  }


  // Create ghost particles in z-dimension
  // --------------------------------------------------------------------------
  if (ndim == 3 && (simbox.z_boundary_lhs == "open" && 
		    simbox.z_boundary_rhs == "open") == 0) {
    for (i=0; i<sph->Ntot; i++) {
      if (sphdata[i].r[2] < simbox.boxmin[2] + 
	  ghost_range*kernrange*sphdata[i].h) {
	if (simbox.z_boundary_lhs == "periodic")
	  CreateGhostParticle(i,2,sphdata[i].r[2] + simbox.boxsize[2],
			      sphdata[i].v[2],
			      sphdata[i].r[2] - simbox.boxmin[2]);
	if (simbox.z_boundary_lhs == "mirror")
	  CreateGhostParticle(i,2,2.0*simbox.boxmin[2] - 
			      sphdata[i].r[2],-sphdata[i].v[2],
			      sphdata[i].r[2] - simbox.boxmin[2]);
      }
      if (sphdata[i].r[2] > simbox.boxmax[2] - 
	  ghost_range*kernrange*sphdata[i].h) {
	if (simbox.z_boundary_rhs == "periodic")
	  CreateGhostParticle(i,2,sphdata[i].r[2] - simbox.boxsize[2],
			      sphdata[i].v[2],
			      simbox.boxmax[2] - sphdata[i].r[2]);
	if (simbox.z_boundary_rhs == "mirror")
	  CreateGhostParticle(i,2,2.0*simbox.boxmax[2] - 
			      sphdata[i].r[2],-sphdata[i].v[2],
			      simbox.boxmax[2] - sphdata[i].r[2]);
      }
    }
    sph->Ntot = sph->Nsph + sph->Nghost;
  }

  // Quit here if we've run out of memory for ghosts
  if (sph->Ntot > sph->Nsphmax) {
    string message="Not enough memory for ghost particles";
    ExceptionHandler::getIstance().raise(message);
  }

  return;
}



//=============================================================================
//  SphSimulation::CreateGhostParticle
/// Create a new ghost particle from either 
/// (i) a real SPH particle (i < Nsph), or 
/// (ii) an existing ghost particle (i >= Nsph).
//=============================================================================
template <int ndim>
void Simulation<ndim>::CreateGhostParticle
(int i,                             ///< [in] i.d. of original particle
 int k,                             ///< [in] Boundary dimension for new ghost
 FLOAT rk,                          ///< [in] k-position of original particle
 FLOAT vk,                          ///< [in] k-velocity of original particle
 FLOAT bdist)                       ///< (Feature removed; to be deleted)
{
  // Increase ghost counter and check there's enough space in memory
  if (sph->Nghost > sph->Nghostmax) {
    string message= "Not enough memory for new ghost";
    ExceptionHandler::getIstance().raise(message);
  }

  // If there's enough memory, create ghost particle in arrays
  sph->sphdata[sph->Nsph + sph->Nghost] = sph->sphdata[i];
  sph->sphdata[sph->Nsph + sph->Nghost].r[k] = rk;
  sph->sphdata[sph->Nsph + sph->Nghost].v[k] = vk;
  sph->sphdata[sph->Nsph + sph->Nghost].active = false;

  // Record id of original particle for later copying
  if (i >= sph->Nsph)
    sph->sphdata[sph->Nsph + sph->Nghost].iorig = sph->sphdata[i].iorig;
  else
    sph->sphdata[sph->Nsph + sph->Nghost].iorig = i;

  sph->Nghost = sph->Nghost + 1;

  return;
}



//=============================================================================
//  SphSimulation::CopySphDataToGhosts
/// Copy any newly calculated data from original SPH particles to ghosts.
//=============================================================================
template <int ndim>
void Simulation<ndim>::CopySphDataToGhosts(void)
{
  int i;                            // Particle id
  int iorig;                        // Original (real) particle id
  int j;                            // Ghost particle counter
  int k;                            // Dimension counter
  FLOAT rp[ndim];                   // Particle position
  FLOAT vp[ndim];                   // Particle velocity

  debug2("[SphSimulation::CopySphDataToGhosts]");

  // --------------------------------------------------------------------------
#pragma omp parallel for default(shared) private(i,iorig,k,rp,vp)
  for (j=0; j<sph->Nghost; j++) {
    i = sph->Nsph + j;
    iorig = sph->sphdata[i].iorig;

    for (k=0; k<ndim; k++) rp[k] = sph->sphdata[i].r[k];
    for (k=0; k<ndim; k++) vp[k] = sph->sphdata[i].v[k];
    
    sph->sphdata[i] = sph->sphdata[iorig];
    sph->sphdata[i].iorig = iorig;
    sph->sphdata[i].active = false;
    for (k=0; k<ndim; k++) sph->sphdata[i].r[k] = rp[k];
    for (k=0; k<ndim; k++) sph->sphdata[i].v[k] = vp[k];
    
  }
  // --------------------------------------------------------------------------

  return;
}



//=============================================================================
//  SphSimulation::CopyAccelerationsFromGhosts
/// Copy partial acceleration from ghosts to original particles.
/// (N.B. Not needed anymore; may be deleted in future).
//=============================================================================
template <int ndim>
void Simulation<ndim>::CopyAccelerationFromGhosts(void)
{
  int i;                            // Particle id
  int iorig;                        // Original particle id
  int j;                            // Ghost particle counter
  int k;                            // Dimension counter

  debug2("[SphSimulation::CopyAccelerationFromGhosts]");

  // --------------------------------------------------------------------------
#pragma omp parallel for default(shared) private(i,iorig,k)
  for (j=0; j<sph->Nghost; j++) {
    i = sph->Nsph + j;
    iorig = sph->sphdata[i].iorig;
    
    // Only look at active ghosts
    if (!sph->sphdata[iorig].active) continue;
    
    for (k=0; k<ndim; k++) {
#pragma omp atomic
      sph->sphdata[iorig].a[k] += sph->sphdata[i].a[k];
    }
#pragma omp atomic
    sph->sphdata[iorig].dudt += sph->sphdata[i].dudt;
#pragma omp atomic
    sph->sphdata[iorig].div_v += sph->sphdata[i].div_v;
    
  }
  // --------------------------------------------------------------------------

  return;
}
