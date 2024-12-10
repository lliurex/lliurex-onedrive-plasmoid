import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid
import org.kde.plasma.components as Components
import org.kde.plasma.components as PC3
import org.kde.kquickcontrolsaddons as KQuickControlsAddons
import org.kde.kirigami as Kirigami

import org.kde.plasma.private.lliurexonedrive 1.0

Components.ItemDelegate {
    id: spaceItem
    property string idSpace
    property string nameSpace
    property int statusSpace
    property bool isRunningSpace
    property bool localFolderWarning
    property bool updateRequiredWarning
    readonly property bool isTall: height > Math.round(Kirigami.Units.gridUnit * 2.5)

    enabled:true
    height:65
    width:parent.width

    Item{
        id:label
        height:45
        width:310
        anchors.fill:parent
        MouseArea {
            id: mouseAreaOption
            anchors.fill: parent
            hoverEnabled:true
            propagateComposedEvents:true

            onEntered: {
                listSpaceView.currentIndex = index
            }
        }

        Column{
            id:spaceRow
            anchors.verticalCenter: parent.verticalCenter
            anchors.left:parent.left
            anchors.leftMargin:10

            Components.Label{
                id:spaceName
                text:nameSpace
                width:{
                    if (spaceItem.ListView.isCurrentItem){
                        260
                    }else{
                        310
                    }
                }
                elide:Text.ElideMiddle
                font.bold:{
                    if (spaceItem.ListView.isCurrentItem){
                        true
                    }else{
                        false
                    }
                }	
            }
            Components.Label{
                id:spaceStatusText
                text:getStatusText
                (statusSpace,localFolderWarning,updateRequiredWarning)
                width:260
                elide:Text.ElideMiddle
                visible:spaceItem.ListView.isCurrentItem
                font.italic:true
                font.bold:{
                    if (spaceItem.ListView.isCurrentItem){
                        true
                    }else{
                        false
                    }
                }
            }
        }

        Image {
            id:spaceStatusIcon
            source:getStatusIcon(statusSpace,localFolderWarning,updateRequiredWarning)
            sourceSize.width:32
            sourceSize.height:32
            anchors.leftMargin:10
            anchors.left:spaceRow.right
            anchors.verticalCenter:parent.verticalCenter
        }

        Image {
            id:spaceRunningIcon
            source:{
                if (isRunningSpace){
                    "/usr/share/icons/breeze/status/16/media-playback-playing.svg"
                }else{
                    "/usr/share/icons/breeze/status/16/media-playback-stopped.svg"
                }
            }
            sourceSize.width:32
            sourceSize.height:32
            anchors.leftMargin:15
            anchors.left:spaceStatusIcon.right
            anchors.verticalCenter:parent.verticalCenter
        }
        PC3.ToolButton {
            id:loadSpaceBtn
            width:35
            height:35

            anchors{
                left: spaceRunningIcon.right
                top: spaceName.isTall? parent.top : undefined
                verticalCenter: parent.verticalCenter
                leftMargin:10
                rightMargin:10
            }
            icon.name:"configure"
            visible:spaceItem.ListView.isCurrentItem
            PC3.ToolTip{
                id:detailsTP
                text:i18n("Click to get details of:\n")+nameSpace
            }
            onClicked:{
                detailsTP.hide()
                lliurexOneDriveWidget.goToSpace(idSpace)
            }
       }
   }

   function getStatusIcon(statusSpace,localFolderWarning,updateRequiredWarning){
        if (localFolderWarning || updateRequiredWarning){
            return "/usr/share/icons/breeze/status/16/state-warning.svg"
        }else{
            switch (statusSpace){
                case 0:
                    return "/usr/share/icons/breeze/status/16/state-ok.svg"
                    break; 
                case 3:
                    return "/usr/share/icons/breeze/status/16/state-offline.svg"
                    break;
                case 2:
                case 4:
                    return "/usr/share/icons/breeze/status/16/state-sync.svg"
                    break;
                default:
                    return "/usr/share/icons/breeze/status/16/state-warning.svg"
                    break
            }
      
        }
    }

    function getStatusText(statusSpace,localFolderWarning,updateRequiredWarning){

        var msg=""
        if (localFolderWarning){
            msg=i18n("Local folder has been deleted or emptied")

        }else{
            if (updateRequiredWarning){
                msg=i18n("Your attention is required")
            }else{
                switch (statusSpace){
                    case 0:
                        msg=i18n("All content synchronized")
                        break
                    case 2:
                        msg=i18n("Content pending syncing")
                        break;
                    case 4:
                        msg=i18n("Uploading pending changes")
                        break
                    case -1:
                        msg=i18n("OneDrive return an error")
                        break
                    case -2:
                        msg=i18n("No connection with OneDrive")
                        break
                    case -4:
                        msg=i18n("Free space is 0")
                        break
                    case -7:
                        msg=i18n("Authorization has expired")
                        break
                    case -9:
                        msg=i18n("OneDrive not available")
                        break
                    case -14:
                        msg=i18n("Access denied")
                    default:
                        msg=i18n("Information not available")
                        break
                }

            }
        }
        return msg

    }

}
