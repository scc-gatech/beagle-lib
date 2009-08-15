/*
 *  BeagleCPU4StateImpl.cpp
 *  BEAGLE
 *
 * @author Andrew Rambaut
 * @author Marc Suchard
 * @author Daniel Ayres
 * @author Mark Holder
 */

#ifdef HAVE_CONFIG_H
#include "libhmsbeagle/config.h"
#endif

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <cmath>
#include <cassert> 

#include "libhmsbeagle/beagle.h"
#include "libhmsbeagle/CPU/BeagleCPUImpl.h"
#include "libhmsbeagle/CPU/BeagleCPU4StateImpl.h"

using namespace beagle;
using namespace beagle::cpu;

#if defined (BEAGLE_IMPL_DEBUGGING_OUTPUT) && BEAGLE_IMPL_DEBUGGING_OUTPUT
const bool DEBUGGING_OUTPUT = true;
#else
const bool DEBUGGING_OUTPUT = false;
#endif

BeagleCPU4StateImpl::~BeagleCPU4StateImpl() {
    // free all that stuff...
    // If you delete partials, make sure not to delete the last element
    // which is TEMP_SCRATCH_PARTIAL twice.
}

///////////////////////////////////////////////////////////////////////////////
// private methods

/*
 * Calculates partial likelihoods at a node when both children have states.
 */
void BeagleCPU4StateImpl::calcStatesStates(double* destP,
                                     const int* child1States,
                                     const double* child1TransMat,
                                     const int* child2States,
                                     const double*child2TransMat) {
    int v = 0;
    for (int l = 0; l < kCategoryCount; l++) {

        for (int k = 0; k < kPatternCount; k++) {

            int state1 = child1States[k];
            int state2 = child2States[k];

            int w = l * kMatrixSize;

            destP[v] = child1TransMat[w + state1] * child2TransMat[w + state2];
            v++;    w += 5;
            destP[v] = child1TransMat[w + state1] * child2TransMat[w + state2];
            v++;    w += 5;
            destP[v] = child1TransMat[w + state1] * child2TransMat[w + state2];
            v++;    w += 5;
            destP[v] = child1TransMat[w + state1] * child2TransMat[w + state2];
            v++;    w += 5;

        }
    }
}

/*
 * Calculates partial likelihoods at a node when one child has states and one has partials.
 */
void BeagleCPU4StateImpl::calcStatesPartials(double* destP,
                                       const int* states1,
                                       const double* matrices1,
                                       const double* partials2,
                                       const double* matrices2) {
    int u = 0;
    int v = 0;

    for (int l = 0; l < kCategoryCount; l++) {
        for (int k = 0; k < kPatternCount; k++) {

            int state1 = states1[k];

            int w = l * kMatrixSize;

            destP[u] = matrices1[w + state1];

            double sum = matrices2[w] * partials2[v]; w++;
            sum +=  matrices2[w] * partials2[v + 1]; w++;
            sum +=  matrices2[w] * partials2[v + 2]; w++;
            sum +=  matrices2[w] * partials2[v + 3]; w++;
            w++; // increment for the extra column at the end
            destP[u] *= sum;    u++;

            destP[u] = matrices1[w + state1];

            sum = matrices2[w] * partials2[v]; w++;
            sum +=  matrices2[w] * partials2[v + 1]; w++;
            sum +=  matrices2[w] * partials2[v + 2]; w++;
            sum +=  matrices2[w] * partials2[v + 3]; w++;
            w++; // increment for the extra column at the end
            destP[u] *= sum;    u++;

            destP[u] = matrices1[w + state1];

            sum = matrices2[w] * partials2[v]; w++;
            sum +=  matrices2[w] * partials2[v + 1]; w++;
            sum +=  matrices2[w] * partials2[v + 2]; w++;
            sum +=  matrices2[w] * partials2[v + 3]; w++;
            w++; // increment for the extra column at the end
            destP[u] *= sum;    u++;

            destP[u] = matrices1[w + state1];

            sum = matrices2[w] * partials2[v]; w++;
            sum +=  matrices2[w] * partials2[v + 1]; w++;
            sum +=  matrices2[w] * partials2[v + 2]; w++;
            sum +=  matrices2[w] * partials2[v + 3]; w++;
            w++; // increment for the extra column at the end
            destP[u] *= sum;    u++;

            v += 4;

        }
    }
}

void BeagleCPU4StateImpl::calcPartialsPartials(double* destP,
                                         const double* partials1,
                                         const double* matrices1,
                                         const double* partials2,
                                         const double* matrices2) {
    double sum1, sum2;
    int u = 0;
    int v = 0;

    for (int l = 0; l < kCategoryCount; l++) {
        for (int k = 0; k < kPatternCount; k++) {

            int w = l * kMatrixSize;

            sum1 = matrices1[w] * partials1[v];
            sum2 = matrices2[w] * partials2[v]; w++;
            sum1 += matrices1[w] * partials1[v + 1];
            sum2 += matrices2[w] * partials2[v + 1]; w++;
            sum1 += matrices1[w] * partials1[v + 2];
            sum2 += matrices2[w] * partials2[v + 2]; w++;
            sum1 += matrices1[w] * partials1[v + 3];
            sum2 += matrices2[w] * partials2[v + 3]; w++;
            w++; // increment for the extra column at the end
            destP[u] = sum1 * sum2; u++;

            sum1 = matrices1[w] * partials1[v];
            sum2 = matrices2[w] * partials2[v]; w++;
            sum1 += matrices1[w] * partials1[v + 1];
            sum2 += matrices2[w] * partials2[v + 1]; w++;
            sum1 += matrices1[w] * partials1[v + 2];
            sum2 += matrices2[w] * partials2[v + 2]; w++;
            sum1 += matrices1[w] * partials1[v + 3];
            sum2 += matrices2[w] * partials2[v + 3]; w++;
            w++; // increment for the extra column at the end
            destP[u] = sum1 * sum2; u++;

            sum1 = matrices1[w] * partials1[v];
            sum2 = matrices2[w] * partials2[v]; w++;
            sum1 += matrices1[w] * partials1[v + 1];
            sum2 += matrices2[w] * partials2[v + 1]; w++;
            sum1 += matrices1[w] * partials1[v + 2];
            sum2 += matrices2[w] * partials2[v + 2]; w++;
            sum1 += matrices1[w] * partials1[v + 3];
            sum2 += matrices2[w] * partials2[v + 3]; w++;
            w++; // increment for the extra column at the end
            destP[u] = sum1 * sum2; u++;

            sum1 = matrices1[w] * partials1[v];
            sum2 = matrices2[w] * partials2[v]; w++;
            sum1 += matrices1[w] * partials1[v + 1];
            sum2 += matrices2[w] * partials2[v + 1]; w++;
            sum1 += matrices1[w] * partials1[v + 2];
            sum2 += matrices2[w] * partials2[v + 2]; w++;
            sum1 += matrices1[w] * partials1[v + 3];
            sum2 += matrices2[w] * partials2[v + 3]; w++;
            w++; // increment for the extra column at the end
            destP[u] = sum1 * sum2; u++;

            v += 4;

        }
    }
}

void BeagleCPU4StateImpl::calcRootLogLikelihoods(const int bufferIndex,
                                                const double* inWeights,
                                                const double* inStateFrequencies,
                                                const int scalingFactorsIndex,
                                                double* outLogLikelihoods) {

     // We treat this as a special case so that we don't have convoluted logic
     //      at the end of the loop over patterns
     const double* rootPartials = partials[bufferIndex];
     assert(rootPartials);
     const double* wt = inWeights;
     int u = 0;
     int v = 0;
     for (int k = 0; k < kPatternCount; k++) {
        integrationTmp[v] = rootPartials[v] * wt[0]; v++;
        integrationTmp[v] = rootPartials[v] * wt[0]; v++;
        integrationTmp[v] = rootPartials[v] * wt[0]; v++;
        integrationTmp[v] = rootPartials[v] * wt[0]; v++;
     }
     for (int l = 1; l < kCategoryCount; l++) {
         u = 0;
         for (int k = 0; k < kPatternCount; k++) {
             integrationTmp[u] += rootPartials[v] * wt[l]; u++; v++;
             integrationTmp[u] += rootPartials[v] * wt[l]; u++; v++;
             integrationTmp[u] += rootPartials[v] * wt[l]; u++; v++;
             integrationTmp[u] += rootPartials[v] * wt[l]; u++; v++;
         }
     }
     u = 0;
     for (int k = 0; k < kPatternCount; k++) {
         double sum = inStateFrequencies[0] * integrationTmp[u]; u++;
         sum += inStateFrequencies[1] * integrationTmp[u]; u++;
         sum += inStateFrequencies[2] * integrationTmp[u]; u++;
         sum += inStateFrequencies[3] * integrationTmp[u]; u++;
         outLogLikelihoods[k] = log(sum);   // take the log
     }
}

///////////////////////////////////////////////////////////////////////////////
// BeagleCPUImplFactory public methods

BeagleImpl* BeagleCPU4StateImplFactory::createImpl(int tipCount,   
                                             int partialsBufferCount,
                                             int compactBufferCount,
                                             int stateCount,
                                             int patternCount,
                                             int eigenBufferCount,
                                             int matrixBufferCount,
                                             int categoryCount,
                                             int scaleBufferCount) {
	
	if (stateCount != 4) {         
		return NULL;
    }
    
   	BeagleImpl* impl = new BeagleCPU4StateImpl();                                         

	try {
        if (impl->createInstance(tipCount, partialsBufferCount, compactBufferCount, stateCount,
                                 patternCount, eigenBufferCount, matrixBufferCount,
                                 categoryCount,scaleBufferCount) == 0)
            return impl;
    }
    catch(...) {
        if (DEBUGGING_OUTPUT)
            std::cerr << "exception in initialize\n";
        delete impl;
        throw;
    }
    
    delete impl;
    
    return NULL;
}

const char* BeagleCPU4StateImplFactory::getName() {
    return "CPU-4State";
}
