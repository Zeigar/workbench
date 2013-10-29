#ifndef __PLANE_H__
#define __PLANE_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


#include "CaretObject.h"


namespace caret {

///Operations on a plane.
    class Plane : public CaretObject {
        
    public:
        Plane();
        
        Plane(const float p1[3],
              const float p2[3],
              const float p3[3]);
        
        Plane(const float unitNormalVector[3],
              const float pointOnPlane[3]);
        
        virtual ~Plane();
        
        Plane(const Plane& p);
        
        Plane& operator=(const Plane& p);
        
        bool isValidPlane() const;
        
        bool triangleIntersectPlane(
                                    const float t1[3],
                                    const float t2[3],
                                    const float t3[3],
                                    float intersectionPointOut1[3],
                                    float intersectionPointOut2[3]) const;
        
        double absoluteDistanceToPlane(const float p[3]) const;
        
        double signedDistanceToPlane(const float p[3]) const;
        
        bool lineSegmentIntersectPlane(
                                       const float lp1[3],
                                       const float lp2[3],
                                       float intersectionOut[3]) const;
        
        void projectPointToPlane(const float pointIn[3],
                                 float pointProjectedOut[3]) const;
        
        void getPlane(double& aOut,
                      double& bOut,
                      double& cOut,
                      double& dOut) const;
        
        void getNormalVector(double normalVectorOut[3]) const;
        
        void getNormalVector(float normalVectorOut[3]) const;
        
        bool rayIntersection(const float rayOrigin[3],
                             const float rayVector[3],
                             float intersectionXYZandDistance[4]);
        
        virtual AString toString() const;
        
        static void unitTest(std::ostream& stream,
                      const bool isVerbose);
        
    private:
        static void unitTestLineIntersectPlane(std::ostream& stream,
                                        const AString& testName,
                                        const float p1[3],
                                        const float p2[3],
                                        const float p3[3],
                                        const float l1[3],
                                        const float l2[3],
                                        const float correctIntersectionPoint[3],
                                        const bool intersectionValid);
        
        static void unitTest1(std::ostream& stream);
        
        static void unitTest2(std::ostream& stream);
        
        static void unitTest3(std::ostream& stream);
        
        void copyHelperPlane(const Plane& p);
        
        double m_pointOnPlane[3];
        
        double m_normalVector[3];
        
        bool m_validPlaneFlag;
        
        double m_A;
        
        double m_B;
        
        double m_C;
        
        double m_D;
        
    };
    
} // namespace

#endif // __PLANE_H__
