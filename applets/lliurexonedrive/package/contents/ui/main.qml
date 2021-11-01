import QtQuick 2.2
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.6
import QtQml.Models 2.3
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 2.0 as Components
import org.kde.plasma.components 3.0 as Components3

import org.kde.plasma.extras 2.0 as PlasmaExtras

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
        GridLayout{
            id: optionsAccount
            rows: 3
            flow: GridLayout.TopToBottom
            rowSpacing:10
            width:parent.width

            RowLayout {
                id:folderRow
                Layout.fillWidth:true
                Layout.leftMargin:5
                Layout.rightMargin:5

                Components.Label{
                    id:oneDriveFolder
                    text:i18n("OneDrive folder:")
                    Layout.rightMargin:5
                }
                Components.Label{
                    id:oneDriveFolderValue
                    text:lliurexOneDriveWidget.oneDriveFolder
                    Layout.fillWidth:true
                }
                Components3.ToolButton {
                    width:35
                    height:35
                    display:AbstractButton.IconOnly
                    icon.name:"document-open-folder.svg"
                    onClicked:lliurexOneDriveWidget.openFolder()
                    Components3.ToolTip{
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
                    text:i18n("Free Space on OneDrive:")
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

                Components3.ToolButton {
                    width:35
                    height:35
                    display:AbstractButton.IconOnly
                    icon.name:lliurexOneDriveWidget.syncStatus?"kt-stop.svg":"kt-start.svg"
                    enabled:!lliurexOneDriveWidget.lliurexOneDriveOpen
                    onClicked:lliurexOneDriveWidget.manageSync() 
                    Components3.ToolTip{
                        text:lliurexOneDriveWidget.syncStatus?i18n("Click to stop synchronization"):i18n("Click to start synchronization")
                    }
                } 
            }   
        }    
    }

    function action_launchOneDrive() {
        lliurexOneDriveWidget.launchOneDrive()
    }
}	
