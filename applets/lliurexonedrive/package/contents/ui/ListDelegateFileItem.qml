import QtQuick
import org.kde.ksvg as KSvg
import org.kde.plasma.components as PC3

PC3.ItemDelegate {
    id: fileItem

    property string fileName
    property string filePath
    property string fileDate
    property string fileTime

    width: listView.width
    highlighted: hovered || ListView.isCurrentItem

    onHoveredChanged: {
        if (hovered) {
            listView.currentIndex = index
        }
    }

    contentItem: Item {
        id: label
        implicitHeight: 35

        Column {
            id: labelRow
            anchors {
                left: parent.left
                right: searchBtn.left
                rightMargin: 10
                verticalCenter: parent.verticalCenter
            }

            PC3.Label {
                id: fileText
                text: fileName
                font.bold:fileItem.ListView.isCurrentItem
                width: parent.width
                elide: Text.ElideMiddle
            }

            PC3.Label {
                id: fileData
                text: i18n("Last modification: %1-%2", fileDate, fileTime)
                font.bold: true
                visible: fileItem.ListView.isCurrentItem
                width: parent.width
                elide: Text.ElideRight
            }
        }

        PC3.ToolButton {
            id: searchBtn
            width: 35
            height: 35
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            icon.name: "document-open-recent"
            visible: fileItem.ListView.isCurrentItem

            PC3.ToolTip {
                text: i18n("Click to access the file location")
            }

            onClicked: {
                if (lliurexOneDriveWidget.checkIfFileExists(filePath)) {
                    lliurexOneDriveWidget.goToFile(filePath)
                } else {
                    PC3.ToolTip.show(i18n("File does not exist. Update the list"))
                }
            }
        }
    }
}
