
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

#include <iostream>

#include <QAction>
#include <QActionGroup>
#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#define __SPEC_FILE_DIALOG_DECLARE__
#include "SpecFileDialog.h"
#undef __SPEC_FILE_DIALOG_DECLARE__

#include "CaretAssert.h"
#include "EventDataFileRead.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventSurfaceColoringInvalidate.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "ProgressReportingDialog.h"
#include "SpecFile.h"
#include "SpecFileDataFile.h"
#include "SpecFileDataFileTypeGroup.h"
#include "StructureEnum.h"
#include "StructureEnumComboBox.h"
#include "WuQEventBlockingFilter.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class caret::SpecFileDialog 
 * \brief Dialog for selection of files in a spec file.
 *
 * Presents a dialog that allows the user to select data
 * files and their attributes for loading into the 
 * application.
 */

/**
 * Create a new instance of the SpecFile Dialog for
 * loading a spec file.  Run the returned dialog
 * with 'exec()'.
 * @param specFile
 *    The spec file that will be loaded.
 * @param parent
 *    Parent widget on which dialog is displayed.
 * @return Pointer to dialog that is displayed with
 * '->exec()'.  Caller MUST delete the dialog when
 * finished with it.
 */
SpecFileDialog* 
SpecFileDialog::createForLoadingSpecFile(SpecFile* specFile,
                            QWidget* parent)
{
    SpecFileDialog* sfd = new SpecFileDialog(SpecFileDialog::MODE_LOAD_SPEC,
                                             specFile,
                                             parent);
    return sfd;
}

/**
 * Launch a Spec File Dialog for opening data files.
 * @param specFile
 *    The spec file for file selection.
 * @param parent
 *    Parent widget on which dialog is displayed.
 * Dialog will be deleted automatically when it is closed.
 */
void 
SpecFileDialog::displayFastOpenDataFile(const SpecFile* specFile,
                                            QWidget* parent)
{
    /*
     * Spec file must be copied since the spec file is non-modal
     * for this usage and some operations reference the spec file's
     * content.
     */
    SpecFile* specFileCopy = new SpecFile(*specFile);
    SpecFileDialog* sfd = new SpecFileDialog(SpecFileDialog::MODE_FAST_OPEN,
                                             specFileCopy,
                                             parent);
    sfd->m_specFileCopyThatIsDeletedWhenDialogDestroyed = specFileCopy;
    sfd->setDeleteWhenClosed(true);
    sfd->setVisible(true);
    sfd->show();
    sfd->activateWindow();
}


/**
 * Constructor.
 */
SpecFileDialog::SpecFileDialog(const Mode mode,
                               SpecFile* specFile,
                               QWidget* parent)
: WuQDialogModal(("Spec File Data File Selection: " + specFile->getFileNameNoPath()),
                 parent)
{
    m_specFileCopyThatIsDeletedWhenDialogDestroyed = NULL;
    m_mode = mode;
    
    /*
     * Mac wheel event causes unintentional selection of combo box
     */
    m_comboBoxWheelEventBlockingFilter = new WuQEventBlockingFilter(this);
#ifdef CARET_OS_MACOSX
    m_comboBoxWheelEventBlockingFilter->setEventBlocked(QEvent::Wheel, 
                                                            true);
#endif // CARET_OS_MACOSX
    
    m_specFile = specFile;
    QWidget* fileGroupWidget = new QWidget();
    QVBoxLayout* fileGroupLayout = new QVBoxLayout(fileGroupWidget);
    
    /*
     * File Group Toolbar
     */
    QToolBar* fileGroupToolBar = new QToolBar();
    QLabel* viewFilesLabel = new QLabel("View Files: ");
    fileGroupToolBar->addWidget(viewFilesLabel);
    
    /*
     * Action group for all buttons in tool bar
     */
    QActionGroup* toolBarActionGroup = new QActionGroup(this);
    toolBarActionGroup->setExclusive(true);
    QObject::connect(toolBarActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(toolBarButtonTriggered(QAction*)));
    
    /*
     * All action tool button
     */
    QAction* allToolButtonAction = WuQtUtilities::createAction("All", 
                                                               "Show files of all types", 
                                                               this);
    allToolButtonAction->setData(qVariantFromValue(0));
    allToolButtonAction->setCheckable(true);
    QToolButton* allToolButton = new QToolButton(this);
    allToolButton->setDefaultAction(allToolButtonAction);
    fileGroupToolBar->addWidget(allToolButton);
    toolBarActionGroup->addAction(allToolButtonAction);

    /*
     * Get ALL of the connectivity file's in a single group
     */
    std::vector<SpecFileDataFile*> connectivityFiles;
    m_specFile->getAllConnectivityFileTypes(connectivityFiles);
    
    bool haveConnectivityFiles = false;
    bool haveSceneFiles = false;
    
    /*
     * Display each type of data file
     */
    const int32_t numGroups = m_specFile->getNumberOfDataFileTypeGroups();
    for (int32_t ig = 0 ; ig < numGroups; ig++) {
        /*
         * File type of group
         */
        SpecFileDataFileTypeGroup* group = m_specFile->getDataFileTypeGroup(ig);
        const DataFileTypeEnum::Enum dataFileType = group->getDataFileType();
        
        std::vector<SpecFileDataFile*> dataFileInfoVector;
        
        /*
         * Is this a connectivity group?
         */
        AString groupName;
        if (DataFileTypeEnum::isConnectivityDataType(dataFileType)) {
            if (haveConnectivityFiles == false) {
                haveConnectivityFiles = true;
                m_specFile->getAllConnectivityFileTypes(dataFileInfoVector);
                groupName = "Connectivity";
            }
        }
        else {
            /*
             * Create a widget for displaying the group's files
             */
            const int32_t numFiles = group->getNumberOfFiles();
            for (int32_t j = 0; j < numFiles; j++) {
                dataFileInfoVector.push_back(group->getFileInformation(j));
            }
            groupName = DataFileTypeEnum::toGuiName(dataFileType);
        }
                          
        /*
         * Group has files?
         */
        GuiSpecGroup* guiSpecGroup = createDataTypeGroup(dataFileType, 
                                                               dataFileInfoVector,
                                                               groupName);
        if (guiSpecGroup != NULL) {
            m_dataTypeGroups.push_back(guiSpecGroup);
            fileGroupLayout->addWidget(guiSpecGroup->widget);
            
            /*
             * Action and toolbutton for limiting display to groups files
             */
            QAction* groupToolButtonAction = WuQtUtilities::createAction(groupName, 
                                                                         "Show only files of this type", 
                                                                         this);
            groupToolButtonAction->setData(qVariantFromValue((void*)guiSpecGroup));
            groupToolButtonAction->setCheckable(true);
            
            QToolButton* groupToolButton = new QToolButton(this);
            groupToolButton->setDefaultAction(groupToolButtonAction);
            fileGroupToolBar->addWidget(groupToolButton);
            
            toolBarActionGroup->addAction(groupToolButtonAction);
            
            if (dataFileType == DataFileTypeEnum::SCENE) {
                haveSceneFiles = true;
            }
        }
    }
    
    fileGroupLayout->addStretch();
    
    /*
     * Show everything
     */
    allToolButtonAction->trigger();
    
    /*
     * Select toolbar
     */
    QToolBar* selectToolBar = createSelectToolBar();
    
    /*
     * Toolbars in one widget with no spacing
     */ 
    QWidget* toolbarWidget = new QWidget();
    QVBoxLayout* toolbarWidgetLayout = new QVBoxLayout(toolbarWidget);
    WuQtUtilities::setLayoutMargins(toolbarWidgetLayout, 0, 0);
    toolbarWidgetLayout->addWidget(fileGroupToolBar);
    toolbarWidgetLayout->addWidget(selectToolBar);
    
    /*
     * Options
     */
    QWidget* optionsWidget = NULL;
    if (m_mode == MODE_FAST_OPEN) {
        m_autoCloseFastOpenCheckBox = new QCheckBox("Auto Close");
        m_autoCloseFastOpenCheckBox->setChecked(true);
        WuQtUtilities::setToolTipAndStatusTip(m_autoCloseFastOpenCheckBox, 
                                              "If checked, the dialog will be closed\n"
                                              "immediately after loading a file.");
        optionsWidget = new QWidget();
        QHBoxLayout* optionsLayout = new QHBoxLayout(optionsWidget);
        WuQtUtilities::setLayoutMargins(optionsLayout, 0, 0);
#ifdef CARET_OS_MACOSX
        optionsLayout->addStretch();
#endif
        optionsLayout->addWidget(m_autoCloseFastOpenCheckBox);
#ifndef CARET_OS_MACOSX
        optionsLayout->addStretch();
#endif
    }
    
    /*
     * Add contents to the dialog
     */
    setTopBottomAndCentralWidgets(toolbarWidget,
                                        fileGroupWidget,
                                        optionsWidget);
    
    m_loadScenesPushButton = NULL;
    switch (m_mode) {
        case MODE_FAST_OPEN:
            /*
             * Hide OK button.
             */
            setOkButtonText("");
            setCancelButtonText("Close");
            break;
        case MODE_LOAD_SPEC:
            /*
             * Change OK button to Load
             */
            setOkButtonText("Load");
            
            /*
             * Load scenes push button
             */
            m_loadScenesPushButton = addUserPushButton("Load Scenes", QDialogButtonBox::AcceptRole);
            m_loadScenesPushButton->setEnabled(haveSceneFiles);
            break;
    }
}

/**
 * Destructor.
 */
SpecFileDialog::~SpecFileDialog()
{    
    const int32_t numGroups = static_cast<int32_t>(m_dataTypeGroups.size());
    for (int32_t ig = 0; ig < numGroups; ig++) {
        delete m_dataTypeGroups[ig];
    }
    if (m_specFileCopyThatIsDeletedWhenDialogDestroyed != NULL) {
        delete m_specFileCopyThatIsDeletedWhenDialogDestroyed;
        m_specFileCopyThatIsDeletedWhenDialogDestroyed = NULL;
    }
}

/**
 * Create the select tool bar.
 */
QToolBar* 
SpecFileDialog::createSelectToolBar()
{
    m_selectAllFilesToolButtonAction = WuQtUtilities::createAction("All", "Select all files", this);
    m_selectNoneFilesToolButtonAction = WuQtUtilities::createAction("None", "Deselect all files", this);
    
    QActionGroup* selectActionGroup = new QActionGroup(this);
    selectActionGroup->addAction(m_selectAllFilesToolButtonAction);
    selectActionGroup->addAction(m_selectNoneFilesToolButtonAction);
    QObject::connect(selectActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(selectToolButtonTriggered(QAction*)));
    
    QLabel* selectLabel = new QLabel("Select Files: ");
    
    QToolBar* toolbar = new QToolBar();
    toolbar->addWidget(selectLabel);
    toolbar->addAction(m_selectAllFilesToolButtonAction);
    toolbar->addAction(m_selectNoneFilesToolButtonAction);
    
    return toolbar;
}

void 
SpecFileDialog::selectToolButtonTriggered(QAction* action)
{
    const bool status = (action == m_selectAllFilesToolButtonAction);
    
    const int32_t numGroups = static_cast<int32_t>(m_dataTypeGroups.size());
    
    for (int32_t i = 0; i < numGroups; i++) {
        GuiSpecGroup* gsg = m_dataTypeGroups[i];
        if (gsg->widget->isVisible()) {
            const int32_t numFiles = static_cast<int32_t>(gsg->dataFiles.size());
            for (int32_t i = 0; i < numFiles; i++) {
                if (gsg->dataFiles[i]->m_dataFileInfo->isRemovedFromSpecFileWhenWritten() == false) {
                    gsg->dataFiles[i]->m_selectionCheckBox->setChecked(status);
                }
            }
        }
    }
}

/**
 * Called when a tool button is selected.
 * @param action
 *   Action for button that was pressed.
 */
void 
SpecFileDialog::toolBarButtonTriggered(QAction* action)
{
    void* p = action->data().value<void*>();
    
    const int32_t numGroups = static_cast<int32_t>(m_dataTypeGroups.size());
    
    GuiSpecGroup* selectedGroup = NULL;
    
    if (p != NULL) {
        selectedGroup = (GuiSpecGroup*)p;
    }
    
    for (int32_t i = 0; i < numGroups; i++) {
        GuiSpecGroup* gsg = m_dataTypeGroups[i];
        bool showIt = true;
        if (selectedGroup != NULL) {
            if (gsg != selectedGroup) {
                showIt = false;
            }
        }
        gsg->widget->setVisible(showIt);
    }
}

/**
 * List the files in the data file type group in a widget.
 * 
 * @param dataFileType 
 *   Type of files
 * @param dataFileInfoVector
 *   Vector containing info about each file.
 * @return 
 *   If there are files in the group, a widget listing the file
 *   or NULL if no files in the group.
 */
SpecFileDialog::GuiSpecGroup* 
SpecFileDialog::createDataTypeGroup(const DataFileTypeEnum::Enum dataFileType,
                                    std::vector<SpecFileDataFile*>& dataFileInfoVector,
                                    const AString& groupName)
{
    AString specFilePath;
    FileInformation specFileInfo(m_specFile->getFileName());
    if (specFileInfo.isAbsolute()) {
        specFilePath = specFileInfo.getPathName();
    }
    
    const int32_t numFiles = static_cast<int32_t>(dataFileInfoVector.size());
    if (numFiles <= 0) {
        return NULL;
    }
    
    GuiSpecGroup* guiSpecGroup = new GuiSpecGroup();
    
    int ctr = 0;
    const int COLUMN_OPEN_BUTTON = ctr++;
    const int COLUMN_CHECKBOX  = ctr++;
    const int COLUMN_METADATA  = ctr++;
    const int COLUMN_REMOVE    = ctr++;
    const int COLUMN_STRUCTURE = ctr++;
    const int COLUMN_NAME      = ctr++;
    const int NUMBER_OF_COLUMNS = ctr;
    
    QGroupBox* groupBox = new QGroupBox(groupName);
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    WuQtUtilities::setLayoutMargins(gridLayout, 4, 2);
    for (int32_t i = 0; i < NUMBER_OF_COLUMNS; i++) {
        gridLayout->setColumnStretch(i, 0);
    }
    gridLayout->setColumnStretch(NUMBER_OF_COLUMNS, 100);
    

    const bool hasStructure = DataFileTypeEnum::isFileUsedWithOneStructure(dataFileType);
    
    for (int idf = 0; idf < numFiles; idf++) {
        SpecFileDataFile* dataFileInfo = dataFileInfoVector[idf];
        
        GuiSpecDataFileInfo* dfi = new GuiSpecDataFileInfo(this,
                                                           specFilePath,
                                                           dataFileInfo,
                                                           hasStructure);
                                                 
        const int iRow = gridLayout->rowCount();
        gridLayout->addWidget(dfi->m_openFilePushButton, iRow, COLUMN_OPEN_BUTTON);
        gridLayout->addWidget(dfi->m_selectionCheckBox, iRow, COLUMN_CHECKBOX);
        gridLayout->addWidget(dfi->m_metadataToolButton, iRow, COLUMN_METADATA);
        gridLayout->addWidget(dfi->m_removeToolButton, iRow, COLUMN_REMOVE);
        gridLayout->addWidget(dfi->m_structureEnumComboBox->getWidget(), iRow, COLUMN_STRUCTURE);
        gridLayout->addWidget(dfi->m_nameLabel, iRow, COLUMN_NAME);
        
        /*
         * Do not let wheel alter the structure combo box
         */
        dfi->m_structureEnumComboBox->getWidget()->installEventFilter(m_comboBoxWheelEventBlockingFilter);
        
        switch (m_mode) {
            case MODE_FAST_OPEN:
                dfi->m_selectionCheckBox->setVisible(false);
                dfi->m_removeToolButton->setVisible(false);
                dfi->m_structureEnumComboBox->getWidget()->blockSignals(true); // do not allow spec to change
                QObject::connect(dfi, SIGNAL(signalFileWasLoaded()),
                                 this, SLOT(fastOpenDataFileWasLoaded()));
                break;
            case MODE_LOAD_SPEC:
                dfi->m_openFilePushButton->setVisible(false);
                break;
        }
        
        guiSpecGroup->dataFiles.push_back(dfi);
    }
    
    guiSpecGroup->widget = groupBox;
    
    return guiSpecGroup;
}

/**
 * Called when a push button was added using addUserPushButton().
 * Subclasses MUST override this if user push buttons were 
 * added using addUserPushButton().
 *
 * @param userPushButton
 *    User push button that was pressed.
 * @return 
 *    The result that indicates action that should be taken
 *    as a result of the button being pressed.
 */
WuQDialogModal::ModalDialogUserButtonResult 
SpecFileDialog::userButtonPressed(QPushButton* userPushButton)
{
    if (userPushButton == m_loadScenesPushButton) {
        /*
         * Load all of the scene files but nothing else
         */
        m_specFile->setAllSceneFilesSelectedAndAllOtherFilesNotSelected();
        
        writeUpdatedSpecFile(true);
        
        GuiManager::get()->processShowSceneDialog(GuiManager::get()->getOpenBrowserWindow());
        
        return RESULT_ACCEPT;
    }
    else {
        CaretAssert(0);
    }
    
    return RESULT_NONE;    
}

/**
 * Called when user presses the OK button.
 */
void 
SpecFileDialog::okButtonClicked()
{
    const int32_t numGroups = static_cast<int32_t>(m_dataTypeGroups.size());
    for (int32_t ig = 0; ig < numGroups; ig++) {
        GuiSpecGroup* guiSpecGroup = m_dataTypeGroups[ig];
        const int32_t numFiles = static_cast<int32_t>(guiSpecGroup->dataFiles.size());
        for (int j = 0; j < numFiles; j++) {
            GuiSpecDataFileInfo* fileInfo = guiSpecGroup->dataFiles[j];
            fileInfo->m_dataFileInfo->setSelected(fileInfo->m_selectionCheckBox->isChecked());
        }        
    }

    writeUpdatedSpecFile(true);
   
    WuQDialogModal::okButtonClicked();
}

/**
 * Called when user presses the Cancel button.
 */
void 
SpecFileDialog::cancelButtonClicked()
{
    writeUpdatedSpecFile(true);
    WuQDialogModal::cancelButtonClicked();
}

/**
 * Write the SpecFile if it has been updated.
 */
void 
SpecFileDialog::writeUpdatedSpecFile(const bool confirmIt)
{
    if (m_specFile->hasBeenEdited() == false) {
        return;
    }
    
    bool writeIt = true;
    
    if (confirmIt) {
        if (WuQMessageBox::warningYesNo(this,
                                        "You have changed the Spec File.  Save changes?")  == false) {
            writeIt = false;
        }
    }
    
    if (writeIt) {
        try {
            m_specFile->writeFile(m_specFile->getFileName());
        }
        catch (const DataFileException& e) {
            WuQMessageBox::errorOk(this, e.whatString());
        }
    }
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SpecFileDialog::toString() const
{
    return "SpecFileDialog";
}

/**
 * Called when a data file is loaded in
 * fast open mode.
 */
void 
SpecFileDialog::fastOpenDataFileWasLoaded()
{
    if (m_autoCloseFastOpenCheckBox->isChecked()) {
        close(); 
    }
}


//=====================================================================================
/**
 * Constructor.
 * @param dataFileInfo
 *    Info about the data file.
 * @param isStructureFile
      If true, file is structure related.
 */
GuiSpecDataFileInfo::GuiSpecDataFileInfo(QObject* parent,
                                         const AString& specFilePath,
                                         SpecFileDataFile* dataFileInfo,
                                         const bool isStructureFile)
: QObject(parent)
{
    m_dataFileInfo = dataFileInfo;
    
    m_openFilePushButton = new QPushButton("Open");
    m_openFilePushButton->setAutoDefault(false);
    WuQtUtilities::setToolTipAndStatusTip(m_openFilePushButton,
                                          "Pressing this button will open the data file.");
    QObject::connect(m_openFilePushButton, SIGNAL(clicked()),
                     this, SLOT(openFilePushButtonClicked()));
    
    m_selectionCheckBox = new QCheckBox(" ");
    m_selectionCheckBox->setChecked(dataFileInfo->isSelected());
    
    m_metadataAction = WuQtUtilities::createAction("M",
                                                     "View the file's metadata",
                                                     this,
                                                     this,
                                                     SLOT(metadataActionTriggered()));
    m_metadataToolButton = new QToolButton();
    m_metadataToolButton->setDefaultAction(m_metadataAction);
    
    
    m_removeAction = WuQtUtilities::createAction("X",
                                                     "Remove file from the spec file (does NOT delete file)",
                                                     this,
                                                     this,
                                                     SLOT(removeActionTriggered(bool)));
    m_removeAction->setCheckable(true);
    m_removeAction->setChecked(false);
    m_removeToolButton = new QToolButton();
    m_removeToolButton->setDefaultAction(m_removeAction);
    
    m_structureEnumComboBox = new StructureEnumComboBox(this);
    m_structureEnumComboBox->setSelectedStructure(dataFileInfo->getStructure());
    QObject::connect(m_structureEnumComboBox, SIGNAL(structureSelected(const StructureEnum::Enum)),
                     this, SLOT(structureSelectionChanged(const StructureEnum::Enum)));
    if (isStructureFile == false) {
        m_structureEnumComboBox->getWidget()->setVisible(false);
        m_structureEnumComboBox->getWidget()->blockSignals(true);
    }
    
    AString dataFileName = dataFileInfo->getFileName();
    FileInformation fileInfo(dataFileName);
    if (fileInfo.isAbsolute()) {
        bool updatedName = false;
        if (specFilePath.isEmpty() == false) {
            if (fileInfo.getPathName() == specFilePath) {
                dataFileName = fileInfo.getFileName();
                updatedName = true;
            }
        }
        if (updatedName == false) {
            dataFileName = (fileInfo.getFileName()
                            + " ("
                            + fileInfo.getPathName()
                            + ")");
        }
    }
    m_nameLabel = new QLabel(dataFileName);
    
    m_widgetGroup = new WuQWidgetObjectGroup(this);
    m_widgetGroup->add(m_selectionCheckBox);
    m_widgetGroup->add(m_metadataToolButton);
    if (isStructureFile) {
        m_widgetGroup->add(m_structureEnumComboBox->getWidget());
    }
    m_widgetGroup->add(m_nameLabel);
}

/**
 * Destructor.
 */
GuiSpecDataFileInfo::~GuiSpecDataFileInfo()
{
    
}

/**
 * Called when Open file pushbutton clicked.
 */
void 
GuiSpecDataFileInfo::openFilePushButtonClicked()
{
    AString errorMessages;
    const AString name = m_dataFileInfo->getFileName();
    bool isValidType = false;
    DataFileTypeEnum::Enum fileType = DataFileTypeEnum::fromFileExtension(name, &isValidType);
    StructureEnum::Enum structure = m_structureEnumComboBox->getSelectedStructure();
    if (isValidType) {
        EventDataFileRead loadFileEvent(GuiManager::get()->getBrain(),
                                        false);
        loadFileEvent.addDataFile(structure,
                                  fileType,
                                  name);
        
        ProgressReportingDialog::runEvent(&loadFileEvent,
                                          m_openFilePushButton,
                                          "Loading File");
        
        if (loadFileEvent.isError()) {
            if (errorMessages.isEmpty() == false) {
                errorMessages += "\n";
            }
            errorMessages += loadFileEvent.getErrorMessage();
        }                    
    }
    else {
        if (errorMessages.isEmpty() == false) {
            errorMessages += "\n";
        }
        errorMessages += ("Extension for " + name + " does not match a Caret file type");
    }
    
    if (errorMessages.isEmpty() == false) {
        QMessageBox::critical(m_openFilePushButton, 
                              "ERROR", 
                              errorMessages);
    }
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    if (errorMessages.isEmpty()) {
        emit signalFileWasLoaded();
    }
}

/**
 * Called when structure selection control is changed.
 * @param structure
 *    New structure.
 */
void 
GuiSpecDataFileInfo::structureSelectionChanged(const StructureEnum::Enum structure)
{
    m_dataFileInfo->setStructure(structure);
}

/**
 * Called when metadata button is pressed.
 */
void 
GuiSpecDataFileInfo::metadataActionTriggered()
{
    WuQMessageBox::informationOk(m_metadataToolButton, 
                                 "Editing/Viewing of metadata has not been implemented.");
}

/**
 * Called when remove button is pressed.
 * @param status
 *   Status of action (selected or not)
 */
void GuiSpecDataFileInfo::removeActionTriggered(bool status)
{
    const bool removeIt = status;
    m_dataFileInfo->setRemovedFromSpecFileWhenWritten(removeIt);
    if (removeIt) {
        m_selectionCheckBox->setChecked(false);
    }
    m_widgetGroup->setDisabled(removeIt);
}
