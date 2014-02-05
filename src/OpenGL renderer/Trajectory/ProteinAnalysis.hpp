
/******************************************************************************\
                     This file is part of Folding Atomata,
          a program that displays 3D views of Folding@home proteins.

                      Copyright (c) 2013, Jesse Victors

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see http://www.gnu.org/licenses/

                For information regarding this software email:
                                Jesse Victors
                         jvictors@jessevictors.com
\******************************************************************************/

#ifndef PROTEIN_ANALYSIS
#define PROTEIN_ANALYSIS

/**
    Right now, the primary function of the ProteinAnalysis class is to solve
    the protein split glitch, wherein FAHClient returns a protein in more than
    one piece. The FahCores use Fourier transforms as part of its force
    calculations, and they require a bounding box to be set around the protein,
    and that bounding box (and the protein inside it) to be mathematically
    repeated in all directions to infinity, or in other words, periodic.
    FAHClient attemps to identify a single instance of this protein in real
    space, but it doesn't do a perfect job. The end result is that pieces of the
    protein may appear to wrap around the box. The protein is supposed to end up
    like |---abcde--| but instead it's like |bcde-----a| which is mathematically
    the same to the Fourier transforms. This class aims to identify these pieces
    and then reassemble the protein.
**/

#include "Trajectory.hpp"
#include <unordered_map>
#include <unordered_set>

class ProteinAnalysis
{
    public:
        const float BOND_LENGTH = 2;

        struct Bucket
        {
            int groupID = -1;
            std::vector<AtomPtr> atoms;
        };

        typedef std::vector<std::vector<std::vector<Bucket>>> BucketMap;
        typedef std::vector<std::vector<AtomPtr>> AtomGroups;

    public:
        ProteinAnalysis(const TrajectoryPtr& trajectory);
        void fixProteinSplits();
        BucketMap getBucketMap();
        void assignGroups(BucketMap& bucketMap);
        void assignGroup(BucketMap& map, int x, int y, int z, int id);
        AtomGroups getGroups(const BucketMap& map);
        void fixGroups(const AtomGroups& groups);
        std::vector<int> getUnassignedBucket(const BucketMap& map);

    private:
        TrajectoryPtr trajectory_;
};



#endif
