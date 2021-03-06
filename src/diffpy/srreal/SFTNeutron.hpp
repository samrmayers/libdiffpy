/*****************************************************************************
*
* libdiffpy         by DANSE Diffraction group
*                   Simon J. L. Billinge
*                   (c) 2009 The Trustees of Columbia University
*                   in the City of New York.  All rights reserved.
*
* File coded by:    Pavol Juhas
*
* See AUTHORS.txt for a list of people who contributed.
* See LICENSE_DANSE.txt for license information.
*
******************************************************************************
*
* class SFTNeutron
*
* Neutron ScatteringFactorTable using coherent cross sections extracted
* from Paul Kienzle periodictable library for Python.
*
*****************************************************************************/

#ifndef SFTNEUTRON_HPP_INCLUDED
#define SFTNEUTRON_HPP_INCLUDED

#include <diffpy/srreal/ScatteringFactorTable.hpp>

namespace diffpy {
namespace srreal {

class SFTNeutron : public ScatteringFactorTable
{
    public:

        // HasClassRegistry methods
        ScatteringFactorTablePtr create() const;
        ScatteringFactorTablePtr clone() const;
        const std::string& type() const;
        // own methods
        const std::string& radiationType() const;
        // method overloads
        double standardLookup(const std::string& smbl, double q) const;

    private:

        // serialization
        friend class boost::serialization::access;

        template<class Archive>
            void serialize(Archive& ar, const unsigned int version)
        {
            using boost::serialization::base_object;
            ar & base_object<ScatteringFactorTable>(*this);
        }

};  // class SFTNeutron


}   // namespace srreal
}   // namespace diffpy

// Serialization -------------------------------------------------------------

BOOST_CLASS_EXPORT_KEY(diffpy::srreal::SFTNeutron)

#endif  // SFTNEUTRON_HPP_INCLUDED
