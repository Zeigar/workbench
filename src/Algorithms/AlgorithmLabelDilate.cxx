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

#include "AlgorithmLabelDilate.h"
#include "AlgorithmException.h"
#include "LabelFile.h"
#include "GiftiLabelTable.h"
#include "SurfaceFile.h"

using namespace caret;
using namespace std;

AString AlgorithmLabelDilate::getCommandSwitch()
{
    return "-label-dilate";
}

AString AlgorithmLabelDilate::getShortDescription()
{
    return "DILATE A LABEL FILE";
}

OperationParameters* AlgorithmLabelDilate::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addLabelParameter(1, "label", "the input label");
    
    ret->addSurfaceParameter(2, "surface", "the surface to dilate on");
    
    ret->addDoubleParameter(3, "dilate-dist", "distance in mm to dilate the labels");
    
    ret->addLabelOutputParameter(4, "label-out", "the output label file");
    
    OptionalParameter* columnSelect = ret->createOptionalParameter(5, "-column", "select a single column to dilate");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    
    ret->setHelpText(
        AString("Fills in label information where the label is currently unassigned, up to the specified distance away from other labels.  ") +
        "By default, dilates all columns of the input label."
    );
    return ret;
}

void AlgorithmLabelDilate::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LabelFile* myLabel = myParams->getLabel(1);
    SurfaceFile* mySurf = myParams->getSurface(2);
    float myDist = (float)myParams->getDouble(3);
    LabelFile* myLabelOut = myParams->getLabel(4);
    OptionalParameter* columnSelect = myParams->getOptionalParameter(5);
    int columnNum = -1;
    if (columnSelect->m_present)
    {//set up to use the single column
        columnNum = (int)myLabel->getMapIndexFromNameOrNumber(columnSelect->getString(1));
        if (columnNum < 0)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    AlgorithmLabelDilate(myProgObj, myLabel, mySurf, myDist, myLabelOut, columnNum);
}

AlgorithmLabelDilate::AlgorithmLabelDilate(ProgressObject* myProgObj, const LabelFile* myLabel, const SurfaceFile* mySurf, float myDist, LabelFile* myLabelOut, int columnNum) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int32_t unusedLabel = myLabel->getLabelTable()->getUnassignedLabelKey();
    int numColumns = myLabel->getNumberOfColumns();
    if (myDist < 0.0f)
    {
        throw AlgorithmException("invalid distance specified");
    }
    if (columnNum < -1 || columnNum >= numColumns)
    {
        throw AlgorithmException("invalid column specified");
    }
    int numNodes = myLabel->getNumberOfNodes();
    if (mySurf->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("surface has wrong number of nodes for this label");
    }
    CaretArray<int32_t> colScratch(numNodes);
    vector<float> myAreas;
    CaretArray<int> markArray(numNodes);
    mySurf->computeNodeAreas(myAreas);
    *(myLabelOut->getLabelTable()) = *(myLabel->getLabelTable());
    if (columnNum == -1)
    {
        myLabelOut->setNumberOfNodesAndColumns(numNodes, myLabel->getNumberOfColumns());
        for (int thisCol = 0; thisCol < myLabel->getNumberOfColumns(); ++thisCol)
        {
            const int32_t* myInputData = myLabel->getLabelKeyPointerForColumn(thisCol);
            myLabelOut->setColumnName(thisCol, myLabel->getColumnName(thisCol) + " dilated");
            for (int i = 0; i < numNodes; ++i)
            {
                if (myInputData[i] == unusedLabel)
                {
                    markArray[i] = 0;
                } else {
                    markArray[i] = 1;
                }
            }
            CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
            CaretPointer<GeodesicHelper> myGeoHelp = mySurf->getGeodesicHelper();
            for (int i = 0; i < numNodes; ++i)
            {
                if (markArray[i] == 0)
                {
                    vector<int32_t> nodeList;
                    vector<float> distList;
                    myGeoHelp->getNodesToGeoDist(i, myDist, nodeList, distList);
                    int numInRange = (int)nodeList.size();
                    bool first = true;
                    float bestDist = -1.0f;
                    int32_t bestLabel = unusedLabel;
                    for (int j = 0; j < numInRange; ++j)
                    {
                        if (markArray[nodeList[j]] == 1)
                        {
                            if (first || distList[j] < bestDist)
                            {
                                first = false;
                                bestDist = distList[j];
                                bestLabel = myInputData[nodeList[j]];
                            }
                        }
                    }
                    if (!first)
                    {
                        colScratch[i] = bestLabel;
                    } else {
                        myTopoHelp->getNodeNeighbors(i, nodeList);
                        nodeList.push_back(i);
                        myGeoHelp->getGeoToTheseNodes(i, nodeList, distList);//ok, its a little silly to do this
                        numInRange = (int)nodeList.size();
                        for (int j = 0; j < numInRange; ++j)
                        {
                            if (markArray[nodeList[j]] == 1)
                            {
                                if (first || distList[j] < bestDist)
                                {
                                    first = false;
                                    bestDist = distList[j];
                                    bestLabel = myInputData[nodeList[j]];
                                }
                            }
                        }
                        if (!first)
                        {
                            colScratch[i] = bestLabel;
                        } else {
                            colScratch[i] = unusedLabel;
                        }
                    }
                } else {
                    colScratch[i] = myInputData[i];
                }
            }
            myLabelOut->setLabelKeysForColumn(thisCol, colScratch.getArray());
        }
    } else {
        myLabelOut->setNumberOfNodesAndColumns(numNodes, 1);
        const int32_t* myInputData = myLabel->getLabelKeyPointerForColumn(columnNum);
        myLabelOut->setColumnName(0, myLabel->getColumnName(columnNum) + " dilated");
        for (int i = 0; i < numNodes; ++i)
        {
            if (myInputData[i] == unusedLabel)
            {
                markArray[i] = 0;
            } else {
                markArray[i] = 1;
            }
        }
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
        CaretPointer<GeodesicHelper> myGeoHelp = mySurf->getGeodesicHelper();
        for (int i = 0; i < numNodes; ++i)
        {
            if (markArray[i] == 0)
            {
                vector<int32_t> nodeList;
                vector<float> distList;
                myGeoHelp->getNodesToGeoDist(i, myDist, nodeList, distList);
                int numInRange = (int)nodeList.size();
                bool first = true;
                float bestDist = -1.0f;
                int32_t bestLabel = unusedLabel;
                for (int j = 0; j < numInRange; ++j)
                {
                    if (markArray[nodeList[j]] == 1)
                    {
                        if (first || distList[j] < bestDist)
                        {
                            first = false;
                            bestDist = distList[j];
                            bestLabel = myInputData[nodeList[j]];
                        }
                    }
                }
                if (!first)
                {
                    colScratch[i] = bestLabel;
                } else {
                    myTopoHelp->getNodeNeighbors(i, nodeList);
                    nodeList.push_back(i);
                    myGeoHelp->getGeoToTheseNodes(i, nodeList, distList);//ok, its a little silly to do this
                    numInRange = (int)nodeList.size();
                    for (int j = 0; j < numInRange; ++j)
                    {
                        if (markArray[nodeList[j]] == 1)
                        {
                            if (first || distList[j] < bestDist)
                            {
                                first = false;
                                bestDist = distList[j];
                                bestLabel = myInputData[nodeList[j]];
                            }
                        }
                    }
                    if (!first)
                    {
                        colScratch[i] = bestLabel;
                    } else {
                        colScratch[i] = unusedLabel;
                    }
                }
            } else {
                colScratch[i] = myInputData[i];
            }
        }
        myLabelOut->setLabelKeysForColumn(0, colScratch.getArray());
    }
}

float AlgorithmLabelDilate::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmLabelDilate::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
