
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __SURFACE_OVERLAY_DECLARE__
#include "SurfaceOverlay.h"
#undef __SURFACE_OVERLAY_DECLARE__

#include "EventGetNodeDataFiles.h"
#include "EventManager.h"
#include "GiftiTypeFile.h"
#include <algorithm>

using namespace caret;


/**
 * Constructor.
 */
SurfaceOverlay::SurfaceOverlay()
: CaretObject()
{
    this->selectedType = SurfaceOverlayDataTypeEnum::NONE;
    this->opacity      = 1.0;
    
    this->name = "Overlay ";
    this->enabled = true;
    
    this->selectedDataFile = NULL;
    this->selectedColumnName = "";
}

/**
 * Destructor.
 */
SurfaceOverlay::~SurfaceOverlay()
{
    
}

/**
 * Set the number of this overlay.
 * 
 * @param overlayIndex
 *    Index for this overlay.
 */
void 
SurfaceOverlay::setOverlayNumber(const int32_t overlayIndex)
{    
    this->name = "Overlay " + AString::number(overlayIndex + 1);
}

/**
 * Get the selected overlay type.
 
 * @return The selected overlay type.
 */
SurfaceOverlayDataTypeEnum::Enum 
SurfaceOverlay::getSelectedType() const
{
    return this->selectedType;
}

/**
 * Set the selected overlay type.
 *
 * @param selectedType
 *    New type of overlay.
 */
void 
SurfaceOverlay::setSelectedType(SurfaceOverlayDataTypeEnum::Enum selectedType)
{
    this->selectedType = selectedType;
}

/**
 * Get the opacity.
 * 
 * @return  The opacity.
 */
float 
SurfaceOverlay::getOpacity() const
{
    return this->opacity;
}

/**
 * Set the opacity.
 *
 * @param opacity
 *    New value for opacity.
 */
void 
SurfaceOverlay::setOpacity(const float opacity)
{
    this->opacity = opacity;
}

AString 
SurfaceOverlay::getName() const
{
    return this->name;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SurfaceOverlay::toString() const
{
    return "SurfaceOverlay";
}

/**
 * @return Enabled status for this surface overlay.
 */
bool 
SurfaceOverlay::isEnabled() const
{
    return this->enabled;
}

/**
 * Set the enabled status for this surface overlay.
 * @param enabled
 *    New status.
 */
void 
SurfaceOverlay::setEnabled(const bool enabled)
{
    this->enabled = enabled;
}

/**
 * Return the selection information.
 * @param dataFilesOut
 *    Contains all data files that can be selected.
 * @param selectedFileOut
 *    The selected file.  May be NULL.
 * @param selectedColumnNameOut
 *    Name of selected column.
 * @param selectedColumnIndexOut
 *    Index of selected column.
 */
void 
SurfaceOverlay::getSelectionData(std::vector<GiftiTypeFile*>& dataFilesOut,
                                 GiftiTypeFile* &selectedFileOut,
                                 AString& selectedColumnNameOut,
                                 int32_t& selectedColumnIndexOut)
{
    dataFilesOut.clear();
    selectedFileOut = NULL;
    selectedColumnNameOut = "";
    selectedColumnIndexOut = -1;
    
    /**
     * Get the data files.
     */
    EventGetNodeDataFiles eventGetDataFiles;
    EventManager::get()->sendEvent(eventGetDataFiles.getPointer());
    eventGetDataFiles.getAllFiles(dataFilesOut);
    
    /*
     * Does selected data file still no longer exist?
     */
    if (std::find(dataFilesOut.begin(), 
                  dataFilesOut.end(),
                  this->selectedDataFile) == dataFilesOut.end()) {
        this->selectedDataFile = NULL;
    }
    
    /*
     * If selected data file is valid, see if selected
     * column is still valid.  If not, use first column.
     */
    if (this->selectedDataFile != NULL) {
        const int32_t columnIndex = this->selectedDataFile->getColumnIndexFromColumnName(this->selectedColumnName);
        if (columnIndex < 0) {
            this->selectedColumnName = this->selectedDataFile->getColumnName(0);
        }
    }
    else {
        /*
         * Look for a file that contains the selected column name.
         */
        if (this->selectedColumnName.isEmpty() == false) {
            for (std::vector<GiftiTypeFile*>::iterator iter = dataFilesOut.begin();
                 iter != dataFilesOut.end();
                 iter++) {
                GiftiTypeFile* giftiTypeFile = *iter;
                const int32_t column = giftiTypeFile->getColumnIndexFromColumnName(this->selectedColumnName);
                if (column >= 0) {
                    this->selectedDataFile = giftiTypeFile;
                    break;
                }
            }
        }
        
        /*
         * Use first column in first file.
         */
        if (this->selectedDataFile == NULL) {
            if (dataFilesOut.empty() == false) {
                this->selectedDataFile = dataFilesOut[0];
                this->selectedColumnName = dataFilesOut[0]->getColumnName(0);
            }
        }
    }
    
    selectedFileOut = this->selectedDataFile;
    if (selectedFileOut != NULL) {
        selectedColumnNameOut = this->selectedColumnName;
        selectedColumnIndexOut = this->selectedDataFile->getColumnIndexFromColumnName(selectedColumnNameOut);
    }
}

/**
 * Set the selected data file and column.
 * @param selectedDataFile 
 *    File that is selected.
 * @param selectedColumnName
 *    Column name that is selected.
 */
void 
SurfaceOverlay::setSelectionData(GiftiTypeFile* selectedDataFile,
                                 const int32_t selectedColumnIndex)
{
    this->selectedDataFile = selectedDataFile;
    this->selectedColumnName = selectedDataFile->getColumnName(selectedColumnIndex);
}


