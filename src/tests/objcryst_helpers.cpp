/*****************************************************************************
*
* libdiffpy         by DANSE Diffraction group
*                   Simon J. L. Billinge
*                   (c) 2010 The Trustees of Columbia University
*                   in the City of New York.  All rights reserved.
*
* File coded by:    Pavol Juhas
*
* See AUTHORS.txt for a list of people who contributed.
* See LICENSE_DANSE.txt for license information.
*
******************************************************************************
*
* shared helpers for loading diffpy structure objects.
*
*****************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>

#include <ObjCryst/ObjCryst/CIF.h>

#include "objcryst_helpers.hpp"
#include "test_helpers.hpp"

using namespace std;

namespace {

void NoMessages(const string&)  { }

}   // namespace


ObjCryst::Crystal* loadTestCrystal(const string& tailname)
{
    string fp = prepend_testdata_dir(tailname);
    ifstream in(fp.c_str());
    // suppress the chatty output from CreateCrystalFromCIF
    void (*OCMessages)(const string&) = ObjCryst::fpObjCrystInformUser;
    ObjCryst::fpObjCrystInformUser = NoMessages;
    ObjCryst::CIF cif(in);
    ObjCryst::Crystal* crst = ObjCryst::CreateCrystalFromCIF(cif, false);
    ObjCryst::fpObjCrystInformUser = OCMessages;
    return crst;
}

// End of file
