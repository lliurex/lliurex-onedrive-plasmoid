import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import QtQml.Models
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid 
import org.kde.plasma.components as Components

import org.kde.plasma.extras as PlasmaExtras
import org.kde.kirigami as Kirigami

import org.kde.plasma.private.lliurexonedrive 1.0
// Item - the most basic plasmoid component, an empty container.

PlasmoidItem {

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

    switchWidth: Kirigami.Units.gridUnit * 5
    switchHeight: Kirigami.Units.gridUnit * 5
    Plasmoid.icon:lliurexOneDriveWidget.iconName
    toolTipMainText: lliurexOneDriveWidget.toolTip
    toolTipSubText: lliurexOneDriveWidget.subToolTip

    Component.onCompleted: {
        /*plasmoid.removeAction("configure");
        plasmoid.setAction("launchOneDrive", i18n("Open Lliurex OneDrive"), "configure");
        plasmoid.setAction("openHelp",i18n("See help"),"help-contents"); */
        Plasmoid.setInternalAction("configure", configureAction)
    }

    
    fullRepresentation: PlasmaExtras.Representation{
        id:root
        Layout.fillWidth:true
        QQC2.StackView{
            id:stackLayout
            property int currentIndex:lliurexOneDriveWidget.currentIndex
            width:parent.width
            height:parent.height
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
       
        }
    }

    function action_launchOneDrive() {
        lliurexOneDriveWidget.launchOneDrive(false)
    }

    function action_openHelp(){
        lliurexOneDriveWidget.openHelp()
    }

    Plasmoid.contextualActions: [
        PlasmaCore.Action{
            text: i18n("See help")
            icon.name:"help-contents.svg"
           onTriggered:action_openHelp()
        }

    ]

    PlasmaCore.Action {
        id: configureAction
        text: i18n("Open Lliurex OneDrive")
        icon.name:"lliurex-onedrive.svg"
        onTriggered:action_launchOneDrive()
    }


}   
