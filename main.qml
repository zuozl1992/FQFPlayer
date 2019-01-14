import QtQuick 2.12
import QtQuick.Window 2.12
import Qt.labs.platform 1.1
import QtQuick.Controls 1.4

Window {
    function openMedia(path,type)
    {
        if(!VideoControl.openMedia(path,type))
            return;
        if(type === 0 || type === 2)
        {
            img.visible = true
            video.videoWidth = VideoControl.getVideoWidth();
            video.videoHeight = VideoControl.getVideoHeight();
            update()
        }
        else
        {
            img.visible = false
            img.source = "image://Color/black"
            video.videoWidth = 0;
            video.videoHeight = 0;
        }
        var filename = String(path).split("/")[String(path).split("/").length-1]
        var arr = String(filename).split(".")
        var str = ""
        for(var i = 0; i < arr.length-1; i++)
        {
            if(i != 0)
                str += "."
            str += arr[i]
        }

        headInfo.text = str
        headAni1.running = true
    }

    id: mainPage
    visible: true
    width: 1024
    height: 768
    minimumWidth: 600
    minimumHeight: 600
    property bool isChangeSlider: true
    title: qsTr("FQFVidePlayer")

    Connections {
        target: VideoControl
        onCallQmlRefeshImg: {
            img.source = "image://CodeImg/"+ Math.random()
        }
    }

    Timer {
        id: headHideTimer
        interval: 2000; running: false; repeat: true
        onTriggered: headAni1.running = true
    }

    Timer {
        id: updateTimer
        interval: 100; running: true; repeat: true
        onTriggered: {
            if(!isChangeSlider)
                return
            var pos = VideoControl.getMediaPos()
            videoPosPro.value = pos
        }
    }

    Rectangle {
        id: head
        width: parent.width
        height: 30
        x: 0
        y: 0
        z: 3
        color: "#3f3f3f"
        PropertyAnimation {
            id: headAni1
            target: head
            properties: "y"
            to: -30
            duration: 500
        }
        PropertyAnimation {
            id: headAni2
            target: head
            properties: "y"
            to: 0
            duration: 500
            onRunningChanged: {
                if(running == true)
                {
                    headHideTimer.stop()
                }
                else
                    headHideTimer.restart()
            }
        }
        Text {
            id: headInfo
            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.top: parent.top
            height: parent.height
            width: parent.width / 2 - 24
            font.pointSize: 12
            text: qsTr("")
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            color: "white"
            z: 2
        }
        Text {
            id: openFileBtn
            anchors.left: headInfo.right
            anchors.leftMargin: 8
            anchors.top: parent.top
            height: parent.height
            width: (parent.width / 2 - 24) / 2 - 8
            font.pointSize: 12
            text: qsTr("打开文件")
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            color: "white"
            z: 3
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    fileDialog.open()
                }
            }
        }
        Text {
            id: openStreamBtn
            anchors.left: openFileBtn.right
            anchors.leftMargin: 8
            anchors.top: parent.top
            height: parent.height
            width: (parent.width / 2 - 24) / 2 - 8
            font.pointSize: 12
            text: qsTr("打开串流")
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            color: "white"
            z: 3
            MouseArea {
                anchors.fill: parent
                onClicked: {
//                    openMedia("http://ivi.bupt.edu.cn/hls/cctv1hd.m3u8",2)
                    openMedia("rtsp://admin:briup2017@192.168.1.120",2)

                }
            }
        }
    }

    Item {
        id: foot
        width: parent.width
        height: 60
        y: parent.height - 60
        x: 0
        z: 3
        Slider {
            id: videoPosPro
            width: parent.width
            height: 22
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            maximumValue: 1
            minimumValue: 0
            value: 0
            onPressedChanged: {
                if(pressed)
                {
                    isChangeSlider = false
                }
                else
                {
                    headAni2.running = false
                    VideoControl.seek(value)
                    headAni2.running = true
                    isChangeSlider = true
                }
            }
        }
        Button {
            id: pausePlay
            anchors.left: parent.left
            anchors.bottom: videoPosPro.top
            property bool isPause: false
            onClicked: {
                VideoControl.setPause(!isPause)
                isPause = !isPause
            }
        }
    }

    Rectangle {
        id: video
        color: "black"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        z: 2
        property double videoWidth: 0
        property double videoHeight: 0
        //0--无  1--窄视频  2--宽视频
        property double videoPropStatus: (videoHeight <= 0) ?
                                             0 :
                                             (((width / height) > (videoWidth / videoHeight)) ? 1 : 2)
        Image {
            id: img
            width: parent.videoPropStatus == 0 ? 0 : parent.videoPropStatus == 1 ?
                           (parent.height / parent.videoHeight * parent.videoWidth) : parent.width
            height: parent.videoPropStatus == 0 ? 0 : parent.videoPropStatus == 1 ?
                        parent.height : (parent.width / parent.videoWidth * parent.videoHeight)
            anchors.left: parent.left
            anchors.leftMargin: (parent.width - width) / 2
            anchors.top: parent.top
            anchors.topMargin: (parent.height - height) / 2
            z: 2
            visible: false
        }
        Image {
            id: logo
            width: 300
            height: 300
            anchors.centerIn: parent
            source: "qrc:/images/logo_player2.png"
            visible: !img.visible
            z: 3
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    fileDialog.open()
                }
            }
        }
        MouseArea {
            z: 2
            anchors.fill: parent
            anchors.margins: 60
            onClicked: {
                if(head.y == -30)
                    headAni2.running = true
                else
                    headAni1.running = true
            }
        }
    }

    FileDialog {
        id: fileDialog;
        title: qsTr("Please choose an image file");
        selectedNameFilter.index: 0
        nameFilters: [
            "Video Files (*.mp4 *.mov)",
            "Audio Files (*.mp3 *.flac)"
        ];
        onAccepted: {
            var path = file.toString()
            var cs;
            if(Qt.platform.os == "windows"){
                cs = path.slice(8);
            }else{
                cs = path.slice(7);
            }
            openMedia(cs,selectedNameFilter.index)
        }

    }
}
