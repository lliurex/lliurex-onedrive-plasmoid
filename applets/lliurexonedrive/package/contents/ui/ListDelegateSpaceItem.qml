import QtQuick 2.15
import org.kde.plasma.components 3.0 as PC3
import org.kde.kirigami 2.12 as Kirigami

PC3.ItemDelegate {
    id: spaceItem

    property string idSpace
    property string nameSpace
    property int statusSpace
    property bool isRunningSpace
    property bool localFolderWarning
    property bool updateRequiredWarning
    property string filesPendingUpload

    width: listSpaceView.width
    highlighted: hovered || ListView.isCurrentItem

    onHoveredChanged: {
        if (hovered) {
            listSpaceView.currentIndex = index
        } 
    }

    contentItem: Item {
        id: label
        implicitHeight: 45

        Column {
            id: spaceRow
            anchors.left: parent.left
            anchors.right: spaceStatusIcon.left
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter

            PC3.Label {
                id: spaceName
                text: nameSpace
                width: parent.width
                elide: Text.ElideMiddle
                font.bold: spaceItem.ListView.isCurrentItem
            }

            PC3.Label {
                id: spaceStatusText
                text: getStatusText(statusSpace, localFolderWarning, updateRequiredWarning, filesPendingUpload)
                width: parent.width
                elide: Text.ElideMiddle
                visible: spaceItem.ListView.isCurrentItem
                font.italic: true
            }
        }

        Kirigami.Icon {
            id: spaceStatusIcon
            source: getStatusIcon(statusSpace, localFolderWarning, updateRequiredWarning)
            implicitWidth: 32
            implicitHeight: 32
            anchors.right: spaceRunningIcon.left
            anchors.rightMargin: 15
            anchors.verticalCenter: parent.verticalCenter
        }

        Kirigami.Icon {
            id: spaceRunningIcon
            source: isRunningSpace ? "media-playback-playing" : "media-playback-stopped"
            implicitWidth: 32
            implicitHeight: 32
            anchors.right: loadSpaceBtn.left
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
        }

        PC3.ToolButton {
            id: loadSpaceBtn
            width: 32
            height: 32
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            icon.name: "configure"
            visible: spaceItem.ListView.isCurrentItem

            PC3.ToolTip {
                text: i18n("Click to get details of:\n") + nameSpace
            }

            onClicked: {
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
                case 5:
                    return "/usr/share/icons/breeze/status/16/state-sync.svg"
                    break;
                default:
                    return "/usr/share/icons/breeze/status/16/state-warning.svg"
                    break
            }
               
     
        }

    }

    function getStatusText(statusSpace,localFolderWarning,updateRequiredWarning,filesPendingUpload){

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
                    case 5:
                        msg=filesPendingUpload + " "+ i18n("files pending to upload")
                        break;
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
