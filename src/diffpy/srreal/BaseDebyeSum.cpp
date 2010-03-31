/*****************************************************************************
*
* diffpy.srreal     by DANSE Diffraction group
*                   Simon J. L. Billinge
*                   (c) 2010 Trustees of the Columbia University
*                   in the City of New York.  All rights reserved.
*
* File coded by:    Pavol Juhas
*
* See AUTHORS.txt for a list of people who contributed.
* See LICENSE.txt for license information.
*
******************************************************************************
*
* class BaseDebyeSum -- base class for concrete Debye sum calculators
*
* $Id$
*
*****************************************************************************/

#include <cassert>
#include <string>
#include <stdexcept>
#include <sstream>
#include <functional>

#include <diffpy/srreal/BaseDebyeSum.hpp>
#include <diffpy/mathutils.hpp>
#include <diffpy/validators.hpp>
#include <diffpy/srreal/StructureAdapter.hpp>
#include <diffpy/srreal/BaseBondGenerator.hpp>

using namespace std;
using namespace diffpy::validators;
using diffpy::mathutils::eps_gt;
using diffpy::mathutils::eps_eq;

namespace diffpy {
namespace srreal {

// Declaration of Local Helpers ----------------------------------------------

namespace {

/// Default cutoff for the Q-decreasing scale of the sine contributions.
const double DEFAULT_DEBYE_PRECISION = 1e-6;

}   // namespace

// Constructor ---------------------------------------------------------------

BaseDebyeSum::BaseDebyeSum()
{
    // default configuration
    this->setPeakWidthModel("jeong");
    this->setQmin(0.0);
    this->setQmax(10.0);
    this->setQstep(0.05);
    this->setDebyePrecision(DEFAULT_DEBYE_PRECISION);
    // attributes
    this->registerDoubleAttribute("debyeprecision", this,
            &BaseDebyeSum::getDebyePrecision,
            &BaseDebyeSum::setDebyePrecision);
}

// Public Methods ------------------------------------------------------------

// results

QuantityType BaseDebyeSum::getF() const
{
    QuantityType rv = mvalue;
    const double& totocc = mstructure_cache.totaloccupancy;
    const int npts = this->totalQPoints();
    for (int kq = this->qminPoints(); kq < npts; ++kq)
    {
        double sfavg = this->sfAverageAtkQ(kq);
        double fscale = (sfavg * totocc) == 0 ? 0.0 :
            1.0 / (sfavg * sfavg * totocc);
        rv[kq] *= fscale;
    }
    return rv;
}


QuantityType BaseDebyeSum::getQgrid() const
{
    const int npts = this->totalQPoints();
    QuantityType rv;
    rv.reserve(npts);
    for (int kq = 0; kq < npts; ++kq)  rv.push_back(kq * this->getQstep());
    return rv;
}

// Q-range configuration

void BaseDebyeSum::setQmin(double qmin)
{
    ensureNonNegative("Qmin", qmin);
    mqmin = qmin;
    this->cacheQpointsData();
}


const double& BaseDebyeSum::getQmin() const
{
    return mqmin;
}


void BaseDebyeSum::setQmax(double qmax)
{
    ensureNonNegative("Qmax", qmax);
    mqmax = qmax;
    this->cacheQpointsData();
}


const double& BaseDebyeSum::getQmax() const
{
    return mqmax;
}


void BaseDebyeSum::setQstep(double qstep)
{
    ensureEpsilonPositive("Qstep", qstep);
    mqstep = qstep;
    this->cacheQpointsData();
}


const double& BaseDebyeSum::getQstep() const
{
    return mqstep;
}


void BaseDebyeSum::setDebyePrecision(double precision)
{
    mdebyeprecision = precision;
}


const double& BaseDebyeSum::getDebyePrecision() const
{
    return mdebyeprecision;
}

// Protected Methods ---------------------------------------------------------

// PairQuantity overloads

void BaseDebyeSum::resetValue()
{
    this->cacheQpointsData();
    this->cacheStructureData();
    this->resizeValue(this->totalQPoints());
    this->PairQuantity::resetValue();
}


void BaseDebyeSum::addPairContribution(const BaseBondGenerator& bnds)
{
    const int summationscale = 2;
    const double dist = bnds.distance();
    if (eps_eq(0.0, dist))  return;
    // calculate sigma parameter for the Debye-Waller dampign Gaussian
    const double fwhm = this->getPeakWidthModel().calculate(bnds);
    const double fwhmtosigma = 1.0 / (2 * sqrt(2 * M_LN2));
    const double dwsigma = fwhmtosigma * fwhm;
    for (int kq = this->qminPoints(); kq < this->totalQPoints(); ++kq)
    {
        const double q = kq * this->getQstep();
        const double dwscale = exp(-0.5 * pow(dwsigma * q, 2));
        const double sinescale = summationscale * dwscale *
            this->sfSiteAtkQ(bnds.site0(), kq) *
            this->sfSiteAtkQ(bnds.site1(), kq) / dist;
        if (sinescale < this->getDebyePrecision())   break;
        mvalue[kq] += sinescale * sin(q * dist);
    }
}


double BaseDebyeSum::sfSiteAtQ(int siteidx, const double& q) const
{
    return 1.0;
}


int BaseDebyeSum::qminPoints() const
{
    return mqpoints_cache.qminpoints;
}


int BaseDebyeSum::totalQPoints() const
{
    return mqpoints_cache.totalpoints;
}

// Private Methods -----------------------------------------------------------

void BaseDebyeSum::cacheQpointsData()
{
    const double& dq = this->getQstep();
    mqpoints_cache.qminpoints = int(this->getQmin() / dq);
    mqpoints_cache.totalpoints = int(ceil(this->getQmax() / dq));
    // include point for qmax when it is a close multiple of dq
    if (eps_eq(this->getQmax(), mqpoints_cache.totalpoints * dq))
    {
        mqpoints_cache.totalpoints += 1;
    }
}


double BaseDebyeSum::sfSiteAtkQ(int siteidx, int kq) const
{
    assert(siteidx < int(mstructure_cache.sfsiteatkq.size()));
    assert(mstructure_cache.sfsiteatkq[siteidx].get());
    const QuantityType& sfarray = *(mstructure_cache.sfsiteatkq[siteidx]);
    assert(0 <= kq && kq < int(sfarray.size()));
    return sfarray[kq];
}


double BaseDebyeSum::sfAverageAtkQ(int kq) const
{
    assert(kq < int(mstructure_cache.sfaverageatkq.size()));
    return mstructure_cache.sfaverageatkq[kq];
}


void BaseDebyeSum::cacheStructureData()
{
    int cntsites = mstructure->countSites();
    QuantityType zeros(this->totalQPoints(), 0.0);
    map<string,int> atomtypeidx;
    // sfsiteatkq
    mstructure_cache.sfsiteatkq.clear();
    for (int siteidx = 0; siteidx < cntsites; ++siteidx)
    {
        const string& smbl = mstructure->siteAtomType(siteidx);
        if (!atomtypeidx.count(smbl))  atomtypeidx[smbl] = siteidx;
        int idx = atomtypeidx[smbl];
        assert(mstructure->siteAtomType(siteidx) ==
                mstructure->siteAtomType(idx));
        assert(idx <= int(mstructure_cache.sfsiteatkq.size()));
        // link to an existing array
        if (idx < int(mstructure_cache.sfsiteatkq.size()))
        {
            assert(mstructure_cache.sfsiteatkq[idx].get());
            mstructure_cache.sfsiteatkq.push_back(
                    mstructure_cache.sfsiteatkq[idx]);
            continue;
        }
        // here we need to build a new array
        boost::shared_ptr<QuantityType> pa(new QuantityType(zeros));
        mstructure_cache.sfsiteatkq.push_back(pa);
        QuantityType& sfarray = *(mstructure_cache.sfsiteatkq.back());
        for (int kq = this->qminPoints(); kq < this->totalQPoints(); ++kq)
        {
            double q = this->getQstep() * kq;
            sfarray[kq] = this->sfSiteAtQ(siteidx, q);
        }
    }
    assert(cntsites == int(mstructure_cache.sfsiteatkq.size()));
    // sfaverageatkq
    QuantityType& sfak = mstructure_cache.sfaverageatkq;
    sfak = zeros;
    for (int siteidx = 0; siteidx < cntsites; ++siteidx)
    {
        QuantityType& sfarray = *(mstructure_cache.sfsiteatkq[siteidx]);
        const int multipl = mstructure->siteMultiplicity(siteidx);
        for (int kq = this->qminPoints(); kq < this->totalQPoints(); ++kq)
        {
            sfak[kq] += sfarray[kq] * multipl;
        }
    }
    double tosc = eps_gt(mstructure->totalOccupancy(), 0.0) ?
        (1.0 / mstructure->totalOccupancy()) : 1.0;
    transform(sfak.begin(), sfak.end(), sfak.begin(),
            bind1st(multiplies<double>(), tosc));
    // totaloccupancy
    mstructure_cache.totaloccupancy = mstructure->totalOccupancy();
}


}   // namespace srreal
}   // namespace diffpy

// End of file