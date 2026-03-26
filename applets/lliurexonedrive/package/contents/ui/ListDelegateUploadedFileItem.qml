import QtQuick
import org.kde.ksvg as KSvg
import org.kde.plasma.components as PC3

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
                visible:uploadedFileItem.ListView.isCurrentItem?true:false
                width:fileText.width 
            }
        }
        
   }     
    
}
