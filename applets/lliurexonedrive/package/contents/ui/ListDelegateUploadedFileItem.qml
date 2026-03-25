import QtQuick 2.15
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PC3

PC3.ItemDelegate {
    id: uploadedFileItem
    property string fileName
    property string filePath
    property string fileDate
    property string fileTime

    width: uploadedListView.width
    highlighted: hovered || ListView.isCurrentItem

    onHoveredChanged: {
        if (hovered) {
            uploadedListView.currentIndex = index
            uploadedListView.forceActiveFocus()
        } else {
            uploadedListView.currentIndex = -1
        }
    }

    contentItem: Item {
        id:label
        implicitHeight:35
        Column{
            id:labelRow
            width:uploadedListView.width-50
            anchors.verticalCenter:parent.verticalCenter
            anchors.leftMargin:10

            PC3.Label{
                id:fileText
                text:fileName
                font.bold:uploadedFileItem.ListView.isCurrentItem?true:false 
                width:labelRow.width-10
                elide:Text.ElideMiddle  

            }

            PC3.Label{
                id:fileData
                text: i18n("Uploaded on: ")+fileDate+"-"+fileTime
                font.bold:true
                font.pointSize: PC3.Theme.defaultFont.pointSize * 0.85
                visible:uploadedFileItem.ListView.isCurrentItem?true:false
                width:fileText.width 
            }
        }
        
   }     
    
}
