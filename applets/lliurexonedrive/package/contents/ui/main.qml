import QtQuick 2.6
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.6 as QQC2
import QtQml.Models 2.3
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 2.0 as Components

import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.kirigami 2.12 as Kirigami

import org.kde.plasma.private.lliurexonedrive 1.0
// Item - the most basic plasmoid component, an empty container.

Item {

    id:lliurexonedriveApplet
    LliurexOneDriveWidget{
        id:lliurexOneDriveWidget
    }

    Plasmoid.status: {
        /* Warn! Enum types are accesed through ClassName not ObjectName */
        switch (lliurexOneDriveWidget.status){
            case LliurexOneDriveWidget.ActiveStatus:
                return PlasmaCore.Types.ActiveStatus
            case LliurexOneDriveWidget.PassiveStatus:
                return PlasmaCore.Types.PassiveStatus
        }
        return  PlasmaCore.Types.ActiveStatus
    }

    Plasmoid.switchWidth: units.gridUnit * 5
    Plasmoid.switchHeight: units.gridUnit * 5
    Plasmoid.icon:lliurexOneDriveWidget.iconName
    Plasmoid.toolTipMainText: lliurexOneDriveWidget.toolTip
    Plasmoid.toolTipSubText: lliurexOneDriveWidget.subToolTip

    Component.onCompleted: {
        plasmoid.removeAction("configure");
        plasmoid.setAction("launchOneDrive", i18n("Open Lliurex OneDrive"), "configure"); 
    }

    Plasmoid.preferredRepresentation: Plasmoid.fullRepresentation
    
    Plasmoid.fullRepresentation: Item{
        id:root
        Layout.fillWidth:true
        StackLayout{
            id:stackLayout
            currentIndex:lliurexOneDriveWidget.currentIndex
            width:parent.width
            height:parent.height

            GridLayout{
                id:spacesLayout
                rows: 2
                flow: GridLayout.TopToBottom
                rowSpacing:10
                width:parent.width
                                
                PlasmaExtras.ScrollArea {
                    Layout.topMargin:10
                    Layout.bottomMargin:10
                    Layout.leftMargin:5
                    Layout.rightMargin:5
                    implicitWidth:parent.width-10
                    implicitHeight:300
                    ListView{
                        id:listSpaceView
                        focus:true
                        model:lliurexOneDriveWidget.spacesModel
                        currentIndex: -1
                        boundsBehavior: Flickable.StopAtBounds
                        interactive: contentHeight > height
                        highlight: Rectangle { color: "#add8e6"; opacity:0.8;border.color:"#53a1c9" }
                        highlightMoveDuration: 0
                        highlightResizeDuration: 0
                        delegate: ListDelegateSpaceItem {
                            idSpace: model.id
                            nameSpace: model.name
                            statusSpace: model.status
                            isRunningSpace: model.isRunning
                            localFolderWarning: model.localFolderWarning
                        }
                        Kirigami.PlaceholderMessage { 
                            id: emptySpaceHint
                            anchors.centerIn: parent
                            width: parent.width - (units.largeSpacing * 4)
                            visible: {
                                if (listSpaceView.count === 0){
                                    return true
                                }else{
                                    return false
                                } 
                            }    
                            text: i18n("Information is not available")
                        }
                        Kirigami.PlaceholderMessage {
                            id: showSpaceHint
                            anchors.centerIn: parent
                            width: parent.width - (units.largeSpacing * 4)
                            visible: listSpaceView.count>0?false:true
                            text: i18n("No spaces configured")
                        }
                       
                    }
                }
                RowLayout{
                    id:helpRow
                    Layout.fillWidth:true
                    Layout.leftMargin:5
                    Layout.rightMargin:5

                    Components.Label{
                        id:helpText
                        text:i18n("See help documentation")
                        Layout.fillWidth:true
                    }

                   QQC2.ToolButton {
                        width:35
                        height:35
                        icon.name:"help-contents.svg"
                        onClicked:lliurexOneDriveWidget.openHelp() 
                        QQC2.ToolTip{
                            text:i18n("Click to see help")
                        }
                    } 
                }
            }
            
            GridLayout{
                id: optionsAccount
                rows: 7
                flow: GridLayout.TopToBottom
                rowSpacing:10
                width:parent.width

                RowLayout{
                    id:head
                    Layout.fillWidth:true
                    QQC2.ToolButton {
                        height:35
                        width:355
                        Layout.rightMargin:optionsAccount.width/3 - 50
                        icon.name:"arrow-left.svg"
                        QQC2.ToolTip{
                            text:i18n("Back to main view")
                        }
                        onClicked:lliurexOneDriveWidget.manageNavigation(0)
                    } 

                    Components.Label{
                        id:headText
                        text:i18n("Space details")
                        font.italic:true
                        font.pointSize:11
                        Layout.fillWidth:true
                        Layout.alignment:Qt.AlignHCenter
                    }
                    QQC2.ToolButton {
                        id:configureBtn
                        height:35
                        width:35
                        Layout.rightMargin:5
                        icon.name:"configure.svg"
                        QQC2.ToolTip{
                            text:i18n("Manage space")
                        }                        onClicked:lliurexOneDriveWidget.launchOneDrive(true)
                    } 
                } 
                 RowLayout{
                    id:spaceMailRow
                    Layout.fillWidth:true
                    Layout.leftMargin:5
                    Layout.rightMargin:5
                    Layout.bottomMargin:5

                    Components.Label{
                        id:spaceMail
                        text:i18n("Email associated:")
                        Layout.rightMargin:5
                    }
                    Components.Label{
                        id:spaceMailVa5lue
                        text:lliurexOneDriveWidget.spaceMail
                        Layout.fillWidth:true
                    }
                }
                RowLayout{
                    id:spaceTypeRow
                    Layout.fillWidth:true
                    Layout.leftMargin:5
                    Layout.rightMargin:5

                    Components.Label{
                        id:spaceType
                        text:i18n("Type:")
                        Layout.rightMargin:5

                    }
                    Components.Label{
                        id:spaceTypeValue
                        text:{
                            if (lliurexOneDriveWidget.spaceType=="onedrive"){
                                "OneDrive"
                            }else{
                                "SharePoint"
                            }
                        }
                        Layout.fillWidth:true
                    }
                }
                RowLayout {
                    id:folderRow
                    Layout.leftMargin:5
                    Layout.rightMargin:5
                    Layout.fillWidth:true

                    Components.Label{
                        id:oneDriveFolder
                        text:i18n("Local folder:")
                        Layout.rightMargin:5
                    }
                    Components.Label{
                        id:oneDriveFolderValue
                        text:lliurexOneDriveWidget.oneDriveFolder
                        Layout.preferredWidth:295
                        elide:Text.ElideMiddle
                    }
                    QQC2.ToolButton {
                        width:35
                        height:35
                        icon.name:"document-open-folder.svg"
                        onClicked:lliurexOneDriveWidget.openFolder()
                        QQC2.ToolTip{
                            text:i18n("Click to open folder")
                        }
                    } 
                }

                RowLayout{
                    id:freeSpaceRow
                    Layout.fillWidth:true
                    Layout.leftMargin:5
                    Layout.rightMargin:5

                    Components.Label{
                        id:freeSpace
                        text:i18n("Free Space:")
                        Layout.rightMargin:5
                    }
                    Components.Label{
                        id:freeSpaceValue
                        text:{
                            if (lliurexOneDriveWidget.freeSpace!=""){
                                lliurexOneDriveWidget.freeSpace
                            }else{
                                i18n("Information not available")
                            }
                        }
                        Layout.fillWidth:true
                    }
                }

                RowLayout{
                    id:syncRow
                    Layout.fillWidth:true
                    Layout.leftMargin:5
                    Layout.rightMargin:5

                    Components.Label{
                        id:syncText
                        text:i18n("Synchronization:")
                        Layout.rightMargin:5
                    }

                    Components.Label{
                        id:syncStatus
                        text:lliurexOneDriveWidget.syncStatus?i18n("Running"):i18n("Stopped")
                        Layout.fillWidth:true
                    }

                    QQC2.ToolButton {
                        width:35
                        height:35
                        icon.name:lliurexOneDriveWidget.syncStatus?"kt-stop.svg":"kt-start.svg"
                        enabled:!lliurexOneDriveWidget.lliurexOneDriveOpen
                        onClicked:lliurexOneDriveWidget.manageSync() 
                        QQC2.ToolTip{
                            text:lliurexOneDriveWidget.syncStatus?i18n("Click to stop synchronization"):i18n("Click to start synchronization")
                        }
                    } 
                } 
                RowLayout{
                    id:latestFiles
                    Layout.fillWidth:true
                    Layout.leftMargin:5
                    Layout.rightMargin:5

                    Components.Label{
                        id:latestText
                        text:i18n("Recently modified files")
                        Layout.fillWidth:true
                    }
                   
                    QQC2.ToolButton {
                        width:35
                        height:35
                        icon.name:"arrow-right.svg"
                        onClicked:{
                            lliurexOneDriveWidget.getLatestFiles()
                            listView.forceActiveFocus()
                            if (listView.count > 0){
                                listView.currentIndex=0
                            }
                            lliurexOneDriveWidget.manageNavigation(2)
                        }
                        QQC2.ToolTip{
                            text:i18n("Click to see the list")
                        }
                    } 
                }
                  
            } 
            GridLayout{
                id:filesLayout
                rows: 2
                flow: GridLayout.TopToBottom
                rowSpacing:10
                width:parent.width
                RowLayout{
                    id:headLatestFiles
                    Layout.fillWidth:true
                    QQC2.ToolButton {
                        height:35
                        width:35
                        icon.name:"arrow-left.svg"
                        Layout.rightMargin:filesLayout.width/3-35/2
                        QQC2.ToolTip{   
                            text:i18n("Back to space view")
                        }
                        onClicked:lliurexOneDriveWidget.manageNavigation(1) 
                    } 
                   Components.Label{
                        id:headFilesText
                        text:i18n("List of files")
                        font.italic:true
                        font.pointSize:11
                        Layout.fillWidth:true
                        Layout.alignment:Qt.AlignHCenter
                    }
                    QQC2.ToolButton {
                        width:35
                        height:35
                        Layout.alignment:Qt.AlignRight
                        icon.name:"view-refresh"
                        QQC2.ToolTip{
                            text:i18n("Click to refresh list")
                        }
                        onClicked:{
                            lliurexOneDriveWidget.getLatestFiles()
                            listView.forceActiveFocus()
                            if (listView.count > 0){
                                listView.currentIndex=0
                            }

                        }
 
                    } 
                }
               PlasmaExtras.ScrollArea {
                    Layout.topMargin:10
                    Layout.bottomMargin:10
                    Layout.leftMargin:5
                    Layout.rightMargin:5
                    implicitWidth:parent.width-10
                    implicitHeight:250
                    ListView{
                        id:listView
                        focus:true
                        model:lliurexOneDriveWidget.filesModel
                        currentIndex: -1
                        boundsBehavior: Flickable.StopAtBounds
                        interactive: contentHeight > height
                        highlight: Rectangle { color: "#add8e6"; opacity:0.8;border.color:"#53a1c9" }
                        highlightMoveDuration: 0
                        highlightResizeDuration: 0
                        delegate: ListDelegateFileItem {
                            fileName: model.fileName
                            filePath: model.filePath
                            fileDate: model.fileDate
                            fileTime: model.fileTime
                        }
                         Kirigami.PlaceholderMessage { 
                            id: emptyHint
                            anchors.centerIn: parent
                            width: parent.width - (units.largeSpacing * 4)
                            visible: {
                                if ((listView.count === 0)&&(!lliurexOneDriveWidget.showSearchFiles)){
                                    return true
                                }else{
                                    return false
                                } 
                            }    
                            text: i18n("Information is not available")
                        }
                         Kirigami.PlaceholderMessage {
                            id: showHint
                            anchors.centerIn: parent
                            width: parent.width - (units.largeSpacing * 4)
                            visible: lliurexOneDriveWidget.showSearchFiles
                            text: i18n("Searching information. Wait a moment...")
                        }
                       
                    }
                }
            }
        }
    }
    

    function action_launchOneDrive() {
        lliurexOneDriveWidget.launchOneDrive(false)
    }
}   
