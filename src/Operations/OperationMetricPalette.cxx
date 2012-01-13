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

#include "OperationMetricPalette.h"
#include "OperationException.h"
#include "MetricFile.h"
#include "PaletteColorMapping.h"
#include <vector>

using namespace caret;
using namespace std;

AString OperationMetricPalette::getCommandSwitch()
{
    return "-metric-palette";
}

AString OperationMetricPalette::getShortDescription()
{
    return "SET THE PALETTE OF A METRIC FILE";
}

OperationParameters* OperationMetricPalette::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "metric", "the metric to modify");
    ret->addStringParameter(2, "mode", "the mapping mode");
    OptionalParameter* columnSelect = ret->createOptionalParameter(3, "-column", "select a single column");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    
    OptionalParameter* posMinMaxPercent = ret->createOptionalParameter(4, "-pos-percent", "percentage min/max for positive data coloring");
    posMinMaxPercent->addDoubleParameter(1, "pos-min-%", "the percentile for the least positive data");
    posMinMaxPercent->addDoubleParameter(2, "pos-max-%", "the percentile for the most positive data");
    
    OptionalParameter* negMinMaxPercent = ret->createOptionalParameter(5, "-neg-percent", "percentage min/max for negative data coloring");
    negMinMaxPercent->addDoubleParameter(1, "neg-min-%", "the percentile for the least negative data");
    negMinMaxPercent->addDoubleParameter(2, "neg-max-%", "the percentile for the most negative data");
    
    OptionalParameter* posMinMaxValue = ret->createOptionalParameter(11, "-pos-user", "user min/max values for positive data coloring");
    posMinMaxValue->addDoubleParameter(1, "pos-min-user", "the value for the least positive data");
    posMinMaxValue->addDoubleParameter(2, "pos-max-user", "the value for the most positive data");
    
    OptionalParameter* negMinMaxValue = ret->createOptionalParameter(12, "-neg-user", "user min/max values for negative data coloring");
    negMinMaxValue->addDoubleParameter(1, "neg-min-user", "the value for the least negative data");
    negMinMaxValue->addDoubleParameter(2, "neg-max-user", "the value for the most negative data");
    
    OptionalParameter* interpolate = ret->createOptionalParameter(9, "-interpolate", "interpolate colors");
    interpolate->addBooleanParameter(1, "interpolate", "boolean, whether to interpolate");
    
    OptionalParameter* displayPositive = ret->createOptionalParameter(6, "-disp-pos", "display positive data");
    displayPositive->addBooleanParameter(1, "display", "boolean, whether to display");
    
    OptionalParameter* displayNegative = ret->createOptionalParameter(7, "-disp-neg", "display positive data");
    displayNegative->addBooleanParameter(1, "display", "boolean, whether to display");
    
    OptionalParameter* displayZero = ret->createOptionalParameter(8, "-disp-zero", "display data closer to zero than the min cutoff");
    displayZero->addBooleanParameter(1, "display", "boolean, whether to display");
    
    OptionalParameter* paletteName = ret->createOptionalParameter(10, "-palette-name", "set the palette used");
    paletteName->addStringParameter(1, "name", "the name of the palette");
    
    //TODO: add thresholding options once you can write good descriptions of what they do
    
    AString myText = AString("The original metric file is overwritten with the modified version.  By default, all columns of the metric file are adjusted ") +
            "to the new settings, use the -column option to change only one column.  Mapping settings not specified in options will be taken from the first column.  " +
            "The mode argument must be one of the following:\n\n";
    vector<PaletteScaleModeEnum::Enum> myEnums;
    PaletteScaleModeEnum::getAllEnums(myEnums);
    for (int i = 0; i < (int)myEnums.size(); ++i)
    {
        myText += PaletteScaleModeEnum::toName(myEnums[i]) + "\n";
    }
    ret->setHelpText(myText);
    return ret;
}

void OperationMetricPalette::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString myMetricName = myParams->getString(1);
    AString myModeName = myParams->getString(2);
    bool ok = false;
    PaletteScaleModeEnum::Enum myMode = PaletteScaleModeEnum::fromName(myModeName, &ok);
    if (!ok)
    {
        throw OperationException("unknown mapping mode");
    }
    MetricFile myMetric;
    myMetric.readFile(myMetricName);
    int myColumn = -1;
    OptionalParameter* columnSelect = myParams->getOptionalParameter(3);
    if (columnSelect->m_present)
    {
        myColumn = (int)myMetric.getMapIndexFromNameOrNumber(columnSelect->getString(1));
        if (myColumn < 0 || myColumn >= myMetric.getNumberOfColumns())
        {
            throw OperationException("invalid column specified");
        }
    }
    PaletteColorMapping myMapping = *(myMetric.getMapPaletteColorMapping(0));//create the mapping, then use operator= to set for all requested columns, take defaults from first map
    myMapping.setScaleMode(myMode);
    OptionalParameter* posMinMaxPercent = myParams->getOptionalParameter(4);
    if (posMinMaxPercent->m_present)
    {
        myMapping.setAutoScalePercentagePositiveMinimum(posMinMaxPercent->getDouble(1));
        myMapping.setAutoScalePercentagePositiveMaximum(posMinMaxPercent->getDouble(2));
    }
    OptionalParameter* negMinMaxPercent = myParams->getOptionalParameter(5);
    if (negMinMaxPercent->m_present)
    {
        myMapping.setAutoScalePercentageNegativeMinimum(negMinMaxPercent->getDouble(1));
        myMapping.setAutoScalePercentageNegativeMaximum(negMinMaxPercent->getDouble(2));
    }
    OptionalParameter* posMinMaxValue = myParams->getOptionalParameter(11);
    if (posMinMaxValue->m_present)
    {
        myMapping.setUserScalePositiveMinimum(posMinMaxValue->getDouble(1));
        myMapping.setUserScalePositiveMaximum(posMinMaxValue->getDouble(2));
    }
    OptionalParameter* negMinMaxValue = myParams->getOptionalParameter(12);
    if (negMinMaxValue->m_present)
    {
        myMapping.setUserScaleNegativeMinimum(negMinMaxValue->getDouble(1));
        myMapping.setUserScaleNegativeMaximum(negMinMaxValue->getDouble(2));
    }
    OptionalParameter* displayPositive = myParams->getOptionalParameter(6);
    if (displayPositive->m_present)
    {
        myMapping.setDisplayPositiveDataFlag(displayPositive->getBoolean(1));
    }
    OptionalParameter* displayNegative = myParams->getOptionalParameter(7);
    if (displayNegative->m_present)
    {
        myMapping.setDisplayNegativeDataFlag(displayNegative->getBoolean(1));
    }
    OptionalParameter* interpolate = myParams->getOptionalParameter(9);
    if (interpolate->m_present)
    {
        myMapping.setInterpolatePaletteFlag(interpolate->getBoolean(1));
    }
    OptionalParameter* displayZero = myParams->getOptionalParameter(8);
    if (interpolate->m_present)
    {
        myMapping.setDisplayZeroDataFlag(displayZero->getBoolean(1));
    }
    OptionalParameter* paletteName = myParams->getOptionalParameter(10);
    if (paletteName->m_present)
    {
        myMapping.setSelectedPaletteName(paletteName->getString(1));
    }
    if (myColumn == -1)
    {
        for (int i = 0; i < myMetric.getNumberOfMaps(); ++i)
        {
            *(myMetric.getMapPaletteColorMapping(i)) = myMapping;
        }
    } else {
        *(myMetric.getMapPaletteColorMapping(myColumn)) = myMapping;
    }
    myMetric.writeFile(myMetricName);
}
