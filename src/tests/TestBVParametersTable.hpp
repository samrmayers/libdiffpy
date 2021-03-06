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
* class TestBVParametersTable -- unit tests for BVParametersTable class
*
*****************************************************************************/

#include <cxxtest/TestSuite.h>

#include <diffpy/srreal/BVParametersTable.hpp>
#include "serialization_helpers.hpp"

using namespace std;
using namespace diffpy::srreal;


class TestBVParametersTable : public CxxTest::TestSuite
{

    private:

        BVParametersTablePtr mbvtb;

    public:

        void setUp()
        {
            mbvtb.reset(new BVParametersTable);
        }


        void test_copy()
        {
            BVParam mynacl("Na", 1, "Cl", -1);
            mynacl.mRo = 1.23;
            mynacl.mB = 0.377;
            mbvtb->setCustom(mynacl);
            TS_ASSERT_EQUALS(1.23, mbvtb->lookup("Na", 1, "Cl", -1).mRo);
            TS_ASSERT_EQUALS(0.377, mbvtb->lookup("Na", 1, "Cl", -1).mB);
            BVParametersTable bvcp(*mbvtb);
            TS_ASSERT_EQUALS(1.23, bvcp.lookup("Na", 1, "Cl", -1).mRo);
            TS_ASSERT_EQUALS(0.377, bvcp.lookup("Na", 1, "Cl", -1).mB);
            bvcp.resetAll();
            TS_ASSERT_EQUALS(2.15, bvcp.lookup("Na", 1, "Cl", -1).mRo);
            TS_ASSERT_EQUALS(0.37, bvcp.lookup("Na", 1, "Cl", -1).mB);
            TS_ASSERT_EQUALS(1.23, mbvtb->lookup("Na", 1, "Cl", -1).mRo);
            TS_ASSERT_EQUALS(0.377, mbvtb->lookup("Na", 1, "Cl", -1).mB);
        }


        void test_none()
        {
            const BVParam& bpnone = BVParametersTable::none();
            TS_ASSERT_EQUALS(&bpnone, &(mbvtb->none()));
            TS_ASSERT_EQUALS(0.0, bpnone.bondvalence(0));
            TS_ASSERT_EQUALS(0.0, bpnone.bondvalence(3));
            TS_ASSERT_EQUALS(0.0, bpnone.bondvalence(10));
            TS_ASSERT_EQUALS(0.0, bpnone.bondvalenceToDistance(0.5));
        }


        void test_atomvalence()
        {
            TS_ASSERT_EQUALS(0, mbvtb->getAtomValence("O"));
            mbvtb->setAtomValence("O", -2);
            TS_ASSERT_EQUALS(-2, mbvtb->getAtomValence("O"));
            mbvtb->resetAtomValences();
            TS_ASSERT_EQUALS(0, mbvtb->getAtomValence("O"));
        }


        void test_lookup()
        {
            BVParam bp = mbvtb->lookup("Xx", 0, "Yy", 3);
            TS_ASSERT(bp.matom0.empty());
            TS_ASSERT(bp.matom1.empty());
            TS_ASSERT_EQUALS(0, bp.mvalence0);
            TS_ASSERT_EQUALS(0, bp.mvalence1);
            TS_ASSERT_EQUALS(0.0, bp.mRo);
            TS_ASSERT_EQUALS(0.0, bp.mB);
            TS_ASSERT_EQUALS(&mbvtb->lookup("Xx", 0, "Yy", 3),
                    &mbvtb->lookup("Na", 33, "Cl", -11));
            BVParam bk("Na", 1, "Cl", -1);
            TS_ASSERT_EQUALS(&mbvtb->lookup(bk),
                    &mbvtb->lookup("Cl", -1, "Na", 1));
            TS_ASSERT_EQUALS(2.15, mbvtb->lookup(bk).mRo);
            TS_ASSERT_EQUALS(0.37, mbvtb->lookup(bk).mB);
            TS_ASSERT_EQUALS("b", mbvtb->lookup(bk).mref_id);
            const BVParam& bnacl0 = mbvtb->lookup("Na", 1, "Cl", -1);
            TS_ASSERT_EQUALS(&bnacl0, &mbvtb->lookup("Na+", "Cl-"));
            TS_ASSERT_EQUALS(&bnacl0, &mbvtb->lookup("Cl1-", "Na1+"));
            const BVParam& bpnone = BVParametersTable::none();
            TS_ASSERT_EQUALS(&bpnone, &mbvtb->lookup("Na", "Cl"));
            mbvtb->setAtomValence("Na", +1);
            mbvtb->setAtomValence("Cl", -1);
            TS_ASSERT_EQUALS(&bnacl0, &mbvtb->lookup("Na", "Cl"));
        }


        void test_setCustom()
        {
            BVParam mymgo("Mg", 2, "O", -2);
            mymgo.mRo = 2.34;
            mymgo.mB = 0.345;
            mbvtb->setCustom(mymgo);
            TS_ASSERT_EQUALS(2.34, mbvtb->lookup("Mg", 2, "O", -2).mRo);
            TS_ASSERT_EQUALS(0.345, mbvtb->lookup(mymgo).mB);
            mbvtb->resetCustom(mymgo);
            TS_ASSERT_DIFFERS(2.34, mbvtb->lookup("Mg", 2, "O", -2).mRo);
            TS_ASSERT_DIFFERS(0.345, mbvtb->lookup(mymgo).mB);
            BVParam zrh = mbvtb->lookup("Zr", 4, "H", -1);
            TS_ASSERT_EQUALS(1.79, zrh.mRo);
            TS_ASSERT_EQUALS(0.37, zrh.mB);
            TS_ASSERT_EQUALS(string("b"), zrh.mref_id);
            mbvtb->setCustom("H", -1, "Zr", 4, 1.791, 0.371, "check");
            zrh = mbvtb->lookup("Zr", 4, "H", -1);
            TS_ASSERT_EQUALS(1.791, zrh.mRo);
            TS_ASSERT_EQUALS(0.371, zrh.mB);
            TS_ASSERT_EQUALS(string("check"), zrh.mref_id);
            mbvtb->setCustom("H", -1, "Zr", 4, 1.791, 0.444, "mate");
            zrh = mbvtb->lookup("Zr", 4, "H", -1);
            TS_ASSERT_EQUALS(0.444, zrh.mB);
            TS_ASSERT_EQUALS(string("mate"), zrh.mref_id);
        }


        void test_resetAll()
        {
            BVParam mynacl("Cl", -1, "Na", 1, 2.345, 0.44, "pj1");
            BVParam mymgo("O", -2, "Mg", 2, 3.456, 0.55, "pj2");
            mbvtb->setCustom(mynacl);
            mbvtb->setCustom(mymgo);
            TS_ASSERT_EQUALS(string("pj1"), mbvtb->lookup(mynacl).mref_id);
            TS_ASSERT_EQUALS(string("pj2"), mbvtb->lookup(mymgo).mref_id);
            mbvtb->resetAll();
            TS_ASSERT_DIFFERS(string("pj1"), mbvtb->lookup(mynacl).mref_id);
            TS_ASSERT_DIFFERS(string("pj2"), mbvtb->lookup(mymgo).mref_id);
        }


        void test_getAllCustom()
        {
            TS_ASSERT(mbvtb->getAllCustom().empty());
            BVParam mymgo("Mg", 2, "O", -2);
            mbvtb->setCustom(mymgo);
            TS_ASSERT_EQUALS(1u, mbvtb->getAllCustom().size());
            const BVParam& bp = mbvtb->lookup("O2-", "Mg2+");
            BVParametersTable::SetOfBVParam::const_iterator ii;
            ii = mbvtb->getAllCustom().find(mymgo);
            TS_ASSERT_DIFFERS(ii, mbvtb->getAllCustom().end());
            TS_ASSERT_DIFFERS(&mymgo, &(*ii));
            TS_ASSERT_EQUALS(&bp, &(*ii));
            TS_ASSERT_EQUALS(mymgo, *ii);
            // erase the only custom parameter
            mbvtb->resetCustom(*ii);
            TS_ASSERT(mbvtb->getAllCustom().empty());
        }


        void test_getAll()
        {
            BVParametersTable::SetOfBVParam allpars0, allpars1;
            allpars0 = mbvtb->getAll();
            TS_ASSERT(!allpars0.empty());
            size_t cnt0 = allpars0.size();
            BVParam mymgo("Mg", 2, "O", -2);
            mbvtb->setCustom(mymgo);
            TS_ASSERT_EQUALS(cnt0, mbvtb->getAll().size());
            mbvtb->setCustom(BVParam("Mg", 7, "O", -3));
            allpars1 = mbvtb->getAll();
            size_t cnt1 = allpars1.size();
            TS_ASSERT_EQUALS(cnt0 + 1, cnt1);
            mbvtb->resetAll();
            TS_ASSERT_EQUALS(cnt0, mbvtb->getAll().size());
            mbvtb->setCustom(BVParam("Mg", 2, "O", -2, 77, 88));
            allpars1 = mbvtb->getAll();
            const BVParam& mgo = *allpars1.find(BVParam("Mg", 2, "O", -2));
            TS_ASSERT_EQUALS(77.0, mgo.mRo);
            TS_ASSERT_EQUALS(88.0, mgo.mB);
        }


        void test_serialization()
        {
            BVParam mynacl("Cl", -1, "Na", 1, 2.345, 0.44, "pj1");
            BVParam mymgo("O", -2, "Mg", 2, 3.456, 0.55, "pj2");
            mbvtb->setCustom(mynacl);
            mbvtb->setCustom(mymgo);
            // check serialization of shared pointers
            BVParametersTablePtr bvtb1 = dumpandload(mbvtb);
            TS_ASSERT_DIFFERS(mbvtb.get(), bvtb1.get());
            TS_ASSERT_EQUALS(2.345,
                    bvtb1->lookup("Cl", -1, "Na", 1).mRo);
            TS_ASSERT_EQUALS(0.44,
                    bvtb1->lookup("Cl", -1, "Na", 1).mB);
            TS_ASSERT_EQUALS(string("pj1"),
                    bvtb1->lookup("Cl", -1, "Na", 1).mref_id);
            TS_ASSERT_EQUALS(3.456,
                    bvtb1->lookup("O", -2, "Mg", 2).mRo);
            TS_ASSERT_EQUALS(0.55,
                    bvtb1->lookup("O", -2, "Mg", 2).mB);
            TS_ASSERT_EQUALS(string("pj2"),
                    bvtb1->lookup("O", -2, "Mg", 2).mref_id);
            // check serialization of instances
            BVParametersTable tb2 = dumpandload(*mbvtb);
            TS_ASSERT_EQUALS(2u, tb2.getAllCustom().size());
            TS_ASSERT_EQUALS(mynacl, tb2.lookup("Cl-", "Na+"));
            // check serialization of customized valences
            tb2.setAtomValence("Na", 1);
            tb2.setAtomValence("Cl", -1);
            BVParametersTable tb3 = dumpandload(tb2);
            TS_ASSERT_EQUALS(mynacl, tb3.lookup("Cl", "Na"));
            const BVParam& bpnone = BVParametersTable::none();
            TS_ASSERT_EQUALS(bpnone, mbvtb->lookup("Cl", "Na"));
        }

};  // class TestBVParametersTable

// End of file
