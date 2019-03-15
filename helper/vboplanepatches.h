#ifndef VBOPLANEPATCHES_H
#define VBOPLANEPATCHES_H

#include "drawable.h"

class VBOPlanePatches : public Drawable
{
private:
    unsigned int vaoHandle;
    int faces;

public:
    VBOPlanePatches(float xsize, float zsize, int xdivs, int zdivs, float smax = 1.0f, float tmax = 1.0f);

    void render() const;
};

#endif // VBOPLANEPATCHES_H
