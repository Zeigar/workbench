
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#define __ANNOTATION_CHART_TWO_AXIS_LABEL_DECLARE__
#include "AnnotationChartTwoAxisLabel.h"
#undef __ANNOTATION_CHART_TWO_AXIS_LABEL_DECLARE__

#include "AnnotationCoordinate.h"
#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::AnnotationChartTwoAxisLabel 
 * \brief Contains text for a chart two axis label
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param attributeDefaultType
 *     Defaults for new instances.
 * @param fontSizeType
 *     Type of font sizing (percent width or height)
 */
AnnotationChartTwoAxisLabel::AnnotationChartTwoAxisLabel(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType,
                                                         const AnnotationTextFontSizeTypeEnum::Enum fontSizeType)
: AnnotationPercentSizeText(AnnotationTypeEnum::CHART_AXIS_LABEL,
                            attributeDefaultType,
                            fontSizeType)
{
    setCoordinateSpace(AnnotationCoordinateSpaceEnum::VIEWPORT);
    setTextColor(CaretColorEnum::RED);
    setLineColor(CaretColorEnum::NONE);
    setBackgroundColor(CaretColorEnum::NONE);
}

/**
 * Destructor.
 */
AnnotationChartTwoAxisLabel::~AnnotationChartTwoAxisLabel()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationChartTwoAxisLabel::AnnotationChartTwoAxisLabel(const AnnotationChartTwoAxisLabel& obj)
: AnnotationPercentSizeText(obj)
{
    this->copyHelperAnnotationChartTwoAxisLabel(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationChartTwoAxisLabel&
AnnotationChartTwoAxisLabel::operator=(const AnnotationChartTwoAxisLabel& obj)
{
    if (this != &obj) {
        AnnotationPercentSizeText::operator=(obj);
        this->copyHelperAnnotationChartTwoAxisLabel(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationChartTwoAxisLabel::copyHelperAnnotationChartTwoAxisLabel(const AnnotationChartTwoAxisLabel& obj)
{
//    m_axisViewport[0] = obj.m_axisViewport[0];
//    m_axisViewport[1] = obj.m_axisViewport[1];
//    m_axisViewport[2] = obj.m_axisViewport[2];
//    m_axisViewport[3] = obj.m_axisViewport[3];
}

///**
// * Set the viewport in which the axis data is drawn.
// *
// * @param axisViewport
// *     Values for viewport.
// */
//void
//AnnotationChartTwoAxisLabel::setAxisViewport(const int32_t axisViewport[4])
//{
//    m_axisViewport[0] = axisViewport[0];
//    m_axisViewport[1] = axisViewport[1];
//    m_axisViewport[2] = axisViewport[2];
//    m_axisViewport[3] = axisViewport[3];
//}
//
///*
// * Get the viewport in which the axis is drawn.
// * 
// * @param axisViewportOut
// *     Output with the axis viewport.
// */
//void
//AnnotationChartTwoAxisLabel::getAxisViewport(int32_t axisViewportOut[4]) const
//{
//    axisViewportOut[0] = m_axisViewport[0];
//    axisViewportOut[1] = m_axisViewport[1];
//    axisViewportOut[2] = m_axisViewport[2];
//    axisViewportOut[3] = m_axisViewport[3];
//}
//
