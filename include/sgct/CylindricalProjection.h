/*************************************************************************
Copyright (c) 2012-2015 Miroslav Andel
All rights reserved.

For conditions of distribution and use, see copyright notice in sgct.h
*************************************************************************/

#ifndef _CYLINDRICAL_PROJECTION_H
#define _CYLINDRICAL_PROJECTION_H

#include "NonLinearProjection.h"
#include "CorrectionMesh.h"
#include <glm/glm.hpp>



namespace sgct_core
{
    /*!
    This class manages and renders non linear fisheye projections
    */
    class CylindricalProjection : public NonLinearProjection
    {
    public:
        CylindricalProjection();
        ~CylindricalProjection();

        void setSectorAngle(float angle);
        void setHeight(float height);
        void setRadius(float radius);

        void setCylindricalResolution(int width);
        void setHeightResolution(int height);

        void setNFrustums(int nFrustums);

        void setOffset(const glm::vec3 & offset);
        void setBaseOffset(const glm::vec3 & offset);
        glm::vec3 totalOffset() const;

        void update(float width, float height) override;
        void render() override;
        void renderCubemap(std::size_t * subViewPortIndex) override;
        
    private:
        void initOneViewport();
        void initTwoViewports();
        void initThreeViewports();
        void initFourViewports();

        void initTextures() override;
        void initVBO() override;
        void initViewports() override;
        void initShaders() override;

        float mRadius = 1.f;
        float mSectorAngle = 180.f;
        float mHeight = 1.f;
        int mCylindricalResolution = 1024;
        int mHeightResolution = 512;

        glm::vec3 mOffset;
        glm::vec3 mBaseOffset;

        int mNFrustums = 0;
    };
}


#endif
