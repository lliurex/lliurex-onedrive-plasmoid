import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.plasmoid 2.0

import org.kde.plasma.private.lliurexonedrive 2.0
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
        plasmoid.setAction("openHelp",i18n("See help"),"help-contents"); 
    }

    Plasmoid.preferredRepresentation: Plasmoid.fullRepresentation
    
    Plasmoid.fullRepresentation: Item{
        id:root
        Layout.fillWidth:true
        QQC2.StackView{
            id:stackLayout
            property int currentIndex:lliurexOneDriveWidget.currentIndex
            anchors.fill: parent
            initialItem:spacesPanel
            onCurrentIndexChanged:{
                switch (currentIndex){
                    case 0:
                        stackLayout.replace(spacesPanel)
                        break;
                    case 1:
                        stackLayout.replace(spacePanel)
                        break;
                    case 2:
                        stackLayout.replace(filesPanel)
                        break;
                    case 3:
                        stackLayout.replace(uploadedFilesPanel)
                        break;
                }
            }
            

            Component{
                id:spacesPanel
                SpacesView{
                    id:spacesView
                }
            }
            Component{
                id:spacePanel
                SpaceInfo{
                    id:spaceInfo
                }
            }
            Component{
                id:filesPanel
                LatestFiles{
                    id:latestFiles
                }
            }
            Component{
                id:uploadedFilesPanel
                UploadedFiles{
                    id:uploadedFiles
                }
            }
       
        }
    }

    function action_launchOneDrive() {
        lliurexOneDriveWidget.launchOneDrive(false)
    }

    function action_openHelp(){
        lliurexOneDriveWidget.openHelp()
    }


}   
