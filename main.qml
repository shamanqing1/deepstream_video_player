import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2

import org.freedesktop.gstreamer.GLVideoItem 1.0
import Player 1.0

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    menuBar: MenuBar {
        Menu {
            id: fileMenu
            title: qsTr("&File")

            MenuItem {
                action: fileOpenAction
            }
        }
    }

    FileDialog {
        id: fileOpenDialog
        title: qsTr("Select a video file")
        folder: shortcuts.movies
        nameFilters: {
            "Video files (*.mp4 *.avi *.wmv)"
        }

        onAccepted: player.addVideo(fileUrl);
    }

    Action {
        id: fileOpenAction
        text: "&Open"
        onTriggered: fileOpenDialog.open()
    }


    Item {
        anchors.fill: parent

        Player {
            id: player
            output: videoOuput
            onPlaylistChanged: {
                console.log(playlist)
                let model = playlistView.model
                model.clear()
                for(var i = 0; i < playlist.length; i++) {
                    model.append({"name": playlist[i]})
                }
            }
        }

        GstGLVideoItem {
            id: videoOuput
            objectName: "videoItem"
            anchors.centerIn: parent
            width: parent.width
            height: parent.height
        }
    }


   Item {
       id: playlistItem
       anchors.right: parent.right
       anchors.top: parent.top
       width: 400
       height: parent.height

       ListView {
           id: playlistView
           anchors.fill: parent
           anchors.margins: 10
           spacing: 4
           clip: true
           orientation: ListView.Vertical
           model: ListModel{}
           delegate: listItem
       }

       Component {
           id: listItem
           Rectangle {
               width: playlistView.width
               height: 32
               color: Qt.rgba(0, 0, 0, 0.2)
               Text {
                   anchors.left: parent.left
                   width: parent.width - 32
                   text: index + ": " + name
                   font.pixelSize: 20
                   color: Qt.rgba(1, 1, 1, 1.0)
                   wrapMode: Text.NoWrap
                   elide: Text.ElideLeft
               }

               Button {
                   anchors.right: parent.right
                   width: 32
                   height: 32
                   icon.source: "images/close.png"
                   opacity: 0.5
                   onClicked: {
                       player.removeVideo(index);
                   }
               }
           }
       }
   }
}
