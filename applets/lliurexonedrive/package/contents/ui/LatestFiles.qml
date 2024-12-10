import QtQuick
import QtQuick.Layouts
import QtQml.Models
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid
import org.kde.plasma.components as Components
import org.kde.plasma.components as PC3
import org.kde.kirigami as Kirigami
import org.kde.plasma.private.lliurexonedrive 1.0

Rectangle{
	color:"transparent"
	GridLayout{
        id:filesLayout
        rows: 2
        flow: GridLayout.TopToBottom
        rowSpacing:10
        width:parent.width
        RowLayout{
            id:headLatestFiles
            Layout.fillWidth:true
            PC3.ToolButton {
                height:35
                width:35
                icon.name:"arrow-left.svg"
                Layout.rightMargin:filesLayout.width/3-35/2
                PC3.ToolTip{
                    id:backTP   
                    text:i18n("Back to space view")
                }
                onClicked:{
                    id:backTP.hide()
                    lliurexOneDriveWidget.manageNavigation(1)
                } 
            } 
            Components.Label{
                id:headFilesText
                text:i18n("List of files")
                font.italic:true
                font.pointSize:11
                Layout.fillWidth:true
                Layout.alignment:Qt.AlignHCenter
            }
            PC3.ToolButton {
                width:35
                height:35
                Layout.alignment:Qt.AlignRight
                icon.name:"view-refresh"
                PC3.ToolTip{
                    id:filesTP
                    text:i18n("Click to refresh list")
                }
                onClicked:{
                    filesTP.hide()
                    lliurexOneDriveWidget.getLatestFiles()
                    listView.forceActiveFocus()
                    if (listView.count > 0){
                        listView.currentIndex=0
                    }

                }

            } 
        }
       PC3.ScrollView {
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
                    width: parent.width - (Kirigami.Units.largeSpacing * 4)
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
                    width: parent.width - (Kirigami.Units.largeSpacing * 4)
                    visible: lliurexOneDriveWidget.showSearchFiles
                    text: i18n("Searching information. Wait a moment...")
                }
               
            }
        }
    }

}
