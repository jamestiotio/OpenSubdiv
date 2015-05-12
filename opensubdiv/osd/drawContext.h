//
//   Copyright 2013 Pixar
//
//   Licensed under the Apache License, Version 2.0 (the "Apache License")
//   with the following modification; you may not use this file except in
//   compliance with the Apache License and the following modification to it:
//   Section 6. Trademarks. is deleted and replaced with:
//
//   6. Trademarks. This License does not grant permission to use the trade
//      names, trademarks, service marks, or product names of the Licensor
//      and its affiliates, except as required to comply with Section 4(c) of
//      the License and to reproduce the content of the NOTICE file.
//
//   You may obtain a copy of the Apache License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the Apache License with the above modification is
//   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//   KIND, either express or implied. See the Apache License for the specific
//   language governing permissions and limitations under the Apache License.
//

#ifndef OSD_DRAW_CONTEXT_H
#define OSD_DRAW_CONTEXT_H

#include "../version.h"

#include "../far/patchDescriptor.h"
#include "../far/types.h"

#include <utility>
#include <string>

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

namespace Far {
    class PatchTables;
}

namespace Osd {

/// \brief Base DrawContext class
///
/// DrawContext derives several sub-classes with API specific functionality
/// (GL, D3D11, ...).
///
/// Current specificiation GPU hardware tessellation limitations require transition
/// patches to be split-up into several triangular bi-cubic sub-patches.
/// DrawContext processes FarPatchArrays from Far::PatchTables and generates the
/// additional sets of sub-patches.
///
/// Contexts interface the serialized topological data pertaining to the
/// geometric primitives with the capabilities of the selected discrete
/// compute device.
///
class DrawContext {

public:
    typedef Far::Index Index;

    class PatchArray {
    public:
        /// Constructor
        ///
        /// @param desc       Patch descriptor defines the type, pattern, rotation of
        ///                   the patches in the array
        ///
        /// @param npatches   The number of patches in the array
        ///
        /// @param vertIndex  Index of the first control vertex in the array
        ///
        /// @param patchIndex Index of the first patch in the array
        ///
        /// @param qoIndex    Index of the first quad-offset entry
        ///
        PatchArray(Far::PatchDescriptor desc, int npatches,
            Index vertIndex, Index patchIndex, Index qoIndex) :
                _desc(desc), _npatches(npatches),
                    _vertIndex(vertIndex), _patchIndex(patchIndex), _quadOffsetIndex(qoIndex) { }

        /// Returns a patch descriptor defining the type of patches in the array
        Far::PatchDescriptor GetDescriptor() const {
            return _desc;
        }

        /// Update a patch descriptor
        void SetDescriptor(Far::PatchDescriptor desc) {
            _desc = desc;
        }

        /// Returns the index of the first control vertex of the first patch
        /// of this array in the global PTable
        unsigned int GetVertIndex() const {
            return _vertIndex;
        }

        /// Returns the global index of the first patch in this array (Used to
        /// access ptex / fvar table data)
        unsigned int GetPatchIndex() const {
            return _patchIndex;
        }

        /// Returns the number of patches in the array
        unsigned int GetNumPatches() const {
            return _npatches;
        }

        /// Returns the number of patch indices in the array
        unsigned int GetNumIndices() const {
            return _npatches * _desc.GetNumControlVertices();
        }

        /// Returns the offset of quad offset table
        unsigned int GetQuadOffsetIndex() const {
            return _quadOffsetIndex;
        }

        /// Set num patches (used at batch glomming)
        void SetNumPatches(int npatches) {
            _npatches = npatches;
        }

    private:
        Far::PatchDescriptor _desc;
        int _npatches;
        Index _vertIndex,
              _patchIndex,
              _quadOffsetIndex;
    };

    /// Constructor
    DrawContext(int maxValence) : _isAdaptive(false), _maxValence(maxValence) {}

    /// Descrtuctor
    virtual ~DrawContext();

    /// Returns true if the primitive attached to the context uses feature adaptive
    /// subdivision
    bool IsAdaptive() const {
        return _isAdaptive;
    }

    typedef std::vector<PatchArray> PatchArrayVector;

    PatchArrayVector const & GetPatchArrays() const {
        return _patchArrays;
    }

    /// The writable accessor to the internal patch array (tentative).
    /// We should have a different api something like ConvertPatchArrays().
    PatchArrayVector &GetPatchArrays() {
        return _patchArrays;
    }

    // processes FarPatchArrays and inserts requisite sub-patches for the arrays
    // containing transition patches
    static void ConvertPatchArrays(Far::PatchTables const &patchTables,
                                   DrawContext::PatchArrayVector &osdPatchArrays);

    // maxValence is needed for legacy gregorypatch drawing
    int GetMaxValence() const {
        return _maxValence;
    }

    typedef std::vector<float> FVarData;


protected:

     static void packPatchVerts(Far::PatchTables const & patchTables,
         std::vector<Index> & dst);

     static void packSharpnessValues(Far::PatchTables const & patchTables,
         std::vector<unsigned int> & dst);

     static void packFVarData(Far::PatchTables const & patchTables,
         int fvarWidth, FVarData const & src, FVarData & dst);

    // XXXX: move to private member
    PatchArrayVector _patchArrays;

    bool _isAdaptive;

    int _maxValence;
};

}  // end namespace Osd

}  // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;

} // end namespace OpenSubdiv

#endif /* OSD_DRAW_CONTEXT_H */
