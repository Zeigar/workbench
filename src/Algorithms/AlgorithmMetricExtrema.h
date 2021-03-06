#ifndef __ALGORITHM_METRIC_EXTREMA_H__
#define __ALGORITHM_METRIC_EXTREMA_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include "AbstractAlgorithm.h"
#include <utility>
#include <vector>

namespace caret {
    
    class AlgorithmMetricExtrema : public AbstractAlgorithm
    {
        void precomputeNeighborhoods(const SurfaceFile* mySurf, const float* roiColumn, const float& distance, std::vector<std::vector<int32_t> >& neighborhoods);
        void findExtremaConsolidate(const SurfaceFile* mySurf, const float* data, const float* roiColumn, const float& distance, const bool& threshMode,
                                    const float& lowThresh, const float& highThresh, const bool& ignoreMinima, const bool& ignoreMaxima, std::vector<int>& minima, std::vector<int>& maxima);
        void findExtremaNeighborhoods(const float* data, const std::vector<std::vector<int32_t> >& neighborhoods, const bool& threshMode,
                                      const float& lowThresh, const float& highThresh, const bool& ignoreMinima, const bool& ignoreMaxima, std::vector<int>& minima, std::vector<int>& maxima);
        void consolidateStep(const SurfaceFile* mySurf, const float& distance, std::vector<std::pair<int, int> > initExtrema[2], std::vector<int>& minima, std::vector<int>& maxima);
        AlgorithmMetricExtrema();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmMetricExtrema(ProgressObject* myProgObj, const SurfaceFile* mySurf,const MetricFile* myMetric, const float& distance, MetricFile* myMetricOut,
                                               const MetricFile* myRoi = NULL, const float& presmooth = -1.0f, const bool& sumColumns = false, const bool& consolidateMode = false,
                                               const bool& ignoreMinima = false, const bool& ignoreMaxima = false, const int& columnNum = -1);
        AlgorithmMetricExtrema(ProgressObject* myProgObj, const SurfaceFile* mySurf,const MetricFile* myMetric, const float& distance,
                                               MetricFile* myMetricOut, const float& lowThresh, const float& highThresh, const MetricFile* myRoi = NULL,
                                               const float& presmooth = -1.0f, const bool& sumColumns = false, const bool& consolidateMode = false,
                                               const bool& ignoreMinima = false, const bool& ignoreMaxima = false, const int& columnNum = -1);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmMetricExtrema> AutoAlgorithmMetricExtrema;

}

#endif //__ALGORITHM_METRIC_EXTREMA_H__
