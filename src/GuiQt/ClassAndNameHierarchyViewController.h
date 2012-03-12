#ifndef __CLASS_AND_NAME_HIERARCHY_VIEW_CONTROLLER__H_
#define __CLASS_AND_NAME_HIERARCHY_VIEW_CONTROLLER__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <vector>

#include <QWidget>

#include "AString.h"

class QCheckBox;
class QTreeWidget;
class QTreeWidgetItem;

namespace caret {

    class BorderFile;
    class ClassAndNameHierarchyModel;
    class ClassAndNameHierarchySelectedItem;
    
    class ClassAndNameHierarchyViewController : public QWidget {
        
        Q_OBJECT

    public:
        ClassAndNameHierarchyViewController(const int32_t browserWindowIndex,
                                            QWidget* parent = 0);
        
        virtual ~ClassAndNameHierarchyViewController();
        
        void updateContents(std::vector<BorderFile*> borderFiles);
        
    signals:
        void itemSelected(ClassAndNameHierarchySelectedItem* selectedItem);
        
    private slots:
        void treeWidgetItemChanged(QTreeWidgetItem* item,
                                   int column);
        

    private:
        ClassAndNameHierarchyViewController(const ClassAndNameHierarchyViewController&);

        ClassAndNameHierarchyViewController& operator=(const ClassAndNameHierarchyViewController&);
        
        void updateContents(std::vector<ClassAndNameHierarchyModel*>& ClassAndNameHierarchyModels);
        
        void deleteItemSelectionInfo();
        
        QTreeWidgetItem* createTreeWidgetItem(const AString& name,
                                              const bool isSelected,
                                              ClassAndNameHierarchySelectedItem* selectionInfo);
        
        std::vector<ClassAndNameHierarchyModel*> classAndNameHierarchyModels;
        
        QTreeWidget* treeWidget;
        
        std::vector<ClassAndNameHierarchySelectedItem*> itemSelectionInfo;

        int32_t browserWindowIndex;
    };
        
#ifdef __CLASS_AND_NAME_HIERARCHY_VIEW_CONTROLLER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CLASS_AND_NAME_HIERARCHY_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CLASS_AND_NAME_HIERARCHY_VIEW_CONTROLLER__H_
